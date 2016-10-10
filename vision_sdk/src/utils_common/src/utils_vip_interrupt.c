/*
 *******************************************************************************
 *
 * Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \file utils_vip_interrupt.c
 *
 * \brief Utility functions implementation
 *
 * \version 0.0 (July 2014) : [VT] First version
 *
 *******************************************************************************
 */

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */
#include <src/utils_common/include/utils_vip_interrupt.h>

/*******************************************************************************
 *  Local Functions Declarations
 *******************************************************************************
 */
static void WR_MEM_32(UInt32 addr, UInt32 val);
static UInt32 RD_MEM_32(UInt32 addr);



/*******************************************************************************
 *  Functions
 *******************************************************************************
 */

static void WR_MEM_32(UInt32 addr, UInt32 val)
{
    if (0U != addr)
    {
        *(UInt32*)addr =(UInt32)val;
    }
}

static UInt32 RD_MEM_32(UInt32 addr)
{
    UInt32 val = 0U;
    if (0U != addr)
    {
        val = *(UInt32*)addr;
    }

    return (val);
}
/**
 *******************************************************************************
 *
 * \brief   API to be executed at the end of ISR
 *
 *
 *******************************************************************************
 */
void Utils_VIP_Interrupt_EndOfInterrupt(void)
{
    WR_MEM_32(UTILS_EVE_END_OF_INTERRUPT_ADDR, UTILS_EVE_END_OF_INTERRUPT_MASK);
}

/**
*******************************************************************************
*
* \brief   API to check if VIP interrupt occurrent due to a subframe event
*
* \return  TRUE/FALSE
*
*******************************************************************************
*/
Bool Utils_VIP_Interrupt_IsSubframe(void)
{
    Bool status;
    UInt32 subFrameTop, subframeVPDMA;

    /* Read the subframe Top and VPDMA event register locations */
    subFrameTop = RD_MEM_32(UTILS_EVE_VIPTOP_SUBFRAME_EVENT_CLEAR_ADDR);
    subframeVPDMA = RD_MEM_32(UTILS_EVE_VPDMA_SUBFRAME_EVENT_CLEAR_ADDR);

    if((0U != (subFrameTop & UTILS_EVE_VIPTOP_SUBFRAME_EVENT_CLEAR_MASK)) &&
        (0U != (subframeVPDMA & UTILS_EVE_VPDMA_SUBFRAME_EVENT_CLEAR_MASK)))
    {
        status = (Bool)TRUE;
    }
    else
    {
        status = (Bool)FALSE;
    }

    return (status);
}

/**
*******************************************************************************
*
* \brief   API to clear subframe interrupts
*
*******************************************************************************
*/
void Utils_VIP_Interrupt_ClearSubframe_Interrupts(void)
{
    WR_MEM_32(UTILS_EVE_VIPTOP_SUBFRAME_EVENT_CLEAR_ADDR,
                UTILS_EVE_VIPTOP_SUBFRAME_EVENT_CLEAR_MASK);
    WR_MEM_32(UTILS_EVE_VPDMA_SUBFRAME_EVENT_CLEAR_ADDR,
                UTILS_EVE_VPDMA_SUBFRAME_EVENT_CLEAR_MASK);

}

/**
*******************************************************************************
*
* \brief   API to check if VIP interrupt occurrent due to a frame complete event
*
* \return  TRUE/FALSE
*
*******************************************************************************
*/
Bool Utils_VIP_Interrupt_IsFrame(void)
{
    Bool status;
    UInt32 frameTop, frameVPDMA;

    /* Read the frame Top and VPDMA  list complete event locations */
    frameTop = RD_MEM_32(UTILS_EVE_VIPTOP_LC_EVENT_CLEAR_ADDR);
    frameVPDMA = RD_MEM_32(UTILS_EVE_VPDMA_LC_EVENT_CLEAR_ADDR);

    if((0U != (frameTop & UTILS_EVE_VIPTOP_LC_EVENT_CLEAR_MASK)) &&
        (0U != (frameVPDMA & UTILS_EVE_VPDMA_LC_EVENT_CLEAR_MASK)))
    {
        status = (Bool)TRUE;
    }
    else
    {
        status = (Bool)FALSE;
    }

    return (status);
}

/**
*******************************************************************************
*
* \brief   API to clear list complete event (frame complete)
*
*******************************************************************************
*/
void Utils_VIP_Interrupt_Clearframe_Interrupts(void)
{
    WR_MEM_32(UTILS_EVE_VIPTOP_LC_EVENT_CLEAR_ADDR,
                UTILS_EVE_VIPTOP_LC_EVENT_CLEAR_MASK);
    WR_MEM_32(UTILS_EVE_VPDMA_LC_EVENT_CLEAR_ADDR,
                UTILS_EVE_VPDMA_LC_EVENT_CLEAR_MASK);

}

/**
 *******************************************************************************
 *
 * \brief Register subframe and frame complete interrupt with VIP
 *
 *       This function registers an ISR for an interrupt from VIP on subframe
 *       and frame complete events
 *
 * \param   xbarSrc     [IN] Source Crossbar line
 * \param   intrId      [IN] Interrupt ID
 * \param   iSRfunc     [IN] ISR function to be registered
 * \param   pPtr        [IN] Application pointer that is registered as arg with HWI
 *
 * \return  BspOsal_IntrHandle
 *
 *******************************************************************************
 */
BspOsal_IntrHandle Utils_EVE_RegisterInterrupts_FromVIP(
                UInt xbarSrc, UInt32 intrId, Hwi_FuncPtr iSRfunc, void *pPtr)
{
    UInt32 cookie = 0;
    BspOsal_IntrHandle hwi;

    /* Connect the source and destination crossbar lines */
    IntXbar_connectIRQ(
        intrId,
        xbarSrc
        );

    /* Disabling the global interrupts */
    cookie = Hwi_disable();

    Vps_printf(" UTILS: VIP INTERRUPT: HWI Create for INT%d !!!\n", intrId);

    /* MISRA.CAST.Func.Ptr
     * MISRAC_2004_Rule_11.1
     * MISRAC_WAIVER:
     * Casting it to interrupt handler pointer. this cannot be avoided as
     * register interrupt function expects function pointer of this type.
     */
    hwi = BspOsal_registerIntr(intrId + 32U,
                    (BspOsal_IntrFuncPtr)iSRfunc,
                    pPtr);

    if (hwi == NULL)
    {
        Vps_printf(" UTILS: VIP INTERRUPT: HWI Create Failed !!!\n");
        UTILS_assert((Bool)FALSE);
    }

    /* Enable the interrupt */
    Hwi_enableInterrupt(intrId);

    /* Restore interrupts */
    Hwi_restore(cookie);

    /*
    * Enable and clear the list complete and subframe interrupts from VIP
    */

    /* LC 0 Event Enable (VIP Top) */
    WR_MEM_32(UTILS_EVE_VIPTOP_LC_EVENT_ENABLE_ADDR,
                UTILS_EVE_VIPTOP_LC_EVENT_ENABLE_MASK);

    /* LC 0 Event Enable (VPDMA) */
    WR_MEM_32(UTILS_EVE_VPDMA_LC_EVENT_ENABLE_ADDR,
                UTILS_EVE_VPDMA_LC_EVENT_ENABLE_MASK);

    /* Sub-Frame N-line Enable (VIP Top) */
    WR_MEM_32(UTILS_EVE_VIPTOP_SUBFRAME_EVENT_ENABLE_ADDR,
                UTILS_EVE_VIPTOP_SUBFRAME_EVENT_ENABLE_MASK);

    /* Sub-Frame N-line Enable (VPDMA) */
    WR_MEM_32(UTILS_EVE_VPDMA_SUBFRAME_EVENT_ENABLE_ADDR,
                UTILS_EVE_VPDMA_SUBFRAME_EVENT_ENABLE_MASK);

    /* LC 0 Event clear (VIP Top) */
    WR_MEM_32(UTILS_EVE_VIPTOP_LC_EVENT_CLEAR_ADDR,
                UTILS_EVE_VIPTOP_LC_EVENT_CLEAR_MASK);

    /* LC 0 Event clear (VPDMA) */
    WR_MEM_32(UTILS_EVE_VPDMA_LC_EVENT_CLEAR_ADDR,
                UTILS_EVE_VPDMA_LC_EVENT_CLEAR_MASK);

    /* Sub-Frame N-line clear (VIP Top) */
    WR_MEM_32(UTILS_EVE_VIPTOP_SUBFRAME_EVENT_CLEAR_ADDR,
                UTILS_EVE_VIPTOP_SUBFRAME_EVENT_CLEAR_MASK);

    /* Sub-Frame N-line clear (VPDMA) */
    WR_MEM_32(UTILS_EVE_VPDMA_SUBFRAME_EVENT_CLEAR_ADDR,
                UTILS_EVE_VPDMA_SUBFRAME_EVENT_CLEAR_MASK);

    return hwi;
}


/**
 *******************************************************************************
 *
 * \brief Unregisters subframe and frame complete interrupt with VIP
 *
 * \param   hwi          [IN] Hwi handle to be deleted
 *
 *******************************************************************************
 */
void Utils_EVE_UnregisterInterrupts_FromVIP(BspOsal_IntrHandle *hwi)
{
    UInt32 cookie = 0;

    /* Disabling the global interrupts */
    cookie = Hwi_disable();

    BspOsal_unRegisterIntr(hwi);

    /* Restore interrupts */
    Hwi_restore(cookie);
}

