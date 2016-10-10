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
 * \ingroup UTILS_API
 * \defgroup UTILS_VIP_INTERRUPT_API APIs to register VIP interrupt from EVE
 *
 * @{
 *
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \file utils_vip_interrupt.h
 *
 * \brief  APIs to register VIP interrupt from EVE
 *
 * \version 0.0 (Jul 2014) : [VT] First version
 *
 *
 *******************************************************************************
 */

#ifndef UTILS_VIP_INTERRUPT_PRV_H
#define UTILS_VIP_INTERRUPT_PRV_H

#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 *  Include files
 *******************************************************************************
 */
#include <src/utils_common/include/utils.h>
#include <ti/sysbios/family/shared/vayu/IntXbar.h>

/*******************************************************************************
 *  Defines
 *******************************************************************************
 */

/**
 *******************************************************************************
 * \brief VIP Interrupt no. for frame and Subframe Completion
 *******************************************************************************
 */
#define UTILS_IRQ_XBAR_VIP1_IRQ_2                   (392U)


/**
 *******************************************************************************
 * \brief EVE Interrupt no. for frame and Subframe Completion
 *******************************************************************************
 */
#define UTILS_IRQ_XBAR_EVE_VIP                      (0U)


/**
 *******************************************************************************
 * \brief All the register locations and mask values for enabling and clearing
 * List complete and subframe interrupts from VIP
 *******************************************************************************
 */

#define UTILS_EVE_VIPTOP_LC_EVENT_ENABLE_ADDR       (0x48970050U)
#define UTILS_EVE_VIPTOP_LC_EVENT_ENABLE_MASK       (0x01U)

#define UTILS_EVE_VPDMA_LC_EVENT_ENABLE_ADDR        (0x4897D0DCU)
#define UTILS_EVE_VPDMA_LC_EVENT_ENABLE_MASK        (0x01U)

#define UTILS_EVE_VIPTOP_SUBFRAME_EVENT_ENABLE_ADDR (0x48970054U)
#define UTILS_EVE_VIPTOP_SUBFRAME_EVENT_ENABLE_MASK (0x80U)

/* Enable interrupts for VIP1_SLICE0_UP_Y (TDA2XX_EVM) & VIP1_SLICE1_UP_Y (TDA2XX_MC) */
#define UTILS_EVE_VPDMA_SUBFRAME_EVENT_ENABLE_ADDR  (0x4897D0D4U)
#define UTILS_EVE_VPDMA_SUBFRAME_EVENT_ENABLE_MASK  (0x110U)


#define UTILS_EVE_VIPTOP_LC_EVENT_CLEAR_ADDR        (0x48970048U)
#define UTILS_EVE_VIPTOP_LC_EVENT_CLEAR_MASK        (0x01U)

#define UTILS_EVE_VPDMA_LC_EVENT_CLEAR_ADDR         (0x4897D0D8U)
#define UTILS_EVE_VPDMA_LC_EVENT_CLEAR_MASK         (0x01U)

#define UTILS_EVE_VIPTOP_SUBFRAME_EVENT_CLEAR_ADDR  (0x4897004CU)
#define UTILS_EVE_VIPTOP_SUBFRAME_EVENT_CLEAR_MASK  (0x80U)

/* Check interrupts for VIP1_SLICE0_UP_Y (TDA2XX_EVM) & VIP1_SLICE1_UP_Y (TDA2XX_MC) */
#define UTILS_EVE_VPDMA_SUBFRAME_EVENT_CLEAR_ADDR   (0x4897D0D0U)
#define UTILS_EVE_VPDMA_SUBFRAME_EVENT_CLEAR_MASK   (0x110U)

#define UTILS_EVE_END_OF_INTERRUPT_ADDR             (0x489700A0U)
#define UTILS_EVE_END_OF_INTERRUPT_MASK             (0x01U)


/*******************************************************************************
 *  Functions
 *******************************************************************************
 */
BspOsal_IntrHandle Utils_EVE_RegisterInterrupts_FromVIP(
                UInt xbarSrc, UInt32 intrId, Hwi_FuncPtr iSRfunc, void *pPtr);

void Utils_EVE_UnregisterInterrupts_FromVIP(BspOsal_IntrHandle *hwi);

void Utils_VIP_Interrupt_EndOfInterrupt(void);

Bool Utils_VIP_Interrupt_IsSubframe(void);

void Utils_VIP_Interrupt_ClearSubframe_Interrupts(void);

Bool Utils_VIP_Interrupt_IsFrame(void);

void Utils_VIP_Interrupt_Clearframe_Interrupts(void);

#ifdef __cplusplus
}
#endif

#endif /* UTILS_VIP_INTERRUPT_PRV_H */

/*@}*/
