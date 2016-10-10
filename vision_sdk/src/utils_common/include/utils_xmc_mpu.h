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
 * \defgroup UTILS_XMC_API DSP XMC related utilities
 *
 * @{
 *
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \file utils_xmc_mpu.h
 *
 * \brief  This file has the interface for utility functions for using
 *         XMC(Extended Memory Controller) and MPU(Memory Protection Unit)
 *         in C66x in TDA2x and TDA3x.
 *
 * \version 0.0 (Jan 2016) : [CSG] First version
 *
 *******************************************************************************
 */

#ifndef UTILS_XMC_MPU_H
#define UTILS_XMC_MPU_H

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
 * \brief Setup XMC and MPU error handlers
 *
 * \param  None
 *
 * \return None
 */
void Utils_xmcMpuInit(void);

/*
 * \brief Unregister XMC and MPU error handlers
 *
 * \param  None
 *
 * \return None
 */
void Utils_xmcMpuDeInit(void);

#ifdef ECC_FFI_INCLUDE
/* Interface to provide LinkStats memory section info to be mapped as QM
 * in DSP.
 */
void Utils_xmcMpuSetLinkStatsSectInfo(UInt32 addr, UInt32 size);
#endif
#ifdef __cplusplus
}
#endif

#endif

/* @} */
