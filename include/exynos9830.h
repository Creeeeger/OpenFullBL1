#pragma once

#define IRAM_BASE                           (0x02020000)
#define IRAM_UNK1                           (IRAM_BASE + 0x002C)
#define IRAM_BL1_SIZE                       (IRAM_BASE + 0x0030)
#define PTR_GRAB_BL1_BOOT_FLAG              (IRAM_BASE + 0x00A8) // Not sure, send help
#define PTR_SET_STATUS_BIT                  (IRAM_BASE + 0x00A8) // Not exactly sure, send help
#define PTR_USB_SEND                        (IRAM_BASE + 0x00D0)
#define PTR_USB_RECEIVE                     (IRAM_BASE + 0x00DC)
#define IRAM_EPBL_SIZE                      (IRAM_BASE + 0x0120)
#define IRAM_EPBL_EXPECTED_HASH             (IRAM_BASE + 0x0124)
#define PTR_EPBL_LOAD_ADDRESS               (IRAM_BASE + 0x0128)
#define IRAM_UNK2                           (IRAM_BASE + 0x0130)
#define IRAM_UNK3                           (IRAM_BASE + 0x0134)
#define IRAM_UNK4                           (IRAM_BASE + 0x0138)
#define IRAM_UNK5                           (IRAM_BASE + 0x013C)

#define EXYNOS9830_POWER_BASE               (0x15860000)
#define EXYNOS9830_POWER_PS_HOLD_CONTROL    (EXYNOS9830_POWER_BASE + 0x030C)