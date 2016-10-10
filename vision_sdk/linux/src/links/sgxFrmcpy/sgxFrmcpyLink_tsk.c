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
 * \file sgxFrmcpyLink_tsk.c
 *
 * \brief  This file has the implementation of SgxFrmcpy Link Init and Run API
 *
 *         This file implements the state machine logic for this link.
 *         SgxFrmcpyLink_init() get calls from system_init and the same create
 *         the link task and basic messaging interfaces. Once the link is
 *         initiated it waits for the create cmd. This create cmd creates the
 *         complete link infrastructure.  Then waits for various data and
 *         control cmds.
 *
 *         This file also implements the sgxFrmcpy link tear down functionality
 *
 * \version 0.0 (Jun 2014) : [SS] First version
 *
 *******************************************************************************
*/

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */
#include "sgxFrmcpyLink_priv.h"

/**
 *******************************************************************************
 * \brief SgxFrmcpy Link object, stores all link related information
 *******************************************************************************
 */
SgxFrmcpyLink_Obj gSgxFrmcpyLink_obj[SGXFRMCPY_LINK_OBJ_MAX];

/**
 *******************************************************************************
 *
 * \brief This function implements the sgxFrmcpy link Run/Steady state
 *
 *        In this state link gets commands to
 *         - Stop/delete of link
 *         - Data events/cmds
 *         - All dynamic cmds that the link supports
 *         - All stats/status cmds
 *
 * \param  pObj     [IN]  SgxFrmcpy link instance handle
 * \param  pTsk     [IN]  Link Task Handle
 * \param  pMsg     [IN]  Message Handle
 * \param  done     [IN]  sgxFrmcpy link Run state, set to TRUE once it get the
 *                        is DELETE CMD
 * \param  ackMsg   [OUT] ACK message
 *
 * \return status   SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
 */
Int32 SgxFrmcpyLink_tskRun(SgxFrmcpyLink_Obj *pObj, OSA_TskHndl *pTsk,
                            OSA_MsgHndl **pMsg, Bool *done, Bool *ackMsg)
{
    Int32 status = SYSTEM_LINK_STATUS_SOK;
    Bool runDone = FALSE, runAckMsg = FALSE;
    OSA_MsgHndl *pRunMsg;
    UInt32 cmd;

    *done = FALSE;
    *ackMsg = FALSE;

    runDone = FALSE;
    runAckMsg = FALSE;

    *pMsg = NULL;

    /*
     * This while loop implements RUN state. All the run time commands for
     * ackMsg Link are received and serviced in this while loop.
     * Control remains in this loop until delete commands arrives.
     */
    while (!runDone)
    {
        status = OSA_tskWaitMsg(pTsk, &pRunMsg);
        if (status != OSA_SOK)
            break;

        cmd = OSA_msgGetCmd(pRunMsg);

        /*
         * Different commands are serviced via this switch case. For each
         * command, after servicing, ACK or free message is sent before
         * proceeding to next state.
         */
        switch (cmd)
        {
            case SYSTEM_CMD_STOP:
                runDone = TRUE;
                runAckMsg = TRUE;
                break;

            case SYSTEM_CMD_NEW_DATA:
                OSA_tskAckOrFreeMsg(pRunMsg, status);
                status = SgxFrmcpyLink_drvDoProcessFrames(pObj);
                break;

            case SYSTEM_CMD_DELETE:
                *done = TRUE;
                *ackMsg = TRUE;
                *pMsg = pRunMsg;
                runDone = TRUE;
                break;

            case SYSTEM_CMD_PRINT_STATISTICS:
                SgxFrmcpyLink_drvPrintStatistics(pObj);
                OSA_tskAckOrFreeMsg(pRunMsg, status);
                break;

            case SYSTEM_COMMON_CMD_GET_SGX_LOAD: /* SGX Load */
                system_get_pvrscope_profile_data(&pObj->pvrscopeObj);
                {
                    Utils_SystemLoadStats *pStats =
                          (Utils_SystemLoadStats *) OSA_msgGetPrm(pRunMsg);
                    pStats->totalLoadParams.integerValue =
                          pObj->pvrscopeObj.loadStats.totalLoadParams.integerValue;
                    pStats->totalLoadParams.fractionalValue =
                          pObj->pvrscopeObj.loadStats.totalLoadParams.fractionalValue;
                }
                OSA_tskAckOrFreeMsg(pRunMsg, status);
                break;

            default:
                OSA_tskAckOrFreeMsg(pRunMsg, status);
                break;
        }
    }

    SgxFrmcpyLink_drvStop(pObj);

    if (runAckMsg)
        OSA_tskAckOrFreeMsg(pRunMsg, status);

    return status;
}

/**
 *******************************************************************************
 *
 * \brief This function implements the START/RUN state of sgxFrmcpy Link.
 *
 *        In this state link gets commands to
 *         - Create the SgxFrmcpy Driver
 *         - Start the SgxFrmcpy Driver
 *         - Moves to RUN state
 * \param  pTsk [IN] Task Handle
 * \param  pMsg [IN] Message Handle
 *
 * \return  void
 *
 *******************************************************************************
 */
Int32 SgxFrmcpyLink_tskMain(struct OSA_TskHndl *pTsk,
                                    OSA_MsgHndl *pMsg, UInt32 curState)
{
    UInt32 cmd = OSA_msgGetCmd(pMsg);
    Bool ackMsg, done;
    Int32 status;
    SgxFrmcpyLink_Obj *pObj = (SgxFrmcpyLink_Obj *) pTsk->appData;

    /*
     * At this stage only create command is the expected command.
     * If other message gets received Ack with error status
     */
    if (cmd != SYSTEM_CMD_CREATE)
    {
        OSA_tskAckOrFreeMsg(pMsg, OSA_EFAIL);
        return OSA_EFAIL;
    }

    /*
     * Create command received, create the driver
     */
    status = SgxFrmcpyLink_drvCreate(pObj, OSA_msgGetPrm(pMsg));

    OSA_tskAckOrFreeMsg(pMsg, status);

    if (status != SYSTEM_LINK_STATUS_SOK)
        return OSA_EFAIL;

    done = FALSE;
    ackMsg = FALSE;

    while (!done)
    {
        status = OSA_tskWaitMsg(pTsk, &pMsg);
        if (status != SYSTEM_LINK_STATUS_SOK)
            break;

        cmd = OSA_msgGetCmd(pMsg);

        switch (cmd)
        {
            /*
             * CMD to prime and start the sgxFrmcpy driver. Link will be in
             * running state and ready to sgxFrmcpys the video/graphic frames
             * available at the input side
             */
            case SYSTEM_CMD_START:
                status = SgxFrmcpyLink_drvStart(pObj);

                OSA_tskAckOrFreeMsg(pMsg, status);

                /*
                 * Entering RUN state
                 */
                if (status == SYSTEM_LINK_STATUS_SOK)
                {
                    status =
                        SgxFrmcpyLink_tskRun(pObj, pTsk, &pMsg, &done, &ackMsg);
                }

                break;
            case SGXLINK_CMD_ACQUIRE_EGL_INFO:
                status = SgxFrmcpyLink_drvDoProcessEglInfo(pObj, OSA_msgGetPrm(pMsg));
                OSA_tskAckOrFreeMsg(pMsg, status);
                break;
            case SYSTEM_CMD_DELETE:
                done = TRUE;
                ackMsg = TRUE;
                break;

            default:
                OSA_tskAckOrFreeMsg(pMsg, status);
                break;
        }
    }

    SgxFrmcpyLink_drvDelete(pObj);

    if (ackMsg && pMsg != NULL)
        OSA_tskAckOrFreeMsg(pMsg, status);

    return OSA_SOK;
}

/**
 *******************************************************************************
 *
 *   \brief SgxFrmcpy link register and init function
 *
 *          For each sgxFrmcpy instance (VID1, VID2, VID3 or GRPX1)
 *          - Creates link task
 *          - Registers as a link with the system API
 *
 *   \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
*/
Int32 SgxFrmcpyLink_init(void)
{
    Int32 status;
    System_LinkObj linkObj;
    UInt32 sgxFrmcpyId;
    SgxFrmcpyLink_Obj *pObj;
    char tskName[32];

    for (sgxFrmcpyId = 0; sgxFrmcpyId < SGXFRMCPY_LINK_OBJ_MAX; sgxFrmcpyId++)
    {
        pObj = &gSgxFrmcpyLink_obj[sgxFrmcpyId];

        memset(pObj, 0, sizeof(*pObj));

        pObj->linkId = SYSTEM_LINK_ID_SGXFRMCPY_0 + sgxFrmcpyId;

        linkObj.pTsk = &pObj->tsk;
        linkObj.linkGetFullBuffers  = &SgxFrmcpyLink_getFullBuffers;
        linkObj.linkPutEmptyBuffers = &SgxFrmcpyLink_putEmptyBuffers;
        linkObj.getLinkInfo         = &SgxFrmcpyLink_getLinkInfo;

        System_registerLink(pObj->linkId, &linkObj);

        sprintf(tskName, "SGXFRMCPY%u", (unsigned int)sgxFrmcpyId);

        /*
         * Create link task, task remains in IDLE state.
         * SgxFrmcpyLink_tskMain is called when a message command is received.
         */
        status = OSA_tskCreate(&pObj->tsk,
                               SgxFrmcpyLink_tskMain,
                               SGXFRMCPY_LINK_TSK_PRI,
                               SGXFRMCPY_LINK_TSK_STACK_SIZE,
                               0,
                               pObj);
        OSA_assert(status == SYSTEM_LINK_STATUS_SOK);
    }

    return status;
}

/**
 *******************************************************************************
 *
 *   \brief SgxFrmcpy link de-register and de-init function
 *
 *          For each sgxFrmcpy instance (VID1, VID2, VID3 or GRPX1)
 *          - Deletes link task
 *          - De-registers as a link with the system API
 *
 *   \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
*/
Int32 SgxFrmcpyLink_deInit(void)
{
    SgxFrmcpyLink_Obj *pObj;
    UInt32 sgxFrmcpyId;

    for (sgxFrmcpyId = 0; sgxFrmcpyId < SGXFRMCPY_LINK_OBJ_MAX; sgxFrmcpyId++)
    {
        pObj = &gSgxFrmcpyLink_obj[sgxFrmcpyId];

        OSA_tskDelete(&pObj->tsk);
    }

    return SYSTEM_LINK_STATUS_SOK;
}

/**
 *******************************************************************************
 *
 * \brief Get the buffer and queue information about link.
 *
 * \param  ptr  [IN] Task Handle
 * \param  info [IN] Pointer to link information handle

 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
*/
Int32 SgxFrmcpyLink_getLinkInfo(Void *pTsk,
                             System_LinkInfo *info)
{
    OSA_TskHndl * pTskHndl = (OSA_TskHndl *)pTsk;
    SgxFrmcpyLink_Obj * pObj = (SgxFrmcpyLink_Obj * )pTskHndl->appData;

    /* 'info' structure is set with valid values during 'create' phase
     * Simply pass on previous link info to next link
     */

    memcpy(info, &pObj->tskInfo, sizeof(*info));

    return SYSTEM_LINK_STATUS_SOK;
}

/**
 *******************************************************************************
 * \brief Function called by links connected to Sgx3Dsrv link to get data from
 *    the output queue of the link
 *
 * \param  ptr      [IN]  Handle to task
 * \param  queId    [IN]  output queue Id
 * \param  pBufList [OUT] A List of buffers needed for the next link
 *
 * \return SYSTEM_LINK_STATUS_SOK on success
 *******************************************************************************
*/
Int32 SgxFrmcpyLink_getFullBuffers(Void * ptr, UInt16 queId,
                            System_BufferList * pBufList)
{
    System_Buffer *nextBuf;
    OSA_TskHndl * pTskHndl = (OSA_TskHndl *)ptr;
    SgxFrmcpyLink_Obj * pObj = (SgxFrmcpyLink_Obj * )pTskHndl->appData;
    Int32 status;

    pBufList->numBuf = 0;

    /*Get all the ready buffers and send them in one buflist*/
    do {
        status = OSA_queGet(&(pObj->eglObj.eglFullBufQue), (Int32 *)&nextBuf, OSA_TIMEOUT_NONE);
        if(status == OSA_SOK) {
            pBufList->buffers[pBufList->numBuf] = nextBuf;
            pBufList->numBuf++;
        }
    } while (status == OSA_SOK);

    return SYSTEM_LINK_STATUS_SOK;
}

/**
 *******************************************************************************
 * \brief Function called by links connected to Sgx3Dsrv link to put empty frames
 *    to the output queue of the link
 *
 * \param  ptr      [IN]  Handle to task
 * \param  queId    [IN]  output queue Id
 * \param  pBufList [OUT] A List of buffers needed for the next link
 *
 * \return SYSTEM_LINK_STATUS_SOK on success
 *******************************************************************************
*/
Int32 SgxFrmcpyLink_putEmptyBuffers(Void * ptr, UInt16 queId, System_BufferList * pBufList)
{
    System_Buffer *nextBuf;
    OSA_TskHndl * pTskHndl = (OSA_TskHndl *)ptr;
    SgxFrmcpyLink_Obj * pObj = (SgxFrmcpyLink_Obj * )pTskHndl->appData;
    Int32 status;
    Int32 count;

    /*Add them to the empty list. ProcessBUffers will fill them up*/
    for (count = 0; count < pBufList->numBuf; count++) {
      nextBuf = pBufList->buffers[count];
      status = OSA_quePut(&(pObj->eglObj.eglEmptyBufQue), (Int32)nextBuf, OSA_TIMEOUT_FOREVER);
      OSA_assert(status == SYSTEM_LINK_STATUS_SOK);
    }

    return SYSTEM_LINK_STATUS_SOK;
}

/* Nothing beyond this point */
