/*
 *******************************************************************************
 *
 * Copyright (C) 2016 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \file utils_esm.c
 *
 * \brief Utility functions for using ESM on TDA3x.
 *
 * \version 0.0 (Jan 2016) : [CSG] First version
 *
 *******************************************************************************
 */

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */
#include <xdc/std.h>
#include <soc.h>
#include <soc_defines.h>
#include <esm.h>
#include <irq_xbar_interrupt_ids.h>
#include <pmhal_prcm.h>
#include <pmhal_mm.h>
#include "utils_common/include/utils_esm.h"
#include <ti/sysbios/family/shared/vayu/IntXbar.h>

/*******************************************************************************
 *  Defines
 *******************************************************************************
 */
#define ESM_IPU_INTR_ID             (68U)
#define ESM_MAX_EVENTS_SUPPORTED    (10U)

static Void Utils_esmISR(UArg ctx);
/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */
/* Interrupt handles */
static BspOsal_IntrHandle   gEsmHwiHandle = NULL;
static UInt32               argArray[ESM_MAX_EVENTS_SUPPORTED] = {0};
static BspOsal_IntrFuncPtr  fxnArray[ESM_MAX_EVENTS_SUPPORTED] = {0};
static UInt32               evtArray[ESM_MAX_EVENTS_SUPPORTED] = {0};


/* ========================================================================== */
/*                         Function definitions                               */
/* ========================================================================== */

static Void Utils_esmISR(UArg ctx)
{
    UInt32 i;
    UInt32 status;
    BspOsal_IntrFuncPtr fxnPtr;
    for(i = 0; i < ESM_MAX_EVENTS_SUPPORTED; i++)
    {
        if(NULL != fxnArray[i])
        {
            status = ESMGetIntrStatus(SOC_ESM_BASE, evtArray[i]);
            if(0U != status)
            {
                Vps_printf(" ESM-UTILS: Event %d occurred!!\n", evtArray[i]);
                fxnPtr = fxnArray[i];
                fxnPtr(argArray[i]);
            }
            ESMClearIntrStatus(SOC_ESM_BASE, evtArray[i]);
        }
    }
}

BspOsal_IntrHandle System_esmRegisterEvent(UInt32 eventId,
                                           BspOsal_IntrFuncPtr func,
                                           const Void *arg0)
{
    UInt32 i;
    UTILS_assert(NULL != func);
    for(i = 0; i < ESM_MAX_EVENTS_SUPPORTED; i++)
    {
        /* MISRA.CAST.Func.Ptr
         * MISRAC_2004_Rule_11.1
         * MISRAC_WAIVER:
         * Casting it to BspOsal_IntrFuncPtr as required by the OSAL API
         */
        if(NULL == fxnArray[i])
        {
            fxnArray[i] = func;
            argArray[i] = (UInt32)arg0;
            evtArray[i] = eventId;
            ESMEnableIntr(SOC_ESM_BASE, eventId);
            ESMSetIntrPriorityLvl(
                SOC_ESM_BASE, eventId, ESM_INTR_PRIORITY_LEVEL_HIGH);
            break;
        }
    }

    if(ESM_MAX_EVENTS_SUPPORTED == i)
    {
        i = 0;
        Vps_printf(" ESM-UTILS: Exceeded max supported events!!\n");
    }
    else
    {
        i = i + 1U;
    }

    /* MISRA.CAST.Func.Ptr
     * MISRAC_2004_Rule_11.1
     * MISRAC_WAIVER:
     * Casting it to BspOsal_IntrFuncPtr as required by the OSAL API
     */
    return (BspOsal_IntrHandle)i;
}

void System_esmUnRegisterEvent(const BspOsal_IntrHandle *pIntrHandle)
{
    Int32 status = SYSTEM_LINK_STATUS_SOK;
    UInt32 i;

    /* MISRA.CAST.PTR_TO_INT
     * MISRAC_2004 Rule_11.3
     * MISRAC_WAIVER:
     * Passing pointer arguments to ISR and callback functions which
     * takes arguments as UInt32 for legacy reasons and vice-versa.
     */
    i = *(UInt32 const*)pIntrHandle;
    /* Handles can take values from 1 to ESM_MAX_EVENTS_SUPPORTED */
    if((i == 0) ||
       (i > ESM_MAX_EVENTS_SUPPORTED))
    {
        status = SYSTEM_LINK_STATUS_EFAIL;
    }

    if(SYSTEM_LINK_STATUS_SOK == status)
    {
        i = i - 1U;
        if(NULL == fxnArray[i])
        {
            status = SYSTEM_LINK_STATUS_EFAIL;
        }
    }

    if(SYSTEM_LINK_STATUS_SOK == status)
    {
        ESMDisableIntr(SOC_ESM_BASE, evtArray[i]);
        fxnArray[i] = NULL;
        argArray[i] = NULL;
        evtArray[i] = 0;
    }
    else
    {
        Vps_printf(" ESM-UTILS: Exceeded max supported events!!\n");
    }
}

void System_esmInit(void)
{
    Int32 status;
    status = PMHALModuleModeSet(PMHAL_PRCM_MOD_ESM,
                                PMHAL_PRCM_MODULE_MODE_ENABLED,
                                PM_TIMEOUT_INFINITE);
    UTILS_assert(0 == status);

    if(NULL == gEsmHwiHandle)
    {
       /* MISRA.CAST.Func.Ptr
        * MISRAC_2004_Rule_11.1
        * MISRAC_WAIVER:
        * Casting it to BspOsal_IntrFuncPtr as required by the OSAL API
        */
        /* Enable interrupt in XBAR and register ISR */
        IntXbar_connectIRQ(ESM_IPU_INTR_ID, ESM_IRQ_HIGH);
        gEsmHwiHandle =
            BspOsal_registerIntr(ESM_IPU_INTR_ID,
                                 (BspOsal_IntrFuncPtr) &Utils_esmISR,
                                 NULL);

        ESMSetMode(SOC_ESM_BASE, ESM_OPERATION_MODE_NORMAL);
    }
}

void System_esmDeInit(void)
{
    if(NULL != gEsmHwiHandle)
    {
        /* Enable interrupt in XBAR and register ISR */
        IntXbar_disconnectIRQ(ESM_IPU_INTR_ID);
        /* De-register ISRs */
        BspOsal_unRegisterIntr(&gEsmHwiHandle);
        gEsmHwiHandle = NULL;
    }
}
