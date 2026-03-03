#pragma once

#include <stdint.h>

void call_set_registers(uint32_t selector, uint32_t bits);

void call_usb_send(const char *addr, uint32_t len);
uint32_t call_usb_receive(uintptr_t addr, uint32_t max_size);
uint8_t call_usb_enumerate_and_wait_ready(uint32_t ctx, uint32_t timeout, uint32_t flags);

uint32_t call_get_bl1_expected_sz(void);
uint8_t call_mmc_read_blocks_parent(uint32_t lba, uint32_t count, uintptr_t dst);

uint32_t call_get_boot_flow_mode_word(void);
uintptr_t call_get_bootrom_api_ptr_by_index(uint32_t idx);

void call_dat_020200b0(void);
void call_dat_020200cc(uint32_t arg);
void call_dat_020200f8(void);
uint8_t call_dat_020200fc(void);

uint8_t call_ufs_link_startup_and_probe(void);
uint8_t call_ufs_ready_check(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
uint8_t call_ufs_read_blocks_wrapper(uint32_t host, uint32_t slot, uint32_t lun, uintptr_t dst,
                                     uint32_t lba, uint32_t count, uint32_t blk_sz);

void call_delay_wait(uint32_t a0, uint32_t a1);
void call_calc_hash(uintptr_t out, uintptr_t data, uint32_t len, uint32_t mode);

void call_bootrom_panic(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
uint32_t call_secure_auth_mode_word(void);
