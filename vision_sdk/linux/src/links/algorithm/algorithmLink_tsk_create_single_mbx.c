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
#include "algorithmLink_priv.h"

/**
 *******************************************************************************
 * \brief Link Stack
 *******************************************************************************
 */

UInt8 gAlgorithmLink_tskStack[ALGORITHM_LINK_OBJ_MAX][ALGORITHM_LINK_TSK_STACK_SIZE];

#define ALG_STR_SZ (32U)

/**
 *******************************************************************************
 *
 * \brief Create task for this link
 *
 * \return  SYSTEM_LINK_STATUS_SOK
 *
 *******************************************************************************
*/
Int32 AlgorithmLink_tskCreate(UInt32 instId)
{
    Int32                status;
    AlgorithmLink_Obj   *pObj;
    char                 tskNameStr[ALG_STR_SZ];

    pObj = &gAlgorithmLink_obj[instId];

    snprintf(tskNameStr, ALG_STR_SZ, "ALGORITHM%u", instId);

    /*
     * Create link task, task remains in IDLE state.
     * AlgorithmLink_tskMain is called when a message command is received.
     */
    status = OSA_tskCreate(&pObj->tsk,
                             (OSA_TskFncMain) AlgorithmLink_tskMain,
                             ALGORITHM_LINK_TSK_PRI,
                             ALGORITHM_LINK_TSK_STACK_SIZE,
                             SYSTEM_LINK_STATE_CREATED,
                             pObj);

    OSA_assert(status == SYSTEM_LINK_STATUS_SOK);

    return status;
}

