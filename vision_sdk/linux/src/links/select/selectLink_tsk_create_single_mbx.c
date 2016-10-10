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
#include "selectLink_priv.h"

/**
 *******************************************************************************
 * \brief Link Stack
 *******************************************************************************
 */

UInt8 gSelectLink_tskStack[SELECT_LINK_OBJ_MAX][SELECT_LINK_TSK_STACK_SIZE];

/**
 *******************************************************************************
 *
 * \brief Create task for this link
 *
 * \return  SYSTEM_LINK_STATUS_SOK
 *
 *******************************************************************************
*/
Int32 SelectLink_tskCreate(UInt32 instId)
{
    Int32                status;
    SelectLink_Obj        *pObj;
    char                 tskName[32];

    pObj = &gSelectLink_obj[instId];

    snprintf(tskName, sizeof(tskName), "SELECT%u", (UInt32)instId);

    /*
     * Create link task, task remains in IDLE state.
     * SelectLink_tskMain is called when a message command is received.
     */

    status = OSA_tskCreate(&pObj->tsk,
                             (OSA_TskFncMain) SelectLink_tskMain,
                             SELECT_LINK_TSK_PRI,
                             SELECT_LINK_TSK_STACK_SIZE,
                             SYSTEM_LINK_STATE_CREATED,
                             pObj);

    OSA_assert(status == SYSTEM_LINK_STATUS_SOK);

    return status;
}

