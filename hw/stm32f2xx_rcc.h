#include "sysbus.h"
#include "clktree.h"
#include "stm32f2xx.h"
#include "stm32_rcc.h"

typedef struct Stm32f2xxRcc {
    /* Inherited */
    union {
        Stm32Rcc inherited;
        struct {
            /* Inherited */
            SysBusDevice busdev;

            /* Properties */
            uint32_t osc_freq;
            uint32_t osc32_freq;

            /* Private */
            MemoryRegion iomem;
            qemu_irq irq;
        };
    };
    
    /* Peripheral clocks */
    Clk PERIPHCLK[STM32F2XX_PERIPH_COUNT], // MUST be first field after `inherited`, because Stm32Rcc's last field aliases this array
    HSICLK,
    HSECLK,
    LSECLK,
    LSICLK,
    SYSCLK,
    IWDGCLK,
    RTCCLK,
    PLLM, /* Applies "M" division and "N" multiplication factors. When adding support for PLLI2S, this will need to be separated. */
    PLLCLK,
    PLL48CLK,
    PLLI2S,
    HCLK, /* Output from AHB Prescaler */
    PCLK1, /* Output from APB1 Prescaler */
    PCLK2; /* Output from APB2 Prescaler */

    /* Register Values */
    uint32_t
    RCC_CIR,
    RCC_APB1ENR,
    RCC_APB2ENR;

    /* Register Field Values */
    uint32_t
    RCC_CFGR_PPRE1,
    RCC_CFGR_PPRE2,
    RCC_CFGR_HPRE,
    RCC_AHB1ENR,
    RCC_CFGR_SW;

    uint8_t
    RCC_PLLCFGR_PLLM,
    RCC_PLLCFGR_PLLP,
    RCC_PLLCFGR_PLLSRC;

    uint16_t
    RCC_PLLCFGR_PLLN;

} Stm32f2xxRcc;
