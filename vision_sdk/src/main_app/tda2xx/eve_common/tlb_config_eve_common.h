/*
 *******************************************************************************
 *
 * Copyright (C)
 * \version 0.0 (Jul 2015 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \file tlb_config_eve_common.h
 *
 * \brief  This file gives interfaces for common MMU configuration for all EVE
 *         in TDA2x as per VisionSDK requirments
 *
 * \version 0.0 (Jul 2015) : [CSG] First version
 *
 *******************************************************************************
*/

#ifndef TLB_CONFIG_EVE_COMMON_H_
#define TLB_CONFIG_EVE_COMMON_H_

/* =============================================================================
 * INCLUDE FILES (only if necessary)
 * =============================================================================
 */

#include <mmu.h>
#include <soc.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *******************************************************************************
 *
 * \brief This function implements the MMU configuration common for all EVEs
 *        in TDA2x as required by VisionSDK
 *
 * \return  void
 *
 *******************************************************************************
 */
void eveCommonMmuConfig(UInt32 baseAddr);

#ifdef __cplusplus
}
#endif

#endif  /* TLB_CONFIG_EVE_COMMON_H_ */
/* EOF */

