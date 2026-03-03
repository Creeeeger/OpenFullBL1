#pragma once

#include <stdint.h>

void set_ps_hold(void);

uint32_t read_pmu_cfg(void);
uint32_t pmu_cfg_bit24(void);
uint32_t pmu_cfg_bit16(void);

void set_soc_reg_1586030c_bootsrc4_mode(void);
void set_soc_reg_15861240_bit2(uint8_t clear_bit2);
