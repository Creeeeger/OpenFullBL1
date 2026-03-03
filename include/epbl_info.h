#pragma once

#include <stdint.h>

uint32_t get_epbl_expected_hash(void);
uint32_t get_epbl_expected_sz_bytes(void);
uint32_t get_epbl_load_addr(void);
uint32_t get_epbl_footer_base(void);

void set_epbl_expected_hash(uint32_t hash);
void set_epbl_expected_sz(uint32_t sz);
void set_epbl_load_addr(void);
void set_epbl_magic_sticky_flag_if_bootdev1(void);
