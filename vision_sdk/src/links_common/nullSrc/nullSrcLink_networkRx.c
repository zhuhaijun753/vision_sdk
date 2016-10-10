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
#include "nullSrcLink_priv.h"

 /*****************************************************************************
  *  Function declarations
  *****************************************************************************
  */
Int32 NullSrcLink_networkRxWaitConnect(NullSrcLink_Obj *pObj,
                                       NullSrcLink_NetworkRxObj *pNetRxObj);
Int32 NullSrcLink_networkRxWriteHeader(NullSrcLink_Obj *pObj,
                                       NullSrcLink_NetworkRxObj *pNetRxObj,
                                       NetworkRx_CmdHeader *pHeader);
Int32 NullSrcLink_networkRxReadHeader(NullSrcLink_Obj *pObj,
                                      NullSrcLink_NetworkRxObj *pNetRxObj,
                                      NetworkRx_CmdHeader *pHeader);
Int32 NullSrcLink_networkRxReadPayload(NullSrcLink_Obj *pObj,
                                       NullSrcLink_NetworkRxObj *pNetRxObj,
                                       UInt32 numBuf,
                                       UInt8  *bufAddr[],
                                       UInt32 bufSize[]);

#define NETWORK_RX_SERVER_CLOSED        (0U)
#define NETWORK_RX_SERVER_LISTEN        (1U)
#define NETWORK_RX_SERVER_CONNECTED     (2U)

#define NETWORK_RX_SERVER_POLL_TIMEOUT  (10U)

Int32 NullSrcLink_networkRxCreate(NullSrcLink_Obj *pObj)
{
    NullSrcLink_NetworkRxObj *pNetRxObj = &pObj->netRxObj;
    Int32 status;

    Network_sessionOpen(NULL);

    status = Network_open(&pNetRxObj->sockObj,
                          pObj->createArgs.networkServerPort);
    UTILS_assert(status==SYSTEM_LINK_STATUS_SOK);

    pNetRxObj->state = NETWORK_RX_SERVER_LISTEN;

    Vps_printf(" NULL_SRC: NETWORK_RX: Server listening (port=%d) !!!\n",
        pObj->createArgs.networkServerPort);

    return status;
}

Int32 NullSrcLink_networkRxDelete(NullSrcLink_Obj *pObj)
{
    NullSrcLink_NetworkRxObj *pNetRxObj = &pObj->netRxObj;
    Int32 status;

    status = Network_close(&pNetRxObj->sockObj, (Bool)TRUE);
    UTILS_assert(status==SYSTEM_LINK_STATUS_SOK);

    Network_sessionClose(NULL);

    pNetRxObj->state = NETWORK_RX_SERVER_CLOSED;

    Vps_printf(" NULL_SRC: NETWORK_RX: Server Closed (port=%d) !!!\n",
        pObj->createArgs.networkServerPort);

    return status;
}

Int32 NullSrcLink_networkRxWaitConnect(NullSrcLink_Obj *pObj,
                                       NullSrcLink_NetworkRxObj *pNetRxObj)
{
    Int32 status = SYSTEM_LINK_STATUS_SOK;
    Int32 isConnected;

    if(pNetRxObj->state == NETWORK_RX_SERVER_LISTEN)
    {
        isConnected = Network_waitConnect(
                    &pNetRxObj->sockObj,
                    NETWORK_RX_SERVER_POLL_TIMEOUT);

        /* MISRA.UNREACH.GEN
         * MISRAC_2004 Rule_14.1
         * MISRAC_WAIVER:
         * Code is currently unreachable but as the implementation of the function
         * changes, it will not be unreachable.
         */
        if((Int32)TRUE == isConnected)
        {
            /* connected to client */
            pNetRxObj->state = NETWORK_RX_SERVER_CONNECTED;
            Vps_printf(" NULL_SRC: NETWORK_RX: Connected to client (port=%d) !!!\n",
                pObj->createArgs.networkServerPort);
        }
        else
        {
            status = SYSTEM_LINK_STATUS_EFAIL;
        }
    }

    return status;
}

Int32 NullSrcLink_networkRxWriteHeader(NullSrcLink_Obj *pObj,
                                       NullSrcLink_NetworkRxObj *pNetRxObj,
                                       NetworkRx_CmdHeader *pHeader)
{
    Int32 status = SYSTEM_LINK_STATUS_SOK;

    pHeader->header = NETWORK_RX_HEADER;

    /* MISRA.CAST.PTR
     * MISRAC_2004 Rule_11.4
     * MISRAC_WAIVER:
     * This is a generic prototype which supports all dataTypes.
     * This typecasting is necessary.
     */
    /* write header to client, if write failed, then client
     * disconnected, so go back to listening
     */
    status = Network_write(&pNetRxObj->sockObj,
                           (UInt8*)pHeader,
                           sizeof(NetworkRx_CmdHeader));

    /* MISRA.UNREACH.GEN
     * MISRAC_2004 Rule_14.1
     * MISRAC_WAIVER:
     * Code is currently unreachable but as the implementation of the function
     * changes, it will not be unreachable.
     */
    if(status != SYSTEM_LINK_STATUS_SOK)
    {
        Vps_printf(" NULL_SRC: NETWORK_RX: Disconnected from client "
                   "while writing header (port=%d)!!!\n",
                    pObj->createArgs.networkServerPort
                   );

        Network_close(&pNetRxObj->sockObj, (Bool)FALSE);
        pNetRxObj->state = NETWORK_RX_SERVER_LISTEN;
    }

    return status;
}

Int32 NullSrcLink_networkRxReadHeader(NullSrcLink_Obj *pObj,
                                      NullSrcLink_NetworkRxObj *pNetRxObj,
                                      NetworkRx_CmdHeader *pHeader)
{
    Int32 status = SYSTEM_LINK_STATUS_SOK;
    UInt32 dataSize;

    /* read reply */
    dataSize = sizeof(NetworkRx_CmdHeader);
    /* MISRA.CAST.PTR
     * MISRAC_2004 Rule_11.4
     * MISRAC_WAIVER:
     * This is a generic prototype which supports all dataTypes.
     * This typecasting is necessary.
     */
    status = Network_read(&pNetRxObj->sockObj,
                          (UInt8*)pHeader,
                          &dataSize);

    /* MISRA.UNREACH.GEN
     * MISRAC_2004 Rule_14.1
     * MISRAC_WAIVER:
     * Code is currently unreachable but as the implementation of the function
     * changes, it will not be unreachable.
     */
    if(status != SYSTEM_LINK_STATUS_SOK)
    {
        Vps_printf(" NULL_SRC: NETWORK_RX: Disconnected from client "
                   "while reading header (port=%d)!!!\n",
                    pObj->createArgs.networkServerPort
                   );

        Network_close(&pNetRxObj->sockObj, (Bool)FALSE);
        pNetRxObj->state = NETWORK_RX_SERVER_LISTEN;
    }
    if(status==SYSTEM_LINK_STATUS_SOK)
    {
        if((pHeader->header != NETWORK_RX_HEADER) ||
           (pHeader->dataSize == 0))
        {
            /* invalid header or no data to read */
            status = SYSTEM_LINK_STATUS_EFAIL;
            Vps_printf(" NULL_SRC: NETWORK_RX: Invalid header received"
                       " (port=%d)!!!\n",
                        pObj->createArgs.networkServerPort
                       );

        }
    }

    return status;
}

Int32 NullSrcLink_networkRxReadPayload(NullSrcLink_Obj *pObj,
                                       NullSrcLink_NetworkRxObj *pNetRxObj,
                                       UInt32 numBuf,
                                       UInt8  *bufAddr[],
                                       UInt32 bufSize[])
{
    Int32 status = SYSTEM_LINK_STATUS_SOK;
    UInt32 i;

    for(i=0; i<numBuf; i++)
    {
        if(status==SYSTEM_LINK_STATUS_SOK)
        {
            status = Network_read(&pNetRxObj->sockObj,
                        bufAddr[i],
                        &bufSize[i]
                        );

            /* MISRA.UNREACH.GEN
             * MISRAC_2004 Rule_14.1
             * MISRAC_WAIVER:
             * Code is currently unreachable but as the implementation of the
             * function changes, it will not be unreachable.
             */
            if(status != SYSTEM_LINK_STATUS_SOK)
            {
                Vps_printf(" NULL_SRC: NETWORK_RX: Disconnected from client "
                           "while reading payload (port=%d)!!!\n",
                            pObj->createArgs.networkServerPort
                           );
                Network_close(&pNetRxObj->sockObj, (Bool)FALSE);
                pNetRxObj->state = NETWORK_RX_SERVER_LISTEN;
                break;
            }

            /* MISRA.CAST.PTR_TO_INT
             * MISRAC_2004 Rule_11.3
             * MISRAC_WAIVER:
             * This is used to ensure address alignment required by hardware.
             */
            Cache_wb((Ptr)SystemUtils_floor((UInt32)bufAddr[i], 128U),
                     SystemUtils_align(bufSize[i] + 128U, 128U),
                     Cache_Type_ALLD,
                     (Bool)TRUE);

            #if 0
            Vps_printf(" NULL_SRC: NETWORK_RX: BUF%d: %d bytes recevied (port=%d)!!!\n",
                i,
                bufSize[i],
                pObj->createArgs.networkServerPort
                );
            #endif
        }
    }

    return status;
}

Int32 NullSrcLink_networkRxFillData(NullSrcLink_Obj * pObj,
                                    UInt32 channelId,
                                    const System_Buffer *pBuffer)
{
    NullSrcLink_NetworkRxObj *pNetRxObj = &pObj->netRxObj;
    Int32 status = SYSTEM_LINK_STATUS_SOK;
    System_VideoDataFormat dataFormat;
    System_VideoFrameBuffer *videoFrame;
    System_BitstreamBuffer *bitstreamBuf;

    status = NullSrcLink_networkRxWaitConnect(pObj, pNetRxObj);

    if(pNetRxObj->state == NETWORK_RX_SERVER_CONNECTED)
    {
        NetworkRx_CmdHeader cmdHeader;
        UInt32 bufSize;

        memset(&cmdHeader, 0, sizeof(NetworkRx_CmdHeader));

        cmdHeader.chNum = channelId;

        switch(pBuffer->bufType)
        {
            case SYSTEM_BUFFER_TYPE_BITSTREAM:
                bitstreamBuf = (System_BitstreamBuffer *)pBuffer->payload;

                /* right now only MJPEG RX is supported */
                cmdHeader.payloadType = NETWORK_RX_TYPE_BITSTREAM_MJPEG;
                cmdHeader.dataSize    = bitstreamBuf->bufSize; /* max buffer */
                break;

            case SYSTEM_BUFFER_TYPE_VIDEO_FRAME:
                videoFrame = (System_VideoFrameBuffer *)pBuffer->payload;
                dataFormat =
                    (System_VideoDataFormat)
                        System_Link_Ch_Info_Get_Flag_Data_Format(
                            videoFrame->chInfo.flags);

                if(dataFormat == SYSTEM_DF_YUV420SP_UV)
                {
                    bufSize =
                        (videoFrame->chInfo.pitch[0] *
                         videoFrame->chInfo.height) +
                        ((videoFrame->chInfo.pitch[1] *
                          videoFrame->chInfo.height) / 2U);

                    cmdHeader.payloadType = NETWORK_RX_TYPE_VIDEO_FRAME_YUV420SP_UV;
                }
                else if(dataFormat == SYSTEM_DF_YUV422I_YUYV)
                {
                    bufSize =
                        videoFrame->chInfo.pitch[0] * videoFrame->chInfo.height;
                    cmdHeader.payloadType =
                        NETWORK_RX_TYPE_VIDEO_FRAME_YUV422I_YUYV;
                }
                else
                {
                    status = SYSTEM_LINK_STATUS_EFAIL;
                }

                if(status==SYSTEM_LINK_STATUS_SOK)
                {
                    cmdHeader.dataSize  = bufSize; /* max buffer size */
                    cmdHeader.width     = videoFrame->chInfo.width;
                    cmdHeader.height    = videoFrame->chInfo.height;
                    cmdHeader.pitch[0U] = videoFrame->chInfo.pitch[0U];
                    cmdHeader.pitch[1U] = videoFrame->chInfo.pitch[1U];
                }

                break;

            default:
                status = SYSTEM_LINK_STATUS_EFAIL;
                break;
        }

        if(status == SYSTEM_LINK_STATUS_SOK)
        {
            status =
                NullSrcLink_networkRxWriteHeader(pObj, pNetRxObj, &cmdHeader);
        }

        if(status == SYSTEM_LINK_STATUS_SOK)
        {
            status =
                NullSrcLink_networkRxReadHeader(pObj, pNetRxObj, &cmdHeader);
        }

        if(status==SYSTEM_LINK_STATUS_SOK)
        {
            UInt32 numBuf = 0U;
            UInt8 *dataAddr[2U];
            UInt32 dataSize[2U];

            /* read payload data */
            switch(pBuffer->bufType)
            {
                case SYSTEM_BUFFER_TYPE_BITSTREAM:
                    numBuf = 1U;
                    dataAddr[0U] = bitstreamBuf->bufAddr;
                    dataSize[0U] = cmdHeader.dataSize;
                    bitstreamBuf->fillLength = cmdHeader.dataSize;
                    break;

                case SYSTEM_BUFFER_TYPE_VIDEO_FRAME:

                    if(dataFormat == SYSTEM_DF_YUV422I_YUYV)
                    {
                        numBuf = 1U;
                        dataAddr[0U] = videoFrame->bufAddr[0U];
                        dataSize[0U] =
                            cmdHeader.pitch[0U] * cmdHeader.height * 2U;
                        UTILS_assert(dataSize[0U] == cmdHeader.dataSize);
                    }
                    else
                    if(dataFormat == SYSTEM_DF_YUV420SP_UV)
                    {
                        numBuf = 2U;
                        dataAddr[0U] = videoFrame->bufAddr[0];
                        dataAddr[1U] = videoFrame->bufAddr[1];
                        dataSize[0U] = cmdHeader.pitch[0U]*cmdHeader.height;
                        dataSize[1U] =
                            (cmdHeader.pitch[1U]*cmdHeader.height) / 2U;

                        UTILS_assert(
                            (dataSize[0]+dataSize[1]) == cmdHeader.dataSize);
                    }
                    else
                    {
                        /* Can never reach here */
                        UTILS_assert((Bool)FALSE);
                    }
                    break;

                default:
                    /* Can never reach here */
                    UTILS_assert((Bool)FALSE);
                    break;
            }

            status = NullSrcLink_networkRxReadPayload(
                                pObj, pNetRxObj, numBuf, dataAddr, dataSize);

        }
    }

    return status;
}
