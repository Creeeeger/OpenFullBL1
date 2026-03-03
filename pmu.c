#include <stdint.h>

#include <fwbl1_map.h>
#include <pmu.h>

void set_ps_hold(void)
{
    mmio_write32(REG_1586030C, 0x1300U);
}

uint32_t read_pmu_cfg(void)
{
    return mmio_read32(REG_PMU_CFG);
}

uint32_t pmu_cfg_bit24(void)
{
    return (read_pmu_cfg() >> 24U) & 1U;
}

uint32_t pmu_cfg_bit16(void)
{
    return (read_pmu_cfg() >> 16U) & 1U;
}

void set_soc_reg_1586030c_bootsrc4_mode(void)
{
    mmio_write32(REG_1586030C, 0x1300U);
}

void set_soc_reg_15861240_bit2(uint8_t clear_bit2)
{
    uint32_t value = mmio_read32(REG_15861240);

    if (clear_bit2 == 1U) {
        value &= ~0x4U;
    } else {
        value |= 0x4U;
    }

    mmio_write32(REG_15861240, value);
}
