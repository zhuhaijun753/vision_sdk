/*
 *******************************************************************************
 *
 * Copyright (C) 2015 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */
#include "gateLink_priv.h"

/**
 *******************************************************************************
 * \brief Link Stack
 *******************************************************************************
 */
/* MISRA.PRAGMA
 * MISRAC_2004 Rule_3.4
 * Non-documented pragma directive.
 * KW State: Fixed in Later Release -> Waiver
 * MISRAC_WAIVER:
 * KW tool cannot understand the pragma supported by CGTools like
 * DATA_ALIGN and DATA_SECTION. But these are valid pragmas.
 */
#pragma DATA_ALIGN(gGateLink_tskStack, 32)

/* MISRA.PRAGMA
 * MISRAC_2004 Rule_3.4
 * Non-documented pragma directive.
 * KW State: Fixed in Later Release -> Waiver
 * MISRAC_WAIVER:
 * KW tool cannot understand the pragma supported by CGTools like
 * DATA_ALIGN and DATA_SECTION. But these are valid pragmas.
 */
#pragma DATA_SECTION(gGateLink_tskStack, ".bss:taskStackSection")

UInt8 gGateLink_tskStack[GATE_LINK_OBJ_MAX][GATE_LINK_TSK_STACK_SIZE];

/**
 *******************************************************************************
 *
 * \brief Create task for this link
 *
 * \return  SYSTEM_LINK_STATUS_SOK
 *
 *******************************************************************************
*/
Int32 GateLink_tskCreate(UInt32 instId)
{
    Int32                status;
    GateLink_Obj        *pObj;
    char                 tskName[32];

    pObj = &gGateLink_obj[instId];

    snprintf(tskName, sizeof(tskName), "GATE%u", (UInt32)instId);

    /*
     * Create link task, task remains in IDLE state.
     * GateLink_tskMain is called when a message command is received.
     */
    status = Utils_tskCreate(&pObj->tsk,
                             GateLink_tskMain,
                             GATE_LINK_TSK_PRI,
                             gGateLink_tskStack[instId],
                             GATE_LINK_TSK_STACK_SIZE,
                             pObj,
                             tskName,
                             UTILS_TSK_AFFINITY_CORE0);
    UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

    return status;
}

