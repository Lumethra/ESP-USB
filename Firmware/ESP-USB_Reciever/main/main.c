#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "esp_now.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "tinyusb.h"
#include "tinyusb_default_config.h"
#include "class/hid/hid_device.h"
#include "device/usbd.h"

static const char *TAG = "receiver";

#define PKT_DEVICE_DESC 0xFD
#define PKT_CONFIG_DESC 0xFE
#define PKT_HID_REPORT  0xFF

#define MAX_CONFIG_DESC_SIZE 1024

static const char *hid_string_descriptor[5] = {
    (char[]){0x09, 0x04},
    "ESP-USB",
    "ESP-USB HID",
    "123456",
    "ESP-USB Device",
};

static const uint8_t fallback_report_desc[] = {
    TUD_HID_REPORT_DESC_KEYBOARD(HID_REPORT_ID(HID_ITF_PROTOCOL_KEYBOARD)),
};

#define FALLBACK_DESC_TOTAL_LEN (TUD_CONFIG_DESC_LEN + TUD_HID_DESC_LEN)
static const uint8_t fallback_config_desc[] = {
    TUD_CONFIG_DESCRIPTOR(1, 1, 0, FALLBACK_DESC_TOTAL_LEN,
                          TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 100),
    TUD_HID_DESCRIPTOR(0, 4, false, sizeof(fallback_report_desc), 0x81, 16, 10),
};

static tusb_desc_device_t received_device_desc = {0};
static uint8_t received_config_desc[MAX_CONFIG_DESC_SIZE] = {0};
static uint16_t received_config_len = 0;
static uint8_t config_chunks_received = 0;
static uint8_t config_chunks_total = 0;
static uint8_t received_report_desc[249] = {0};
static size_t received_report_desc_len = 0;
static uint8_t built_config_desc[TUD_CONFIG_DESC_LEN + TUD_HID_DESC_LEN];

static bool device_desc_ready = false;
static bool config_desc_ready = false;
static bool report_desc_ready = false;
static bool tusb_installed = false;
static TaskHandle_t main_task_handle = NULL;

uint8_t const *tud_hid_descriptor_report_cb(uint8_t instance)
{
    return received_report_desc_len > 0 ? received_report_desc : fallback_report_desc;
}

uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id,
                                hid_report_type_t report_type, uint8_t *buffer,
                                uint16_t reqlen)
{
    return 0;
}

void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id,
                            hid_report_type_t report_type,
                            uint8_t const *buffer, uint16_t bufsize)
{
}

static void do_install_tusb(bool use_real_desc)
{
    if (tusb_installed) {
        ESP_LOGI(TAG, "Uninstalling TinyUSB...");
        tinyusb_driver_uninstall();
        tusb_installed = false;
        vTaskDelay(pdMS_TO_TICKS(500));
    }

    tinyusb_config_t tusb_cfg = TINYUSB_DEFAULT_CONFIG();
    tusb_cfg.descriptor.string = hid_string_descriptor;
    tusb_cfg.descriptor.string_count =
        sizeof(hid_string_descriptor) / sizeof(hid_string_descriptor[0]);

    if (use_real_desc) {
        received_device_desc.bMaxPacketSize0 = 64;
        tusb_cfg.descriptor.device = &received_device_desc;
            
        uint16_t total_len = TUD_CONFIG_DESC_LEN + TUD_HID_DESC_LEN;
        const uint8_t cfg[] = {
            TUD_CONFIG_DESCRIPTOR(1, 1, 0, total_len, TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 100),
            TUD_HID_DESCRIPTOR(0, 4, false, received_report_desc_len, 0x81, 16, 10),
        };
        memcpy(built_config_desc, cfg, sizeof(cfg));
        tusb_cfg.descriptor.full_speed_config = built_config_desc;

        ESP_LOGI(TAG, "Installing TinyUSB with real VID/PID and report desc (%d bytes)",
                 (int)received_report_desc_len);
    } else {
        tusb_cfg.descriptor.device = NULL;
        tusb_cfg.descriptor.full_speed_config = fallback_config_desc;
        ESP_LOGI(TAG, "Installing TinyUSB with fallback descriptor");
    }

    esp_err_t err = tinyusb_driver_install(&tusb_cfg);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "TinyUSB install failed: %s", esp_err_to_name(err));
        return;
    }
    tusb_installed = true;
}

static void on_recv(const esp_now_recv_info_t *info, const uint8_t *data, int len)
{
    if (data[0] == PKT_DEVICE_DESC && len == 19) {
        ESP_LOGI(TAG, "Received device descriptor");
        memcpy(&received_device_desc, &data[1], sizeof(tusb_desc_device_t));
        device_desc_ready = true;
        config_desc_ready = false;
        report_desc_ready = false;
        config_chunks_received = 0;
        config_chunks_total = 0;
        received_config_len = 0;
        received_report_desc_len = 0;
        return;
    }

    if (data[0] == PKT_CONFIG_DESC && len >= 4) {
        uint8_t chunk_idx = data[1];
        uint8_t total_chunks = data[2];
        uint8_t chunk_data_len = len - 3;
        uint16_t offset = chunk_idx * 247;

        if (chunk_idx == 0) {
            config_chunks_received = 0;
            config_chunks_total = total_chunks;
            received_config_len = 0;
        }

        if (offset + chunk_data_len <= MAX_CONFIG_DESC_SIZE) {
            memcpy(&received_config_desc[offset], &data[3], chunk_data_len);
            received_config_len += chunk_data_len;
            config_chunks_received++;
            ESP_LOGI(TAG, "Config chunk %d/%d received", chunk_idx + 1, total_chunks);

            if (config_chunks_received == config_chunks_total) {
                ESP_LOGI(TAG, "Config descriptor complete (%d bytes)", received_config_len);
                config_desc_ready = true;
                if (device_desc_ready && report_desc_ready) {
                    xTaskNotifyGive(main_task_handle);
                }
            }
        }
        return;
    }

    if (data[0] == PKT_HID_REPORT) {
        uint8_t proto = data[1];
        size_t desc_len = len - 2;
        const char *proto_name = proto == 1 ? "keyboard" : proto == 2 ? "mouse" : "vendor";
        ESP_LOGI(TAG, "Received HID report descriptor proto=%s (%d bytes)", proto_name, (int)desc_len);

        if (proto == 0) return;
        if (!report_desc_ready) {
            memcpy(received_report_desc, &data[2], desc_len);
            received_report_desc_len = desc_len;
            report_desc_ready = true;
            if (device_desc_ready && config_desc_ready) {
                xTaskNotifyGive(main_task_handle);
            }
        }
        return;
    }

    ESP_LOGI(TAG, "HID report (%d bytes): %02X %02X %02X %02X %02X %02X %02X %02X",
             len, data[0], data[1], data[2], data[3], data[4], data[5],
             data[6], data[7]);

    if (tusb_installed && tud_mounted()) {
        tud_hid_n_report(0, 0, data, len);
    }
}

void app_main(void)
{
    main_task_handle = xTaskGetCurrentTaskHandle();

    nvs_flash_init();
    esp_netif_init();
    esp_event_loop_create_default();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_start();

    esp_now_init();
    esp_now_register_recv_cb(on_recv);

    do_install_tusb(false);
    ESP_LOGI(TAG, "Receiver ready, waiting for device descriptors...");

    while (1) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        ESP_LOGI(TAG, "Real descriptors received, reinstalling TinyUSB...");
        do_install_tusb(true);
    }
}
