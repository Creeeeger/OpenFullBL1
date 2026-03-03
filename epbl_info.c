#include <stdint.h>

#include <bl1_info.h>
#include <bootdev.h>
#include <epbl_info.h>
#include <fwbl1_map.h>

uint32_t get_epbl_expected_hash(void)
{
    return mmio_read32(IRAM_EPBL_EXPECTED_HASH);
}

uint32_t get_epbl_expected_sz_bytes(void)
{
    return mmio_read32(IRAM_EPBL_EXPECTED_SZ_BYTES);
}

uint32_t get_epbl_load_addr(void)
{
    return mmio_read32(IRAM_EPBL_LOAD_ADDR);
}

uint32_t get_epbl_footer_base(void)
{
    return get_epbl_load_addr() + get_epbl_expected_sz_bytes() - 0x210U;
}

void set_epbl_expected_hash(uint32_t hash)
{
    mmio_write32(IRAM_EPBL_EXPECTED_HASH, hash);
}

void set_epbl_expected_sz(uint32_t sz)
{
    mmio_write32(IRAM_EPBL_EXPECTED_SZ_BYTES, sz);
}

void set_epbl_load_addr(void)
{
    mmio_write32(IRAM_EPBL_LOAD_ADDR, BL1_IMAGE_BASE + get_bl1_sz() + BL1_EPBL_LOAD_OFFSET);
}

void set_epbl_magic_sticky_flag_if_bootdev1(void)
{
    if (mmio_read32(get_epbl_load_addr() + 0x14U) == EPBL_MAGIC_STICKY_WORD) {
        if (get_bootdev_id() == 1U) {
            mmio_write32(REG_158609BC, mmio_read32(REG_158609BC) | 1U);
        }
    }
}
