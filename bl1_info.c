#include <stdint.h>

#include <bl1_info.h>
#include <fwbl1_map.h>

uint32_t get_bl1_sz(void)
{
    return mmio_read32(IRAM_BL1_SIZE);
}

uint32_t get_bl1_size(void)
{
    return get_bl1_sz();
}
