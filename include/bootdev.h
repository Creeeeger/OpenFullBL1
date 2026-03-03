#pragma once

#include <stdint.h>

uint32_t get_boot_source_from_boot_device_word(void);
uint32_t get_bootdev_id(void);

void set_boot_device_id(uint32_t attempt);
void set_bootdev_id_low_nibble(uint32_t bootdev_id);

uint32_t get_bootdev_table_entry(uint32_t idx);
void set_bootdev_table_entry(uint32_t idx, uint32_t value);
void update_epbl_progress_word(uint32_t field_id, uint32_t value);
