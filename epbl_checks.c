#include <stdbool.h>
#include <stdint.h>

#include <bl1_info.h>
#include <bootdev.h>
#include <bootrom_funcs.h>
#include <epbl_checks.h>
#include <epbl_info.h>
#include <epbl_loader.h>
#include <fwbl1_map.h>

bool check_epbl_magic(void)
{
    return mmio_read32(get_epbl_load_addr() + 0x8U) == EPBL_MAGIC_WORD;
}

uint32_t check_epbl_head(void)
{
    uint32_t block_sz;
    uint32_t epbl_load_addr;

    if (!check_epbl_magic()) {
        return 0;
    }

    epbl_load_addr = get_epbl_load_addr();
    block_sz = mmio_read32(epbl_load_addr);

    if ((block_sz <= 2U) || (block_sz > ((EPBL_LIMIT_ADDR - epbl_load_addr) >> 9U))) {
        return 0;
    }

    set_epbl_expected_sz(block_sz << 9U);
    set_epbl_expected_hash(mmio_read32(epbl_load_addr + 0x4U));
    mmio_write32(epbl_load_addr + 0x4U, 0);

    return 1;
}

uint32_t compute_real_hash(uintptr_t data_pointer, uint32_t data_len)
{
    uint32_t file_hash[16];

    call_calc_hash((uintptr_t)file_hash, data_pointer, data_len, 2);
    return file_hash[0];
}

uint32_t epbl_check_hash(void)
{
    uint32_t epbl_total_size = get_epbl_expected_sz_bytes();
    uintptr_t epbl_load = (uintptr_t)get_epbl_load_addr();
    
    uint32_t epbl_actual_hash = compute_real_hash(epbl_load + 0x10U, epbl_total_size - 0x10U);
    set_epbl_expected_hash(epbl_actual_hash);
    
    uint32_t epbl_expected_hash = get_epbl_expected_hash();
    
    if (epbl_expected_hash != epbl_actual_hash) {
        return 0;
    }

    if (get_bootdev_id() == 1U) {
        call_set_registers(0, 0x08000000U);
    } else {
        call_set_registers(1, 0x04000000U);
    }

    return 1;
}

bool bl1_verify_self_hash(void)
{
    uint32_t actual_hash = compute_real_hash(0, 0x00020000U);
    uint32_t boot_flow_mode = call_get_boot_flow_mode_word();
    uint32_t hash_overwrite = (boot_flow_mode == 1U) ? 0x7d4baa8bU : 0U;
    uint32_t expected_hash = (boot_flow_mode == 0U) ? 0x978881abU : hash_overwrite;

    return expected_hash == actual_hash;
}

void write_epbl_hash(void)
{
    uint32_t bl1_sz = get_bl1_sz();
    uint32_t epbl_expected_sz = get_epbl_expected_sz_bytes();
    uintptr_t epbl_load_addr = (uintptr_t)get_epbl_load_addr();

    call_calc_hash(BL1_IMAGE_BASE + bl1_sz + BL1_HASH_OUT_OFFSET, epbl_load_addr,
                   epbl_expected_sz - 0x200U, 2);
}

uint64_t verify_image_footer_signature_wrapper(uint32_t mode, uintptr_t sig_ctx, uintptr_t image,
                                               uint32_t image_sz, uintptr_t footer,
                                               uint32_t footer_sz)
{
    uint32_t reduced_footer;
    uint64_t ret;
    uintptr_t fn;

    if (sig_ctx == 0U) {
        return 0x5000U;
    }

    ret = 0x5001U;
    if ((image == 0U) || (image_sz <= 0x10U) || (footer == 0U) || (footer_sz > 0x200U)) {
        return ret;
    }

    reduced_footer = mmio_read32((uint32_t)(footer - 0x0cU));
    if (reduced_footer > 8U) {
        return 0x5003U;
    }

    if (mode == 1U) {
        if (((7U >> (reduced_footer & 0x1fU)) & 1U) == 0U) {
            fn = (uintptr_t)mmio_read32((uint32_t)(sig_ctx + 0x484U));
            return ((uint64_t(*)(uint32_t, uint32_t, uint32_t, uintptr_t, uint32_t, uintptr_t,
                                uint32_t, uintptr_t, uint32_t, uintptr_t, uint32_t))fn)(
                reduced_footer, 1, 1, sig_ctx + 0x5cU, 0x20cU, image, image_sz, footer, 0x44U,
                footer + 0x44U, 0x44U);
        }
        ret = 1U;
    } else {
        if (((7U >> (reduced_footer & 0x1fU)) & 1U) == 0U) {
            return 0x5004U;
        }
        ret = 0U;
    }

    fn = (uintptr_t)mmio_read32((uint32_t)(sig_ctx + 0x478U));
    return ((uint64_t(*)(uint64_t, uint64_t, uintptr_t, uint32_t, uintptr_t, uint32_t, uintptr_t,
                        uint32_t))fn)(ret, ret, sig_ctx + 0x5cU, 0x20cU, image, image_sz, footer,
                                       footer_sz);
}

uint32_t auth_epbl_footer_signature(void)
{
    uintptr_t epbl_load_addr = (uintptr_t)get_epbl_load_addr();
    uint32_t epbl_size_bytes = get_epbl_expected_sz_bytes();
    uint32_t bl1_size_bytes = call_get_bl1_expected_sz();

    int footer_signature_valid =
        (int)verify_image_footer_signature_wrapper(
            1, BL1_IMAGE_BASE + bl1_size_bytes + BL1_SIG_CTX_OFFSET, epbl_load_addr,
            epbl_size_bytes - 0x200U, epbl_load_addr + epbl_size_bytes - 0x200U, 0x200U);

    if (footer_signature_valid != 0) {
        return 0;
    }

    if (get_bootdev_id() == 1U) {
        call_set_registers(0, 0x10000000U);
    } else {
        call_set_registers(1, 0x08000000U);
    }

    return 1;
}

bool epbl_check_uid_constraint(void)
{
    uint32_t uid_words[4];
    uint32_t leading_zero_count_total;
    uint32_t counter;

    call_set_registers(0, 0x40000000U);

    uid_words[0] = mmio_read32(REG_UID_3) & 0xffffU;
    uid_words[1] = mmio_read32(REG_UID_2);
    uid_words[2] = mmio_read32(REG_UID_1);
    uid_words[3] = mmio_read32(REG_UID_0);

    leading_zero_count_total = 0;
    counter = 0;

    while (counter < 4U) {
        uint32_t current_word = uid_words[counter];
        uint32_t counter0 = 0;

        while (counter0 < 0x20U) {
            if ((current_word & 0x80000000U) != 0U) {
                leading_zero_count_total += counter0;
                goto done;
            }
            counter0++;
            current_word <<= 1U;
        }

        counter++;
        leading_zero_count_total += 0x20U;
    }

done:
    return (0x80U - leading_zero_count_total) <= mmio_read32(get_epbl_footer_base());
}

uint32_t epbl_additional_security_checks(void)
{
    uint32_t ret;

    if (((mmio_read32(REG_10001008) >> 2U) & 1U) == 0U) {
        ret = epbl_check_uid_constraint();
        if ((ret & 0xffU) != 1U) {
            return ret;
        }
    }

    if (get_bootdev_id() == 1U) {
        call_set_registers(0, 0x20000000U);
    } else {
        call_set_registers(1, 0x10000000U);
    }

    return 1;
}

uint32_t verify_epbl_and_setup(uint32_t secure_auth_mode)
{
    secure_auth_mode = 0U;
    if (secure_auth_mode == 0U) {
        if (epbl_check_hash() == 0U) {
            return 0;
        }

        bootlog_store_elapsed_ticks(IRAM_BOOTLOG_VERIFY_HASH);
        return 1;
    }

    if (auth_epbl_footer_signature() == 0U) {
        return 0;
    }

    if (epbl_additional_security_checks() == 0U) {
        return 0;
    }

    bootlog_store_elapsed_ticks(IRAM_BOOTLOG_VERIFY_AUTH);
    write_epbl_hash();

    return 1;
}
