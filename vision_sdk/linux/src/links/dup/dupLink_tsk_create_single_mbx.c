/*
 *******************************************************************************
 *
 * Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */
#include "dupLink_priv.h"

/**
 *******************************************************************************
 * \brief Link Stack
 *******************************************************************************
 */

UInt8 gDupLink_tskStack[DUP_LINK_OBJ_MAX][DUP_LINK_TSK_STACK_SIZE];

#define DUP_STR_SZ (32U)

/**
 *******************************************************************************
 *
 * \brief Create task for this link
 *
 * \return  SYSTEM_LINK_STATUS_SOK
 *
 *******************************************************************************
*/
Int32 DupLink_tskCreate(UInt32 instId)
{
    Int32        status;
    DupLink_Obj *pObj;
    char         tskNameStr[DUP_STR_SZ];

    pObj = &gDupLink_obj[instId];

    snprintf(tskNameStr, DUP_STR_SZ, "DUP%u", (UInt32)instId);

    /*
     * Create link task, task remains in IDLE state.
     * DupLink_tskMain is called when a message command is received.
     */
    status = OSA_tskCreate(&pObj->tsk,
                             (OSA_TskFncMain) DupLink_tskMain,
                             DUP_LINK_TSK_PRI,
                             DUP_LINK_TSK_STACK_SIZE,
                             SYSTEM_LINK_STATE_CREATED,
                             pObj);

    OSA_assert(status == SYSTEM_LINK_STATUS_SOK);

    return status;
}

