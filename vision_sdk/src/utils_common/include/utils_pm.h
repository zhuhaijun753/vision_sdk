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
 * \defgroup UTILS_PM_API PM UTIL API
 *
 * \brief  module define APIs commonly used for PM utility functions.
 *
 * @{
 *
 *******************************************************************************
*/

/**
 *******************************************************************************
 *
 * \file utils_pm.h
 *
 * \brief PM UTIL
 *
 * \version 0.0 First version
 *
 *******************************************************************************
*/

#ifndef UTILS_PM_H_
#define UTILS_PM_H_

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */
#include <src/utils_common/include/utils.h>

/*******************************************************************************
 *  Functions
 *******************************************************************************
*/

/**
 *******************************************************************************
 *
 * \brief Util function to return the Module id for corresponding PROC Id.
 *        This function maps the SDK macro for proc id to prcm module id
 *
 * \return Module-Id prcm module id corresponding to the procId.
 *
 *******************************************************************************
 */

UInt32 Utils_pmGetModId(UInt32 ProcId);

#endif

/* @} */
