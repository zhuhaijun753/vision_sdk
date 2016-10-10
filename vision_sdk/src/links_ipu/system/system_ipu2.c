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
 * \file system_ipu2.c
 *
 * \brief  This file has the implementataion of system and link init and deinit
 *
 *         This file implements system Init deinit , Link init and deinit calls,
 *         IPU2 link and platform initialization for IPU2
 *
 *
 * \version 0.0 (Jul 2013) : [SS] First version
 *
 *******************************************************************************
*/

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */
#include "system_priv_ipu2.h"
#include "include/link_api/dcanCtrl_api.h"


/**
 *******************************************************************************
 * \brief IPU2 global Object
 *******************************************************************************
 */
System_Ipu2_Obj gSystem_objipu2;

/*******************************************************************************
 *  Functions
 *******************************************************************************
 */

/**
 *******************************************************************************
 * \brief API to Initialize the system
 *
 *   - Initialize various links present in the core
 *   - Initialize the resources
 *
 * \return SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
 */
Int32 System_init(void)
{
    Int32 status = SYSTEM_LINK_STATUS_SOK;

#ifdef SYSTEM_DEBUG
    Vps_printf(" SYSTEM: System IPU2 Init in progress !!!\n");
#endif
    Utils_dmaInit();
    System_initLinks();


#ifdef SYSTEM_DEBUG
    Vps_printf(" SYSTEM: System IPU2 Init Done !!!\n");
#endif
    return status;
}

/**
 *******************************************************************************
 *
 * \brief API to De-Initialize the system
 *
 *  - De-Initialize various links present in the core
 *  - De-Initialize the resources
 *
 * \param shutdownRemoteProcs   [IN] Set FALSE always for Bios only build.
 *                                   For Linux builds choose TRUE or FALSE
 *
 * \return SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
 */
Int32 System_deInit(UInt32 shutdownRemoteProcs)
{
#ifdef SYSTEM_DEBUG
    Vps_printf(" SYSTEM: System IPU2 De-Init in progress !!!\n");
#endif

    System_deInitLinks();

    Utils_dmaDeInit();

#ifdef SYSTEM_DEBUG
    Vps_printf(" SYSTEM: System IPU2 De-Init Done !!!\n");
#endif

    return SYSTEM_LINK_STATUS_SOK;
}

/**
 *******************************************************************************
 *
 * \brief Initialize the IPU2 system links
 *
 *******************************************************************************
*/
Void System_initLinks(void)
{
    Vps_printf(" SYSTEM: Initializing IPU2 Links !!! \r\n");
    System_memPrintHeapStatus();

    DupLink_init();
    GateLink_init();
    MergeLink_init();
    SelectLink_init();
    SyncLink_init();
    NullLink_init();
    NullSrcLink_init();
    IpcOutLink_init();
    IpcInLink_init();
    AlgorithmLink_init();

    Vps_printf(" SYSTEM: Initializing IPU2 Links ... DONE !!! \r\n");
}

/**
 *******************************************************************************
 *
 * \brief De-initialize the previously initialized IPU2 link
 *
 *******************************************************************************
*/
Void System_deInitLinks(void)
{
    Vps_printf(" SYSTEM: De-Initializing IPU2 Links !!! \r\n");

    DupLink_deInit();
    GateLink_deInit();
    MergeLink_deInit();
    SelectLink_deInit();
    SyncLink_deInit();
    NullLink_deInit();
    NullSrcLink_deInit();
    IpcOutLink_deInit();
    IpcInLink_deInit();
    AlgorithmLink_deInit();

    System_memPrintHeapStatus();
    Vps_printf(" SYSTEM: De-Initializing IPU2 Links ... DONE !!! \r\n");
}

/* Nothing beyond this point */

