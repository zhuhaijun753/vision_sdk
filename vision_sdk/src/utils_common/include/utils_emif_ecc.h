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
 * \defgroup UTILS_EMIF_ECC_API EMIF ECC related utilities
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
 *         EMIF ECC on TDA2x and TDA3x
 *
 * \version 0.0 (Dec 2015) : [CSG] First version
 *
 *******************************************************************************
 */

#ifndef UTILS_EMIF_ECC_H
#define UTILS_EMIF_ECC_H

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
 * \brief  Setup EMIF ECC error handlers
 * 
 * \param  None
 *
 * \return None
 */
void Utils_emifEccInit(void);
/* 
 * \brief Unregister EMIF ECC error handlers
 * 
 * \param  None
 *
 * \return None
 */
void Utils_emifEccDeInit(void);

#ifdef __cplusplus
}
#endif

#endif

/* @} */
