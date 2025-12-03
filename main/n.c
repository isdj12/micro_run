#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_rom_sys.h"
#include "esp_partition.h"
#include "esp_ota_ops.h"
#include "esp_system.h"
#include <string.h>

#define LED_PIN GPIO_NUM_2
#define BUTTON_OTA_0_PIN GPIO_NUM_4 
#define BUTTON_OTA_1_PIN GPIO_NUM_19 

static const char *TAG = "STAGE2";

static bool ota0 = false;
static bool ota1 = false;

static void switch_to_ota_partition(const char *partition_label) {
    ESP_LOGI(TAG, "Переключение на раздел: %s", partition_label);
    
    const esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_ANY, partition_label);
    
    if (partition == NULL) {
        ESP_LOGE(TAG, "Раздел %s не найден!", partition_label);
        return;
    }
    
    esp_err_t err = esp_ota_set_boot_partition(partition);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "Загрузочный раздел установлен на %s", partition_label);
        
        for (int i = 0; i < 3; i++) {
            gpio_set_level(LED_PIN, 1);
            esp_rom_delay_us(100000); 
            gpio_set_level(LED_PIN, 0);
            esp_rom_delay_us(100000);
        }
        
        // Перезагрузка для применения изменений
        ESP_LOGI(TAG, "Перезагрузка для применения изменений...");
        esp_rom_delay_us(500000); 
        esp_restart();
    } else {
        ESP_LOGE(TAG, "Ошибка установки загрузочного раздела: %s", esp_err_to_name(err));
    }
}

void app_main(void) {
    ESP_LOGI(TAG, "Stage2 Bootloader started");

    gpio_config_t io_conf_led = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL << LED_PIN),
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE
    };
    gpio_config(&io_conf_led);

    gpio_config_t io_conf_button_ota0 = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1ULL << BUTTON_OTA_0_PIN),
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_ENABLE
    };
    gpio_config(&io_conf_button_ota0);
    
    gpio_config_t io_conf_button_ota1 = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1ULL << BUTTON_OTA_1_PIN),
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_ENABLE
    };
    gpio_config(&io_conf_button_ota1);

    ESP_LOGI(TAG, "Stage2 Bootloader готов к работе");
    ESP_LOGI(TAG, "GPIO %d - переключение на ota_0", BUTTON_OTA_0_PIN);
    ESP_LOGI(TAG, "GPIO %d - переключение на ota_1", BUTTON_OTA_1_PIN);

    while (1) {
        gpio_set_level(LED_PIN, 1);
        esp_rom_delay_us(500000); 

        gpio_set_level(LED_PIN, 0);
        esp_rom_delay_us(500000);

        bool current_ota0 = (gpio_get_level(BUTTON_OTA_0_PIN) == 0);
        if (current_ota0 && !ota0) {
            ESP_LOGI(TAG, "Кнопка ota_0 нажата! Переключение на ota_0...");
            switch_to_ota_partition("ota_0");
        }
        ota0 = current_ota0;

        bool current_ota1 = (gpio_get_level(BUTTON_OTA_1_PIN) == 0);
        if (current_ota1 && !ota1) {
            ESP_LOGI(TAG, "Кнопка ota_1 нажата! Переключение на ota_1...");
            switch_to_ota_partition("ota_1");
        }
        ota1 = current_ota1;
    }
}
