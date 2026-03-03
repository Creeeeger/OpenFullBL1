#include <stdint.h>

#include <bootdev.h>
#include <fwbl1_map.h>

static uint32_t get_bootdev_table_base(void)
{
    return (get_bootdev_id() == 1U) ? IRAM_BOOTDEV_TABLE_USB : IRAM_BOOTDEV_TABLE_OTHER;
}

static uint32_t bit_mask(uint32_t width)
{
    return (1U << width) - 1U;
}

static uint32_t insert_bits(uint32_t original, uint32_t value, uint32_t lsb, uint32_t width)
{
    uint32_t mask = bit_mask(width) << lsb;
    return (original & ~mask) | ((value & bit_mask(width)) << lsb);
}

uint32_t get_boot_source_from_boot_device_word(void)
{
    uint32_t boot_device = mmio_read32(IRAM_BOOT_DEVICE_WORD);

    if ((boot_device >> 24U) != 0xcbU) {
        return 0;
    }

    return (boot_device >> ((boot_device & 7U) << 2U)) & 0xfU;
}

uint32_t get_bootdev_id(void)
{
    return mmio_read32(IRAM_BOOT_DEVICE_WORD) & 0xfU;
}

void set_boot_device_id(uint32_t attempt)
{
    uint32_t bootdev_id = get_bootdev_id();
    uint32_t boot_device = mmio_read32(IRAM_BOOT_DEVICE_WORD);
    uint32_t shift = (bootdev_id << 2U) & 0x1fU;

    boot_device &= ~(0xfU << shift);
    boot_device |= (attempt & 0xfU) << shift;

    mmio_write32(IRAM_BOOT_DEVICE_WORD, boot_device);
}

void set_bootdev_id_low_nibble(uint32_t bootdev_id)
{
    uint32_t boot_device = mmio_read32(IRAM_BOOT_DEVICE_WORD);
    boot_device = (boot_device & 0xfffffff0U) | (bootdev_id & 0xfU);
    mmio_write32(IRAM_BOOT_DEVICE_WORD, boot_device);
}

uint32_t get_bootdev_table_entry(uint32_t idx)
{
    if (idx > 7U) {
        return 0;
    }

    return mmio_read32(get_bootdev_table_base() + (idx << 2U));
}

void set_bootdev_table_entry(uint32_t idx, uint32_t value)
{
    if (idx < 8U) {
        mmio_write32(get_bootdev_table_base() + (idx << 2U), value);
    }
}

void update_epbl_progress_word(uint32_t field_id, uint32_t value)
{
    uint32_t table_entry = get_bootdev_table_entry(7);

    switch (field_id) {
    case 0:
        set_bootdev_table_entry(7, 0);
        return;
    case 1:
        table_entry = (table_entry & 0x80ffffffU) | (value << 24U);
        break;
    case 2:
        table_entry = insert_bits(table_entry, value, 16U, 2U);
        break;
    case 3:
        table_entry = insert_bits(table_entry, value, 18U, 2U);
        break;
    case 4:
        table_entry = insert_bits(table_entry, value, 20U, 2U);
        break;
    case 5:
        table_entry = insert_bits(table_entry, value, 12U, 2U);
        break;
    case 6:
        table_entry = insert_bits(table_entry, value, 22U, 2U);
        break;
    case 7:
        table_entry = insert_bits(table_entry, value, 14U, 2U);
        break;
    case 8:
        table_entry = insert_bits(table_entry, value, 0U, 8U);
        break;
    case 9:
        table_entry = insert_bits(table_entry, value, 10U, 2U);
        break;
    case 10:
        table_entry = insert_bits(table_entry, value, 9U, 1U);
        break;
    case 11:
        table_entry = insert_bits(table_entry, value, 8U, 1U);
        break;
    default:
        return;
    }

    set_bootdev_table_entry(7, table_entry);
}
