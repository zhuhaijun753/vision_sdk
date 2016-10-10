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
 * \ingroup UTILS_API
 * \defgroup UTILS_ESM_API TDA3x ESM related utilities
 *
 * @{
 *
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \file utils_esm.h
 *
 * \brief  This file has the interface for utility function for using
 *         ESM on TDA3x
 *
 * \version 0.0 (Jan 2016) : [CSG] First version
 *
 *******************************************************************************
 */

#ifndef UTILS_ESM_H
#define UTILS_ESM_H

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

/*******************************************************************************
 *  Functions
 *******************************************************************************
 */

/*
 * \brief  Enable clock monitoring using DCC - valid only for TDA3x
 *
 * \param  None
 *
 * \return None
 */
void System_esmInit(void);

/*
 * \brief  Disable clock monitoring using DCC - valid only for TDA3x
 *
 * \param  None
 *
 * \return None
 */
void System_esmDeInit(void);

BspOsal_IntrHandle System_esmRegisterEvent(UInt32 eventId,
                                           BspOsal_IntrFuncPtr func,
                                           const Void *arg0);
void System_esmUnRegisterEvent(const BspOsal_IntrHandle *pIntrHandle);

#ifdef __cplusplus
}
#endif

#endif

/* @} */
