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
 * \file system_bsp_init.c
 *
 * \brief  APIs for initializing BIOS Drivers.
 *
 *         BIOS Support Package inits, which includes BIOS video drivers inits
 *
 * \version 0.0 (Jun 2013) : [CM] First version
 * \version 0.1 (Jul 2013) : [CM] Updates as per code review comments
 *
 *******************************************************************************
*/

/*******************************************************************************
 *                           Include Files                                     *
 ******************************************************************************/

#include "system_bsp_init.h"
#include "chip_config.h"

/**
 *******************************************************************************
 * \brief Align BSS memory with that of BIOS heap library requirement
 *******************************************************************************
 */

/**
 *******************************************************************************
 * \brief Link Stack
 *
 *        Align descriptor memory with that of VPDMA requirement.
 *        Place the descriptor in non-cached heap section.
 *
 *******************************************************************************
 */

#ifdef VPE_INCLUDE
/**
 *  BspUtils_appSetVpeLazyLoad
 *  \brief Sets the VPE lazy loading parameters through the VPS control driver.
 */
static Int32 System_bspSetVpeLazyLoad(UInt32 vpeInstId,
                                 UInt32 scalerId,
                                 Bool   lazyLoadingEnable);
#endif

 /*******************************************************************************
 *  Functions
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \brief Initialize the required modules of BIOS video drivers
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
*/
Int32 System_bspInit(void)
{
    Int32 nStatus = SYSTEM_LINK_STATUS_SOK;
    Bsp_CommonInitParams commonInitPrms; /* Initialized using BSP APIs */
    Bsp_PlatformInitParams platInitPrms; /* Initialized using BSP APIs */
    Vps_InitParams vpsInitPrms; /* Initialized using BSP APIs */

    BspCommonInitParams_init(&commonInitPrms);

    Vps_printf(" SYSTEM: BSP Common Init in progress !!!\n");
    nStatus = Bsp_commonInit(&commonInitPrms);
    if (SYSTEM_LINK_STATUS_SOK != nStatus)
    {
        Vps_printf(" SYSTEM: BSP Common Init Failed !!!\n");
    }
    else
    {
        Vps_printf(" SYSTEM: BSP Common Init Done !!!\n");
    }

    if (SYSTEM_LINK_STATUS_SOK == nStatus)
    {
        BspPlatformInitParams_init(&platInitPrms);
        Vps_printf(" SYSTEM: BSP Platform Init in progress !!!\n");
#ifdef A15_TARGET_OS_LINUX
        platInitPrms.isPinMuxSettingReq  = FALSE;
        platInitPrms.isAllMcASPInitReq   = FALSE;
        platInitPrms.isAllMcSPIInitReq   = FALSE;
        platInitPrms.isDmaXBarSettingReq = FALSE;
        platInitPrms.isIrqXBarSettingReq = FALSE;
#else
        platInitPrms.isPinMuxSettingReq = TRUE;
#endif

        nStatus = Bsp_platformInit(&platInitPrms);

#ifdef A15_TARGET_OS_LINUX
        if (SYSTEM_LINK_STATUS_SOK == nStatus)
        {
            /* Unlock MMR_LOCK_2  */
            nStatus = Bsp_platformUnLockRegion(BSP_PLATFORM_MMR_REG_2);
            if (SYSTEM_LINK_STATUS_SOK == nStatus)
            {
                UInt32 cpuId;
                #if defined(IPU_PRIMARY_CORE_IPU1)
                cpuId = CPU_IPU1;
                #endif
                #if defined(IPU_PRIMARY_CORE_IPU2)
                cpuId = CPU_IPU2;
                #endif

                /* XBAR VIP1_IRQ1 (=351) to IPU1_27 */
                BspOsal_irqXbarConnect(cpuId, XBAR_INST_IPU1_IRQ_27, 351);
                #if !defined (TDA2EX_BUILD)
                /* XBAR VIP2_IRQ1 (=352) to IPU1_28 */
                BspOsal_irqXbarConnect(cpuId, XBAR_INST_IPU1_IRQ_28, 352);
                /* XBAR VIP3_IRQ1 (=353) to IPU1_29 */
                BspOsal_irqXbarConnect(cpuId, XBAR_INST_IPU1_IRQ_29, 353);
                #endif
                /* XBAR VPE1_IRQ1 (=354) to IPU1_30 */
                BspOsal_irqXbarConnect(cpuId, XBAR_INST_IPU1_IRQ_30, 354);
            }
        }
#endif

        if (SYSTEM_LINK_STATUS_SOK != nStatus)
        {
            Vps_printf(" SYSTEM: BSP Platform Init Failed !!!\n");
        }
        else
        {
            Vps_printf(" SYSTEM: BSP Platform Init Done !!!\n");
        }
    }

    if (SYSTEM_LINK_STATUS_SOK == nStatus)
    {
        Vps_printf(" SYSTEM: FVID2 Init in progress !!!\n");
        nStatus = Fvid2_init(NULL);
        if (SYSTEM_LINK_STATUS_SOK != nStatus)
        {
            Vps_printf(" SYSTEM: FVID2 Init Failed !!!\n");
        }
        else
        {
            Vps_printf(" SYSTEM: FVID2 Init Done !!!\n");
        }
    }

    if (SYSTEM_LINK_STATUS_SOK == nStatus)
    {
        Vps_printf(" SYSTEM: VPS Init in progress !!!\n");

        VpsInitParams_init(&vpsInitPrms);

#ifdef A15_TARGET_OS_LINUX
       /* This one to one mapping is required for the 1GB Linux builds */
        vpsInitPrms.virtBaseAddr = 0x80000000U;
        vpsInitPrms.physBaseAddr = 0x80000000U;
#else
        vpsInitPrms.virtBaseAddr = 0xA0000000U;
        vpsInitPrms.physBaseAddr = 0x80000000U;
        /* if Virtual address != Physical address then enable translation */
        vpsInitPrms.isAddrTransReq = TRUE;
#endif
        Vps_printf(" SYSTEM: VPDMA Descriptor Memory Address translation"
                    " ENABLED [0x%08x -> 0x%08x]\n",
                    vpsInitPrms.virtBaseAddr,
                    vpsInitPrms.physBaseAddr);

    }

    if (SYSTEM_LINK_STATUS_SOK == nStatus)
    {
        nStatus = Vps_init(&vpsInitPrms);
        if (SYSTEM_LINK_STATUS_SOK != nStatus)
        {
            Vps_printf(" SYSTEM: VPS Init Failed !!!\n");
        }
        else
        {
            Vps_printf(" SYSTEM: VPS Init Done !!!\n");
        }
    }

#ifdef VPE_INCLUDE
    System_bspSetVpeLazyLoad(VPS_M2M_INST_VPE1, 0, (Bool)TRUE);
#endif

    return nStatus;
}

/**
 *******************************************************************************
 *
 * \brief De-initialize the previously initialized modules
 *  of BIOS video drivers
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
*/
Int32 System_bspDeInit(void)
{
    Int32 nStatus = SYSTEM_LINK_STATUS_EFAIL;

    nStatus = Vps_deInit(NULL);
    if (SYSTEM_LINK_STATUS_SOK != nStatus)
    {
        Vps_printf(" SYSTEM: VPS De-Init Failed !!!\n");
    }

    if(SYSTEM_LINK_STATUS_SOK == nStatus)
    {
        nStatus = Fvid2_deInit(NULL);
        if (SYSTEM_LINK_STATUS_SOK != nStatus)
        {
            Vps_printf(" SYSTEM: FVID2 De-Init Failed !!!\n");
        }
    }

    if(SYSTEM_LINK_STATUS_SOK == nStatus)
    {
           nStatus = Bsp_platformDeInit(NULL);
           if (SYSTEM_LINK_STATUS_SOK != nStatus)
           {
               Vps_printf(" SYSTEM: BSP Platform De-Init Failed !!!\n");
           }
    }

    if(SYSTEM_LINK_STATUS_SOK == nStatus)
    {
           nStatus = Bsp_commonDeInit(NULL);
           if (SYSTEM_LINK_STATUS_SOK != nStatus)
           {
               Vps_printf(" SYSTEM: BSP Common De-Init Failed !!!\n");
           }
    }

    return nStatus;
}

#ifdef VPE_INCLUDE
/**
 *  BspUtils_appSetVpeLazyLoad
 *  \brief Sets the VPE lazy loading parameters through the VPS control driver.
 */
static Int32 System_bspSetVpeLazyLoad(UInt32 vpeInstId,
                                 UInt32 scalerId,
                                 Bool   lazyLoadingEnable)
{
    Int32        retVal     = FVID2_SOK;
    Fvid2_Handle fvidHandle = NULL;
    Vps_ScLazyLoadingParams lazyPrms;

    fvidHandle = Fvid2_create(
        (UInt32) FVID2_VPS_CTRL_DRV,
        VPS_CTRL_INST_0,
        NULL,
        NULL,
        NULL);
    if (NULL == fvidHandle)
    {
        GT_2trace(BspAppTrace, GT_ERR,
                  "%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        retVal = FVID2_EFAIL;
    }

    if (FVID2_SOK == retVal)
    {
        VpsScLazyLoadingParams_init(&lazyPrms);
        lazyPrms.vpeInstId         = vpeInstId;
        lazyPrms.scalerId          = scalerId;
        lazyPrms.lazyLoadingEnable = (UInt32) lazyLoadingEnable;

        /* Set the lazy load params */
        retVal = Fvid2_control(
            fvidHandle,
            IOCTL_VPS_CTRL_SET_VPE_LAZY_LOADING,
            &lazyPrms,
            NULL);
        if (FVID2_SOK != retVal)
        {
            GT_2trace(BspAppTrace, GT_ERR,
                      "%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        }
    }

    if (NULL != fvidHandle)
    {
        retVal = Fvid2_delete(fvidHandle, NULL);
        if (FVID2_SOK != retVal)
        {
            GT_2trace(BspAppTrace, GT_ERR,
                      "%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        }
    }

    return (retVal);
}
#endif
