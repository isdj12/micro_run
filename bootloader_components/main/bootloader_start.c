#include <stdbool.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "bootloader_init.h"
#include "bootloader_utility.h"
#include "bootloader_common.h"

static const char* TAG = "boot";

static int select_partition_number(bootloader_state_t *bs);

void mmu_setup() {
}

void jump_to(uint32_t entry_point_address) {
    void (*func)() = (void (*)())entry_point_address;
    func();
}

void __attribute__((noreturn)) call_start_cpu0(void) {
    if (bootloader_init() != ESP_OK) {
        bootloader_reset();
    }

#ifdef CONFIG_BOOTLOADER_SKIP_VALIDATE_IN_DEEP_SLEEP
    bootloader_utility_load_boot_image_from_deep_sleep();
#endif

    bootloader_state_t bs = {0};
    int boot_index = select_partition_number(&bs);
    if (boot_index == INVALID_INDEX) {
        bootloader_reset();
    }

    ESP_LOGI(TAG, CONFIG_EXAMPLE_BOOTLOADER_WELCOME_MESSAGE);

    bootloader_utility_load_boot_image(&bs, boot_index);
    ESP_LOGE(TAG, "load ");
    jump_to(0x110000);
}

static int select_partition_number(bootloader_state_t *bs) {
    if (!bootloader_utility_load_partition_table(bs)) {
        ESP_LOGE(TAG, "load partition table error!");
        return INVALID_INDEX;
    }

    return bootloader_utility_get_selected_boot_partition(bs);
}

struct _reent *__getreent(void) {
    return _GLOBAL_REENT;
}
