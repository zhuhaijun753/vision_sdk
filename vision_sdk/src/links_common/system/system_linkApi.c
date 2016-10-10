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
 * \file system_linkApi.c
 *
 * \brief  This file implements wrapper around few of link APIs.
 *
 *          This file implements wrapper around few of link APIs. It also
 *          implements utility functions to hide some of the complexity in
 *          links like sending command to other link on local or remote
 *          processor is transparent to link. That complexity is handled by
 *          system link APIs.
 *
 * \version 0.0 (Jun 2013) : [KC] First version taken from DVR RDK and
 *                                cleaned up for Vision_sdk
 * \version 0.1 (Jul 2013) : [HS] Commenting style update as per defined
 *                                format.
 * \version 0.2 (Aug 2013) : [KC] Added APIs to print statistics
 *
 *******************************************************************************
 */

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */
#include "system_priv_ipc.h"
#include <osal/bsp_safety_osal.h>


Int32 System_linkCreate(UInt32 linkId, Ptr createArgs, UInt32 argsSize)
{
    return System_linkControl(linkId, SYSTEM_CMD_CREATE, createArgs, argsSize,
                              (Bool)TRUE);
}


Int32 System_linkStart(UInt32 linkId)
{
    return System_linkControl(linkId, SYSTEM_CMD_START, NULL, 0, (Bool)TRUE);
}


Int32 System_linkStop(UInt32 linkId)
{
    return System_linkControl(linkId, SYSTEM_CMD_STOP, NULL, 0, (Bool)TRUE);
}


Int32 System_linkDelete(UInt32 linkId)
{
    return System_linkControl(linkId, SYSTEM_CMD_DELETE, NULL, 0, (Bool)TRUE);
}

Int32 System_linkPrintBufferStatistics(UInt32 linkId)
{
    return System_linkControl(linkId,
                              SYSTEM_CMD_PRINT_BUFFER_STATISTICS,
                              NULL,
                              0,
                              (Bool)TRUE);
}

Int32 System_linkPrintStatistics(UInt32 linkId)
{
    Int32 status = 0;

    #if 1
    /*
     * This collect stats logged in shared memory and prints from there
     * Even if remote CPU is hung up this wont hang
     */
    Utils_linkStatsPrintLinkStatistics(linkId);
    #else
    /*
     * This sends commands to the link to print the stats log
     * if remote CPU is hung up this API will hang
     */
    status = System_linkControl(linkId,
                              SYSTEM_CMD_PRINT_STATISTICS,
                              NULL,
                              0,
                              TRUE);
    #endif
    return status;
}


Int32 System_linkControl(UInt32 linkId, UInt32 cmd, Void * pPrm, UInt32 prmSize,
                         Bool waitAck)
{
    Int32 status;
    UInt32 procId;
    UInt32 curSafetyMode = BspSafetyOsal_getSafetyMode();
    BspSafetyOsal_setSafetyMode(BSP_SAFETY_OSAL_MODE_ASIL);

    procId = SYSTEM_GET_PROC_ID(linkId);

    UTILS_assert((procId < SYSTEM_PROC_MAX));

    if ((procId != System_getSelfProcId()) &&
        (procId != (UInt32)SYSTEM_PROC_INVALID))
    {
        status = System_ipcSendMsg(linkId, cmd, pPrm, prmSize, waitAck);
    }
    else
    {
        status = System_linkControl_local(linkId, cmd, pPrm, prmSize, waitAck);
    }

    BspSafetyOsal_setSafetyMode(curSafetyMode);
    return status;
}


Int32 System_sendLinkCmd(UInt32 linkId, UInt32 cmd, Void *payload)
{
    Int32 status;
    UInt32 procId;
    UInt32 curSafetyMode = BspSafetyOsal_getSafetyMode();
    BspSafetyOsal_setSafetyMode(BSP_SAFETY_OSAL_MODE_ASIL);

    procId = SYSTEM_GET_PROC_ID(linkId);

    UTILS_assert(procId < SYSTEM_PROC_MAX);

    if ((procId != System_getSelfProcId()) && (procId != SYSTEM_PROC_INVALID))
    {
        status = System_ipcSendMsg(linkId, cmd, NULL, 0, FALSE);
    }
    else
    {
        status = System_sendLinkCmd_local(linkId, cmd, payload);
    }

    BspSafetyOsal_setSafetyMode(curSafetyMode);
    return status;
}


Int32 System_linkGetInfo(UInt32 linkId, System_LinkInfo * info)
{
    Int32 status;
    UInt32 procId;

    procId = SYSTEM_GET_PROC_ID(linkId);

    UTILS_assert(procId < SYSTEM_PROC_MAX);

    if ((procId != System_getSelfProcId()) && (procId != SYSTEM_PROC_INVALID))
    {
        status =
            System_ipcSendMsg(linkId, SYSTEM_CMD_GET_INFO, info,
                                  sizeof(*info), (Bool)TRUE);
    }
    else
    {
        status = System_linkGetInfo_local(linkId, info);
    }

    return status;
}


Int32 System_getLinksFullBuffers(UInt32 linkId, UInt16 queId,
                                 System_BufferList * pBufList)
{
    System_LinkObj *pTsk;
    Int32 retVal;

    linkId = SYSTEM_GET_LINK_ID(linkId);

    UTILS_assert(linkId < SYSTEM_LINK_ID_MAX);

    pTsk = &gSystem_objCommon.linkObj[linkId];

    if (pTsk->linkGetFullBuffers != NULL)
    {
        retVal = pTsk->linkGetFullBuffers(pTsk->pTsk, queId, pBufList);
    }
    else
    {
        retVal = FVID2_EFAIL;
    }
    return retVal ;
}


Int32 System_putLinksEmptyBuffers(UInt32 linkId, UInt16 queId,
                                  System_BufferList * pBufList)
{
    System_LinkObj *pTsk;
    Int32 retVal;
    linkId = SYSTEM_GET_LINK_ID(linkId);

    UTILS_assert(linkId < SYSTEM_LINK_ID_MAX);

    pTsk = &gSystem_objCommon.linkObj[linkId];

    if (pTsk->linkPutEmptyBuffers != NULL)
    {
        retVal = pTsk->linkPutEmptyBuffers(pTsk->pTsk, queId, pBufList);
    }
    else
    {
        retVal = FVID2_EFAIL;
    }
    return retVal;
}

/**
 *******************************************************************************
 *
 * \brief Wrapper function to return task handle from linkID
 *
 *        Task handle is required to call link callback functions. This function
 *        returns the task handle of corresponding link ID
 *
 * \param   linkId      [IN] Link Id for which message is intended.
 *                            returned
 * \return  task handle.
 *
 *******************************************************************************
 */
Utils_TskHndl *System_getLinkTskHndl(UInt32 linkId)
{
    System_LinkObj *pTsk;

    linkId = SYSTEM_GET_LINK_ID(linkId);

    UTILS_assert(linkId < SYSTEM_LINK_ID_MAX);

    pTsk = &gSystem_objCommon.linkObj[linkId];

    return pTsk->pTsk;
}

Int32 System_registerLink(UInt32 linkId, const System_LinkObj * pTskObj)
{
    linkId = SYSTEM_GET_LINK_ID(linkId);

    UTILS_assert(linkId < SYSTEM_LINK_ID_MAX);

    if(gSystem_objCommon.linkObj[linkId].pTsk!=NULL)
    {
        Vps_printf(" SYSTEM: ERROR: Link is already registered at linkId = %d !!!\n", linkId);
        UTILS_assert((Bool)0);
    }

    memcpy(&gSystem_objCommon.linkObj[linkId], pTskObj, sizeof(*pTskObj));

    return SYSTEM_LINK_STATUS_SOK;
}
