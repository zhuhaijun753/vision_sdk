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
 *
 * \ingroup UTILS_API
 * \defgroup UTILS_SPINLOCK_API SPINLOCK related utilities
 *
 * @{
 *
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \file utils_l3fw.h
 *
 * \brief  This file has the interface for utility functions for using
 *         spinlocks on TDA2x and TDA3x
 *
 * \version 0.0 (Dec 2015) : [CSG] First version
 *
 *******************************************************************************
 */

#ifndef UTILS_SPINLOCK_H
#define UTILS_SPINLOCK_H

#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 *  Include files
 *******************************************************************************
 */
#include <include/link_api/system.h>

/*******************************************************************************
 *  Defines
 *******************************************************************************
 */

/*
 * Maximum number of available spinlocks
 */
#define MAX_NUM_SPINLOCKS               (256U)

/*
 * Spinlocks are statically assigned.
 */
#define UTILS_SPINLOCK_BASE_UTILS_L3FW       (SOC_SPINLOCK_BASE)
#define UTILS_SPINLOCK_ID_UTILS_L3FW         (0U)

/*******************************************************************************
 *  Functions
 *******************************************************************************
 */

/*
 * \brief Acquire the specified spinLock
 *
 * \param  baseAddr           Base Address of SPINLOCK module
 * \param  spinLockId         ID of spinLock to acquire
 * \param  interruptDisable   Set to TRUE to disable interrupts durint the
 *                            function execution.
 *                            Set to FALSE, if interrupt disable in done
 *                            by the caller.
 *
 * \return None
 */
void Utils_acquireSpinLock(UInt32 baseAddr,
                           UInt32 spinLockId,
                           UInt32 interruptDisable);

/*
 * \brief Free the specified spinLock
 *
 * \param  baseAddr     Base Address of SPINLOCK module
 * \param  spinLockId   ID of spinLock to free
 *
 * \return None
 */
void Utils_freeSpinLock(UInt32 baseAddr, UInt32 spinLockId);

#ifdef __cplusplus
}
#endif

#endif

/* @} */
