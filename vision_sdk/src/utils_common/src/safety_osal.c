/*
 *******************************************************************************
 *
 * Copyright (C) 2016 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */
#include <stdint.h>
#include <xdc/std.h>
#include <osal/bsp_safety_osal.h>

#ifdef BUILD_DSP
#include <c66x/dsp_usrSpvSupport.h>
#include "utils_common/include/utils_l3fw.h"
#include <ti/sysbios/family/c66/Cache.h>
#endif
#if defined(BUILD_ARP32) && defined(ECC_FFI_INCLUDE)
#include "soc.h"
#include "soc_defines.h"
#include "l3fw.h"
#include "utils_common/include/utils_l3fw.h"
#endif

UInt32 BspSafetyOsal_setSafetyMode(UInt32 safetyMode)
{
    UInt32 curSafetyMode;
    curSafetyMode = BspSafetyOsal_getSafetyMode();

#ifdef ECC_FFI_INCLUDE
    /* This check avoids recursion */
    if(curSafetyMode != safetyMode)
    {
#ifdef BUILD_DSP
        if (BSP_SAFETY_OSAL_MODE_ASIL == safetyMode)
        {
            DSP_setCpuMode(DSP_CPU_MODE_SPV);
            Utils_l3FwSwitchSafetyMode(safetyMode);
        }
        if (BSP_SAFETY_OSAL_MODE_QM == safetyMode)
        {
            Utils_l3FwSwitchSafetyMode(safetyMode);
            DSP_setCpuMode(DSP_CPU_MODE_USR);
        }
#endif

#ifdef BUILD_ARP32
        Utils_l3FwSwitchSafetyMode(safetyMode);
#endif
    } /* (curSafetyMode != safetyMode) */
#endif /* ECC_FFI_INCLUDE */

    return curSafetyMode;
}

UInt32 BspSafetyOsal_getSafetyMode(void)
{
    UInt32 safetyMode = 0U;

#ifdef ECC_FFI_INCLUDE

#ifdef BUILD_ARP32
    /* There is no USR/SPV in EVE, utils_l3fw.c will identify current
     * safetyMode based on current values of firewall register
     */
    safetyMode = Utils_l3FwGetSafetyMode();
#endif

#ifdef BUILD_DSP
    UInt32 cpuMode = DSP_getCpuMode();
    if (DSP_CPU_MODE_SPV == cpuMode)
    {
        safetyMode = BSP_SAFETY_OSAL_MODE_ASIL;
    }
    if (DSP_CPU_MODE_USR == cpuMode)
    {
        safetyMode = BSP_SAFETY_OSAL_MODE_QM;
    }
#endif

#endif /* ECC_FFI_INCLUDE */

    return safetyMode;
}
