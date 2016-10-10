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
 * \defgroup UTILS_IPU_ECC_API IPU ECC related utilities
 *
 * @{
 *
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \file utils_emif_ecc.h
 *
 * \brief  This file has the interface for utility function for using
 *         IPU ECC on TDA3x
 *
 * \version 0.0 (Feb 2016) : [CSG] First version
 *
 *******************************************************************************
 */

#ifndef UTILS_IPU_ECC_H
#define UTILS_IPU_ECC_H

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
 * \brief  Setup IPU ECC error handlers
 *
 * \param  None
 *
 * \return None
 */
void Utils_ipuEccInit(Void);
/*
 * \brief Unregister IPU ECC error handlers
 *
 * \param  None
 *
 * \return None
 */
void Utils_ipuEccDeInit(Void);

#ifdef __cplusplus
}
#endif

#endif

/* @} */
