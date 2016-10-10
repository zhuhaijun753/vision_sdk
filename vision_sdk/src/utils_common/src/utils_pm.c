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
 * \file utils_pm.c
 *
 * \brief  APIs from this file are used as util functions in SDK
 *
 * \version 0.0 (Jan 2016) : [CM] First version
 *
 *******************************************************************************
*/

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */
#include <stdint.h>
#include <src/utils_common/include/utils_pm.h>
#include <hw/hw_types.h>
#include <soc.h>
#include <pmhal_prcm.h>

/*******************************************************************************
 *  Defines
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
UInt32 Utils_pmGetModId(UInt32 ProcId)
{
    pmhalPrcmModuleId_t moduleId;

    switch(ProcId)
    {
        case SYSTEM_PROC_IPU1_0:
                moduleId = PMHAL_PRCM_MOD_IPU1;
             break;
        case SYSTEM_PROC_IPU1_1:
                moduleId = PMHAL_PRCM_MOD_IPU1;
             break;
#ifndef TDA3XX_BUILD /* Cores is not a present in TDA3xx */
        case SYSTEM_PROC_A15_0:
                moduleId = PMHAL_PRCM_MOD_MPU;
             break;
#endif
        case SYSTEM_PROC_DSP1:
                moduleId = PMHAL_PRCM_MOD_DSP1;
             break;
#ifndef TDA2EX_BUILD  /* Cores is not a present in TDA2ex */
        case SYSTEM_PROC_DSP2:
                moduleId = PMHAL_PRCM_MOD_DSP2;
             break;
        case SYSTEM_PROC_EVE1:
                moduleId = PMHAL_PRCM_MOD_EVE1;
             break;
#ifndef TDA3XX_BUILD  /* Cores is not a present in TDA3xx */
        case SYSTEM_PROC_EVE2:
                moduleId = PMHAL_PRCM_MOD_EVE2;
            break;
        case SYSTEM_PROC_EVE3:
                moduleId = PMHAL_PRCM_MOD_EVE3;
            break;
        case SYSTEM_PROC_EVE4:
                moduleId = PMHAL_PRCM_MOD_EVE4;
            break;
#endif
#endif
#ifndef TDA3XX_BUILD /* Cores is not a present in TDA3xx */
        case SYSTEM_PROC_IPU2:
                moduleId = PMHAL_PRCM_MOD_IPU2;
            break;
#endif
        default:
                moduleId = PMHAL_PRCM_MOD_UNDEF;
             break;
    }
    return (UInt32)moduleId;
}

/* Nothing beyond this point */
