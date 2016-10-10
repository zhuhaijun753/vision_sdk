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
 * \file epLink.c
 *
 * \brief  This file has the implementation of Endpoint Link API
 *
 *         This file implements the software logic needed to exchange frames
 *         between Vision subsystem and VIVI framework
 *
 *******************************************************************************
*/

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */
#include "epLink_priv.h"
#include <include/link_api/system_common.h>
#include <linux/src/osa/include/osa_mem.h>

/**
 *******************************************************************************
 *
 * \brief Create Endpoint(EP) source driver
 *
 *        Following happens during create phase,
 *        - Call 'set link info' to create the chaanel configuration.
 *          When the next link asks link info it gives back
 *
 * \param  pObj     [IN]  Link object
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
 */
Int32 EpLink_drvSourceCreate(EpLink_obj *pObj, EpLink_ConfigSource *srcCfg)
{
    Int32 status = SYSTEM_LINK_STATUS_SOK;

#ifdef SYSTEM_DEBUG_EP
    Vps_printf(" EP_%u   : EP_SOURCE Create in progress !!!\n",
               pObj->instId
               );
#endif
    /**
     * The srcCfg must have been populated by the application
     * TODO check sanity of the information.
     */
    pObj->linkInfo.numQue = 1;
    memcpy(&pObj->linkInfo.queInfo[0], &srcCfg->queInfo, sizeof(System_LinkQueInfo));

    status = OSA_bufCreate(&pObj->outBufQue, FALSE, FALSE);

#ifdef SYSTEM_DEBUG_EP
    Vps_printf(" EP_%u   : EP_SOURCE Create Done !!!\n",
           pObj->instId
          );
#endif
    /**
     * TODO
     * reset timestamp to cmpute latency.
     */
    return status;
}


/**
 *******************************************************************************
 *
 * \brief Start Endpoint(EP) Source driver
 *
 * \param  pObj     [IN]  Link object
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
 */
Int32 EpLink_drvSourceStart(EpLink_obj *pObj)
{
    Int32 status = SYSTEM_LINK_STATUS_SOK;

#ifdef SYSTEM_DEBUG_EP
    Vps_printf(" EP_%u   : EP_SOURCE Start in progress !!!\n",
               pObj->instId
               );
#endif

    /* currently nothing to do */

#ifdef SYSTEM_DEBUG_EP
    Vps_printf(" EP_%u   : EP_SOURCE Start Done !!!\n",
           pObj->instId
          );
#endif

    return status;
}


/**
 *******************************************************************************
 *
 * \brief Stop Endpoint(EP) Source driver
 *
 * \param  pObj     [IN]  Link object
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
 */
Int32 EpLink_drvSourceStop(EpLink_obj *pObj)
{
    Int32 status = SYSTEM_LINK_STATUS_SOK;

#ifdef SYSTEM_DEBUG_EP
    Vps_printf(" EP_%u   : EP_SOURCE Stop in progress !!!\n",
               pObj->instId
               );
#endif

    /* currently nothing to do */

#ifdef SYSTEM_DEBUG_EP
    Vps_printf(" EP_%u   : EP_SOURCE Stop Done !!!\n",
           pObj->instId
          );
#endif

    return status;
}

/**
 *******************************************************************************
 *
 * \brief Delete Endpoint(EP) Source driver
 *
 * \param  pObj     [IN]  Link object
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
 */
Int32 EpLink_drvSourceDelete(EpLink_obj *pObj)
{
    Int32 status = SYSTEM_LINK_STATUS_SOK;

#ifdef SYSTEM_DEBUG_EP
    Vps_printf(" EP_%u   : EP_SOURCE Delete in progress !!!\n",
               pObj->instId
               );
#endif
    /**
     * We will just close the que at this stage
     *
     * TODO we will have to do something more here for a source link.
     * The source link will receive callbacks from next link on the return
     * path of the buffers. And it will execute asynchronously in the context
     * of another thread. However, the link might have been deleted and que
     * closed. We shouldn't be calling the post_buf() in that case.
     *
     * However, we can rely on the fact that trying to post a buffer to an
     * already closed mqueue will fail! 
     * This problem wouldn't occur for the sink because its previous
     * links would have stopped giving data to it, via SYSTEM_CMD_NEW_DATA.
     *
     * We should try to flush the input message queue.
     *
     */
    status = EpLink_drvCloseQueHandle(pObj);

    /* We need to clean up the source's local buffer que */
    OSA_bufDelete(&pObj->outBufQue);

#ifdef SYSTEM_DEBUG_EP
    Vps_printf(" EP_%u   : EP_SOURCE Delete Done !!!\n",
           pObj->instId
          );
#endif

    return status;
}


/**
 *******************************************************************************
 *
 * \brief Process buffer's
 *
 *        - Application will provide buffers through VIVI.
 *        - Post the buffers into a local Que and send command to next link
 *
 * \param  pObj     [IN]  Link object
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
 */
Int32 EpLink_drvSourceProcessBuffers(EpLink_obj *pObj, System_Buffer *pBuffer)
{
    Int32 status    = SYSTEM_LINK_STATUS_SOK;

    /**
     * At this point we are assuming the input addresses are all virtual.
     * And we dont neecd to perform any conversion.
     */

    /**
     * TODO
     * update timestamp to cmpute latency.
     */
    status = OSA_bufPutFullBuffer(&pObj->outBufQue, pBuffer);
    OSA_assert(status == SYSTEM_LINK_STATUS_SOK);

    status = System_sendLinkCmd(pObj->createArgs.outQueParams.nextLink,
                    SYSTEM_CMD_NEW_DATA, NULL);

    return status;
}

/**
 *******************************************************************************
 *
 * \brief Callback function implemented by link to get empty buffers from next
 *        link.
 *
 *        We are going to put the buffers into queue from where VIVI can pick
 *
 *
 * \param  pObj     [IN]  Link object
 * \param  pBufList [IN] Pointer to Buffer
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
*/
Int32 EpLink_drvSourcePutEmptyBuffers(EpLink_obj *pObj,
                                    System_BufferList *pBufList)
{

    Int32 status = SYSTEM_LINK_STATUS_SOK;

    System_Buffer     *pBuffer;

    OSA_assert(pBufList->numBuf == 1);
    pBuffer = pBufList->buffers[0];

    /**
     * At this point we are assuming the input addresses are all virtual.
     * And we dont neecd to perform any conversion.
     */

    /**
     * TODO
     * update timestamp to cmpute latency.
     */
    status = (pObj->post_buf)(&pObj->ep_ctx, pBuffer);
    if (status < 0)
        Vps_printf("EP_%u   : EP_SOURCE: Failed to queue buffer\n", pObj->instId);

    return status;
}

/**
 *******************************************************************************
 *
 * \brief Callback function implemented by link to provide full buffers to next
 *        link.
 *
 * \param  pObj     [IN]  Link object
 * \param  pBufList [IN] Pointer to Buffer
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
*/
Int32 EpLink_drvSourceGetFullBuffers(EpLink_obj *pObj,
                                    System_BufferList *pBufList)
{
    Int32 status = SYSTEM_LINK_STATUS_SOK;

    /**
     * At this point we are assuming the input addresses are all virtual.
     * And we dont neecd to perform any conversion.
     */
    status =  OSA_bufGetFull(&pObj->outBufQue, pBufList, OSA_TIMEOUT_NONE);

    return status;
}

