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

    status = OSA_tskCreate(&pObj->tsk,
                             (OSA_TskFncMain) GateLink_tskMain,
                             GATE_LINK_TSK_PRI,
                             GATE_LINK_TSK_STACK_SIZE,
                             SYSTEM_LINK_STATE_CREATED,
                             pObj);


    UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

    return status;
}

