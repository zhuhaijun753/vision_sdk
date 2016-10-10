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

    IntXbar_connect(
        pObj->pIntrConfig->ccXferCompCtrlModXbarIndex,
        pObj->pIntrConfig->ccXferCompXbarInt
        );

    /* Disabling the global interrupts */
    cookie = Hwi_disable();

    intrId = pObj->pIntrConfig->ccXferCompCpuInt;

    pObj->hwiCCXferCompInt = NULL;
    /* Enable the Xfer Completion Event Interrupt */
    EventCombiner_dispatchPlug(
                    intrId,
                    (EventCombiner_FuncPtr)&lisrEdma3ComplHandler0,
       /* MISRA.CAST.PTR_TO_INT:MISRAC_2004 Rule_11.3
          MISRAC_WAIVER:
          Interrupt private argument, passed back to the handler,
          Handler uses this to knwo instance id.
        */
                    pObj->edma3InstanceId,
                    1U);
    EventCombiner_enableEvent(intrId);

    Hwi_enableInterrupt(pObj->pIntrConfig->ccXferCompCpuHwiInt);

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

    /* Disable the Xfer Completion Event Interrupt */
    EventCombiner_disableEvent(pObj->pIntrConfig->ccXferCompCpuInt);

    /* Restore interrupts */
    Hwi_restore(cookie);

}



