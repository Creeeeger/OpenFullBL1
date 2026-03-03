#include <stdint.h>

#include <bl1_info.h>
#include <bootdev.h>
#include <bootrom_funcs.h>
#include <epbl_checks.h>
#include <epbl_info.h>
#include <epbl_loader.h>
#include <fwbl1_map.h>
#include <pmu.h>

void jump_to_epbl_entrypoint(void)
{
    uintptr_t epbl_entry = (uintptr_t)get_epbl_load_addr() + 0x10U;
    ((void (*)(void))epbl_entry)();
}

void fatal_error_halt(uint32_t error_code)
{
    uint32_t bl1_sz = get_bl1_sz();
    uint32_t panic_ctx = mmio_read32(BL1_IMAGE_BASE + bl1_sz + BL1_PANIC_CTX_OFFSET);

    call_bootrom_panic(1, 1, 1, panic_ctx, error_code);

    for (;;) {
    }
}

void noop_hook_pre_boot(void)
{
}

void noop_hook_post_boot(void)
{
}

void bl1_self_integrity_check(void)
{
    if ((pmu_cfg_bit16() == 1U) && !bl1_verify_self_hash()) {
        fatal_error_halt(0x0eU);
    }
}

static uint32_t should_force_fallback_from_magic_sticky(void)
{
    if ((pmu_cfg_bit24() == 1U) && ((mmio_read32(REG_158609BC) & 1U) != 0U)) {
        return 1;
    }

    return 0;
}

void magic_postcfg_clear_ctrl_bits(void)
{
    uint32_t value = mmio_read32(REG_10050000);
    value &= ~1U;
    mmio_write32(REG_10050000, value);

    value = mmio_read32(REG_10050000);
    value &= ~0x20U;
    mmio_write32(REG_10050000, value);
}

void apply_magic_post_config_sequence(void)
{
    uint32_t upper;
    uint32_t lower;

    magic_postcfg_clear_ctrl_bits();
    set_soc_reg_15861240_bit2(1);

    upper = mmio_read32(REG_10050000) & 0xffff0000U;
    lower = mmio_read32(REG_10050000) & 0x000000fbU;

    mmio_write32(REG_10050004, 0x01b4U);
    mmio_write32(REG_10050008, 0x01b4U);

    set_soc_reg_15861240_bit2(0);
    mmio_write32(REG_10050000, upper | lower | 0x0000ee00U | 0x39U);
}

void maybe_apply_magic_post_config(void)
{
    if (mmio_read32(get_epbl_load_addr() + 0x14U) == EPBL_MAGIC_STICKY_WORD) {
        apply_magic_post_config_sequence();
    }
}

uint64_t fwbl1_main(void)
{
    uint32_t secure_auth_mode;
    uint32_t boot_dev_id;
    uint32_t boot_source;
    uint32_t use_secondary_source;
    uint32_t epbl_verify_success;
    int32_t counter;

    secure_auth_mode = call_secure_auth_mode_word();
    boot_dev_id = get_bootdev_id();
    boot_source = get_boot_source_from_boot_device_word();

    if (boot_source == 4U) {
        set_soc_reg_1586030c_bootsrc4_mode();
    }

    noop_hook_post_boot();
    noop_hook_pre_boot();
    set_epbl_load_addr();

    use_secondary_source = should_force_fallback_from_magic_sticky();
    if (use_secondary_source == 0U) {
        epbl_verify_success = try_load_epbl_primary_source();
        if ((epbl_verify_success & 0xffU) == 1U) {
            epbl_verify_success = verify_epbl_and_setup(secure_auth_mode);
        }
        if ((epbl_verify_success & 0xffU) == 0U) {
            goto fallback;
        }
    } else {
    fallback:
        set_bootdev_id_low_nibble(boot_dev_id + 1U);
        if ((boot_dev_id + 1U) > 2U) {
            goto fail;
        }

        counter = 2 - (int32_t)boot_dev_id;
        do {
            epbl_verify_success = try_load_epbl_fallback_source();
            if ((epbl_verify_success & 0xffU) == 1U) {
                epbl_verify_success = verify_epbl_and_setup(secure_auth_mode);
            }
            counter--;
        } while (counter != 0);

        boot_dev_id = 3U;
    }

    if ((epbl_verify_success & 0xffU) == 1U) {
        call_set_registers(0, 0x80000000U);
        set_epbl_magic_sticky_flag_if_bootdev1();
        if (boot_dev_id == 1U) {
            maybe_apply_magic_post_config();
        }
        jump_to_epbl_entrypoint();
        return 0;
    }

fail:
    fatal_error_halt(0);
    return 0;
}

void bl1_entry(void)
{
    volatile uint32_t *p;
    volatile uint32_t *end;

    if (mmio_read32(REG_RESUME_FLAG) == 1U) {
        jump_to_epbl_entrypoint();
        return;
    }

    p = (volatile uint32_t *)(uintptr_t)0x02023864U;
    end = (volatile uint32_t *)(uintptr_t)0x02023864U;

    while (p != end) {
        *p++ = 0;
    }

    (void)fwbl1_main();
}

void main(void)
{
    bl1_entry();
}
