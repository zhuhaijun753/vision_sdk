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
 * \file tlb_config_eve1.c
 *
 * \brief  This file implements the MMU configuration of EVE1
 *
 * \version 0.0 (Jul 2013) : [SS] First version
 *
 *******************************************************************************
*/

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Error.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/ipc/Ipc.h>

#include <src/main_app/tda2xx/eve_common/tlb_config_eve_common.h>

void eve2MmuConfig(void);

/**
 *******************************************************************************
 *
 * \brief This function implements the MMU configuration of EVE1
 *
 * \return  void
 *
 *******************************************************************************
 */
void eve2MmuConfig(void)
{
    eveCommonMmuConfig(SOC_EVE_MMU0_BASE);
}

/* Nothing beyond this point */

