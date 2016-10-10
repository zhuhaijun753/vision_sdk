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
#include "mergeLink_priv.h"

/**
 *******************************************************************************
 * \brief Link Stack
 *******************************************************************************
 */

#define MERGE_STR_SZ (32U)

/**
 *******************************************************************************
 *
 * \brief Create task for this link
 *
 * \return  SYSTEM_LINK_STATUS_SOK
 *
 *******************************************************************************
*/
Int32 MergeLink_tskCreate(UInt32 instId)
{
    Int32                status;
    MergeLink_Obj        *pObj;
    char                 tskName[MERGE_STR_SZ];

    pObj = &gMergeLink_obj[instId];

    snprintf(tskName, MERGE_STR_SZ, "MERGE%u", instId);

    status = OSA_tskCreate(&pObj->tsk,
                             (OSA_TskFncMain) MergeLink_tskMain,
                             MERGE_LINK_TSK_PRI,
                             MERGE_LINK_TSK_STACK_SIZE,
                             SYSTEM_LINK_STATE_CREATED,
                             pObj);
    OSA_assert(status == SYSTEM_LINK_STATUS_SOK);


    return status;
}

