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
 * \defgroup UTILS_DCC_API TDA3x DCC related utilities
 *
 * @{
 *
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \file utils_dcc.h
 *
 * \brief  This file has the interface for utility function for using
 *         DCC on TDA3x
 *
 * \version 0.0 (Jan 2016) : [CSG] First version
 *
 *******************************************************************************
 */

#ifndef UTILS_DCC_H
#define UTILS_DCC_H

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
void System_dccInit(void);

/*
 * \brief  Disable clock monitoring using DCC - valid only for TDA3x
 *
 * \param  None
 *
 * \return None
 */
void System_dccDeInit(void);

#ifdef __cplusplus
}
#endif

#endif

/* @} */
