#pragma once

#include <stdbool.h>
#include <stdint.h>

void bootlog_store_elapsed_ticks(uint32_t log_slot_addr);

uint32_t load_auth_epbl(void);
bool try_load_epbl_from_mmc(void);
uint32_t try_load_epbl_from_ufs(uint32_t host);
uint64_t try_load_epbl_via_bootrom_or_usb(void);
uint32_t try_load_epbl_primary_source(void);
uint32_t try_load_epbl_fallback_source(void);

void host_clear_wait_status_bit(uint32_t host, uint32_t bit);
uint32_t host_issue_cmd_and_wait(uint32_t host, uint32_t cmd, uint32_t arg);
uint32_t ceil_div_0x1000(uint64_t number);
void usb_init_descriptors(uint32_t host);
