#include "esp_log.h"
#include "esp_system.h"
#include "esp_ota_ops.h"

void jump_to(uint32_t entry_point_address) {
    void (*func)() = (void (*)())entry_point_address; 
    func(); 
} 

void __attribute__((noreturn)) call_start_cpu0(void) {
    ESP_LOGI("Отладка", "Переход на OTA приложение..."); 
    
    const esp_partition_t *partition = esp_ota_get_next_update_partition(NULL); 
    if (partition != NULL) {
        ESP_LOGI("Ota Info", "Переключение на OTA раздел: %s", partition->label);
        
        jump_to(partition->address + 0x1000); 
    } else {
        ESP_LOGE("Ota Error", "Следующий OTA раздел не найден.");
    }

    while(1) {
        vTaskDelay(portMAX_DELAY); 
    }
}
---------------
void app_main() {
    ESP_LOGI("Подготовка", "Готовимся к переключению на OTA");
    call_start_cpu0(); 
}


#include "esp_log.h"

void mmu_setup() {
    
}

void jump_to(uint32_t entery_point_address) {
    void (*func)() = (void (*)())entery_point_address;
    func();
}

void __attribute__((noreturn)) call_start_cpu0(void) {
    ESP_LOGI("ююю", "да");
    jump_to( 0x110000);
}