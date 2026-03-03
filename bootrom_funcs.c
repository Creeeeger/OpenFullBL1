#include <stdint.h>

#include <bootrom_funcs.h>
#include <fwbl1_map.h>

void call_set_registers(uint32_t selector, uint32_t bits)
{
    ((void (*)(uint32_t, uint32_t))(uintptr_t)mmio_read32(IRAM_PTR_SET_REGISTERS))(selector, bits);
}

void call_usb_send(const char *addr, uint32_t len)
{
    ((void (*)(const char *, uint32_t))(uintptr_t)mmio_read32(IRAM_PTR_USB_SEND))(addr, len);
}

uint32_t call_usb_receive(uintptr_t addr, uint32_t max_size)
{
    return ((uint32_t(*)(uintptr_t, uint32_t))(uintptr_t)mmio_read32(IRAM_PTR_USB_RECEIVE))(addr,
                                                                                             max_size);
}

uint8_t call_usb_enumerate_and_wait_ready(uint32_t ctx, uint32_t timeout, uint32_t flags)
{
    return (uint8_t)((uint32_t(*)(uint32_t, uint32_t, uint32_t))(uintptr_t)
                     mmio_read32(IRAM_PTR_USB_ENUMERATE_AND_WAIT_READY))(ctx, timeout, flags);
}

uint32_t call_get_bl1_expected_sz(void)
{
    return ((uint32_t(*)(void))(uintptr_t)mmio_read32(IRAM_PTR_GET_BL1_EXPECTED_SZ))();
}

uint8_t call_mmc_read_blocks_parent(uint32_t lba, uint32_t count, uintptr_t dst)
{
    return (uint8_t)((uint32_t(*)(uint32_t, uint32_t, uintptr_t))(uintptr_t)
                     mmio_read32(IRAM_PTR_MMC_READ_BLOCKS_PARENT))(lba, count, dst);
}

uint32_t call_get_boot_flow_mode_word(void)
{
    return ((uint32_t(*)(void))(uintptr_t)mmio_read32(IRAM_PTR_GET_BOOT_FLOW_MODE_WORD))();
}

uintptr_t call_get_bootrom_api_ptr_by_index(uint32_t idx)
{
    return ((uintptr_t(*)(uint32_t))(uintptr_t)mmio_read32(IRAM_PTR_GET_BOOTROM_API_PTR_BY_INDEX))(idx);
}

void call_dat_020200b0(void)
{
    ((void (*)(void))(uintptr_t)mmio_read32(IRAM_PTR_DAT_020200B0))();
}

void call_dat_020200cc(uint32_t arg)
{
    ((void (*)(uint32_t))(uintptr_t)mmio_read32(IRAM_PTR_DAT_020200CC))(arg);
}

void call_dat_020200f8(void)
{
    ((void (*)(void))(uintptr_t)mmio_read32(IRAM_PTR_DAT_020200F8))();
}

uint8_t call_dat_020200fc(void)
{
    return (uint8_t)((uint32_t(*)(void))(uintptr_t)mmio_read32(IRAM_PTR_DAT_020200FC))();
}

uint8_t call_ufs_link_startup_and_probe(void)
{
    return (uint8_t)((uint32_t(*)(void))(uintptr_t)mmio_read32(IRAM_PTR_UFS_LINK_STARTUP_AND_PROBE))();
}

uint8_t call_ufs_ready_check(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
{
    return (uint8_t)((uint32_t(*)(uint32_t, uint32_t, uint32_t, uint32_t,
                                  uint32_t))(uintptr_t)mmio_read32(IRAM_PTR_UFS_READY_CHECK))(a0,
                                                                                               a1,
                                                                                               a2,
                                                                                               a3,
                                                                                               a4);
}

uint8_t call_ufs_read_blocks_wrapper(uint32_t host, uint32_t slot, uint32_t lun, uintptr_t dst,
                                     uint32_t lba, uint32_t count, uint32_t blk_sz)
{
    return (uint8_t)((uint32_t(*)(uint32_t, uint32_t, uint32_t, uintptr_t, uint32_t, uint32_t,
                                  uint32_t))(uintptr_t)mmio_read32(IRAM_PTR_UFS_READ_BLOCKS_WRAPPER))(
        host, slot, lun, dst, lba, count, blk_sz);
}

void call_delay_wait(uint32_t a0, uint32_t a1)
{
    ((void (*)(uint32_t, uint32_t))(uintptr_t)mmio_read32(IRAM_PTR_DELAY_WAIT))(a0, a1);
}

void call_calc_hash(uintptr_t out, uintptr_t data, uint32_t len, uint32_t mode)
{
    ((void (*)(uintptr_t, uintptr_t, uint32_t, uint32_t))(uintptr_t)
         mmio_read32(IRAM_PTR_CALC_HASH_FUNC))(out, data, len, mode);
}

void call_bootrom_panic(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
{
    ((void (*)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t))(uintptr_t)
         mmio_read32(IRAM_PTR_BOOTROM_PANIC))(a0, a1, a2, a3, a4);
}

uint32_t call_secure_auth_mode_word(void)
{
    return ((uint32_t(*)(void))(uintptr_t)mmio_read32(IRAM_PTR_SECURE_AUTH_MODE_WORD))();
}
