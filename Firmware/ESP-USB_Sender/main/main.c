#include <stdio.h>
#include <string.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_now.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "usb/hid_host.h"
#include "usb/usb_host.h"

static const char *TAG = "sender";

static uint8_t receiver_mac[] = {0xA0, 0xF2, 0x62, 0xE2, 0xE5, 0x18};

#define PKT_DEVICE_DESC 0xFD
#define PKT_CONFIG_DESC 0xFE
#define PKT_HID_REPORT  0xFF

static usb_host_client_handle_t desc_client_hdl = NULL;

QueueHandle_t app_event_queue = NULL;

typedef enum {
    APP_EVENT_HID_HOST
} app_event_group_t;

typedef struct {
    app_event_group_t event_group;
    struct {
        hid_host_device_handle_t handle;
        hid_host_driver_event_t event;
        void *arg;
    } hid_host_device;
} app_event_queue_t;

static void on_sent(const esp_now_send_info_t *info, esp_now_send_status_t status)
{
    ESP_LOGI(TAG, "Send status: %s", status == ESP_NOW_SEND_SUCCESS ? "OK" : "FAIL");
}

static void send_descriptors(uint8_t dev_addr)
{
    usb_device_handle_t dev_hdl;
    if (usb_host_device_open(desc_client_hdl, dev_addr, &dev_hdl) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open device at addr %d", dev_addr);
        return;
    }

    const usb_device_desc_t *dev_desc;
    if (usb_host_get_device_descriptor(dev_hdl, &dev_desc) == ESP_OK) {
        uint8_t pkt[19];
        pkt[0] = PKT_DEVICE_DESC;
        memcpy(&pkt[1], dev_desc, 18);
        esp_now_send(receiver_mac, pkt, 19);
        ESP_LOGI(TAG, "Sent device descriptor (VID=%04X PID=%04X)",
                 dev_desc->idVendor, dev_desc->idProduct);
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    const usb_config_desc_t *cfg_desc;
    if (usb_host_get_active_config_descriptor(dev_hdl, &cfg_desc) == ESP_OK) {
        const uint8_t *data = (const uint8_t *)cfg_desc;
        uint16_t total_len = cfg_desc->wTotalLength;
        uint8_t chunk_size = 247;
        uint8_t total_chunks = (total_len + chunk_size - 1) / chunk_size;

        ESP_LOGI(TAG, "Sending config descriptor (%d bytes, %d chunks)", total_len, total_chunks);

        for (uint8_t i = 0; i < total_chunks; i++) {
            uint16_t offset = i * chunk_size;
            uint8_t len = (offset + chunk_size > total_len) ? (total_len - offset) : chunk_size;
            uint8_t pkt[250];
            pkt[0] = PKT_CONFIG_DESC;
            pkt[1] = i;
            pkt[2] = total_chunks;
            memcpy(&pkt[3], &data[offset], len);
            esp_now_send(receiver_mac, pkt, len + 3);
            ESP_LOGI(TAG, "Sent config chunk %d/%d (%d bytes)", i + 1, total_chunks, len);
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }

    usb_host_device_close(desc_client_hdl, dev_hdl);
}

static void desc_client_event_cb(const usb_host_client_event_msg_t *msg, void *arg)
{
    if (msg->event == USB_HOST_CLIENT_EVENT_NEW_DEV) {
        ESP_LOGI(TAG, "New device at addr %d", msg->new_dev.address);
        send_descriptors(msg->new_dev.address);
    }
}

static void desc_client_task(void *arg)
{
    const usb_host_client_config_t client_config = {
        .is_synchronous = false,
        .max_num_event_msg = 5,
        .async = {
            .client_event_callback = desc_client_event_cb,
            .callback_arg = NULL,
        },
    };
    ESP_ERROR_CHECK(usb_host_client_register(&client_config, &desc_client_hdl));

    while (true) {
        usb_host_client_handle_events(desc_client_hdl, portMAX_DELAY);
    }
}

void hid_host_interface_callback(hid_host_device_handle_t hid_device_handle,
                                 const hid_host_interface_event_t event,
                                 void *arg)
{
    uint8_t data[64] = {0};
    size_t data_length = 0;

    switch (event) {
    case HID_HOST_INTERFACE_EVENT_INPUT_REPORT:
        ESP_ERROR_CHECK(hid_host_device_get_raw_input_report_data(
            hid_device_handle, data, 64, &data_length));
        ESP_LOGI(TAG, "HID report (%d bytes): %02X %02X %02X %02X %02X %02X %02X %02X",
                 (int)data_length, data[0], data[1], data[2], data[3],
                 data[4], data[5], data[6], data[7]);
        esp_now_send(receiver_mac, data, data_length);
        break;
    case HID_HOST_INTERFACE_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "HID Device DISCONNECTED");
        ESP_ERROR_CHECK(hid_host_device_close(hid_device_handle));
        break;
    case HID_HOST_INTERFACE_EVENT_TRANSFER_ERROR:
        ESP_LOGI(TAG, "HID Device TRANSFER_ERROR");
        break;
    default:
        break;
    }
}

void hid_host_device_event(hid_host_device_handle_t hid_device_handle,
                           const hid_host_driver_event_t event, void *arg)
{
    hid_host_dev_params_t dev_params;
    ESP_ERROR_CHECK(hid_host_device_get_params(hid_device_handle, &dev_params));

    switch (event) {
    case HID_HOST_DRIVER_EVENT_CONNECTED:
        ESP_LOGI(TAG, "HID Device CONNECTED (proto=%d)", dev_params.proto);
        const hid_host_device_config_t dev_config = {
            .callback = hid_host_interface_callback,
            .callback_arg = NULL
        };
        if (dev_params.proto != HID_PROTOCOL_NONE) {
            ESP_ERROR_CHECK(hid_host_device_open(hid_device_handle, &dev_config));

            size_t desc_len = 0;
            uint8_t *desc = hid_host_get_report_descriptor(hid_device_handle, &desc_len);
            if (desc && desc_len > 0 && desc_len <= 248) {
                uint8_t pkt[250];
                pkt[0] = PKT_HID_REPORT;
                pkt[1] = dev_params.proto;
                memcpy(&pkt[2], desc, desc_len);
                esp_now_send(receiver_mac, pkt, desc_len + 2);
                ESP_LOGI(TAG, "Sent HID report descriptor proto=%d (%d bytes)",
                         dev_params.proto, (int)desc_len);
                vTaskDelay(pdMS_TO_TICKS(10));
            }

            if (HID_SUBCLASS_BOOT_INTERFACE == dev_params.sub_class) {
                ESP_ERROR_CHECK(hid_class_request_set_protocol(
                    hid_device_handle, HID_REPORT_PROTOCOL_BOOT));
                if (HID_PROTOCOL_KEYBOARD == dev_params.proto) {
                    ESP_ERROR_CHECK(hid_class_request_set_idle(hid_device_handle, 0, 0));
                }
            }
            ESP_ERROR_CHECK(hid_host_device_start(hid_device_handle));
        }
        break;
    default:
        break;
    }
}

static void usb_lib_task(void *arg)
{
    const usb_host_config_t host_config = {
        .skip_phy_setup = false,
        .intr_flags = ESP_INTR_FLAG_LOWMED,
    };
    ESP_ERROR_CHECK(usb_host_install(&host_config));
    xTaskNotifyGive(arg);

    while (true) {
        uint32_t event_flags;
        usb_host_lib_handle_events(portMAX_DELAY, &event_flags);
        if (event_flags & USB_HOST_LIB_EVENT_FLAGS_NO_CLIENTS) {
            ESP_ERROR_CHECK(usb_host_device_free_all());
            break;
        }
    }

    vTaskDelay(10);
    ESP_ERROR_CHECK(usb_host_uninstall());
    vTaskDelete(NULL);
}

void hid_host_device_callback(hid_host_device_handle_t hid_device_handle,
                              const hid_host_driver_event_t event, void *arg)
{
    const app_event_queue_t evt_queue = {
        .event_group = APP_EVENT_HID_HOST,
        .hid_host_device.handle = hid_device_handle,
        .hid_host_device.event = event,
        .hid_host_device.arg = arg
    };
    if (app_event_queue) {
        xQueueSend(app_event_queue, &evt_queue, 0);
    }
}

void app_main(void)
{
    nvs_flash_init();
    esp_netif_init();
    esp_event_loop_create_default();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_start();

    esp_now_init();
    esp_now_register_send_cb(on_sent);

    esp_now_peer_info_t peer = {};
    memcpy(peer.peer_addr, receiver_mac, 6);
    peer.channel = 0;
    peer.encrypt = false;
    esp_now_add_peer(&peer);

    app_event_queue_t evt_queue;
    ESP_LOGI(TAG, "ESP-USB Sender starting");

    BaseType_t task_created = xTaskCreatePinnedToCore(usb_lib_task, "usb_events",
                                                      4096, xTaskGetCurrentTaskHandle(),
                                                      2, NULL, 0);
    assert(task_created == pdTRUE);
    ulTaskNotifyTake(false, 1000);

    xTaskCreatePinnedToCore(desc_client_task, "desc_client", 4096, NULL, 4, NULL, 0);

    const hid_host_driver_config_t hid_host_driver_config = {
        .create_background_task = true,
        .task_priority = 5,
        .stack_size = 4096,
        .core_id = 0,
        .callback = hid_host_device_callback,
        .callback_arg = NULL
    };
    ESP_ERROR_CHECK(hid_host_install(&hid_host_driver_config));

    app_event_queue = xQueueCreate(10, sizeof(app_event_queue_t));
    ESP_LOGI(TAG, "Waiting for device...");

    while (1) {
        if (xQueueReceive(app_event_queue, &evt_queue, portMAX_DELAY)) {
            if (APP_EVENT_HID_HOST == evt_queue.event_group) {
                hid_host_device_event(evt_queue.hid_host_device.handle,
                                      evt_queue.hid_host_device.event,
                                      evt_queue.hid_host_device.arg);
            }
        }
    }
}
