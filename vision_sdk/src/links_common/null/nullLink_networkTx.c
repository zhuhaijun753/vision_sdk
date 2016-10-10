/*
 ******************************************************************************
 *
 * Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 ******************************************************************************
 */


 /*****************************************************************************
  *  INCLUDE FILES
  *****************************************************************************
  */
#include "nullLink_priv.h"

#define NETWORK_TX_SERVER_CLOSED        (0U)
#define NETWORK_TX_SERVER_LISTEN        (1U)
#define NETWORK_TX_SERVER_CONNECTED     (2U)

#define NETWORK_TX_SERVER_POLL_TIMEOUT  (10U)

/*****************************************************************************
 *  Function declarations
 *****************************************************************************
 */
Int32 NullLink_networkTxWaitConnect(NullLink_Obj *pObj,
                                    NullLink_NetworkTxObj *pNetTxObj);
Int32 NullLink_networkTxWriteHeader(NullLink_Obj *pObj,
                                    NullLink_NetworkTxObj *pNetTxObj,
                                    NetworkRx_CmdHeader *pHeader);
Int32 NullLink_networkTxWritePayload(NullLink_Obj *pObj,
                        NullLink_NetworkTxObj *pNetTxObj,
                        UInt32 numBuf,
                        UInt8  *bufAddr[],
                        UInt32 bufSize[]);

Int32 NullLink_networkTxCreate(NullLink_Obj *pObj)
{
    NullLink_NetworkTxObj *pNetTxObj = &pObj->netTxObj;
    Int32 status;

    Network_sessionOpen(NULL);

    status = Network_open(
                    &pNetTxObj->sockObj,
                    pObj->createArgs.networkServerPort
                    );
    UTILS_assert(status==SYSTEM_LINK_STATUS_SOK);

    pNetTxObj->state = NETWORK_TX_SERVER_LISTEN;

    Vps_printf(" NULL: NETWORK_TX: Server listening (port=%d) !!!\n",
        pObj->createArgs.networkServerPort);

    return status;
}

Int32 NullLink_networkTxDelete(NullLink_Obj *pObj)
{
    NullLink_NetworkTxObj *pNetTxObj = &pObj->netTxObj;
    Int32 status;

    status = Network_close(&pNetTxObj->sockObj, (Bool)TRUE);
    UTILS_assert(status==SYSTEM_LINK_STATUS_SOK);

    Network_sessionClose(NULL);

    pNetTxObj->state = NETWORK_TX_SERVER_CLOSED;

    Vps_printf(" NULL: NETWORK_TX: Server Closed (port=%d) !!!\n",
        pObj->createArgs.networkServerPort);

    return status;
}

Int32 NullLink_networkTxWaitConnect(NullLink_Obj *pObj,
                                    NullLink_NetworkTxObj *pNetTxObj)
{
    Int32 status = SYSTEM_LINK_STATUS_SOK;
    Int32 isConnected;

    if(pNetTxObj->state == NETWORK_TX_SERVER_LISTEN)
    {
        isConnected = Network_waitConnect(
                    &pNetTxObj->sockObj,
                    NETWORK_TX_SERVER_POLL_TIMEOUT);

        /* MISRA.UNREACH.GEN
         * MISRAC_2004 Rule_14.1
         * MISRAC_WAIVER:
         * Code is currently unreachable but as the implementation of the
         * function changes, it will not be unreachable.
         */
        if((Int32)TRUE == isConnected)
        {
            /* connected to client */
            pNetTxObj->state = NETWORK_TX_SERVER_CONNECTED;
            Vps_printf(" NULL: NETWORK_TX: Connected to client (port=%d) !!!\n",
                pObj->createArgs.networkServerPort);
        }
        else
        {
            status = SYSTEM_LINK_STATUS_EFAIL;
        }
    }

    return status;
}

Int32 NullLink_networkTxWriteHeader(NullLink_Obj *pObj,
                                    NullLink_NetworkTxObj *pNetTxObj,
                                    NetworkRx_CmdHeader *pHeader)
{
    Int32 status = SYSTEM_LINK_STATUS_SOK;

    pHeader->header = NETWORK_TX_HEADER;

    /* MISRA.CAST.PTR_TO_INT
     * MISRAC_2004 Rule_11.3
     * MISRAC_WAIVER:
     * This is used to ensure address alignment required by hardware.
     */
    /* write header to client, if write failed, then client
     * disconnected, so go back to listening
     */
    status = Network_write(&pNetTxObj->sockObj,
                (UInt8*)pHeader,
                sizeof(NetworkRx_CmdHeader)
                );

    /* MISRA.UNREACH.GEN
     * MISRAC_2004 Rule_14.1
     * MISRAC_WAIVER:
     * Code is currently unreachable but as the implementation of the
     * function changes, it will not be unreachable.
     */
    if(status!=SYSTEM_LINK_STATUS_SOK)
    {
        Vps_printf(" NULL: NETWORK_TX: Disconnected from client "
                   "while writing header (port=%d)!!!\n",
                    pObj->createArgs.networkServerPort
                   );

        Network_close(&pNetTxObj->sockObj, FALSE);
        pNetTxObj->state = NETWORK_TX_SERVER_LISTEN;
    }

    return status;
}

Int32 NullLink_networkTxWritePayload(NullLink_Obj *pObj,
                        NullLink_NetworkTxObj *pNetTxObj,
                        UInt32 numBuf,
                        UInt8  *bufAddr[],
                        UInt32 bufSize[])
{
    Int32 status = SYSTEM_LINK_STATUS_SOK;
    UInt32 i;
    Ptr alignedBufPtr;
    UInt32 alignedBufSize;
    UInt32 bufAddrInInt;

    for(i=0; i<numBuf; i++)
    {
        if(status==SYSTEM_LINK_STATUS_SOK)
        {
            alignedBufSize =  bufSize[i] + 128U;
            alignedBufSize = SystemUtils_align(alignedBufSize, 128U);
            /* MISRA.CAST.PTR_TO_INT
             * MISRAC_2004 Rule_11.3
             * MISRAC_WAIVER:
             * This is used to ensure address alignment required by hardware.
             */
            bufAddrInInt = (UInt32)bufAddr[i];
            bufAddrInInt = SystemUtils_floor(bufAddrInInt, 128U);
            alignedBufPtr = (Ptr)bufAddrInInt;
            Cache_inv(
                      alignedBufPtr,
                      alignedBufSize,
                      (UInt16)Cache_Type_ALLD,
                      (Bool)TRUE
                    );

            status = Network_write(&pNetTxObj->sockObj,
                        bufAddr[i],
                        bufSize[i]
                        );

            /* MISRA.UNREACH.GEN
             * MISRAC_2004 Rule_14.1
             * MISRAC_WAIVER:
             * Code is currently unreachable but as the implementation of the
             * function changes, it will not be unreachable.
             */
            if(status!=SYSTEM_LINK_STATUS_SOK)
            {
                Vps_printf(" NULL: NETWORK_TX: Disconnected from client "
                           "while writing payload (port=%d)!!!\n",
                            pObj->createArgs.networkServerPort
                           );
                Network_close(&pNetTxObj->sockObj, FALSE);
                pNetTxObj->state = NETWORK_TX_SERVER_LISTEN;
                break;
            }

            #if 0
            Vps_printf(" NULL: NETWORK_TX: BUF%d: %d bytes sent (port=%d)!!!\n",
                i,
                bufSize[i],
                pObj->createArgs.networkServerPort
                );
            #endif
        }
    }

    return status;
}

Int32 NullLink_networkTxSendData(NullLink_Obj * pObj, UInt32 queId,
                                 UInt32 channelId, UInt32 networkChId,
                                 const System_Buffer *pBuffer)
{
    NullLink_NetworkTxObj *pNetTxObj = &pObj->netTxObj;
    Int32 status = SYSTEM_LINK_STATUS_SOK;
    System_VideoDataFormat dataFormat;
    System_VideoFrameBuffer *videoFrame;
    System_BitstreamBuffer *bitstreamBuf;
    System_MetaDataBuffer *metaBuf;
    System_LinkChInfo *pChInfo;
    UInt16 i;

    status = NullLink_networkTxWaitConnect(pObj, pNetTxObj);

    if(pNetTxObj->state == NETWORK_TX_SERVER_CONNECTED)
    {
        NetworkRx_CmdHeader cmdHeader;
        UInt32 bufSize;

        memset(&cmdHeader, 0, sizeof(cmdHeader));

        cmdHeader.chNum = networkChId;

        pChInfo = &pObj->inQueInfo[queId].chInfo[channelId];

        switch(pBuffer->bufType)
        {
            case SYSTEM_BUFFER_TYPE_BITSTREAM:
                bitstreamBuf = (System_BitstreamBuffer *)pBuffer->payload;

                /* right now only MJPEG TX is supported */
                cmdHeader.payloadType = NETWORK_RX_TYPE_BITSTREAM_MJPEG;
                cmdHeader.dataSize = bitstreamBuf->fillLength; /* max buffer size */
                break;

            case SYSTEM_BUFFER_TYPE_METADATA:
                metaBuf = (System_MetaDataBuffer *)pBuffer->payload;

                /* right now only MJPEG TX is supported */
                cmdHeader.payloadType = NETWORK_RX_TYPE_META_DATA;
                cmdHeader.dataSize = 0; /* max buffer size */

                for(i=0; i<metaBuf->numMetaDataPlanes; i++)
                {
                    cmdHeader.dataSize += metaBuf->metaFillLength[i];
                }
                break;

            case SYSTEM_BUFFER_TYPE_VIDEO_FRAME:
                videoFrame = (System_VideoFrameBuffer*)pBuffer->payload;
                dataFormat = (System_VideoDataFormat)
                    System_Link_Ch_Info_Get_Flag_Data_Format(pChInfo->flags);

                if(dataFormat == SYSTEM_DF_YUV420SP_UV)
                {
                    bufSize =
                    (pChInfo->pitch[0U] * pChInfo->height) +
                    ((pChInfo->pitch[1U] * pChInfo->height) / 2U);

                    cmdHeader.payloadType =
                        NETWORK_RX_TYPE_VIDEO_FRAME_YUV420SP_UV;
                }
                else
                {
                    bufSize = pChInfo->pitch[0U] * pChInfo->height;

                    cmdHeader.payloadType =
                        NETWORK_RX_TYPE_VIDEO_FRAME_YUV422I_YUYV;
                }

                if(status==SYSTEM_LINK_STATUS_SOK)
                {
                    cmdHeader.dataSize = bufSize; /* max buffer size */
                    cmdHeader.width = pChInfo->width;
                    cmdHeader.height = pChInfo->height;
                    cmdHeader.pitch[0U] = pChInfo->pitch[0U];
                    cmdHeader.pitch[1U] = pChInfo->pitch[1U];
                }

                break;

            default:
                status = SYSTEM_LINK_STATUS_EFAIL;
                break;
        }

        if(status==SYSTEM_LINK_STATUS_SOK)
        {
            status = NullLink_networkTxWriteHeader(pObj, pNetTxObj, &cmdHeader);
        }
        if(status==SYSTEM_LINK_STATUS_SOK)
        {
            UInt32 numBuf;
            UInt8 *dataAddr[SYSTEM_MAX_META_DATA_PLANES];
            UInt32 dataSize[SYSTEM_MAX_META_DATA_PLANES];

            /* read payload data */
            switch(pBuffer->bufType)
            {
                case SYSTEM_BUFFER_TYPE_BITSTREAM:
                    numBuf = 1U;
                    dataAddr[0U] = bitstreamBuf->bufAddr;
                    dataSize[0U] = cmdHeader.dataSize;
                    break;

                case SYSTEM_BUFFER_TYPE_METADATA:
                    numBuf = metaBuf->numMetaDataPlanes;

                    for(i=0; i<metaBuf->numMetaDataPlanes; i++)
                    {
                        dataAddr[i] = metaBuf->bufAddr[i];
                        dataSize[i] = metaBuf->metaFillLength[i];
                    }
                    break;

                case SYSTEM_BUFFER_TYPE_VIDEO_FRAME:

                    if(dataFormat == SYSTEM_DF_YUV420SP_UV)
                    {
                        numBuf = 2U;
                        dataAddr[0U] = videoFrame->bufAddr[0U];
                        dataAddr[1U] = videoFrame->bufAddr[1U];
                        dataSize[0U] = cmdHeader.pitch[0U]*cmdHeader.height;
                        dataSize[1U] =
                            (cmdHeader.pitch[1U] * cmdHeader.height)/2U;

                        UTILS_assert(
                            (dataSize[0U]+dataSize[1U]) == cmdHeader.dataSize );
                    }
                    else
                    {
                        numBuf = 1U;
                        dataAddr[0] = videoFrame->bufAddr[0U];
                        dataSize[0] =
                            cmdHeader.pitch[0U] * cmdHeader.height * 2U;
                        UTILS_assert( dataSize[0] == cmdHeader.dataSize );
                    }
                    break;

                default:
                    /* Can never reach here */
                    UTILS_assert((Bool)FALSE);
                    break;
            }

            status = NullLink_networkTxWritePayload(
                                pObj, pNetTxObj, numBuf, dataAddr, dataSize);
        }
    }

    return status;
}
