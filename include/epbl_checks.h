#pragma once

#include <stdbool.h>
#include <stdint.h>

bool check_epbl_magic(void);
uint32_t check_epbl_head(void);

uint32_t epbl_check_hash(void);
bool bl1_verify_self_hash(void);

uint32_t compute_real_hash(uintptr_t data_pointer, uint32_t data_len);

void write_epbl_hash(void);
uint32_t auth_epbl_footer_signature(void);
uint32_t epbl_additional_security_checks(void);
bool epbl_check_uid_constraint(void);

uint32_t verify_epbl_and_setup(uint32_t secure_auth_mode);

uint64_t verify_image_footer_signature_wrapper(uint32_t mode, uintptr_t sig_ctx, uintptr_t image,
                                               uint32_t image_sz, uintptr_t footer,
                                               uint32_t footer_sz);
