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
 * \file fpComputeLink_algPlugin.c
 *
 * \brief  This file contains plug in functions for feature Plane computation
 *         Link
 *
 * \version 0.0 (Nov 2015) : [YM] First version
 *
 *******************************************************************************
*/

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */

#include "fpComputeLink_priv.h"


/**
 *******************************************************************************
 *
 * \brief Implementation of function to init plugins()
 *
 *        This function will be called by AlgorithmLink_initAlgPlugins, so as
 *        register plugins of gAlign algorithm
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
 */
Int32 AlgorithmLink_fpCompute_initPlugin()
{
    AlgorithmLink_FuncTable pluginFunctions;
    UInt32 algId = (UInt32)-1;

    pluginFunctions.AlgorithmLink_AlgPluginCreate =
        AlgorithmLink_fpComputeCreate;
    pluginFunctions.AlgorithmLink_AlgPluginProcess =
        AlgorithmLink_fpComputeProcess;
    pluginFunctions.AlgorithmLink_AlgPluginControl =
        AlgorithmLink_fpComputeControl;
    pluginFunctions.AlgorithmLink_AlgPluginStop =
        AlgorithmLink_fpComputeStop;
    pluginFunctions.AlgorithmLink_AlgPluginDelete =
        AlgorithmLink_fpComputeDelete;

#ifdef BUILD_ARP32
    algId = ALGORITHM_LINK_EVE_ALG_FPCOMPUTE;
#endif

    AlgorithmLink_registerPlugin(algId, &pluginFunctions);

    return SYSTEM_LINK_STATUS_SOK;
}

/**
 *******************************************************************************
 *
 * \brief Implementation of Create Plugin for feature plane computation alg link
 *
 *
 * \param  pObj              [IN] Algorithm link object handle
 * \param  pCreateParams     [IN] Pointer to create time parameters
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
 */
Int32 AlgorithmLink_fpComputeCreate(void * pObj,void * pCreateParams)
{
    UInt32 status;
    UInt32 prevLinkQId;
    System_LinkInfo                                     prevLinkInfo;
    AlgorithmLink_FPComputeObj                          * pFPlaneObj;
    AlgorithmLink_FPComputeCreateParams                 * pLinkCreateParams;
    AlgorithmLink_OutputQueueInfo                       * pOutputQInfo;
    AlgorithmLink_InputQueueInfo                        * pInputQInfo;
    System_LinkChInfo                                   * pOutChInfo;
    System_LinkChInfo                                   * pPrevChInfo;

    pFPlaneObj = (AlgorithmLink_FPComputeObj *)
                    Utils_memAlloc(
                        UTILS_HEAPID_DDR_CACHED_LOCAL,
                        sizeof(AlgorithmLink_FPComputeObj),
                        32);
    UTILS_assert(pFPlaneObj != NULL);

    AlgorithmLink_setAlgorithmParamsObj(pObj, pFPlaneObj);

    pLinkCreateParams = (AlgorithmLink_FPComputeCreateParams *)
                         pCreateParams;
    pInputQInfo       = &pFPlaneObj->inputQInfo;
    pOutputQInfo      = &pFPlaneObj->outputQInfo;

    /*
     * Taking copy of needed create time parameters in local object for future
     * reference.
     */
    memcpy(
            (void*)(&pFPlaneObj->algLinkCreateParams),
            (void*)(pLinkCreateParams),
            sizeof(AlgorithmLink_FPComputeCreateParams)
           );

    pInputQInfo->qMode          = ALGORITHM_LINK_QUEUEMODE_NOTINPLACE;
    pOutputQInfo->qMode         = ALGORITHM_LINK_QUEUEMODE_NOTINPLACE;
    pOutputQInfo->queInfo.numCh = 1;

    status = System_linkGetInfo(
                                  pLinkCreateParams->inQueParams.prevLinkId,
                                  &prevLinkInfo
                                );
    UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

    UTILS_assert(prevLinkInfo.numQue >= 1);

    prevLinkQId = pLinkCreateParams->inQueParams.prevLinkQueId;

    pFPlaneObj->inChInfo = prevLinkInfo.queInfo[prevLinkQId].chInfo[0];
    pPrevChInfo = &pFPlaneObj->inChInfo;

    if(System_Link_Ch_Info_Get_Flag_Data_Format(pPrevChInfo->flags)
        !=
        SYSTEM_DF_YUV420SP_UV)
    {
      UTILS_assert(0);
    }

    UTILS_assert(
                  (pPrevChInfo->width  >= pLinkCreateParams->imgFrameWidth)
                  ||
                  (pPrevChInfo->height >= pLinkCreateParams->imgFrameHeight)
                );

    pOutChInfo          = &pOutputQInfo->queInfo.chInfo[0];
    pOutChInfo->flags   = 0;
    pOutChInfo->height  = pLinkCreateParams->imgFrameHeight;
    pOutChInfo->width   = pLinkCreateParams->imgFrameWidth;

    /*
     * Initializations needed for book keeping of buffer handling.
     * Note that this needs to be called only after setting inputQMode and
     * outputQMode.
     */
    AlgorithmLink_queueInfoInit(
                                pObj,
                                1,
                                pInputQInfo,
                                1,
                                pOutputQInfo
                               );


    pFPlaneObj->fpComputeTime = 0;
    pFPlaneObj->frameCount = 0;


    pFPlaneObj->linkStatsInfo = Utils_linkStatsCollectorAllocInst(
        AlgorithmLink_getLinkId(pObj), "ALG_FPCOMPUTE");
    UTILS_assert(NULL != pFPlaneObj->linkStatsInfo);

    return SYSTEM_LINK_STATUS_SOK;
}

Int32 AlgorithmLink_fpComputeProcess(void * pObj)
{
    UInt32 bufId;
    Int32  status = SYSTEM_LINK_STATUS_SOK;
    Bool   bufDropFlag;
    AlgorithmLink_FPComputeObj                          * pFPlaneObj;
    System_BufferList                                     inputBufList;
    AlgorithmLink_FPComputeCreateParams                 * pLinkCreateParams;
    System_Buffer                                       * pSysOutBuffer;
    System_Buffer                                       * pSysInBuffer;
    System_MetaDataBuffer                               * pInMetaDataBuffer;
    System_MetaDataBuffer                               * pOutMetaDataBuffer;
    System_BufferList                                     bufListReturn;
    System_LinkStatistics                               * linkStatsInfo;
    UInt64 curTime;

    pFPlaneObj = (AlgorithmLink_FPComputeObj *)
                                AlgorithmLink_getAlgorithmParamsObj(pObj);

    linkStatsInfo = pFPlaneObj->linkStatsInfo;
    UTILS_assert(NULL != linkStatsInfo);

    pLinkCreateParams = &pFPlaneObj->algLinkCreateParams;

    if (pFPlaneObj->isFirstFrameRecv == FALSE)
    {
        pFPlaneObj->isFirstFrameRecv = TRUE;

        Utils_resetLinkStatistics(&linkStatsInfo->linkStats, 1, 1);

        Utils_resetLatency(&linkStatsInfo->linkLatency);
        Utils_resetLatency(&linkStatsInfo->srcToLinkLatency);
    }

    Utils_linkStatsCollectorProcessCmd(linkStatsInfo);

    linkStatsInfo->linkStats.newDataCmdCount++;

    System_getLinksFullBuffers(
                        pLinkCreateParams->inQueParams.prevLinkId,
                        pLinkCreateParams->inQueParams.prevLinkQueId,
                        &inputBufList);

    if (inputBufList.numBuf)
    {
        for (bufId = 0; bufId < inputBufList.numBuf; bufId++)
        {
            pSysInBuffer = inputBufList.buffers[bufId];
            if(pSysInBuffer == NULL)
            {
                linkStatsInfo->linkStats.inBufErrorCount++;
                continue;
            }

            linkStatsInfo->linkStats.chStats[0].inBufRecvCount++;

            status = AlgorithmLink_getEmptyOutputBuffer(
                                                        pObj,
                                                        0,
                                                        0,
                                                        &pSysOutBuffer
                                                        );
            if(status != SYSTEM_LINK_STATUS_SOK)
            {
                 linkStatsInfo->linkStats.chStats
                                [0].inBufDropCount++;
                 linkStatsInfo->linkStats.chStats
                                [0].outBufDropCount[0]++;
            }
            else
            {
                pSysOutBuffer->srcTimestamp = pSysInBuffer->srcTimestamp;
                pSysOutBuffer->frameId = pSysInBuffer->frameId;
                pSysOutBuffer->linkLocalTimestamp = Utils_getCurGlobalTimeInUsec();

                pInMetaDataBuffer  = (System_MetaDataBuffer *)pSysInBuffer->payload;
                pOutMetaDataBuffer = (System_MetaDataBuffer *)pSysOutBuffer->payload;

                curTime = Utils_getCurGlobalTimeInUsec();

                /* Logic for new fp compute*/

                Alg_fpComputeProcess(
                    &pFPlaneObj->algFPComputeObj,
                    pInMetaDataBuffer->bufAddr[0],
                    pOutMetaDataBuffer->bufAddr[0]
                    );

                curTime = Utils_getCurGlobalTimeInUsec() - curTime;

                pFPlaneObj->fpComputeTime += curTime;

                pFPlaneObj->frameCount++;

                #if 0
                    Vps_printf(
                        " ALG_FPCOMPUTE: fpCompute (avg) = %d us\n",
                            pFPlaneObj->fpComputeTime/pFPlaneObj->frameCount
                        );
                    pFPlaneObj->fpComputeTime = 0;
                    pFPlaneObj->frameCount = 0;
                }
                #endif

                Utils_updateLatency(&linkStatsInfo->linkLatency,
                                    pSysOutBuffer->linkLocalTimestamp);
                Utils_updateLatency(&linkStatsInfo->srcToLinkLatency,
                                    pSysOutBuffer->srcTimestamp);

                linkStatsInfo->linkStats.chStats
                                [0].inBufProcessCount++;
                linkStatsInfo->linkStats.chStats
                                [0].outBufCount[0]++;

                status = AlgorithmLink_putFullOutputBuffer(
                                                pObj,
                                                0,
                                                pSysOutBuffer);
                UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

                /*
                 * Informing next link that a new data has peen put for its
                 * processing
                 */
                System_sendLinkCmd(
                      pLinkCreateParams->outQueParams.nextLink,
                      SYSTEM_CMD_NEW_DATA,
                      NULL);
                /*
                 * Releasing (Free'ing) output buffers, since algorithm
                 * does not need it for any future usage.
                 */
                bufListReturn.numBuf = 1;
                bufListReturn.buffers[0] = pSysOutBuffer;

                AlgorithmLink_releaseOutputBuffer(
                                                  pObj,
                                                  0,
                                                  &bufListReturn
                                                  );
            }
            bufListReturn.numBuf = 1;
            bufListReturn.buffers[0] = pSysInBuffer;
            bufDropFlag = FALSE;
            AlgorithmLink_releaseInputBuffer(
                              pObj,
                              0,
                              pLinkCreateParams->inQueParams.prevLinkId,
                              pLinkCreateParams->inQueParams.prevLinkQueId,
                              &bufListReturn,
                              &bufDropFlag);
        }
    }

    return status;
}

Int32 AlgorithmLink_fpComputeControl(void * pObj,
                                     void * pControlParams)
{
    AlgorithmLink_FPComputeObj          * pFPlaneObj;
    AlgorithmLink_ControlParams         * pAlgLinkControlPrm;
    AlgorithmLink_FPComputeSetROIParams * pROIPrm;
    AlgorithmLink_ImgPyramidInfo        * pImgPmdInfo;
    AlgorithmLink_ImgPyramidImgPyramidInfoControlParams *imgPyramidInfoControlParams;
    AlgorithmLink_FPComputeCreateParams * pLinkCreateParams;

    System_Buffer                       * pSystemBuffer;
    System_MetaDataBuffer               * pMetaDataBuffer;
    UInt32                                outBufferSize;
    UInt32                                bufId;

    Int32                        status    = SYSTEM_LINK_STATUS_SOK;

    pFPlaneObj = (AlgorithmLink_FPComputeObj *)
                        AlgorithmLink_getAlgorithmParamsObj(pObj);

    pLinkCreateParams = &(pFPlaneObj->algLinkCreateParams);

    pAlgLinkControlPrm = (AlgorithmLink_ControlParams *)pControlParams;

    /*
     * There can be other commands to alter the properties of the alg link
     * or properties of the core algorithm.
     * In this simple example, there is just a control command to print
     * statistics and a default call to algorithm control.
     */

    switch(pAlgLinkControlPrm->controlCmd)
    {

        case ALGORITHM_LINK_FPCOMPUTE_CMD_CREATE_ALG:
                imgPyramidInfoControlParams = (AlgorithmLink_ImgPyramidImgPyramidInfoControlParams *)
                                        pControlParams;

                if (NULL != imgPyramidInfoControlParams)
                {
                    pImgPmdInfo = &(imgPyramidInfoControlParams->imgPyramidInfo);

                    Alg_fpComputeCreate(
                        &pFPlaneObj->algFPComputeObj,
                        pImgPmdInfo,
                        &pFPlaneObj->outBufferSize
                        );

                    /*
                     * Allocate memory for the output buffers and link metadata buffer with
                     * system Buffer
                     */

                    outBufferSize   =   pFPlaneObj->outBufferSize;

                    for (bufId = 0; bufId < pLinkCreateParams->numOutBuffers; bufId++)
                    {
                        pSystemBuffer       =   &pFPlaneObj->buffers[bufId];
                        pMetaDataBuffer     =   &pFPlaneObj->featurePlanes[bufId];

                        /*
                         * Properties of pSystemBuffer, which do not get altered during
                         * run time (frame exchanges) are initialized here
                         */
                        pSystemBuffer->bufType      =   SYSTEM_BUFFER_TYPE_METADATA;
                        pSystemBuffer->payload      =   pMetaDataBuffer;
                        pSystemBuffer->payloadSize  =   sizeof(System_MetaDataBuffer);
                        pSystemBuffer->chNum        =   0;

                        pMetaDataBuffer->numMetaDataPlanes  =   1;
                        pMetaDataBuffer->metaBufSize[0]     =   outBufferSize;
                        pMetaDataBuffer->metaFillLength[0]  =   outBufferSize;
                        pMetaDataBuffer->bufAddr[0]         =   Utils_memAlloc(
                                                                    UTILS_HEAPID_DDR_CACHED_SR,
                                                                    outBufferSize,
                                                                    ALGORITHMLINK_FRAME_ALIGN
                                                                    );
                        UTILS_assert(pMetaDataBuffer->bufAddr[0] != NULL);
                        pMetaDataBuffer->flags = 0;

                        AlgorithmLink_putEmptyOutputBuffer(pObj, 0, pSystemBuffer);
                    }
                }
                else
                {
                    status = SYSTEM_LINK_STATUS_EINVALID_PARAMS;
                }

            break;

        case ALGORITHM_LINK_FPCOMPUTE_CMD_SET_ROI:
            if(pAlgLinkControlPrm->size != sizeof(*pROIPrm))
            {
                status = SYSTEM_LINK_STATUS_EINVALID_PARAMS;
            }
            else
            {
                pROIPrm = (AlgorithmLink_FPComputeSetROIParams *)
                                    pControlParams;

                pFPlaneObj->algLinkCreateParams.imgFrameStartX
                    = pROIPrm->imgFrameStartX;

                pFPlaneObj->algLinkCreateParams.imgFrameStartY
                    = pROIPrm->imgFrameStartY;

                pFPlaneObj->algLinkCreateParams.imgFrameWidth
                    = pROIPrm->imgFrameWidth;

                pFPlaneObj->algLinkCreateParams.imgFrameHeight
                    = pROIPrm->imgFrameHeight;
            }
            break;

        case SYSTEM_CMD_PRINT_STATISTICS:
            AlgorithmLink_fpComputePrintStatistics(pObj,
                                                   pFPlaneObj);
            break;

        default:
            break;
    }

    return status;
}

Int32 AlgorithmLink_fpComputeStop(void * pObj)
{
    return 0;
}

Int32 AlgorithmLink_fpComputeDelete(void * pObj)
{
    Int32  status = SYSTEM_LINK_STATUS_SOK;
    UInt32 bufId;
    AlgorithmLink_FPComputeObj            * pFPlaneObj;
    AlgorithmLink_FPComputeCreateParams   * pLinkCreateParams;
    System_MetaDataBuffer                 * pMetaDataBuffer;

    pFPlaneObj = (AlgorithmLink_FPComputeObj *)
                                AlgorithmLink_getAlgorithmParamsObj(pObj);

    pLinkCreateParams = &pFPlaneObj->algLinkCreateParams;

    status = Utils_linkStatsCollectorDeAllocInst(pFPlaneObj->linkStatsInfo);
    UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

    Alg_fpComputeDelete(&pFPlaneObj->algFPComputeObj);

    /*
     * Free link buffers
     */
    for (bufId = 0; bufId < pLinkCreateParams->numOutBuffers; bufId++)
    {
        pMetaDataBuffer     =   &pFPlaneObj->featurePlanes[bufId];


        status = Utils_memFree(
                                UTILS_HEAPID_DDR_CACHED_SR,
                                pMetaDataBuffer->bufAddr[0],
                                pMetaDataBuffer->metaBufSize[0]
                               );
        UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

    }

    Utils_memFree(
                       UTILS_HEAPID_DDR_CACHED_LOCAL,
                       pFPlaneObj,
                       sizeof(AlgorithmLink_FPComputeObj)
                    );
    return status;
}

Int32 AlgorithmLink_fpComputePrintStatistics(void *pObj,
                AlgorithmLink_FPComputeObj *pFPComputeObj)
{
    UTILS_assert(NULL != pFPComputeObj->linkStatsInfo);

    Utils_printLinkStatistics(&pFPComputeObj->linkStatsInfo->linkStats,
                            "ALG_FPCOMPUTE",
                            TRUE);

    Utils_printLatency("ALG_FPCOMPUTE",
                       &pFPComputeObj->linkStatsInfo->linkLatency,
                       &pFPComputeObj->linkStatsInfo->srcToLinkLatency,
                        TRUE
                       );

    return SYSTEM_LINK_STATUS_SOK;
}

