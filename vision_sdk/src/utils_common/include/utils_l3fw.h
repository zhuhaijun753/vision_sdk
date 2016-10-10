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
 * \defgroup UTILS_L3FW_API L3 Firewall related utilities
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
 *         firewalls on TDA2x and TDA3x
 *
 * \version 0.0 (Dec 2015) : [CSG] First version
 *
 *******************************************************************************
 */

#ifndef UTILS_L3FW_H
#define UTILS_L3FW_H

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
 * \brief Setup L3FW and register error handlers
 *
 * \param  None
 *
 * \return None
 */
void Utils_l3FwSetup(void);

/*
 * \brief Reset all L3FW configuration and de-register error handlers
 *
 * \param  None
 *
 * \return None
 */
void Utils_l3FwReset(void);

/*
 * \brief  Switch background region to READ-ONLY or READ-WRITE for EVE
 *
 * \param  safetyMode - BSP_SAFETY_OSAL_MODE_ASIL/BSP_SAFETY_OSAL_MODE_QM
 *
 * \return None
 */
void Utils_l3FwSwitchSafetyMode(UInt32 safetyMode);

#ifdef BUILD_ARP32
/* Interface to identify current safetyMode in EVE.
 */
UInt32 Utils_l3FwGetSafetyMode(Void);
#endif

#if ((defined(BUILD_M4_0) && defined(IPU_PRIMARY_CORE_IPU1)) || (defined(BUILD_M4_2) && defined(IPU_PRIMARY_CORE_IPU2)))
/* Interface to provide DSP1/DSP2/EVE1 data section size in
 * to allow FFI between EDMA and CPU
 */
void Utils_l3FwSetDsp1DataSectInfo(UInt32 addr, UInt32 size);
#ifndef TDA2EX_BUILD
void Utils_l3FwSetDsp2DataSectInfo(UInt32 addr, UInt32 size);
void Utils_l3FwSetEve1DataSectInfo(UInt32 addr, UInt32 size);
#endif
void Utils_l3FwSetLinkStatSectInfo(UInt32 addr, UInt32 size);
#endif


#ifdef __cplusplus
}
#endif

#endif

/* @} */
