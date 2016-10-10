/*
 *******************************************************************************
 *
 * Copyright (C) 2015 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */

/**
 *******************************************************************************
 * \file utils_l3fw.c
 *
 * \brief  This file has the implementation for utility functions for using
 *         spinlocks on TDA2x and TDA3x.
 *
 * \version 0.0 (Feb 2016) : [CSG] First version
 *
 *******************************************************************************
*/

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */

#include <xdc/std.h>
#include <string.h>
#include "hw_types.h"
#include "soc.h"
#include "spinlock.h"
#include "utils_common/include/utils_spinlock.h"

/*******************************************************************************
 *  Defines
 *******************************************************************************
 */

/*******************************************************************************
 *  Data structures
 *******************************************************************************
 */

/*******************************************************************************
 *  Global variables
 *******************************************************************************
 */

/*******************************************************************************
 *  Functions
 *******************************************************************************
 */

/*
 * \brief Acquire the specified spinLock
 *
 * \param  baseAddr     Base Address of SPINLOCK module
 * \param  spinLockId   ID of spinLock to acquire
 *
 * \return None
 */
void Utils_acquireSpinLock(UInt32 baseAddr,
                           UInt32 spinLockId,
                           UInt32 interruptDisable)
{
    UInt32 cookie;
    UInt32 status = 1U;

    while(0U != status)
    {
        if(0U != interruptDisable)
        {
            cookie = BspOsal_disableInterrupt();
        }
        status = SPINLOCKLockStatusSet(baseAddr, spinLockId);
        if(0U != interruptDisable)
        {
            BspOsal_restoreInterrupt(cookie);
            /* Sleep to allow other tasks */
            BspOsal_sleep(1U);
        }
    }
}

/*
 * \brief Free the specified spinLock
 *
 * \param  baseAddr     Base Address of SPINLOCK module
 * \param  spinLockId   ID of spinLock to free
 *
 * \return None
 */
void Utils_freeSpinLock(UInt32 baseAddr, UInt32 spinLockId)
{
    SPINLOCKLockStatusFree(baseAddr, spinLockId);
}
