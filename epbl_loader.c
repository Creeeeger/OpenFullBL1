#include <stdbool.h>
#include <stdint.h>

#include <bootdev.h>
#include <bootrom_funcs.h>
#include <epbl_checks.h>
#include <epbl_info.h>
#include <epbl_loader.h>
#include <fwbl1_map.h>

static const char s_ready_to_rx_bl1[] = "Ready to rx BL1";
static const char s_ready_to_rx_epbl[] = "Ready to rx EPBL";
static const char s_epbl_rx_done[] = "EPBL rx done";
static const char s_epbl_rx_fail_upper[] = "EPBL RX Fail";
static const char s_epbl_header_fail_upper[] = "EPBL Header Fail";
static const char s_epbl_header_pass_upper[] = "EPBL Header Pass";
static const char s_epbl_header_fail[] = "EPBL Header fail";
static const char s_epbl_rx_fail[] = "EPBL rx fail";
static const char s_epbl_header_pass[] = "EPBL Header pass";

void bootlog_store_elapsed_ticks(uint32_t log_slot_addr)
{
    mmio_write32(log_slot_addr, mmio_read32(IRAM_BOOT_TIME_BASELINE));
    mmio_write32(log_slot_addr, mmio_read32(REG_TIMER_NOW) - mmio_read32(IRAM_BOOT_TIME_BASELINE));
}

uint32_t host_issue_cmd_and_wait(uint32_t host, uint32_t cmd, uint32_t arg)
{
    uintptr_t host_base = ufs_host_base(host);

    mmio_write32((uint32_t)(host_base + 0x20U), 0x400U);
    mmio_write32((uint32_t)(host_base + 0x94U), arg | (cmd << 16U));
    mmio_write32((uint32_t)(host_base + 0x90U), 1U);

    while (((mmio_read32((uint32_t)(host_base + 0x20U)) >> 10U) & 1U) == 0U) {
    }

    mmio_write32((uint32_t)(host_base + 0x20U), 0x400U);
    return mmio_read32((uint32_t)(host_base + 0x9cU));
}

void host_clear_wait_status_bit(uint32_t host, uint32_t bit)
{
    uintptr_t host_base = ufs_host_base(host);
    uint32_t mask = 1U << (bit & 0x1fU);

    if ((mmio_read32((uint32_t)(host_base + 0x58U)) & mask) != 0U) {
        mmio_write32((uint32_t)(host_base + 0x5cU), mask);
    }

    while ((mmio_read32((uint32_t)(host_base + 0x58U)) & mask) != 0U) {
    }
}

uint32_t ceil_div_0x1000(uint64_t number)
{
    uint32_t ret = (uint32_t)(number >> 12U);

    if ((number & 0xfffULL) != 0ULL) {
        ret++;
    }

    return ret;
}

void usb_init_descriptors(uint32_t host)
{
    uintptr_t base;
    uint64_t counter;

    if (call_get_boot_flow_mode_word() != 0U) {
        call_dat_020200cc(2);
        return;
    }

    base = ufs_host_base(host);

    mmio_write32((uint32_t)(base + 0x410cU), 0x10U);

    for (counter = 0; counter != 0x1000ULL; counter += 0x800ULL) {
        uintptr_t l = base + 0x4accU + counter;

        mmio_write32((uint32_t)(l - 0x128U), 0x1bU);
        mmio_write32((uint32_t)(l + 0x0f4U), 0x20U);
        mmio_write32((uint32_t)(l + 0x0b8U), 0x40U);
        mmio_write32((uint32_t)(l + 0x0e8U), 0x21U);
        mmio_write32((uint32_t)(base + 0x4018U), 0x10U);
        mmio_write32((uint32_t)(base + 0x4accU + counter), 4U);
        mmio_write32((uint32_t)(l + 0x0010U), 0x15U);
        mmio_write32((uint32_t)(l + 0x0020U), 9U);
        mmio_write32((uint32_t)(l - 0x0200U), 0U);
        mmio_write32((uint32_t)(l - 0x0134U), 0x1bU);
        mmio_write32((uint32_t)(l - 0x0130U), 0x32U);
        mmio_write32((uint32_t)(l - 0x0128U), 0x1bU);
        mmio_write32((uint32_t)(l - 0x0124U), 0xa0U);
        mmio_write32((uint32_t)(l - 0x01ecU), 0x13U);
        mmio_write32((uint32_t)(l - 0x01e4U), 1U);
        mmio_write32((uint32_t)(l - 0x01d8U), 1U);
        mmio_write32((uint32_t)(l - 0x01d0U), 0x13U);
        mmio_write32((uint32_t)(l - 0x0154U), 0x3fU);
        mmio_write32((uint32_t)(l - 0x0150U), 0xffU);
        mmio_write32((uint32_t)(l - 0x0114U), 0x5eU);
        mmio_write32((uint32_t)(l - 0x0110U), 0x70U);
        mmio_write32((uint32_t)(l - 0x013cU), 0x4eU);
    }

    mmio_write32((uint32_t)(base + 0x410cU), 0x18U);
    mmio_write32((uint32_t)(base + 0x410cU), 0U);

    call_delay_wait(0, 200);
}

uint32_t load_auth_epbl(void)
{
    uint32_t ret;
    uint8_t head_valid;
    uintptr_t load_addr;

    call_usb_send(s_ready_to_rx_epbl, 0x11U);

    load_addr = (uintptr_t)get_epbl_load_addr();
    ret = call_usb_receive(load_addr, EPBL_LIMIT_ADDR - get_epbl_load_addr());

    if ((ret & 0xffU) == 1U) {
        call_usb_send(s_epbl_rx_done, 0x0dU);
        head_valid = (uint8_t)check_epbl_head();

        if (head_valid == 0U) {
            call_usb_send(s_epbl_header_fail, 0x11U);
            return 0;
        }

        call_usb_send(s_epbl_header_pass, 0x11U);
        return 1;
    }

    call_usb_send(s_epbl_rx_fail, 0x0dU);
    return ret;
}

uint64_t try_load_epbl_via_bootrom_or_usb(void)
{
    uint32_t boot_flow_mode;
    uintptr_t bootrom_setup_and_jump_bl1_func;
    uint8_t usb_ready;
    uint8_t epbl_receive_valid;
    uint32_t epbl_magic_valid;
    uint8_t epbl_head_valid;

    boot_flow_mode = call_get_boot_flow_mode_word();
    if (boot_flow_mode == 1U) {
#if defined(__aarch64__)
        __asm__ volatile("ic ialluis" ::: "memory");
#endif
        bootrom_setup_and_jump_bl1_func = call_get_bootrom_api_ptr_by_index(6);
        return ((uint64_t(*)(uint32_t))bootrom_setup_and_jump_bl1_func)(get_bootdev_id());
    }

    (void)call_get_boot_flow_mode_word();
    call_dat_020200b0();

    call_usb_send(s_ready_to_rx_bl1, 0x10U);
    usb_ready = call_usb_enumerate_and_wait_ready(0x02021400U, 300U, 0U);
    if (usb_ready == 0U) {
        return 0;
    }

    epbl_receive_valid = (uint8_t)call_usb_receive((uintptr_t)get_epbl_load_addr(),
                                                   EPBL_LIMIT_ADDR - get_epbl_load_addr());
    if (epbl_receive_valid == 0U) {
        call_usb_send(s_epbl_rx_fail_upper, 0x0dU);
        return 0;
    }

    epbl_magic_valid = check_epbl_magic();
    if (epbl_magic_valid == 0U) {
        call_usb_send(s_ready_to_rx_epbl, 0x11U);
        epbl_magic_valid = call_usb_receive((uintptr_t)get_epbl_load_addr(),
                                            EPBL_LIMIT_ADDR - get_epbl_load_addr());
    }

    call_usb_send(s_epbl_rx_done, 0x0dU);

    if ((epbl_magic_valid & 0xffU) == 1U) {
        epbl_head_valid = (uint8_t)check_epbl_head();
        if (epbl_head_valid != 0U) {
            call_usb_send(s_epbl_header_pass_upper, 0x11U);
        } else {
            call_usb_send(s_epbl_header_fail_upper, 0x11U);
        }
    } else {
        call_usb_send(s_epbl_rx_fail, 0x0dU);
    }

    return 1;
}

bool try_load_epbl_from_mmc(void)
{
    uint32_t bl1_size_bytes;
    uint32_t base_lba;
    uint8_t epbl_body_read_success;
    uintptr_t epbl_load_addr;
    uint64_t first_usable_lba;
    uint32_t epbl_size_bytes;

    epbl_load_addr = (uintptr_t)get_epbl_load_addr();
    bl1_size_bytes = call_get_bl1_expected_sz();

    epbl_body_read_success = call_mmc_read_blocks_parent(1, 1, epbl_load_addr);
    if (epbl_body_read_success == 0U) {
        return false;
    }

    first_usable_lba = *(uint64_t *)epbl_load_addr == 0x5452415020494645ULL
                           ? *(uint64_t *)(epbl_load_addr + 0x28U)
                           : 0ULL;

    base_lba = (first_usable_lba == 0ULL) ? 1U : (uint32_t)first_usable_lba;
    base_lba += (bl1_size_bytes >> 9U);

    epbl_body_read_success = call_mmc_read_blocks_parent(base_lba, 1, epbl_load_addr);
    if (epbl_body_read_success == 0U) {
        return false;
    }

    if (check_epbl_head() == 0U) {
        return false;
    }

    epbl_size_bytes = get_epbl_expected_sz_bytes();
    epbl_body_read_success =
        call_mmc_read_blocks_parent(base_lba + 1U, (epbl_size_bytes >> 9U) - 1U, epbl_load_addr + 0x200U);

    return epbl_body_read_success == 1U;
}

uint32_t try_load_epbl_from_ufs(uint32_t host)
{
    uint32_t uVar1;
    uint32_t uVar2;
    uint8_t ufs_ready;
    uint8_t epbl_block_read_done;
    uint8_t epbl_head_valid;
    uint8_t ufs_probe_done;
    uint8_t ufs_ready_check_done;
    uint8_t ufs_probe_2;
    bool counter_less_2_cond;
    uint32_t host_ready;
    uint32_t iVar3;
    uint32_t epbl_load_addr1;
    uintptr_t epbl_load_addr0;
    uintptr_t epbl_load_addr;
    uint32_t uVar4;
    uint8_t (*block_read_func)(uint32_t, uint32_t, uint32_t, uintptr_t, uint32_t, uint32_t,
                               uint32_t);
    uint32_t counter1;
    uint32_t iVar5;
    uint32_t iVar6;
    uint32_t iVar7;
    uint32_t counter;

    update_epbl_progress_word(0, 0);
    host_ready = host_issue_cmd_and_wait(host, 0x1571U, 0);
    update_epbl_progress_word(1, host_ready);

    if (host_ready != 0x11U) {
        ufs_ready = call_ufs_ready_check(1, 1, 1, 1, 2);
        if (ufs_ready == 0U) {
            update_epbl_progress_word(9, 3);
            counter = 1;
            call_delay_wait(1, 10);
            call_dat_020200f8();
            usb_init_descriptors(0);
            epbl_head_valid = call_dat_020200fc();
            do {
                if (epbl_head_valid == 1U) {
                    ufs_probe_done = call_ufs_link_startup_and_probe();
                    if (ufs_probe_done != 1U) {
                        epbl_load_addr0 = 0x0bU;
                        goto LAB_00000960;
                    }
                    ufs_ready_check_done = call_ufs_ready_check(1, 1, 1, 1, 2);
                    if (ufs_ready_check_done == 1U)
                        goto LAB_00000684;
                } else {
                    epbl_load_addr0 = 10U;
LAB_00000960:
                    update_epbl_progress_word((uint32_t)epbl_load_addr0, 1);
                }
                call_delay_wait(1, 10);
                call_dat_020200f8();
                usb_init_descriptors(0);
                epbl_head_valid = call_dat_020200fc();
                counter_less_2_cond = counter < 2U;
                counter = 2;
            } while (counter_less_2_cond);

            if (epbl_head_valid != 1U) {
                return 0;
            }

            ufs_probe_2 = call_ufs_link_startup_and_probe();
            if (ufs_probe_2 == 0U) {
                return 0;
            }
        } else {
            update_epbl_progress_word(9, 1);
        }
    }

LAB_00000684:
    iVar3 = ceil_div_0x1000(call_get_bl1_expected_sz());
    host_ready = 1;

    do {
        update_epbl_progress_word(6, host_ready);
        block_read_func = call_ufs_read_blocks_wrapper;
        epbl_load_addr0 = (uintptr_t)get_epbl_load_addr();
        epbl_block_read_done = (*block_read_func)(0, 0, 0xb0U, epbl_load_addr0, iVar3, 1, 0x1000U);

        if (epbl_block_read_done == 0U) {
            if (host_ready == 3U) {
                epbl_load_addr0 = 4U;
                goto LAB_000009c8;
            }
            host_clear_wait_status_bit(host, 0);
        } else {
            update_epbl_progress_word(4, 1);
            update_epbl_progress_word(8, 1);
            update_epbl_progress_word(3, host_ready);

            epbl_head_valid = (uint8_t)check_epbl_head();
            if (epbl_head_valid != 0U) {
                counter = 1;
                update_epbl_progress_word(2, 1);
                host_ready = ceil_div_0x1000(get_epbl_expected_sz_bytes());
                uVar1 = host_ready - 1U;
                uVar2 = uVar1 % 5U;
                iVar5 = 1;
                goto LAB_00000774;
            }

            if (host_ready == 3U)
                break;
        }

        host_ready = host_ready + 1U;
    } while (true);

    epbl_load_addr0 = 2U;
LAB_000009c8:
    uVar4 = 3;
LAB_000009cc:
    update_epbl_progress_word((uint32_t)epbl_load_addr0, uVar4);
    return 0;

LAB_00000774:
    do {
        if (uVar1 < 6U) {
            update_epbl_progress_word(7, counter);
            block_read_func = call_ufs_read_blocks_wrapper;
            epbl_load_addr = (uintptr_t)get_epbl_load_addr();
            epbl_head_valid =
                (*block_read_func)(0, iVar5, 0xb0U, epbl_load_addr + 0x1000U, iVar3 + 1U, uVar1,
                                  0x1000U);
            iVar6 = iVar5;
            if (epbl_head_valid != 0U) {
LAB_000009f8:
                update_epbl_progress_word(8, host_ready);
                break;
            }

        joined_r0x00000874:
            if (counter == 3U) {
                epbl_load_addr0 = 5U;
                goto LAB_000009c8;
            }
            host_clear_wait_status_bit(host, 0);
        } else {
            epbl_load_addr1 = get_epbl_load_addr() + 0x1000U;
            update_epbl_progress_word(7, counter);
            counter1 = 0;
            iVar7 = 6;

            do {
                iVar6 = iVar5 + counter1;
                epbl_head_valid = call_ufs_read_blocks_wrapper(0, iVar6, 0xb0U, epbl_load_addr1,
                                                               iVar3 + iVar7 - 5U, 5U, 0x1000U);
                if (epbl_head_valid == 0U) {
                    if (counter == 3U) {
                        epbl_load_addr0 = 5U;
                        uVar4 = 2;
                        goto LAB_000009cc;
                    }
                    host_clear_wait_status_bit(host, 0);
                    goto LAB_000008ac;
                }

                counter1 = counter1 + 1U;
                update_epbl_progress_word(8, iVar7);
                epbl_load_addr1 = epbl_load_addr1 + 0x5000U;
                iVar7 = iVar7 + 5;
            } while (counter1 < uVar1 / 5U);

            iVar6 = iVar5 + counter1;
            if (uVar2 != 0U) {
                epbl_head_valid = call_ufs_read_blocks_wrapper(0, iVar6, 0xb0U, epbl_load_addr1,
                                                               iVar3 + iVar7 - 5U, uVar2, 0x1000U);
                if (epbl_head_valid != 0U) {
                    host_ready = uVar2 + (uVar1 / 5U) * 5U + 1U;
                    goto LAB_000009f8;
                }
                goto joined_r0x00000874;
            }
        }

    LAB_000008ac:
        counter = counter + 1U;
        iVar5 = iVar6;
    } while (counter < 4U);

    update_epbl_progress_word(5, 1);
    return 1;
}

uint32_t try_load_epbl_primary_source(void)
{
    uint32_t boot_source;
    uint32_t boot_dev_id;
    uint32_t task_valid;

    bootlog_store_elapsed_ticks(IRAM_BOOTLOG_PRIMARY_START);

    boot_source = get_boot_source_from_boot_device_word();
    if (boot_source == 0x0bU) {
        set_boot_device_id(4);
    }

    boot_dev_id = get_bootdev_id();
    call_set_registers((boot_dev_id == 1U) ? 0U : 1U, (boot_dev_id == 1U) ? 0x02000000U : 0x01000000U);

    switch (boot_source) {
    case 1:
        task_valid = try_load_epbl_from_ufs(0);
        break;
    case 2:
        task_valid = 0;
        break;
    case 4:
    case 0x0b:
        task_valid = load_auth_epbl();
        break;
    case 5:
        task_valid = try_load_epbl_from_mmc();
        break;
    case 6:
        task_valid = try_load_epbl_from_ufs(1);
        break;
    default:
        bootlog_store_elapsed_ticks(IRAM_BOOTLOG_PRIMARY_END);
        return 0;
    }

    bootlog_store_elapsed_ticks(IRAM_BOOTLOG_PRIMARY_END);

    if ((task_valid & 0xffU) == 1U) {
        call_set_registers(0, 0x04000000U);
        return 1;
    }

    return task_valid;
}

uint32_t try_load_epbl_fallback_source(void)
{
    uint32_t boot_source = get_boot_source_from_boot_device_word();
    uint32_t boot_dev_id = get_bootdev_id();
    uint32_t load_success;

    call_set_registers((boot_dev_id == 1U) ? 0U : 1U, (boot_dev_id == 1U) ? 0x02000000U : 0x01000000U);

    if (boot_source == 4U) {
        load_success = (uint32_t)try_load_epbl_via_bootrom_or_usb();
        if ((load_success & 0xffU) == 1U) {
            call_set_registers(1, 0x02000000U);
            return 1;
        }
        return load_success;
    }

    return 0;
}
