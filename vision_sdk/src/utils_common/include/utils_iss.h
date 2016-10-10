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
 * \defgroup UTILS_ISS_API ISS related utilities
 *
 * @{
 *
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \file utils_iss.h
 *
 * \brief  ISS related utilities
 *
 * \version 0.0 (Dec 2014) : [PS] First version
 *
 *******************************************************************************
 */

#ifndef ISS_INIT_H
#define ISS_INIT_H

#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 *  Include files
 *******************************************************************************
 */
#include <include/link_api/system.h>
#include <src/utils_common/include/utils.h>

/*******************************************************************************
 *  Defines
 *******************************************************************************
 */
void Utils_ispLockCreate(void);
void Utils_pendIspLock(void);
void Utils_postIspLock(void);

#ifdef __cplusplus
}
#endif

#endif

/* @} */
