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
 *
 * \file utils_dma_edma3cc_ipu_a15_intr.c
 *
 * \brief This file has the implementation of registering interrupts for
 *        IPU, A15 CPUs
 *
 * \version 0.0 (Aug 2013) : [KC] First version
 *
 *******************************************************************************
 */

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */
#include <src/utils_common/src/dma_cfg/utils_dma_edma3cc.h>

#include <ti/sysbios/family/c64p/EventCombiner.h>

/**
 *******************************************************************************
 * \brief See utils_dma_edma3cc.h for function description
 *******************************************************************************
 */
void Utils_edma3RegisterInterrupts (Utils_DmaObj *pObj)
{
    UInt32 cookie = 0;
    UInt32 intrId;

    /* Disabling the global interrupts */
    cookie = Hwi_disable();

    intrId = pObj->pIntrConfig->ccXferCompCpuInt;

    Vps_printf(" UTILS: DMA: HWI Create for INT%d !!!\n", intrId);

    pObj->hwiCCXferCompInt = BspOsal_registerIntr(
                                intrId,
                                (BspOsal_IntrFuncPtr)&lisrEdma3ComplHandler0,
    /* MISRA.CAST.PTR_TO_INT:MISRAC_2004 Rule_11.3
       MISRAC_WAIVER:
       Interrupt private argument, passed back to the handler,
       Handler uses this to knwo instance id.
        */
                                (void*)pObj->edma3InstanceId);

    if (pObj->hwiCCXferCompInt == NULL)
    {
        Vps_printf(" UTILS: DMA: HWI Create Failed !!!\n");
        UTILS_assert((Bool)FALSE);
    }

    Hwi_enableInterrupt(intrId);

    /* Restore interrupts */
    Hwi_restore(cookie);
}

/**
 *******************************************************************************
 * \brief See utils_dma_edma3cc.h for function description
 *******************************************************************************
 */
void Utils_edma3UnRegisterInterrupts (Utils_DmaObj *pObj)
{
    UInt32 cookie = 0;

    /* Disabling the global interrupts */
    cookie = Hwi_disable();

    BspOsal_unRegisterIntr(&pObj->hwiCCXferCompInt);

    /* Restore interrupts */
    Hwi_restore(cookie);
}


