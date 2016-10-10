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
#include "ipcInLink_priv.h"

/**
 *******************************************************************************
 * \brief Link Stack
 *******************************************************************************
 */
/* MISRA.PRAGMA
 * MISRAC_2004 Rule_3.4
 * MISRAC_WAIVER:
 * KW tool cannot understand the pragma supported by CGTools like
 * DATA_ALIGN and DATA_SECTION. But these are valid pragmas.
 */
#pragma DATA_ALIGN(gIpcInLink_tskStack, 32)
/* MISRA.PRAGMA
 * MISRAC_2004 Rule_3.4
 * MISRAC_WAIVER:
 * KW tool cannot understand the pragma supported by CGTools like
 * DATA_ALIGN and DATA_SECTION. But these are valid pragmas.
 */
#pragma DATA_SECTION(gIpcInLink_tskStack, ".bss:taskStackSection:ipc")

UInt8 gIpcInLink_tskStack[IPC_IN_LINK_OBJ_MAX][IPC_IN_LINK_TSK_STACK_SIZE];

/**
 *******************************************************************************
 *
 * \brief Create task for this link
 *
 * \return  SYSTEM_LINK_STATUS_SOK
 *
 *******************************************************************************
*/
Int32 IpcInLink_tskCreate(UInt32 instId)
{
    Int32                status;
    IpcInLink_obj        *pObj;
    char                 tskName[32];

    pObj = &gIpcInLink_obj[instId];

    snprintf(tskName, sizeof(tskName), "IPC_IN_%u", (UInt32)instId);

    /*
     * Create link task, task remains in IDLE state.
     * IpcInLink_tskMain is called when a message command is received.
     */
    status = Utils_tskCreate(&pObj->tsk,
                             IpcInLink_tskMain,
                             IPC_LINK_TSK_PRI,
                             gIpcInLink_tskStack[instId],
                             IPC_IN_LINK_TSK_STACK_SIZE,
                             pObj,
                             tskName,
                             UTILS_TSK_AFFINITY_CORE0);
    UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

    return status;
}

