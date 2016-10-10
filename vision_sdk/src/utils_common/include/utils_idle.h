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
 * \defgroup UTILS_IDLE_API CPU IDLE API
 *
 * \brief  APIs to execute CPU Idle instruction and put CPU into IDLE
 *
 * @{
 *
 *******************************************************************************
*/

/**
 *******************************************************************************
 *
 * \file utils_idle.h
 *
 * \brief CPU Idle
 *
 * \version 0.0 First version
 *
 *******************************************************************************
*/

#ifndef UTILS_IDLE_H_
#define UTILS_IDLE_H_

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */
#include <src/utils_common/include/utils.h>

typedef struct {
    UInt32  idleInitDone;
    /* Flag to indicate the CPU low power initialization is complete.
     */
    UInt64  timeInSleep;
    /* This variable holds the amount of time the CPU was in low power
     * in the measure of SYS_CLK frequency ticks. This is read by the
     * load calculation logic to understand how much time the CPU was in low
     * power.
     */
    UInt64  localTimerFrequency;
    /* This variable holds the CPU local timer frequency in kHz */
} Utils_idleCpuObj;

/*******************************************************************************
 *  Functions
 *******************************************************************************
*/
Void Utils_idleFxn(void);

Void Utils_idlePrepare(void);

UInt64 Utils_idleGetLowPowerTime(Void);

#if defined (BUILD_ARP32)
Void Utils_idleEnableEveDMA(void);
Void Utils_idleDisableEveDMA(void);
#endif

#if defined (BUILD_M4)
void Utils_idleSetupSEVONPEND(UInt32 enable);
void Utils_idleSev(Void);
#endif

#endif

/* @} */
