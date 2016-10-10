/*
 *******************************************************************************
 *
 * Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */

/**
 *******************************************************************************
 *   \file  ndk_nsp_hooks.c
 *
 *   \brief Do all necessary board level initialization for NDK.
 *
 *******************************************************************************
 */


/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */

/* Standard language headers */
#include <stddef.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>

/* OS/Posix headers */

/* NDK Dependencies */
#include <ti/ndk/inc/netmain.h>
#include <ti/ndk/inc/tools/servers.h>
#include <ti/ndk/inc/tools/console.h>

/* NSP Dependencies */
#include <ti/nsp/drv/inc/gmacsw.h>
#include <ti/nsp/drv/inc/gmacsw_config.h>

/* Project dependency headers */
#include <src/utils_common/include/utils.h>
#include <include/link_api/systemLink_common.h>
#include <include/link_api/networkCtrl_api.h>

#include <soc_defines.h>
#include <platform.h>

/*******************************************************************************
 *  Defines
 *******************************************************************************
 */

#define NET_IF_IDX  (1)

#define PAB_MII (0)
#define PAB_RMII (0)


/* Ethernet MAC ID registers(Devcice configuration) from EFuse */
#define MAC_ID0_LO              (*(volatile uint32_t*)0x4A002514)
#define MAC_ID0_HI              (*(volatile uint32_t*)0x4A002518)
#define MAC_ID1_LO              (*(volatile uint32_t*)0x4A00251C)
#define MAC_ID1_HI              (*(volatile uint32_t*)0x4A002520)

/* I/O Delay related registers */
#define CFG_IO_DELAY_UNLOCK_KEY     (0x0000AAAA)
#define CFG_IO_DELAY_LOCK_KEY       (0x0000AAAB)

#define CFG_IO_DELAY_ACCESS_PATTERN (0x00029000)
#define CFG_IO_DELAY_LOCK_MASK      (0x400)

#define CFG_IO_DELAY_BASE           (0x4844A000)
#define CFG_IO_DELAY_LOCK           (*(volatile uint32_t*)(CFG_IO_DELAY_BASE + 0x02C))
#define CFG_RGMII0_TXCTL_OUT        (*(volatile uint32_t*)(CFG_IO_DELAY_BASE + 0x74C))
#define CFG_RGMII0_TXD0_OUT         (*(volatile uint32_t*)(CFG_IO_DELAY_BASE + 0x758))
#define CFG_RGMII0_TXD1_OUT         (*(volatile uint32_t*)(CFG_IO_DELAY_BASE + 0x764))
#define CFG_RGMII0_TXD2_OUT         (*(volatile uint32_t*)(CFG_IO_DELAY_BASE + 0x770))
#define CFG_RGMII0_TXD3_OUT         (*(volatile uint32_t*)(CFG_IO_DELAY_BASE + 0x77C))
#define CFG_VIN2A_D13_OUT           (*(volatile uint32_t*)(CFG_IO_DELAY_BASE + 0xA7C))
#define CFG_VIN2A_D17_OUT           (*(volatile uint32_t*)(CFG_IO_DELAY_BASE + 0xAAC))
#define CFG_VIN2A_D16_OUT           (*(volatile uint32_t*)(CFG_IO_DELAY_BASE + 0xAA0))
#define CFG_VIN2A_D15_OUT           (*(volatile uint32_t*)(CFG_IO_DELAY_BASE + 0xA94))
#define CFG_VIN2A_D14_OUT           (*(volatile uint32_t*)(CFG_IO_DELAY_BASE + 0xA88))

/* PAD Configuration Registers */
#define SYSCFG_PAD_RGMII0_TXCTL     (*(volatile uint32_t*)(0x4A003654))
#define SYSCFG_PAD_RGMII0_TXD3      (*(volatile uint32_t*)(0x4A003658))
#define SYSCFG_PAD_RGMII0_TXD2      (*(volatile uint32_t*)(0x4A00365C))
#define SYSCFG_PAD_RGMII0_TXD1      (*(volatile uint32_t*)(0x4A003660))
#define SYSCFG_PAD_RGMII0_TXD0      (*(volatile uint32_t*)(0x4A003664))
#define SYSCFG_PAD_VIN2A_D13        (*(volatile uint32_t*)(0x4A00359C))
#define SYSCFG_PAD_VIN2A_D14        (*(volatile uint32_t*)(0x4A0035A0))
#define SYSCFG_PAD_VIN2A_D15        (*(volatile uint32_t*)(0x4A0035A4))
#define SYSCFG_PAD_VIN2A_D16        (*(volatile uint32_t*)(0x4A0035A8))
#define SYSCFG_PAD_VIN2A_D17        (*(volatile uint32_t*)(0x4A0035AC))

#ifdef TDA2EX_BUILD
/* DP83867IR Register details for delay configuration */
#define DP83867_CTRL         (0x1FU)
#define DP83867_RGMIICTL     (0x0032U)
#define DP83867_RGMIIDCTL    (0x0086U)
#define DP83867_IO_MUX_CTRL  (0x0170U)
/* PHY CTRL bits */
#define DP83867_SW_RESET    (15)
#define DP83867_SW_RESTART  (14)
#define DP83867_PHYCR_FIFO_DEPTH_3_B_NIB    (0x00U)
#define DP83867_PHYCR_FIFO_DEPTH_4_B_NIB    (0x01U)
#define DP83867_PHYCR_FIFO_DEPTH_6_B_NIB    (0x02U)
#define DP83867_PHYCR_FIFO_DEPTH_8_B_NIB    (0x03U)
/* RGMIIDCTL internal delay for rx and tx */
#define DP83867_RGMIIDCTL_250_PS     (0x0U)
#define DP83867_RGMIIDCTL_500_PS     (0x1U)
#define DP83867_RGMIIDCTL_750_PS     (0x2U)
#define DP83867_RGMIIDCTL_1_NS       (0x3U)
#define DP83867_RGMIIDCTL_1_25_NS    (0x4U)
#define DP83867_RGMIIDCTL_1_50_NS    (0x5U)
#define DP83867_RGMIIDCTL_1_75_NS    (0x6U)
#define DP83867_RGMIIDCTL_2_00_NS    (0x7U)
#define DP83867_RGMIIDCTL_2_25_NS    (0x8U)
#define DP83867_RGMIIDCTL_2_50_NS    (0x9U)
#define DP83867_RGMIIDCTL_2_75_NS    (0xaU)
#define DP83867_RGMIIDCTL_3_00_NS    (0xbU)
#define DP83867_RGMIIDCTL_3_25_NS    (0xcU)
#define DP83867_RGMIIDCTL_3_50_NS    (0xdU)
#define DP83867_RGMIIDCTL_3_75_NS    (0xeU)
#define DP83867_RGMIIDCTL_4_00_NS    (0xfU)
#endif

#ifdef BOARD_TYPE_TDA3XX_RVP
    /* DP83867 */
    #define DP83867_RGMIICTL        (0x32U)
    #define DP83867_RGMIIDCTL       (0x86U)
#endif

/*******************************************************************************
 *  Function's
 *******************************************************************************
 */
static void LOCAL_linkStatus( uint32_t phy, uint32_t linkStatus );

/*******************************************************************************
 *  Global's
 *******************************************************************************
 */

/* This string array corresponds to link state */
static char *LinkStr[] = { "No Link",
                           "None",
                           "10Mb/s Half Duplex",
                           "10Mb/s Full Duplex",
                           "100Mb/s Half Duplex",
                           "100Mb/s Full Duplex",
                           "1000Mb/s Half Duplex", /*not suported*/
                           "1000Mb/s Full Duplex"};


/**
 *******************************************************************************
 *
 * \brief HW specific initialization
 *
 *        We changed our CFG file to point call this private init
 *        function. Here we initialize our board and read in our
 *        MAC address.
 *
 *******************************************************************************
 */
void NDK_NSP_Init( void )
{
#if (defined(TDA2XX_FAMILY_BUILD) && (defined(BOARD_TYPE_TDA2XX_EVM) || defined(BOARD_TYPE_TDA2EX_EVM)))
    #if ((PAB_MII != 1) && (PAB_RMII != 1))
    uint32_t disableInternalDelayFlag = (uint32_t)FALSE;
    uint32_t siliconRev = PlatformGetSiliconRev();
    /*
     * We do I/O delay adjustments only if GMAC internal delays are enabled.
     * With TDA2xx & TDA2EX PG2.0 the option of disabling internal delays is
     * added. SBL will disable the internal delays if siliconRev is PG2.0 so no need
     * to adjust I/O delay here.
     */
    /* For TDA2 PG2.0 silicon revision is 2 but for TDA2EX PG2.0 silicon revision is 1 so
     * we need to make decision to disbale delays based on plaform build */
#if (defined (TDA2EX_BUILD))
    if (1U == siliconRev)
    {
    disableInternalDelayFlag = (uint32_t)TRUE;
    }
#else
    if (2U == siliconRev)
    {
    disableInternalDelayFlag = (uint32_t)TRUE;
    }
#endif

    if ((uint32_t)TRUE != disableInternalDelayFlag)
    {
        uint32_t regValue, delta, coarse, fine;

        /*
        * Adjust I/O delays on the Tx control and data lines of each MAC port. This is
        * a workaround in order to work properly with the DP83865 PHYs on the EVM. In 3COM
        * RGMII mode this PHY applies it's own internal clock delay, so we essentially need to
        * counteract the DRA7xx internal delay, and we do this by delaying the control and
        * data lines. If not using this PHY, you probably don't need to do this stuff!
        */

        /* Global unlock for I/O Delay registers */
        CFG_IO_DELAY_LOCK = CFG_IO_DELAY_UNLOCK_KEY;

        /* Tweaks to RGMII0 Tx Control and Data */
        CFG_RGMII0_TXCTL_OUT = (CFG_IO_DELAY_ACCESS_PATTERN & ~CFG_IO_DELAY_LOCK_MASK);
        SYSCFG_PAD_RGMII0_TXCTL = (SYSCFG_PAD_RGMII0_TXCTL & ~0xF) | 0x0;
        delta       = (0x3 << 5) + 0x8;     /* Delay value to add to calibrated value */
        regValue    = CFG_RGMII0_TXCTL_OUT & ~0xFFFFFC00;
        coarse      = ((regValue >> 5) & 0x1F) + ((delta >> 5) & 0x1F);
        coarse      = (coarse > 0x1F) ? (0x1F) : (coarse);
        fine        = (regValue & 0x1F) + (delta & 0x1F);
        fine        = (fine > 0x1F) ? (0x1F) : (fine);
        regValue    = CFG_IO_DELAY_ACCESS_PATTERN | CFG_IO_DELAY_LOCK_MASK | ((coarse << 5) | (fine));
        CFG_RGMII0_TXCTL_OUT = regValue;

        CFG_RGMII0_TXD0_OUT = (CFG_IO_DELAY_ACCESS_PATTERN & ~CFG_IO_DELAY_LOCK_MASK);
        SYSCFG_PAD_RGMII0_TXD0 = (SYSCFG_PAD_RGMII0_TXD0 & ~0xF) | 0x0;
        delta       = (0x3 << 5) + 0x8;     /* Delay value to add to calibrated value */
        regValue    = CFG_RGMII0_TXD0_OUT & ~0xFFFFFC00;
        coarse      = ((regValue >> 5) & 0x1F) + ((delta >> 5) & 0x1F);
        coarse      = (coarse > 0x1F) ? (0x1F) : (coarse);
        fine        = (regValue & 0x1F) + (delta & 0x1F);
        fine        = (fine > 0x1F) ? (0x1F) : (fine);
        regValue    = CFG_IO_DELAY_ACCESS_PATTERN | CFG_IO_DELAY_LOCK_MASK | ((coarse << 5) | (fine));
        CFG_RGMII0_TXD0_OUT = regValue;

        CFG_RGMII0_TXD1_OUT = (CFG_IO_DELAY_ACCESS_PATTERN & ~CFG_IO_DELAY_LOCK_MASK);
        SYSCFG_PAD_RGMII0_TXD1 = (SYSCFG_PAD_RGMII0_TXD1 & ~0xF) | 0x0;
        delta       = (0x3 << 5) + 0x2;     /* Delay value to add to calibrated value */
        regValue    = CFG_RGMII0_TXD1_OUT & ~0xFFFFFC00;
        coarse      = ((regValue >> 5) & 0x1F) + ((delta >> 5) & 0x1F);
        coarse      = (coarse > 0x1F) ? (0x1F) : (coarse);
        fine        = (regValue & 0x1F) + (delta & 0x1F);
        fine        = (fine > 0x1F) ? (0x1F) : (fine);
        regValue    = CFG_IO_DELAY_ACCESS_PATTERN | CFG_IO_DELAY_LOCK_MASK | ((coarse << 5) | (fine));
        CFG_RGMII0_TXD1_OUT = regValue;

        CFG_RGMII0_TXD2_OUT = (CFG_IO_DELAY_ACCESS_PATTERN & ~CFG_IO_DELAY_LOCK_MASK);
        SYSCFG_PAD_RGMII0_TXD2 = (SYSCFG_PAD_RGMII0_TXD2 & ~0xF) | 0x0;
        delta       = (0x4 << 5) + 0x0;     /* Delay value to add to calibrated value */
        regValue    = CFG_RGMII0_TXD2_OUT & ~0xFFFFFC00;
        coarse      = ((regValue >> 5) & 0x1F) + ((delta >> 5) & 0x1F);
        coarse      = (coarse > 0x1F) ? (0x1F) : (coarse);
        fine        = (regValue & 0x1F) + (delta & 0x1F);
        fine        = (fine > 0x1F) ? (0x1F) : (fine);
        regValue    = CFG_IO_DELAY_ACCESS_PATTERN | CFG_IO_DELAY_LOCK_MASK | ((coarse << 5) | (fine));
        CFG_RGMII0_TXD2_OUT = regValue;

        CFG_RGMII0_TXD3_OUT = (CFG_IO_DELAY_ACCESS_PATTERN & ~CFG_IO_DELAY_LOCK_MASK);
        SYSCFG_PAD_RGMII0_TXD3 = (SYSCFG_PAD_RGMII0_TXD3 & ~0xF) | 0x0;
        delta       = (0x4 << 5) + 0x0;     /* Delay value to add to calibrated value */
        regValue    = CFG_RGMII0_TXD3_OUT & ~0xFFFFFC00;
        coarse      = ((regValue >> 5) & 0x1F) + ((delta >> 5) & 0x1F);
        coarse      = (coarse > 0x1F) ? (0x1F) : (coarse);
        fine        = (regValue & 0x1F) + (delta & 0x1F);
        fine        = (fine > 0x1F) ? (0x1F) : (fine);
        regValue    = CFG_IO_DELAY_ACCESS_PATTERN | CFG_IO_DELAY_LOCK_MASK | ((coarse << 5) | (fine));
        CFG_RGMII0_TXD3_OUT = regValue;

        /* Tweaks to RGMII1 Tx Control and Data */
        CFG_VIN2A_D13_OUT = (CFG_IO_DELAY_ACCESS_PATTERN & ~CFG_IO_DELAY_LOCK_MASK);
        SYSCFG_PAD_VIN2A_D13 = (SYSCFG_PAD_VIN2A_D13 & ~0xF) | 0x3;
        delta       = (0x3 << 5) + 0x8;     /* Delay value to add to calibrated value */
        regValue    = CFG_VIN2A_D13_OUT & ~0xFFFFFC00;
        coarse      = ((regValue >> 5) & 0x1F) + ((delta >> 5) & 0x1F);
        coarse      = (coarse > 0x1F) ? (0x1F) : (coarse);
        fine        = (regValue & 0x1F) + (delta & 0x1F);
        fine        = (fine > 0x1F) ? (0x1F) : (fine);
        regValue    = CFG_IO_DELAY_ACCESS_PATTERN | CFG_IO_DELAY_LOCK_MASK | ((coarse << 5) | (fine));
        CFG_VIN2A_D13_OUT = regValue;

        CFG_VIN2A_D17_OUT = (CFG_IO_DELAY_ACCESS_PATTERN & ~CFG_IO_DELAY_LOCK_MASK);
        SYSCFG_PAD_VIN2A_D17 = (SYSCFG_PAD_VIN2A_D17 & ~0xF) | 0x3;
        delta       = (0x3 << 5) + 0x8;
        regValue    = CFG_VIN2A_D17_OUT & ~0xFFFFFC00;
        coarse      = ((regValue >> 5) & 0x1F) + ((delta >> 5) & 0x1F);
        coarse      = (coarse > 0x1F) ? (0x1F) : (coarse);
        fine        = (regValue & 0x1F) + (delta & 0x1F);
        fine        = (fine > 0x1F) ? (0x1F) : (fine);
        regValue    = CFG_IO_DELAY_ACCESS_PATTERN | CFG_IO_DELAY_LOCK_MASK | ((coarse << 5) | (fine));
        CFG_VIN2A_D17_OUT = regValue;

        CFG_VIN2A_D16_OUT = (CFG_IO_DELAY_ACCESS_PATTERN & ~CFG_IO_DELAY_LOCK_MASK);
        SYSCFG_PAD_VIN2A_D16 = (SYSCFG_PAD_VIN2A_D16 & ~0xF) | 0x3;
        delta       = (0x3 << 5) + 0x2;
        regValue    = CFG_VIN2A_D16_OUT & ~0xFFFFFC00;
        coarse      = ((regValue >> 5) & 0x1F) + ((delta >> 5) & 0x1F);
        coarse      = (coarse > 0x1F) ? (0x1F) : (coarse);
        fine        = (regValue & 0x1F) + (delta & 0x1F);
        fine        = (fine > 0x1F) ? (0x1F) : (fine);
        regValue    = CFG_IO_DELAY_ACCESS_PATTERN | CFG_IO_DELAY_LOCK_MASK | ((coarse << 5) | (fine));
        CFG_VIN2A_D16_OUT = regValue;

        CFG_VIN2A_D15_OUT = (CFG_IO_DELAY_ACCESS_PATTERN & ~CFG_IO_DELAY_LOCK_MASK);
        SYSCFG_PAD_VIN2A_D15 = (SYSCFG_PAD_VIN2A_D15 & ~0xF) | 0x3;
        delta       = (0x4 << 5) + 0x0;
        regValue    = CFG_VIN2A_D15_OUT & ~0xFFFFFC00;
        coarse      = ((regValue >> 5) & 0x1F) + ((delta >> 5) & 0x1F);
        coarse      = (coarse > 0x1F) ? (0x1F) : (coarse);
        fine        = (regValue & 0x1F) + (delta & 0x1F);
        fine        = (fine > 0x1F) ? (0x1F) : (fine);
        regValue    = CFG_IO_DELAY_ACCESS_PATTERN | CFG_IO_DELAY_LOCK_MASK | ((coarse << 5) | (fine));
        CFG_VIN2A_D15_OUT = regValue;

        CFG_VIN2A_D14_OUT = (CFG_IO_DELAY_ACCESS_PATTERN & ~CFG_IO_DELAY_LOCK_MASK);
        SYSCFG_PAD_VIN2A_D14 = (SYSCFG_PAD_VIN2A_D14 & ~0xF) | 0x3;
        delta       = (0x4 << 5) + 0x0;
        regValue    = CFG_VIN2A_D14_OUT & ~0xFFFFFC00;
        coarse      = ((regValue >> 5) & 0x1F) + ((delta >> 5) & 0x1F);
        coarse      = (coarse > 0x1F) ? (0x1F) : (coarse);
        fine        = (regValue & 0x1F) + (delta & 0x1F);
        fine        = (fine > 0x1F) ? (0x1F) : (fine);
        regValue    = CFG_IO_DELAY_ACCESS_PATTERN | CFG_IO_DELAY_LOCK_MASK | ((coarse << 5) | (fine));
        CFG_VIN2A_D14_OUT = regValue;

        /* Global lock */
        CFG_IO_DELAY_LOCK = CFG_IO_DELAY_LOCK_KEY;
    }
    #endif
#endif
}

/**
 *******************************************************************************
 *
 * \brief Callback to get GMAC HW config
 *
 *        This is a callback from the Ethernet driver. This function
 *        is used by the driver to an application-specific config structure
 *        for the GMACSW driver. Typically it will be used to provide the
 *        MAC address(es) and the link status update callback function.
 *
 *******************************************************************************
 */
GMACSW_Config *GMACSW_getConfig(void)
{
    int i = 0;
    uint8_t macAddr[6];

    /* Get digital loopback starting config */
    GMACSW_Config *pGMACSWConfig = NULL;

    #ifdef BUILD_M4_0
        #ifdef NDK_PROC_TO_USE_IPU1_0
        pGMACSWConfig = GMACSW_CONFIG_getDefaultConfig();
        #endif
    #endif
    #ifdef BUILD_M4_1
        #ifdef NDK_PROC_TO_USE_IPU1_1
        pGMACSWConfig = GMACSW_CONFIG_getDefaultConfig();
        #endif
    #endif
    #ifdef BUILD_M4_2
        #ifdef NDK_PROC_TO_USE_IPU2
        pGMACSWConfig = GMACSW_CONFIG_getDefaultConfig();
        #endif
    #endif
    #ifdef BUILD_A15
        #ifdef NDK_PROC_TO_USE_A15_0
        pGMACSWConfig = GMACSW_CONFIG_getDefaultConfig();
        #endif
    #endif

    if(pGMACSWConfig == NULL)
        return pGMACSWConfig;

    /* Update default config with the correct MAC addresses */
    for(i=0; i<(pGMACSWConfig->activeMACPortCount); i++)
    {
        if (0==i)
        {
            /* Get the MAC Address from control module register space */
            macAddr[5] = (uint8_t)((MAC_ID0_LO & 0x000000FFu) >> 0u );
            macAddr[4] = (uint8_t)((MAC_ID0_LO & 0x0000FF00u) >> 8u );
            macAddr[3] = (uint8_t)((MAC_ID0_LO & 0x00FF0000u) >> 16u);

            macAddr[2] = (uint8_t)((MAC_ID0_HI & 0x000000FFu) >> 0u );
            macAddr[1] = (uint8_t)((MAC_ID0_HI & 0x0000FF00u) >> 8u );
            macAddr[0] = (uint8_t)((MAC_ID0_HI & 0x00FF0000u) >> 16u);
        }
        else
        {
            /* Get the MAC Address from control module register space */
            macAddr[5] = (uint8_t)((MAC_ID1_LO & 0x000000FFu) >> 0u );
            macAddr[4] = (uint8_t)((MAC_ID1_LO & 0x0000FF00u) >> 8u );
            macAddr[3] = (uint8_t)((MAC_ID1_LO & 0x00FF0000u) >> 16u);

            macAddr[2] = (uint8_t)((MAC_ID1_HI & 0x000000FFu) >> 0u );
            macAddr[1] = (uint8_t)((MAC_ID1_HI & 0x0000FF00u) >> 8u );
            macAddr[0] = (uint8_t)((MAC_ID1_HI & 0x00FF0000u) >> 16u);
        }

        printf("\nMAC Port %d Address:\n\t%02x-%02x-%02x-%02x-%02x-%02x\n", i,
                macAddr[0], macAddr[1], macAddr[2],
                macAddr[3], macAddr[4], macAddr[5]);

        /* Copy the correct MAC address into the driver config */
        memcpy( (void *)&(pGMACSWConfig->macInitCfg[i].macAddr[0]), (void *)&macAddr[0], 6 );

#if (defined(TDA2XX_FAMILY_BUILD) && (defined(BOARD_TYPE_TDA2XX_EVM) || defined(BOARD_TYPE_TDA2EX_EVM)))
    #if ((PAB_MII == 1) || (PAB_RMII == 1))
        /*
         * Adjust the PHY mask numbers for the Vayu PAB. The first MAC
         * port is connected to a PHY with address = 3, the second MAC
         * port is connected to a PHY with address = 2.
         */
        pGMACSWConfig->macInitCfg[i].phyMask = 0x1 << (3 - i);
    #else
        /*
         * Adjust the PHY mask numbers for the Vayu EVM. The first MAC
         * port is connected to a PHY with address = 2, the second MAC
         * port is connected to a PHY with address = 3.
         */
        #ifndef TDA2EX_BUILD
        pGMACSWConfig->macInitCfg[i].phyMask = 0x1 << (2 + i);
        #else
        uint32_t siliconRev = PlatformGetSiliconRev();
        if (1U == siliconRev)
        {
            /*
            *  For TDA2EX 2.0 EVM the PHY address is 2 for first PHY and 3 for second
            *  PHY
            */
            pGMACSWConfig->macInitCfg[i].phyMask = 0x1 << (2 + i);
        }
        else
        {
            /*
            *  For TDA2EX 1.0 EVM the PHY address is 8 for first PHY and 2 for second
            *  PHY
            */
            pGMACSWConfig->macInitCfg[i].phyMask = 0x8 >> (i*2);
        }
        #endif
    #endif
#endif

#if defined(TDA3XX_FAMILY_BUILD)
    #ifdef BOARD_TYPE_TDA3XX_RVP
        /*
         * Adjust the PHY mask numbers for the TDA3XX RVP. The first MAC
         * port is connected to a PHY with address = 12, the second MAC
         * port is connected to a PHY with address = 11.
         */
        pGMACSWConfig->macInitCfg[i].phyMask = 0x1 << (12 - i);
    #else
        /*
         * Adjust the PHY mask numbers for the xCAM and TDA3xx EVM. The first MAC
         * port is connected to a PHY with address = 0, the second MAC
         * port is connected to a PHY with address = 1.
         */
        pGMACSWConfig->macInitCfg[i].phyMask = 0x1 << i;
    #endif

    pGMACSWConfig->macInitCfg[i].macConnectionType =
            MAC_CONNECTION_TYPE_RGMII_DETECT_INBAND;
#endif

#if defined(BOARD_TYPE_TDA2XX_MC)
    /*
     * Adjust the PHY mask numbers for the xCAM and TDA3xx EVM. The first MAC
     * port is connected to a PHY with address = 0, the second MAC
     * port is connected to a PHY with address = 1.
     */
    pGMACSWConfig->macInitCfg[i].phyMask = 0x1 << i;

    pGMACSWConfig->macInitCfg[i].macConnectionType =
            MAC_CONNECTION_TYPE_RGMII_DETECT_INBAND;
#endif

#if (defined(TDA2XX_FAMILY_BUILD) && (defined(BOARD_TYPE_TDA2XX_EVM) || defined(BOARD_TYPE_TDA2EX_EVM)))
    #if (PAB_MII == 1)
        pGMACSWConfig->macInitCfg[i].macConnectionType =
            MAC_CONNECTION_TYPE_MII_100;
    #elif (PAB_RMII == 1)
        pGMACSWConfig->macInitCfg[i].macConnectionType =
            MAC_CONNECTION_TYPE_RMII_100;
    #else
        /*
         * National PHY on Vayu EVM does not work with the default INBAND detection mode.
         * It would seem the Rx clock from the PHY is not generated unless the Tx clock
         * from the Vayu device is present. So set the mode to force 1Gbps to start.
         */
        pGMACSWConfig->macInitCfg[i].macConnectionType =
            MAC_CONNECTION_TYPE_RGMII_FORCE_1000_FULL;
    #endif
#endif
    }

    pGMACSWConfig->linkStatusCallback = &LOCAL_linkStatus;

    /* Return the config */
    return pGMACSWConfig;
}

#ifdef TDA2EX_BUILD
/**
 *******************************************************************************
 *
 * \brief PHY DP83867IR delay config function
 *
 *        This function is used for configuring the receive and transmit delays
 *        for DP83867IR PHY on TDA2EX PG 2.0 EVM (RevC).
 *        For PHY configuration, need to configure DP83867’s RGMII Control
 *        Register (RGMIICTL) for RGMII mode and RGMII Delay Control Register
 *        (RGMIIDCTL) for 0ns TX delay, 2.25ns RX delay. Set IO Drive Strength
 *        Register (IO_IMPEDANCE_CTRL) to maximum drive.
 *        NOTE: Call this function after NSP initialization as it requires GMAC
 *        handle to call GMAC IOCTL.
 *
 *******************************************************************************
 */
void J6eco_configureDelays(GMACSW_DeviceHandle hGMACSW)
{
    if (1U == PlatformGetSiliconRev())
    {
        MDIO_rdWrphyRegIoctlCmd cmd;
        uint32_t regVal, i;

        /* MAC port number */
        for (i = 0; i < MAC_NUM_PORTS; i++)
        {
            cmd.portNum = i;

            /* PHY software reset */
            regVal = (1U << DP83867_SW_RESET);
            cmd.regAddr = DP83867_CTRL;
            cmd.regVal = &regVal;
            GMACSW_ioctl( hGMACSW,
                          GMACSW_IOCTL_MDIO_WRITE_DP83867_PHY_INDIRECT_REGISTER,
                         (void *)&cmd, sizeof(MDIO_rdWrphyRegIoctlCmd));

            /* Set RGMII Delay values: Tx delay 0 and Rx delay 2.25ns */
            regVal = 0x08;
            cmd.regAddr = DP83867_RGMIIDCTL;
            cmd.regVal = &regVal;
            GMACSW_ioctl( hGMACSW,
                          GMACSW_IOCTL_MDIO_WRITE_DP83867_PHY_INDIRECT_REGISTER,
                         (void *)&cmd, sizeof(MDIO_rdWrphyRegIoctlCmd));

            /* Enable RGMII and CLK delay bits */
            regVal = 0x0D1;
            cmd.regAddr = DP83867_RGMIICTL;
            cmd.regVal = &regVal;
            GMACSW_ioctl( hGMACSW,
                          GMACSW_IOCTL_MDIO_WRITE_DP83867_PHY_INDIRECT_REGISTER,
                         (void *)&cmd, sizeof(MDIO_rdWrphyRegIoctlCmd));

            /* Set Drive Strength bits */
            regVal = 0x61F;
            cmd.regAddr = DP83867_IO_MUX_CTRL;
            cmd.regVal = &regVal;
            GMACSW_ioctl( hGMACSW,
                          GMACSW_IOCTL_MDIO_WRITE_DP83867_PHY_INDIRECT_REGISTER,
                         (void *)&cmd, sizeof(MDIO_rdWrphyRegIoctlCmd));

            /* software restart */
            regVal = (1U << DP83867_SW_RESTART);
            cmd.regAddr = DP83867_CTRL;
            cmd.regVal = &regVal;
            GMACSW_ioctl( hGMACSW,
                          GMACSW_IOCTL_MDIO_WRITE_DP83867_PHY_INDIRECT_REGISTER,
                         (void *)&cmd, sizeof(MDIO_rdWrphyRegIoctlCmd));
        }
    }
}
#endif

#ifdef BOARD_TYPE_TDA3XX_RVP
/**
 *******************************************************************************
 *
 * \brief PHY DP83867IR delay config function
 *
 *        This function is used for configuring the receive and transmit delays
 *        for DP83867IR PHY on TDA3X RVP board.
 *        For PHY configuration, need to configure DP83867’s RGMII Control
 *        Register (RGMIICTL) for RGMII mode and RGMII Delay Control Register
 *        (RGMIIDCTL) for 0ns TX delay, 2.25ns RX delay. Set IO Drive Strength
 *        Register (IO_IMPEDANCE_CTRL) to maximum drive.
 *        NOTE: Call this function after NSP initialization as it requires GMAC
 *        handle to call GMAC IOCTL.
 *
 *******************************************************************************
 */
void Tda3x_Rvp_configureDelays(GMACSW_DeviceHandle hGMACSW)
{
    MDIO_rdWrphyRegIoctlCmd cmd;
    uint32_t regVal, i, readRegVal;

    /* MAC port number */
    for (i = 0; i < MAC_NUM_PORTS; i++)
    {
        cmd.portNum = i;

        /* adjusting timing for DP83867 */
        /* write to DP83867_RGMIIDCTL - set tx & rx clk delay to 1.5ns*/
        regVal = 0x55;
        cmd.regAddr = DP83867_RGMIIDCTL;
        cmd.regVal = &regVal;
        GMACSW_ioctl( hGMACSW,
                      GMACSW_IOCTL_MDIO_WRITE_DP83867_PHY_INDIRECT_REGISTER,
                     (void *)&cmd, sizeof(MDIO_rdWrphyRegIoctlCmd));

        /* enable tx and rx clk delays*/
        /* read from DP83867_RGMIICTL */
        readRegVal = 0x00;
        cmd.regAddr = DP83867_RGMIICTL;
        cmd.regVal = &readRegVal;
        GMACSW_ioctl( hGMACSW,
                      GMACSW_IOCTL_MDIO_READ_DP83867_PHY_INDIRECT_REGISTER,
                     (void *)&cmd, sizeof(MDIO_rdWrphyRegIoctlCmd));

        Vps_printf (" readRegVal = 0x%X\n",readRegVal);

        /* write to DP83867_RGMIICTL */
        regVal = (readRegVal | 0x3);
        cmd.regAddr = DP83867_RGMIICTL;
        cmd.regVal = &regVal;
        GMACSW_ioctl( hGMACSW,
                      GMACSW_IOCTL_MDIO_WRITE_DP83867_PHY_INDIRECT_REGISTER,
                     (void *)&cmd, sizeof(MDIO_rdWrphyRegIoctlCmd));
    }
}
#endif

/**
 *******************************************************************************
 * \brief String to displayed on telnet terminal
 *******************************************************************************
 */
char *VerStr = "\n\n **** Vision SDK **** \n\n";

#ifdef BUILD_M4_0
    #ifdef NDK_PROC_TO_USE_IPU1_0
    static HANDLE hEcho = 0;
    static HANDLE hEchoUdp = 0;
    static HANDLE hData = 0;
    static HANDLE hNull = 0;
    static HANDLE hOob = 0;
    #endif
#endif
#ifdef BUILD_M4_1
    #ifdef NDK_PROC_TO_USE_IPU1_1
    static HANDLE hEcho = 0;
    static HANDLE hEchoUdp = 0;
    static HANDLE hData = 0;
    static HANDLE hNull = 0;
    static HANDLE hOob = 0;
    #endif
#endif
#ifdef BUILD_M4_2
    #ifdef NDK_PROC_TO_USE_IPU2
    static HANDLE hEcho = 0;
    static HANDLE hEchoUdp = 0;
    static HANDLE hData = 0;
    static HANDLE hNull = 0;
    static HANDLE hOob = 0;
    #endif
#endif
#ifdef BUILD_A15
    #ifdef NDK_PROC_TO_USE_A15_0
    static HANDLE hEcho = 0;
    static HANDLE hEchoUdp = 0;
    static HANDLE hData = 0;
    static HANDLE hNull = 0;
    static HANDLE hOob = 0;
    #endif
#endif


/**
 *******************************************************************************
 *
 * \brief NDK callback to start DEAMON services
 *
 *******************************************************************************
 */
void netOpenHook(void)
{
#ifdef BUILD_M4_0
    #ifdef NDK_PROC_TO_USE_IPU1_0
    // Create our local servers
    hEcho = DaemonNew( SOCK_STREAMNC, 0, 7, dtask_tcp_echo,
                       OS_TASKPRINORM, OS_TASKSTKNORM, 0, 3 );
    hEchoUdp = DaemonNew( SOCK_DGRAM, 0, 7, dtask_udp_echo,
                          OS_TASKPRINORM, OS_TASKSTKNORM, 0, 1 );
    hData = DaemonNew( SOCK_STREAM, 0, 1000, dtask_tcp_datasrv,
                       OS_TASKPRINORM, OS_TASKSTKNORM, 0, 3 );
    hNull = DaemonNew( SOCK_STREAMNC, 0, 1001, dtask_tcp_nullsrv,
                       OS_TASKPRINORM, OS_TASKSTKNORM, 0, 3 );
    hOob  = DaemonNew( SOCK_STREAMNC, 0, 999, dtask_tcp_oobsrv,
                       OS_TASKPRINORM, OS_TASKSTKNORM, 0, 3 );

    NetworkCtrl_init();
    #endif
#endif
#ifdef BUILD_M4_1
    #ifdef NDK_PROC_TO_USE_IPU1_1
    // Create our local servers
    hEcho = DaemonNew( SOCK_STREAMNC, 0, 7, dtask_tcp_echo,
                       OS_TASKPRINORM, OS_TASKSTKNORM, 0, 3 );
    hEchoUdp = DaemonNew( SOCK_DGRAM, 0, 7, dtask_udp_echo,
                          OS_TASKPRINORM, OS_TASKSTKNORM, 0, 1 );
    hData = DaemonNew( SOCK_STREAM, 0, 1000, dtask_tcp_datasrv,
                       OS_TASKPRINORM, OS_TASKSTKNORM, 0, 3 );
    hNull = DaemonNew( SOCK_STREAMNC, 0, 1001, dtask_tcp_nullsrv,
                       OS_TASKPRINORM, OS_TASKSTKNORM, 0, 3 );
    hOob  = DaemonNew( SOCK_STREAMNC, 0, 999, dtask_tcp_oobsrv,
                       OS_TASKPRINORM, OS_TASKSTKNORM, 0, 3 );

    NetworkCtrl_init();
    #endif
#endif
#ifdef BUILD_M4_2
    #ifdef NDK_PROC_TO_USE_IPU2
    // Create our local servers
    hEcho = DaemonNew( SOCK_STREAMNC, 0, 7, dtask_tcp_echo,
                       OS_TASKPRINORM, OS_TASKSTKNORM, 0, 3 );
    hEchoUdp = DaemonNew( SOCK_DGRAM, 0, 7, dtask_udp_echo,
                          OS_TASKPRINORM, OS_TASKSTKNORM, 0, 1 );
    hData = DaemonNew( SOCK_STREAM, 0, 1000, dtask_tcp_datasrv,
                       OS_TASKPRINORM, OS_TASKSTKNORM, 0, 3 );
    hNull = DaemonNew( SOCK_STREAMNC, 0, 1001, dtask_tcp_nullsrv,
                       OS_TASKPRINORM, OS_TASKSTKNORM, 0, 3 );
    hOob  = DaemonNew( SOCK_STREAMNC, 0, 999, dtask_tcp_oobsrv,
                       OS_TASKPRINORM, OS_TASKSTKNORM, 0, 3 );

    NetworkCtrl_init();
    #endif
#endif
#ifdef BUILD_A15
    #ifdef NDK_PROC_TO_USE_A15_0
    // Create our local servers
    hEcho = DaemonNew( SOCK_STREAMNC, 0, 7, dtask_tcp_echo,
                       OS_TASKPRINORM, OS_TASKSTKNORM, 0, 3 );
    hEchoUdp = DaemonNew( SOCK_DGRAM, 0, 7, dtask_udp_echo,
                          OS_TASKPRINORM, OS_TASKSTKNORM, 0, 1 );
    hData = DaemonNew( SOCK_STREAM, 0, 1000, dtask_tcp_datasrv,
                       OS_TASKPRINORM, OS_TASKSTKNORM, 0, 3 );
    hNull = DaemonNew( SOCK_STREAMNC, 0, 1001, dtask_tcp_nullsrv,
                       OS_TASKPRINORM, OS_TASKSTKNORM, 0, 3 );
    hOob  = DaemonNew( SOCK_STREAMNC, 0, 999, dtask_tcp_oobsrv,
                       OS_TASKPRINORM, OS_TASKSTKNORM, 0, 3 );

    NetworkCtrl_init();
    #endif
#endif
#ifdef TDA2EX_BUILD
    /* Configure Rx/Tx delays for DP83867IR (on PG2.0 J6ECO EVM) */
    GMACSW_DeviceHandle hGMACSW = GMACSW_open(NULL);
    /* We need to call this here to make sure PHY addresses are updated in
     * the MDIO state object. These addresses will be used for reading/writing
     * registers from PHY */
    GMACSW_periodicTick(hGMACSW);
    /* Need to call tick function twice as MDIO_findingState function has timeout
     * macro which checks PHY Status alternatively */
    GMACSW_periodicTick(hGMACSW);
    /* Configure receive and transmit delays */
    J6eco_configureDelays(hGMACSW);
#endif

#ifdef BOARD_TYPE_TDA3XX_RVP
    /* Configure Rx/Tx delays for DP83867IR (on TDA3X RVP board) */
    GMACSW_DeviceHandle hGMACSW = GMACSW_open(NULL);
    /* We need to call this here to make sure PHY addresses are updated in
     * the MDIO state object. These addresses will be used for reading/writing
     * registers from PHY */
    GMACSW_periodicTick(hGMACSW);
    /* Need to call tick function twice as MDIO_findingState function has timeout
     * macro which checks PHY Status alternatively */
    GMACSW_periodicTick(hGMACSW);
    /* Configure receive and transmit delays */
    Tda3x_Rvp_configureDelays(hGMACSW);
#endif
}

/**
 *******************************************************************************
 *
 * \brief NDK callback to stop DEAMON services
 *
 *******************************************************************************
 */
void netCloseHook(void)
{
#ifdef BUILD_M4_0
    #ifdef NDK_PROC_TO_USE_IPU1_0
    DaemonFree(hOob);
    DaemonFree(hNull);
    DaemonFree(hData);
    DaemonFree(hEchoUdp);
    DaemonFree(hEcho);

    // Kill any active console
    ConsoleClose();
    NetworkCtrl_deInit();
    #endif
#endif
#ifdef BUILD_M4_1
    #ifdef NDK_PROC_TO_USE_IPU1_1
    DaemonFree(hOob);
    DaemonFree(hNull);
    DaemonFree(hData);
    DaemonFree(hEchoUdp);
    DaemonFree(hEcho);

    // Kill any active console
    ConsoleClose();
    NetworkCtrl_deInit();
    #endif
#endif
#ifdef BUILD_M4_2
    #ifdef NDK_PROC_TO_USE_IPU2
    DaemonFree(hOob);
    DaemonFree(hNull);
    DaemonFree(hData);
    DaemonFree(hEchoUdp);
    DaemonFree(hEcho);

    // Kill any active console
    ConsoleClose();
    NetworkCtrl_deInit();
    #endif
#endif
#ifdef BUILD_A15
    #ifdef NDK_PROC_TO_USE_A15_0
    DaemonFree(hOob);
    DaemonFree(hNull);
    DaemonFree(hData);
    DaemonFree(hEchoUdp);
    DaemonFree(hEcho);

    // Kill any active console
    ConsoleClose();
    NetworkCtrl_deInit();
    #endif
#endif

}


/**
 *******************************************************************************
 *
 * \brief Print link status
 *
 *        This is a callback from the Ethernet driver. This function
 *        is called whenever there is a change in link state. The
 *        current PHY and current link state are passed as parameters.
 *
 *******************************************************************************
 */
static void LOCAL_linkStatus( uint32_t phy, uint32_t linkStatus )
{
    Vps_printf(" NDK: Link Status: %s on PHY %" PRIu32 "\n",LinkStr[linkStatus],phy);
}

/**
 * \brief Return ID of processor on which networking runs
 */
UInt32 Utils_netGetProcId(void)
{
    UInt32 procId = SYSTEM_PROC_INVALID;

    #ifdef NDK_PROC_TO_USE_IPU1_0
    procId = SYSTEM_PROC_IPU1_0;
    #endif

    #ifdef NDK_PROC_TO_USE_IPU1_1
    procId = SYSTEM_PROC_IPU1_1;
    #endif

    #ifdef NDK_PROC_TO_USE_IPU2
    procId = SYSTEM_PROC_IPU2;
    #endif

    #ifdef NDK_PROC_TO_USE_A15_0
    procId = SYSTEM_PROC_A15_0;
    #endif

    return procId;
}

/**
 *******************************************************************************
 * \brief Retrun IP address as a string
 *
 *        If network stack is not initialized correctly 0.0.0.0 IP address
 *        is returned
 *
 * \param ipAddrStr [OUT] Assigned IP address as a string
 *
 *******************************************************************************
 */
void Utils_ndkGetIpAddrStr(char *ipAddrStr)
{
    IPN ipAddr;

    memset(&ipAddr, 0, sizeof(ipAddr));

    strcpy(ipAddrStr,"none");

#ifdef BUILD_M4_0
    #ifdef NDK_PROC_TO_USE_IPU1_0
    NtIfIdx2Ip(NET_IF_IDX, &ipAddr);
    NtIPN2Str(ipAddr, ipAddrStr);
    #endif
#endif
#ifdef BUILD_M4_1
    #ifdef NDK_PROC_TO_USE_IPU1_1
    NtIfIdx2Ip(NET_IF_IDX, &ipAddr);
    NtIPN2Str(ipAddr, ipAddrStr);
    #endif
#endif
#ifdef BUILD_M4_2
    #ifdef NDK_PROC_TO_USE_IPU2
    NtIfIdx2Ip(NET_IF_IDX, &ipAddr);
    NtIPN2Str(ipAddr, ipAddrStr);
    #endif
#endif
#ifdef BUILD_A15
    #ifdef NDK_PROC_TO_USE_A15_0
    NtIfIdx2Ip(NET_IF_IDX, &ipAddr);
    NtIPN2Str(ipAddr, ipAddrStr);
    #endif
#endif
}

Int32 Utils_netGetIpAddrStr(char *ipAddr)
{
    UInt32 linkId, procId;
    Int32 status;
    SystemCommon_IpAddr prm;

    strcpy(ipAddr, "none" );

    procId = Utils_netGetProcId();

    if(procId==SYSTEM_PROC_INVALID)
    {
        status = SYSTEM_LINK_STATUS_EFAIL;
    }
    else
    {
        linkId = SYSTEM_MAKE_LINK_ID(procId, SYSTEM_LINK_ID_PROCK_LINK_ID);

        status = System_linkControl(
            linkId,
            SYSTEM_COMMON_CMD_GET_IP_ADDR,
            &prm,
            sizeof(prm),
            TRUE
        );

        if(status==SYSTEM_LINK_STATUS_SOK)
        {
            strcpy(ipAddr, prm.ipAddr);
        }
    }

    return status;
}

Bool Utils_netIsAvbEnabled(void)
{
    Bool status = FALSE;

#ifdef NDK_PROC_TO_USE_IPU1_0
    status = TRUE;
#endif

#ifdef NDK_PROC_TO_USE_IPU1_1
    status = TRUE;
#endif

#ifdef NDK_PROC_TO_USE_IPU2
    status = TRUE;
#endif

#ifdef NDK_PROC_TO_USE_A15_0
    status = TRUE;
#endif

    return status;
}

