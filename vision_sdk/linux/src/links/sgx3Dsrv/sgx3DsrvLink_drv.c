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
 * \file sgx3DsrvLink_drv.c
 *
 * \brief  This file has the implementation of Sgx3Dsrv Link
 *
 *         Sgx3Dsrv Link is used to feed video frames to SGX for
 *         rendering.
 *         The rendered output will be pushed to display via DRM.
 *
 * \version 0.0 (Jun 2014) : [SS] First version
 *
 *******************************************************************************
*/

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */
#include "sgx3DsrvLink_priv.h"
#include <linux/src/osa/include/osa_mem.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef USE_STANDALONE_LUT
#include "linux/src/system/sgxRenderUtils/standalone/GAlignLUT.c"
#include "linux/src/system/sgxRenderUtils/standalone/BlendLUT3D.c"
#endif

/**
 *******************************************************************************
 *
 * \brief Sgx3Dsrv link create function
 *
 *        This Set the Link and driver create time parameters.
 *        - Get the channel info from previous link
 *        - Set the internal data structures
 *        - Call the create and control functions
 *
 * \param   pObj     [IN] Sgx3Dsrv Link Instance handle
 * \param   pPrm     [IN] Sgx3Dsrv link create parameters
 *                        This need to be configured by the application
 *
 * \return  SYSTEM_LINK_STATUS_SOK
 *
 *******************************************************************************
 */
Int32 Sgx3DsrvLink_drvCreate(Sgx3DsrvLink_Obj *pObj,
                             Sgx3DsrvLink_CreateParams *pPrm)
{
    Int32 status = SYSTEM_LINK_STATUS_SOK;
    UInt32                  inQue, channelId;
    Sgx3DsrvLink_InputQueId inputQId;
    UInt32                  prevChInfoFlags;
    System_LinkChInfo     * pPrevChInfo;

#ifdef SYSTEM_DEBUG_DISPLAY
    Vps_printf(" SGX3DSRV Link: Create in progress !!!\n");
#endif

    memcpy(&pObj->createArgs, pPrm, sizeof(*pPrm));

    OSA_assert(pPrm->numInQue <= SGX3DSRV_LINK_IPQID_MAXIPQ);

    for (inQue = 0; inQue < pPrm->numInQue; inQue++)
    {
        status = System_linkGetInfo(pPrm->inQueParams[inQue].prevLinkId,
                                    &pObj->inTskInfo[inQue]);
        OSA_assert(status == SYSTEM_LINK_STATUS_SOK);
        OSA_assert(pPrm->inQueParams[inQue].prevLinkQueId <
                   pObj->inTskInfo[inQue].numQue);
        memcpy(&pObj->inQueInfo[inQue],
               &pObj->inTskInfo[inQue].queInfo[pPrm->inQueParams[inQue].prevLinkQueId],
               sizeof(pObj->inQueInfo));
    }

    OSA_assert(pObj->createArgs.inBufType[SGX3DSRV_LINK_IPQID_MULTIVIEW] ==
                     SYSTEM_BUFFER_TYPE_VIDEO_FRAME_CONTAINER);
    OSA_assert(pObj->createArgs.inBufType[SGX3DSRV_LINK_IPQID_PALUT] ==
                     SYSTEM_BUFFER_TYPE_METADATA);
    OSA_assert(pObj->createArgs.inBufType[SGX3DSRV_LINK_IPQID_GALUT] ==
                     SYSTEM_BUFFER_TYPE_METADATA);
    OSA_assert(pObj->createArgs.inBufType[SGX3DSRV_LINK_IPQID_BLENDLUT] ==
                     SYSTEM_BUFFER_TYPE_METADATA);

    inputQId = SGX3DSRV_LINK_IPQID_MULTIVIEW;
    channelId = 0;
    pPrevChInfo   =
        &(pObj->inQueInfo[inputQId].chInfo[channelId]);

    OSA_assert(pObj->createArgs.maxOutputWidth  <= SGX3DSRV_LINK_OUTPUT_FRAME_WIDTH);
    OSA_assert(pObj->createArgs.maxOutputHeight <= SGX3DSRV_LINK_OUTPUT_FRAME_HEIGHT);
    
    memset(&pObj->eglObj, 0, sizeof(pObj->eglObj));
    pObj->eglObj.width = SGX3DSRV_LINK_OUTPUT_FRAME_WIDTH;
    pObj->eglObj.height = SGX3DSRV_LINK_OUTPUT_FRAME_HEIGHT;

    pObj->tskInfo.numQue = SGX_LINK_MAX_OUT_QUE;
    OSA_assert(pObj->tskInfo.numQue == 1);
    pObj->tskInfo.queInfo[0].numCh = 1;
    pObj->tskInfo.queInfo[0].chInfo[0].startX = 0;
    pObj->tskInfo.queInfo[0].chInfo[0].startY = 0;
    pObj->tskInfo.queInfo[0].chInfo[0].width = pObj->eglObj.width;
    pObj->tskInfo.queInfo[0].chInfo[0].height = pObj->eglObj.height;
    pObj->tskInfo.queInfo[0].chInfo[0].pitch[0] = pObj->eglObj.width * 4;
    pObj->tskInfo.queInfo[0].chInfo[0].pitch[1] = pObj->eglObj.width * 4;
    pObj->tskInfo.queInfo[0].chInfo[0].pitch[2] = pObj->eglObj.width * 4;
    pObj->tskInfo.queInfo[0].chInfo[0].flags =
        System_Link_Ch_Info_Set_Flag_Data_Format(pObj->tskInfo.queInfo[0].chInfo[0].flags, SYSTEM_DF_ABGR32_8888);

    prevChInfoFlags    = pPrevChInfo->flags;
    pObj->inDataFormat = System_Link_Ch_Info_Get_Flag_Data_Format(prevChInfoFlags);
    pObj->inPitch[0] = pPrevChInfo->pitch[0];
    pObj->inPitch[1] = pPrevChInfo->pitch[1];

    if((pPrevChInfo->width > pObj->createArgs.maxInputWidth)
       ||
       (pPrevChInfo->height > pObj->createArgs.maxInputHeight)
      )
    {
      OSA_assert(NULL);
    }

    /*
     * Creation of local input Qs for SGX3DSRV_LINK_IPQID_MULTIVIEW and
     * SGX3DSRV_LINK_IPQID_PALUT.
     * For ALGLINK_SYNTHESIS_IPQID_GALUT, always just one entry is kept.
     */
    inputQId = SGX3DSRV_LINK_IPQID_MULTIVIEW;
    status  = OSA_queCreate(&(pObj->localInputQ[inputQId].queHandle),
                               SGX3DSRV_LINK_MAX_LOCALQUEUELENGTH);
    OSA_assert(status == SYSTEM_LINK_STATUS_SOK);

    inputQId = SGX3DSRV_LINK_IPQID_PALUT;
    status  = OSA_queCreate(&(pObj->localInputQ[inputQId].queHandle),
                               SGX3DSRV_LINK_MAX_LOCALQUEUELENGTH);
    OSA_assert(status == SYSTEM_LINK_STATUS_SOK);

    OSA_resetLatency(&pObj->linkLatency);
    OSA_resetLatency(&pObj->srcToLinkLatency);

    pObj->numInputChannels = 1;
    OSA_resetLinkStatistics(&pObj->linkStats, pObj->numInputChannels, 1);

    pObj->isFirstFrameRecv     = FALSE;
    pObj->receivedGALUTFlag    = FALSE;
    pObj->receivedBlendLUTFlag = FALSE;
    pObj->receivedFirstPALUTFlag = FALSE;

    /* For SGX profiling */
    system_create_pvrscope_profile_data(&pObj->pvrscopeObj);

    if (pObj->createArgs.bEglInfoInCreate) {
        status = Sgx3DsrvLink_drvDoProcessEglInfo(pObj,
                (struct control_srv_egl_ctx *)pObj->createArgs.EglInfo);
    }

#ifdef SYSTEM_DEBUG_DISPLAY
    Vps_printf(" SGX3DSRV Link: Create Done !!!\n");
#endif

    return SYSTEM_LINK_STATUS_SOK;
}

/**
 *******************************************************************************
 *
 * \brief This function prints the Link status
 *
 *        prints the Link status, such as
 *        - FPS
 *        - Callback Intervals
 *        - Input DropCount
 *        - etc
 *
 * \param   pObj        [IN] Sgx3Dsrv Link Instance handle
 *
 * \return  SYSTEM_LINK_STATUS_SOK
 *
 *******************************************************************************
 */
Int32 Sgx3DsrvLink_drvPrintStatistics(Sgx3DsrvLink_Obj *pObj)
{
    OSA_printLinkStatistics(&pObj->linkStats, "SGX3DSRV", TRUE);

    OSA_printLatency("SGX3DSRV",
                       &pObj->linkLatency,
                       &pObj->srcToLinkLatency,
                        TRUE
                       );

    return SYSTEM_LINK_STATUS_SOK;
}

Int32 Sgx3DsrvLink_drvDoProcessEglInfo(Sgx3DsrvLink_Obj *pObj, struct control_srv_egl_ctx *pPrm)
{
    Int32 status = SYSTEM_LINK_STATUS_SOK;

    pObj->eglObj.chInfo.flags = pObj->tskInfo.queInfo[0].chInfo[0].flags;
    status = System_eglOpen(&pObj->eglObj, pPrm);
    OSA_assert(status==SYSTEM_LINK_STATUS_SOK);

    pObj->tskInfo.queInfo[0].chInfo[0].width = pObj->eglObj.chInfo.width;
    pObj->tskInfo.queInfo[0].chInfo[0].height = pObj->eglObj.chInfo.height;
    pObj->tskInfo.queInfo[0].chInfo[0].pitch[0] = pObj->eglObj.chInfo.pitch[0];
    pObj->tskInfo.queInfo[0].chInfo[0].flags = pObj->eglObj.chInfo.flags;

    pObj->render3DSRVObj.screen_width = pObj->eglObj.width;
    pObj->render3DSRVObj.screen_height = pObj->eglObj.height;
    pObj->render3DSRVObj.mode = RENDER_MODE_3D_SURROUND_VIEW;
    pObj->render3DSRVObj.carScale = 25.0f;
    pObj->render3DSRVObj.inFramesWidth = (int)pObj->createArgs.maxInputWidth;
    pObj->render3DSRVObj.inFramesHeight = (int)pObj->createArgs.maxInputHeight;
    OSA_assert(pObj->createArgs.maxInputWidth == SGX3DSRV_LINK_INPUT_FRAME_WIDTH);
    OSA_assert(pObj->createArgs.maxInputHeight == SGX3DSRV_LINK_INPUT_FRAME_HEIGHT);
    status = SgxRender3DSRV_setup(&pObj->render3DSRVObj, pObj->createArgs.bAnimate);
    OSA_assert(status==SYSTEM_LINK_STATUS_SOK);
    pObj->render3DSRVObj.egoposePtr = NULL; /*not used in this link */
    pObj->render3DSRVObj.boxesPtr = NULL; /*not used in this link */

    return status;
}
/**
 *******************************************************************************
 *
 * \brief This function de-queue frames/meta data from prvious link output Qs
 *
 *        Function perform the following operations
 *        - De-queue frames/meta data from prvious link output Qs
 *        - Put them in an internal Qs
 *
 * \param   pObj     [IN] Sgx3Dsrv Link Instance handle
 *
 * \return  SYSTEM_LINK_STATUS_SOK
 *
 *******************************************************************************
 */
Int32 Sgx3DsrvLink_getInputFrameData(Sgx3DsrvLink_Obj * pObj)
{
    Sgx3DsrvLink_InputQueId      inputQId;
    Int32                        status    = SYSTEM_LINK_STATUS_SOK;
    UInt32                       bufId;
    System_BufferList            inputBufList;
    System_BufferList            inputBufListReturn;
    System_Buffer              * pSysBufferInput;
    Sgx3DsrvLink_CreateParams  * pPrm;

    pPrm = &pObj->createArgs;

    /*
     * Get Input buffers from previous link for
     * Qid = SGX3DSRV_LINK_IPQID_MULTIVIEW and queue them up locally.
     */
    inputQId = SGX3DSRV_LINK_IPQID_MULTIVIEW;

    System_getLinksFullBuffers(
        pPrm->inQueParams[inputQId].prevLinkId,
        pPrm->inQueParams[inputQId].prevLinkQueId,
        &inputBufList);

    if(inputBufList.numBuf)
    {
        for (bufId = 0; bufId < inputBufList.numBuf; bufId++)
        {
            pSysBufferInput = inputBufList.buffers[bufId];

            /*TBD: Check for parameter correctness. If in error, return input*/

            if (pSysBufferInput != NULL)
            {
                status = OSA_quePut(
                            &(pObj->localInputQ[inputQId].queHandle),
                            (Int32) pSysBufferInput,
                            OSA_TIMEOUT_NONE);
                OSA_assert(status == SYSTEM_LINK_STATUS_SOK);
            }
        }
    }

    /*
     * Get Input buffers from previous link for
     * Qid = SGX3DSRV_LINK_IPQID_PALUT and queue them up locally.
     */
    inputQId = SGX3DSRV_LINK_IPQID_PALUT;

    System_getLinksFullBuffers(
        pPrm->inQueParams[inputQId].prevLinkId,
        pPrm->inQueParams[inputQId].prevLinkQueId,
        &inputBufList);

    if(inputBufList.numBuf)
    {
        for (bufId = 0; bufId < inputBufList.numBuf; bufId++)
        {
            pSysBufferInput = inputBufList.buffers[bufId];
            /*TBD: Check for parameter correctness. If in error, return input*/
            status = OSA_quePut(
                        &(pObj->localInputQ[inputQId].queHandle),
                        (Int32) pSysBufferInput,
                        OSA_TIMEOUT_NONE);
            UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);
            pObj->receivedFirstPALUTFlag = TRUE;
        }
    }

    /*
     * Get Input buffers from previous link for
     * Qid = ALGLINK_SYNTHESIS_IPQID_GALUT and store latest copy locally.
     */
    inputQId = SGX3DSRV_LINK_IPQID_GALUT;
    System_getLinksFullBuffers(
        pPrm->inQueParams[inputQId].prevLinkId,
        pPrm->inQueParams[inputQId].prevLinkQueId,
        &inputBufList);

    if(inputBufList.numBuf)
    {
        for (bufId = 0; bufId < inputBufList.numBuf; bufId++)
        {
            /*
             * At any point in time, Synthesis link will hold only one GA LUT.
             * So whenever GA LUT is received, the previously received one
             * will be released and the newly received one will be archived.
             */
            if(pObj->receivedGALUTFlag == TRUE)
            {
                inputBufListReturn.numBuf     = 1;
                inputBufListReturn.buffers[0] = pObj->sysBufferGALUT;

                if(inputBufListReturn.numBuf)
                {
                    System_putLinksEmptyBuffers(pPrm->inQueParams[inputQId].prevLinkId,
                                                pPrm->inQueParams[inputQId].prevLinkQueId,
                                                &inputBufListReturn);
                }

                pObj->receivedGALUTFlag = FALSE;
            }

            pObj->sysBufferGALUT = inputBufList.buffers[bufId];
            /*TBD: Check for parameter correctness. If in error, return input*/
            pObj->receivedGALUTFlag = TRUE;
#ifdef SYSTEM_DEBUG_DISPLAY
            Vps_printf (" SGX3DSRV Link - GA LUT received !!! \n");
#endif
        }
    }

    /*
     * Get Input buffers from previous link for
     * Qid = SGX3DSRV_LINK_IPQID_BLENDLUT and store latest copy locally.
     */
    inputQId = SGX3DSRV_LINK_IPQID_BLENDLUT;
    System_getLinksFullBuffers(
        pPrm->inQueParams[inputQId].prevLinkId,
        pPrm->inQueParams[inputQId].prevLinkQueId,
        &inputBufList);

    if(inputBufList.numBuf)
    {
        for (bufId = 0; bufId < inputBufList.numBuf; bufId++)
        {
            /*
             * At any point in time, Synthesis link will hold only one GA LUT.
             * So whenever GA LUT is received, the previously received one
             * will be released and the newly received one will be archived.
             */
            if(pObj->receivedBlendLUTFlag == TRUE)
            {
                inputBufListReturn.numBuf     = 1;
                inputBufListReturn.buffers[0] = pObj->sysBufferBlendLUT;

                if(inputBufListReturn.numBuf)
                {
                    System_putLinksEmptyBuffers(pPrm->inQueParams[inputQId].prevLinkId,
                                                pPrm->inQueParams[inputQId].prevLinkQueId,
                                                &inputBufListReturn);
                }

                pObj->receivedBlendLUTFlag = FALSE;
            }

            pObj->sysBufferBlendLUT = inputBufList.buffers[bufId];
            /*TBD: Check for parameter correctness. If in error, return input*/
            pObj->receivedBlendLUTFlag = TRUE;
#ifdef SYSTEM_DEBUG_DISPLAY
            Vps_printf (" SGX3DSRV Link - Blending LUT received !!! \n");
#endif
        }
    }


    pObj->linkStats.newDataCmdCount++;

    return status;
}

/**
 *******************************************************************************
 *
 * \brief This function de-queue and process/sgx3Dsrv the input frames
 *
 *        Function perform the following operations
 *        - De-queue the frames from the previous link output Q
 *        - Put them in intenal link queues
 *        - Get/De-queue the valid frames from the internal queue
 *        - Populate/Map OpenGL/Drm frame structure from system buffer
 *        - Call the OpenGL/DRM process function
 *        - Free-up these frames by send back to previous link after display
 *        - Immediately Free-up the frames which are not part of the display
 *
 * \param   pObj     [IN] Sgx3Dsrv Link Instance handle
 *
 * \return  SYSTEM_LINK_STATUS_SOK
 *
 *******************************************************************************
 */
volatile static unsigned int gDebug_IsCalibDone = 0;
Int32 Sgx3DsrvLink_drvDoProcessFrames(Sgx3DsrvLink_Obj *pObj)
{
    Sgx3DsrvLink_CreateParams  * pPrm;
    Sgx3DsrvLink_InputQueId      inputQId;
    UInt32                       channelId = 0;
    Int32                        status = SYSTEM_LINK_STATUS_SOK;
    System_BufferList            inputBufListReturn;
    System_Buffer              * pSystemBufferMultiview;
    System_Buffer              * pSystemBufferPALUT;
    Bool                         isProcessCallDoneFlag;
    System_MetaDataBuffer      * pPALUTBuffer;
    System_MetaDataBuffer      * pGALUTBuffer;
    System_MetaDataBuffer      * pBlendLUTBuffer;
    System_VideoFrameCompositeBuffer *pVideoCompositeFrame;
    GLuint                       texYuv[4] = {0};
    System_EglTexProperty        texProp;
    System_Buffer              * nextRenderBuf;    
    System_EglPixmapVideoFrameBuffer *eglBuf;

    pPrm = &pObj->createArgs;

    Sgx3DsrvLink_getInputFrameData(pObj);

    /*
     * Continous loop to perform synthesis as long as
     * input buffers are available.
     */
    while(1)
    {
      isProcessCallDoneFlag = FALSE;
      /* Checking if all the inputs are available */

      if(pObj->receivedGALUTFlag == TRUE
              &&
         pObj->receivedBlendLUTFlag == TRUE
              &&
         pObj->receivedFirstPALUTFlag == TRUE
              &&
         OSA_queGetQueuedCount(
              &(pObj->localInputQ[SGX3DSRV_LINK_IPQID_MULTIVIEW].queHandle))>0
	             &&
         OSA_queGetQueuedCount(
              &(pObj->eglObj.eglEmptyBufQue))>0
       )
       {

        pSystemBufferPALUT = NULL;
        status = OSA_queGet(
                    &(pObj->localInputQ[SGX3DSRV_LINK_IPQID_PALUT].
                        queHandle),
                    (Int32 *) &pSystemBufferPALUT,
                    OSA_TIMEOUT_NONE);

        if (pSystemBufferPALUT != NULL)
        {
            pObj->sysBufferPALUT = pSystemBufferPALUT;
        }

        pPALUTBuffer = (System_MetaDataBuffer *)pObj->sysBufferPALUT->payload;
        OSA_assert (pPALUTBuffer != NULL);

        pGALUTBuffer = (System_MetaDataBuffer *)pObj->sysBufferGALUT->payload;
        OSA_assert (pGALUTBuffer != NULL);

        pBlendLUTBuffer = (System_MetaDataBuffer *)pObj->sysBufferBlendLUT->payload;
        OSA_assert (pBlendLUTBuffer != NULL);

        if(pObj->isFirstFrameRecv==FALSE)
        {
            pObj->isFirstFrameRecv = TRUE;

            OSA_resetLinkStatistics(
                    &pObj->linkStats,
                    pObj->numInputChannels,
                    1);

            OSA_resetLatency(&pObj->linkLatency);
            OSA_resetLatency(&pObj->srcToLinkLatency);
        }

        nextRenderBuf = NULL;
        OSA_queGet(&(pObj->eglObj.eglEmptyBufQue), (Int32 *)&nextRenderBuf, OSA_TIMEOUT_NONE);
        OSA_assert(nextRenderBuf != NULL);
        eglBuf = nextRenderBuf->payload;
        System_eglMakeCurrentBuffer(&pObj->eglObj, (EGLSurface)eglBuf->eglSurface);
	
        /*
         * Reaching here means output buffers are available.
         * Hence getting inputs from local Queus
         */
        pSystemBufferMultiview = NULL;
        status = OSA_queGet(
                  &(pObj->localInputQ[SGX3DSRV_LINK_IPQID_MULTIVIEW].
                      queHandle),
                  (Int32 *) &pSystemBufferMultiview,
                  OSA_TIMEOUT_NONE);

        /* Submit the SRV frames to SGX processing & DRM display */
        if (pSystemBufferMultiview != NULL && status == SYSTEM_LINK_STATUS_SOK)
        {
            channelId = pSystemBufferMultiview->chNum;
            if(channelId < pObj->linkStats.numCh)
            {
                pObj->linkStats.chStats[channelId].inBufRecvCount++;
                pObj->linkStats.chStats[channelId].inBufProcessCount++;
                pObj->linkStats.chStats[channelId].outBufCount[0]++;
            }

            pSystemBufferMultiview->linkLocalTimestamp = OSA_getCurGlobalTimeInUsec();

            inputQId = SGX3DSRV_LINK_IPQID_MULTIVIEW;

            texProp.width      = pObj->inQueInfo[inputQId].chInfo[channelId].width;
            texProp.height     = pObj->inQueInfo[inputQId].chInfo[channelId].height;
            texProp.pitch[0]   = pObj->inQueInfo[inputQId].chInfo[channelId].pitch[0];
            texProp.pitch[1]   = pObj->inQueInfo[inputQId].chInfo[channelId].pitch[1];
            texProp.dataFormat = pObj->inDataFormat;

#ifndef USE_STANDALONE_LUT
            // Do we need to specify the stitched frame resolution ?
            // Do we need to set any other parameters in render3DSRVObj ?
            pObj->render3DSRVObj.LUT3D = (void *) pGALUTBuffer->bufAddr[0];
            pObj->render3DSRVObj.blendLUT3D = (void *) pBlendLUTBuffer->bufAddr[0];
            pObj->render3DSRVObj.PALUT3D = (void *) pPALUTBuffer->bufAddr[0];
#else
            //Use standalone version of LUTs
            //FOR DEBUG ONLY
            pObj->render3DSRVObj.LUT3D = (void *) GAlignLUT;
            pObj->render3DSRVObj.blendLUT3D = (void *) BlendLUT3D;
#endif

#if 0  // Enable Dumping out of the tables
            {
            FILE *fp1;
            FILE *fp2;

            gDebug_IsCalibDone++;
            if (gDebug_IsCalibDone == 250)
            {
                fp1=fopen("./3DLUT.bin", "w+b");
                fp2=fopen("./3DBLENDLUT.bin", "w+b");

                fwrite(pObj->render3DSRVObj.LUT3D, (9*POINTS_WIDTH*POINTS_HEIGHT*sizeof(LUT_DATATYPE)), 1, fp1);
                fwrite(pObj->render3DSRVObj.blendLUT3D, (2*POINTS_WIDTH*POINTS_HEIGHT*sizeof(BLENDLUT_DATATYPE)), 1, fp2);
                
                fclose(fp1);
                fclose(fp2);
            }
            }
#endif

            if(pSystemBufferMultiview->bufType==SYSTEM_BUFFER_TYPE_VIDEO_FRAME_CONTAINER)
            {
                pVideoCompositeFrame = (System_VideoFrameCompositeBuffer *)
                                       (pSystemBufferMultiview->payload);
                OSA_assert(pVideoCompositeFrame != NULL);

                /* pick CH0 by default */
                // TODO: check that the A15 mapped address is not NULL
                texYuv[0] = System_eglGetTexYuv(&pObj->eglObj, &texProp, pVideoCompositeFrame->bufAddr[0][0]);
                texYuv[1] = System_eglGetTexYuv(&pObj->eglObj, &texProp, pVideoCompositeFrame->bufAddr[0][1]);
                texYuv[2] = System_eglGetTexYuv(&pObj->eglObj, &texProp, pVideoCompositeFrame->bufAddr[0][2]);
                texYuv[3] = System_eglGetTexYuv(&pObj->eglObj, &texProp, pVideoCompositeFrame->bufAddr[0][3]);
            }
            else
            {
                Vps_printf(" SGX3DSRV Link: ERROR: Recevied invalid buffer type !!!\n");
                OSA_assert(0);
            }
#if 1
            SgxRender3DSRV_renderFrame(
                            &pObj->render3DSRVObj,
                            texYuv
                            );
#else
            OSA_assert (texYuv != NULL);
#endif
            nextRenderBuf->srcTimestamp = pSystemBufferMultiview->srcTimestamp;
            pObj->linkStats.chStats[pSystemBufferMultiview->chNum].outBufCount[0]++;

            OSA_updateLatency(&pObj->linkLatency,
                              pSystemBufferMultiview->linkLocalTimestamp);
            OSA_updateLatency(&pObj->srcToLinkLatency,
                              pSystemBufferMultiview->srcTimestamp);
        }

        System_eglWaitSync(&pObj->eglObj);
        System_eglMakeCurrentNill(&pObj->eglObj);


        isProcessCallDoneFlag = TRUE;

        /*
         * Releasing (Free'ing) Input buffers, since algorithm does not need
         * it for any future usage.
         */
        if (pSystemBufferMultiview != NULL)
        {
          inputQId                      = SGX3DSRV_LINK_IPQID_MULTIVIEW;
          inputBufListReturn.numBuf     = 1;
          inputBufListReturn.buffers[0] = pSystemBufferMultiview;
          if(inputBufListReturn.numBuf)
          {
              System_putLinksEmptyBuffers(pPrm->inQueParams[inputQId].prevLinkId,
                                          pPrm->inQueParams[inputQId].prevLinkQueId,
                                          &inputBufListReturn);
          }
        }

        if (pSystemBufferPALUT != NULL)
        {
          inputQId                      = SGX3DSRV_LINK_IPQID_PALUT;
          inputBufListReturn.numBuf     = 1;
          inputBufListReturn.buffers[0] = pSystemBufferPALUT;
          if(inputBufListReturn.numBuf)
          {
              System_putLinksEmptyBuffers(pPrm->inQueParams[inputQId].prevLinkId,
                                          pPrm->inQueParams[inputQId].prevLinkQueId,
                                          &inputBufListReturn);
          }
        }


        OSA_quePut(&(pObj->eglObj.eglFullBufQue), (Int32)nextRenderBuf, OSA_TIMEOUT_FOREVER);
        System_sendLinkCmd(pObj->createArgs.outQueParams.nextLink,
                           SYSTEM_CMD_NEW_DATA, NULL);
      }

      if(isProcessCallDoneFlag == FALSE)
      {
          /* TBD - the drop frame staus update is kept here temperally.
           * This should done if output buffer is not available, and this
           * will be done when DRM is removed out from this link */
          pObj->linkStats.inBufErrorCount++;
          break;
      }
    } /* End of while(1) */

    return SYSTEM_LINK_STATUS_SOK;
}

/**
 *******************************************************************************
 *
 * \brief This function delete the sgx3Dsrv link & driver
 *
 *        De-queue any frames which are held inside the driver, then
 *        - Delete the simply driver
 *        - delete the semaphore and other link data structures
 *        - delete the link periodic object
 *
 * \param   pObj     [IN] Sgx3Dsrv Link Instance handle
 *
 * \return  SYSTEM_LINK_STATUS_SOK
 *
 *******************************************************************************
 */
Int32 Sgx3DsrvLink_drvDelete(Sgx3DsrvLink_Obj *pObj)
{
    Int32 status = SYSTEM_LINK_STATUS_SOK;
    Sgx3DsrvLink_InputQueId inputQId;

#ifdef SYSTEM_DEBUG_DISPLAY
    Vps_printf(" SGX3DSRV Link: Delete in progress !!!\n");
#endif

    /*
     * Deletion of local input Qs for SGX3DSRV_LINK_IPQID_MULTIVIEW and
     * SGX3DSRV_LINK_IPQID_PALUT.
     * For ALGLINK_SYNTHESIS_IPQID_GALUT, always just one entry is kept.
     */
    inputQId = SGX3DSRV_LINK_IPQID_MULTIVIEW;
    status = OSA_queDelete(&(pObj->localInputQ[inputQId].queHandle));
    UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

    inputQId = SGX3DSRV_LINK_IPQID_PALUT;
    status = OSA_queDelete(&(pObj->localInputQ[inputQId].queHandle));
    UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

    status = System_eglClose(&pObj->eglObj);
    OSA_assert(status==SYSTEM_LINK_STATUS_SOK);

    /* For SGX profiling */
    system_delete_pvrscope_profile_data(&pObj->pvrscopeObj);

#ifdef SYSTEM_DEBUG_DISPLAY
    Vps_printf(" SGX3DSRV Link: Delete Done !!!\n");
#endif

    return SYSTEM_LINK_STATUS_SOK;
}

/**
 *******************************************************************************
 *
 * \brief This function start the simply driver
 *
 *        Primming of a few frames are required to start the Sgx3Dsrv driver.
 *        Use blank buffers to prime and start the simply driver even
 *        before the actual frames are received by the sgx3Dsrv link. This
 *        primming is done while sgx3Dsrv link create. Start shall be called
 *        only after the link create function
 *
 * \param   pObj     [IN] Sgx3Dsrv Link Instance handle
 *
 * \return  status
 *
 *******************************************************************************
 */
Int32 Sgx3DsrvLink_drvStart(Sgx3DsrvLink_Obj *pObj)
{
    Int32 status = SYSTEM_LINK_STATUS_SOK;

#ifdef SYSTEM_DEBUG_DISPLAY
    Vps_printf(" SGX3DSRV Link: Start in progress !!!\n");
#endif

#ifdef SYSTEM_DEBUG_DISPLAY
    Vps_printf(" SGX3DSRV Link: Start Done !!!\n");
#endif

    return status;
}

/**
 *******************************************************************************
 *
 * \brief This function stop the simply driver
 *
 *        When ever the driver is stopped, enable the sgx3Dsrv link periodic
 *        call back function. This will initiate to free-up the input frames
 *        in STOP state. The driver call back will be stopped when sgx3Dsrv
 *        driver stop is done
 *
 * \param   pObj     [IN] Sgx3Dsrv Link Instance handle
 *
 * \return  status
 *
 *******************************************************************************
 */
Int32 Sgx3DsrvLink_drvStop(Sgx3DsrvLink_Obj *pObj)
{
    Int32 status = SYSTEM_LINK_STATUS_SOK;

#ifdef SYSTEM_DEBUG_DISPLAY
    Vps_printf(" SGX3DSRV Link: Stop in progress !!!\n");
#endif

#ifdef SYSTEM_DEBUG_DISPLAY
    Vps_printf(" SGX3DSRV Link: Stop Done !!!\n");
#endif

    return status;
}

/* Nothing beyond this point */

