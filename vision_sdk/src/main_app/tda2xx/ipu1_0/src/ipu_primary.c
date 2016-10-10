/*
 *******************************************************************************
 *
 * Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \file main_ipu1_0.c
 *
 * \brief  This file implements the IPU1_0 main()
 *
 *         This file has the IPU1_0 main(), the entry point to the core.
 *         Set the IPU1_0 clock and call System_start() & BIOS_start()
 *
 * \version 0.0 (Jun 2013) : [SS] First version
 * \version 0.1 (Jul 2013) : [SS] Updates as per code review comments
 * \version 0.2  (Jul 2013) : [HS] Added support for remote debug
 *
 *******************************************************************************
*/

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */
#include <src/utils_common/include/utils.h>
#include <include/link_api/system_common.h>
#include <src/links_common/system/system_priv_common.h>
#include <src/utils_common/include/utils_idle.h>
#include <src/utils_common/include/utils_timer_reconfig.h>
#include <src/links_ipu/system/system_priv_ipu1_0.h>

#include <xdc/cfg/global.h>

#ifdef A15_TARGET_OS_LINUX
/* xdctools header files */
#include <xdc/std.h>
#include <xdc/cfg/global.h>
#include <xdc/runtime/Assert.h>
#include <xdc/runtime/Diags.h>
#include <xdc/runtime/Log.h>
#include <xdc/runtime/Registry.h>

#include <src/utils_common/include/utils_prcm.h>
#include <src/utils_common/include/utils_eveloader.h>
#ifdef IPUMM_INCLUDE
#include <platform/ti/dce/baselib/ipumm_main.h>
#endif
#endif

#include "soc.h"
#include "wd_timer.h"
#ifdef ECC_FFI_INCLUDE
#include "src/utils_common/include/utils_l3fw.h"
#endif


/*******************************************************************************
 *  Function prototypes
 *******************************************************************************
 */

/*******************************************************************************
 *  Return Physical address for VPDMA descriptor memory
 *******************************************************************************
 */
UInt32 System_getVpdmaPhysAddr(void)
{
    return XDC_CFG_VPDMA_DESC_PHYS_ADDR;
    /*  MISRA.LITERAL.UNSIGNED.SUFFIX, Rule 10.6:
     *  MISRA reports error as Unsigned integer literal without 'U' suffix,
     *  this is a labels picked from the .xs file of memory map and are not
     *  #define. Lets ignore this as this will not impact the code quality
     */
}


/**
 *******************************************************************************
 *
 * \brief This is the main() implementation of IPU1_0.
 *
 *        This is the first function  and entry point to IPU1_0, does
 *         - Set the correct/required CPU frequency
 *         - Call the System_start with either Chain_main() or IPU1_0_main()
 *         - Call Chain_main() if App/Chain App is runs on local core
 *         - Call IPU1_0_main() instead of Chain_main if App/Chain is
 *           implemented on any other remote code
 *
 * \return  SYSTEM_LINK_STATUS_SOK
 *
 *******************************************************************************
 */
Int32 IPU_primary_core_init (void)
{

#ifdef ECC_FFI_INCLUDE
    /* Reset Firewall configuration to clear older state when returning from
     * warm resets
     */
    Utils_l3FwReset();
    Utils_l3FwSetDsp1DataSectInfo(gXdcDsp1DataAddr, gXdcDsp1DataSize);
    Utils_l3FwSetDsp2DataSectInfo(gXdcDsp2DataAddr, gXdcDsp2DataSize);
    Utils_l3FwSetEve1DataSectInfo(gXdcEve1DataAddr, gXdcEve1DataSize);
    Utils_l3FwSetLinkStatSectInfo(gXdcLinkStatsAddr, gXdcLinkStatsSize);
#endif

#ifdef A15_TARGET_OS_LINUX
    Utils_prcmInit();

    /* zero initialize NDK_MEM section */
    memset((Ptr)XDC_CFG_NDK_MEM, 0x0, XDC_CFG_NDK_MEM_SIZE);

#ifdef IPU1_LOAD_EVES
    /* Load eves if needed */
    Utils_eveBoot();
#endif

#else
    /* Disable the watchdog timer. The policy for handling WD Timer
     * needs to be defined and implemented appropriately */
    WDTIMERDisable(SOC_WD_TIMER2_BASE);
#endif

#ifdef A15_TARGET_OS_BIOS
    Utils_remoteLogClientInit();
#endif

    /* Timer i767 Silicon Issue workaround */
    Utils_TimerSetTsicrReadMode();

#ifdef IPUMM_INCLUDE
    IPUMM_Main(0, NULL);
#endif

    return SYSTEM_LINK_STATUS_SOK;
}


/* Nothing beyond this point */



