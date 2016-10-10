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
#include "grpxSrcLink_priv.h"
#include "grpxSrcLink_radar.h"

#define ULTRASONIC_LAYOUT_CLIP_TOP       (100)
#define ULTRASONIC_LAYOUT_CLIP_BOTTOM    ( 75)
/* below co-ordinates are relative to surround view window*/
#define ULTRASONIC_LAYOUT_CLIP_CENTER_X  (326)
#define ULTRASONIC_LAYOUT_CLIP_CENTER_Y  (329)
#define ULTRASONIC_LAYOUT_CLIP_CENTER_W  (228) /* this MUST match the jeep width */
#define ULTRASONIC_LAYOUT_CLIP_CENTER_H  (432) /* this MUST match the jeep height */

Int32 GrpxSrcLink_drawUltrasonicResults(GrpxSrcLink_Obj *pObj, System_MetaDataBuffer *pMetaBuffer)
{
    GrpxSrcLink_MetaDataDrawObj *pMetaDataDrawObj;
    Utils_DmaCopyFill2D dmaPrm;
    Int32 status;
    UInt16 startX, startY, winWidth, winHeight, displayWidth, displayHeight;

    pMetaDataDrawObj = &pObj->metaDataDrawObj;

    if(pMetaBuffer && pMetaBuffer->numMetaDataPlanes==1)
    {
        /* there can be only one plane  for overlay data */

        /* check if drawing area is within display are, else dont draw */

        startX    = pObj->createArgs.ultrasonicParams.windowStartX;
        startY    = pObj->createArgs.ultrasonicParams.windowStartY;
        winWidth  = pObj->createArgs.ultrasonicParams.windowWidth;
        winHeight = pObj->createArgs.ultrasonicParams.windowHeight;
        displayWidth  = pObj->info.queInfo[0].chInfo[0].width;
        displayHeight = pObj->info.queInfo[0].chInfo[0].height;

        if(startX + winWidth <= displayWidth
               &&
            startY + winHeight <= displayHeight
               &&
            pObj->createArgs.grpxBufInfo.dataFormat ==
                SYSTEM_DF_BGRA16_4444
               &&
            ULTRASONIC_LAYOUT_CLIP_CENTER_X + ULTRASONIC_LAYOUT_CLIP_CENTER_W <= winWidth
               &&
            ULTRASONIC_LAYOUT_CLIP_CENTER_Y + ULTRASONIC_LAYOUT_CLIP_CENTER_H <= winHeight
               &&
            ULTRASONIC_LAYOUT_CLIP_TOP <= ULTRASONIC_LAYOUT_CLIP_CENTER_Y
               &&
            (Int32)(displayHeight - ULTRASONIC_LAYOUT_CLIP_BOTTOM) >= (ULTRASONIC_LAYOUT_CLIP_CENTER_Y+ULTRASONIC_LAYOUT_CLIP_CENTER_H)
            )
        {
            /* Now DMA from this buffer to Grpx Buffer */

            /* DMA in 4 blocks such that it does not overlap
             * the surround view logo, jeep image and text
             */

            /* Block 0 - top */
            dmaPrm.dataFormat   = SYSTEM_DF_RAW16;
            dmaPrm.destAddr[0]  = (Ptr) pObj->outObj.videoFrames[0].bufAddr[0];
            dmaPrm.destPitch[0] = pObj->info.queInfo[0].chInfo[0].pitch[0];
            dmaPrm.destAddr[1]  = NULL;
            dmaPrm.destPitch[1] = 0;
            dmaPrm.srcStartX    = 0;
            dmaPrm.srcStartY    = ULTRASONIC_LAYOUT_CLIP_TOP;
            dmaPrm.destStartX   = startX + dmaPrm.srcStartX;
            dmaPrm.destStartY   = startY + dmaPrm.srcStartY;
            dmaPrm.width        = winWidth;
            dmaPrm.height       = ULTRASONIC_LAYOUT_CLIP_CENTER_Y - dmaPrm.srcStartY;
            dmaPrm.srcAddr[0]   = (Ptr) pMetaBuffer->bufAddr[0];
            dmaPrm.srcPitch[0]  = dmaPrm.width*2; /* assuming 2 bytes per pixel */
            dmaPrm.srcAddr[1]   = NULL;
            dmaPrm.srcPitch[1]  = 0;

            status = Utils_dmaCopy2D(&pMetaDataDrawObj->dmaObj,
                                     &dmaPrm,
                                     1);

            UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

            /* Block 1 - left */
            dmaPrm.dataFormat   = SYSTEM_DF_RAW16;
            dmaPrm.destAddr[0]  = (Ptr) pObj->outObj.videoFrames[0].bufAddr[0];
            dmaPrm.destPitch[0] = pObj->info.queInfo[0].chInfo[0].pitch[0];
            dmaPrm.destAddr[1]  = NULL;
            dmaPrm.destPitch[1] = 0;
            dmaPrm.srcStartX    = 0;
            dmaPrm.srcStartY    = ULTRASONIC_LAYOUT_CLIP_CENTER_Y;
            dmaPrm.destStartX   = startX + dmaPrm.srcStartX;
            dmaPrm.destStartY   = startY + dmaPrm.srcStartY;
            dmaPrm.width        = ULTRASONIC_LAYOUT_CLIP_CENTER_X;
            dmaPrm.height       = ULTRASONIC_LAYOUT_CLIP_CENTER_H;
            dmaPrm.srcAddr[0]   = (Ptr) pMetaBuffer->bufAddr[0];
            dmaPrm.srcPitch[0]  = dmaPrm.width*2; /* assuming 2 bytes per pixel */
            dmaPrm.srcAddr[1]   = NULL;
            dmaPrm.srcPitch[1]  = 0;

            status = Utils_dmaCopy2D(&pMetaDataDrawObj->dmaObj,
                                     &dmaPrm,
                                     1);

            UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

            /* Block 2 - right */
            dmaPrm.dataFormat   = SYSTEM_DF_RAW16;
            dmaPrm.destAddr[0]  = (Ptr) pObj->outObj.videoFrames[0].bufAddr[0];
            dmaPrm.destPitch[0] = pObj->info.queInfo[0].chInfo[0].pitch[0];
            dmaPrm.destAddr[1]  = NULL;
            dmaPrm.destPitch[1] = 0;
            dmaPrm.srcStartX    = ULTRASONIC_LAYOUT_CLIP_CENTER_X + ULTRASONIC_LAYOUT_CLIP_CENTER_W;
            dmaPrm.srcStartY    = ULTRASONIC_LAYOUT_CLIP_CENTER_Y;
            dmaPrm.destStartX   = startX + dmaPrm.srcStartX;
            dmaPrm.destStartY   = startY + dmaPrm.srcStartY;
            dmaPrm.width        = winWidth - dmaPrm.srcStartX;
            dmaPrm.height       = ULTRASONIC_LAYOUT_CLIP_CENTER_H;
            dmaPrm.srcAddr[0]   = (Ptr) pMetaBuffer->bufAddr[0];
            dmaPrm.srcPitch[0]  = dmaPrm.width*2; /* assuming 2 bytes per pixel */
            dmaPrm.srcAddr[1]   = NULL;
            dmaPrm.srcPitch[1]  = 0;

            status = Utils_dmaCopy2D(&pMetaDataDrawObj->dmaObj,
                                     &dmaPrm,
                                     1);

            /* Block 3 - bottom */
            dmaPrm.dataFormat   = SYSTEM_DF_RAW16;
            dmaPrm.destAddr[0]  = (Ptr) pObj->outObj.videoFrames[0].bufAddr[0];
            dmaPrm.destPitch[0] = pObj->info.queInfo[0].chInfo[0].pitch[0];
            dmaPrm.destAddr[1]  = NULL;
            dmaPrm.destPitch[1] = 0;
            dmaPrm.srcStartX    = 0;
            dmaPrm.srcStartY    = ULTRASONIC_LAYOUT_CLIP_CENTER_Y + ULTRASONIC_LAYOUT_CLIP_CENTER_H;
            dmaPrm.destStartX   = startX + dmaPrm.srcStartX;
            dmaPrm.destStartY   = startY + dmaPrm.srcStartY;
            dmaPrm.width        = winWidth;
            dmaPrm.height       = winHeight - dmaPrm.srcStartY - ULTRASONIC_LAYOUT_CLIP_BOTTOM;
            dmaPrm.srcAddr[0]   = (Ptr) pMetaBuffer->bufAddr[0];
            dmaPrm.srcPitch[0]  = dmaPrm.width*2; /* assuming 2 bytes per pixel */
            dmaPrm.srcAddr[1]   = NULL;
            dmaPrm.srcPitch[1]  = 0;

            status = Utils_dmaCopy2D(&pMetaDataDrawObj->dmaObj,
                                     &dmaPrm,
                                     1);

            UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

        }
    }

    return 0;
}

#ifdef RADAR_INCLUDE
Int32 GrpxSrcLink_drawRadarResults(GrpxSrcLink_Obj *pObj, System_MetaDataBuffer *pMetaBuffer)
{
    UInt32 idx;
    AlgorithmLink_RadarProcessPacketInfo *pRadarOutput;
    AlgorithmLink_CRFAlignedRadarPacket *pAlignedRadarData;
    GrpxRadarLink_targetInformation target;
    GrpxSrcLink_MetaDataDrawObj *pMetaDataDrawObj;
    Draw2D_LinePrm linePrm;
    Int32 angle;
    UInt32 distance;

    pMetaDataDrawObj = &pObj->metaDataDrawObj;

    if(pMetaBuffer)
    {
        if (pObj->createArgs.radarParams.cameraOverlayEnable)
        {
            Utils_dmaCopy2D(&pMetaDataDrawObj->dmaObj,
                            &pMetaDataDrawObj->backgroundDmaPrm,
                            1);

            linePrm.lineColorFormat = SYSTEM_DF_BGR16_565;
            linePrm.lineColor = 0xF800;
            linePrm.lineSize = 1;

            pAlignedRadarData = pMetaBuffer->bufAddr[0];
            angle = (Int32) pObj->createArgs.radarParams.angle;

            for (idx = 0; idx < pAlignedRadarData->object_count; idx++)
            {
                distance = pAlignedRadarData->distance[idx]/pObj->createArgs.radarParams.scale;
                if (((pAlignedRadarData->angle[idx] < angle) &&
                     (pAlignedRadarData->angle[idx] > -angle)) &&
                        (distance < pObj->createArgs.radarParams.radius))
                {
                    target.angle = pAlignedRadarData->angle[idx];
                    target.distance = distance;
                    target.relSpeed = pAlignedRadarData->relSpeed[idx];
                    target.vel_threshold = pAlignedRadarData->vel_threshold;

                    GrpxRadarLink_plotTarget(pObj,
                                             pObj->createArgs.radarParams.centerX,
                                             pObj->createArgs.radarParams.centerY,
                                             pObj->createArgs.radarParams.radius,
                                             &target);
                    Draw2D_drawRect(pObj->draw2DHndl,
                            (954 + pAlignedRadarData->objHorizontalOffset[idx]),
                            (pObj->createArgs.radarParams.centerY-600),
                            4,
                            600,
                            &linePrm);

                }
                else
                {
                    //Vps_printf("GrpxSrcLink: obj [%d] is out of range for display!! ", idx);
                }
            }
        }
        else
        {
            Utils_dmaCopy2D(&pMetaDataDrawObj->dmaObj,
                            &pMetaDataDrawObj->backgroundDmaPrm,
                            1);

            pRadarOutput = pMetaBuffer->bufAddr[0];
            for (idx = 0; idx < pRadarOutput->object_count; idx++)
            {
                angle = (Int32) pObj->createArgs.radarParams.angle;
                if (((pRadarOutput->angle[idx] < angle) &&
                     (pRadarOutput->angle[idx] > -angle)) &&
                        (pRadarOutput->distance[idx] < pObj->createArgs.radarParams.radius))
                {
                    target.angle = pRadarOutput->angle[idx];
                    target.distance = pRadarOutput->distance[idx];
                    target.relSpeed = pRadarOutput->relSpeed[idx];
                    target.vel_threshold = pRadarOutput->vel_threshold;

                    GrpxRadarLink_plotTarget(pObj,
                                             pObj->createArgs.radarParams.centerX,
                                             pObj->createArgs.radarParams.centerY,
                                             pObj->createArgs.radarParams.radius,
                                             &target);
                }
                else
                {
                    Vps_printf("GrpxSrcLink: obj [%d] is out of range for display!! ", idx);
                }
            }
        }
    }

    return 0;
}
#endif

Int32 GrpxSrcLink_drawMetaDataResultsCreate(GrpxSrcLink_Obj *pObj)
{
    Int32 status;

    Utils_DmaChCreateParams dmaPrm;

    memset(&pObj->metaDataDrawObj, 0, sizeof(pObj->metaDataDrawObj));
    pObj->metaDataDrawObj.isFirstTime = TRUE;
    pObj->metaDataDrawObj.startTime = 0;
    pObj->metaDataDrawObj.refreshInterval = 34; /* in msecs */

    Utils_DmaChCreateParams_Init(&dmaPrm);

    status = Utils_dmaCreateCh(&pObj->metaDataDrawObj.dmaObj, &dmaPrm);
    UTILS_assert(status==0);

    if (pObj->createArgs.radarParams.enable)
    {
        pObj->metaDataDrawObj.backgroundBufAddr = (UInt32) Utils_memAlloc(
                                                        UTILS_HEAPID_DDR_CACHED_SR,
                                                        (1920*1080)*2, 128);
        if(pObj->createArgs.radarParams.cameraOverlayEnable)
        {
            pObj->createArgs.radarParams.centerX = 1600;
            pObj->createArgs.radarParams.centerY = 850;
            pObj->createArgs.radarParams.radius = 300;
            pObj->createArgs.radarParams.angle = 45;
            pObj->createArgs.radarParams.scale = 2;
        }
        else
        {
            pObj->createArgs.radarParams.centerX = 960;
            pObj->createArgs.radarParams.centerY = 800;
            pObj->createArgs.radarParams.radius = 700;
            pObj->createArgs.radarParams.angle = 60;
            pObj->createArgs.radarParams.scale = 1;
        }
    }

    return status;
}

Int32 GrpxSrcLink_drawMetaDataResultsDelete(GrpxSrcLink_Obj *pObj)
{
    Int32 status;

    if (pObj->createArgs.radarParams.enable)
    {
        status = Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, (Ptr) pObj->metaDataDrawObj.backgroundBufAddr, (1920*1080)*2);
        UTILS_assert(status==0);
    }

    status = Utils_dmaDeleteCh(&pObj->metaDataDrawObj.dmaObj);

    UTILS_assert(status==0);

    return status;
}

Int32 GrpxSrcLink_drawMetaDataResultsRun(GrpxSrcLink_Obj *pObj)
{
    GrpxSrcLink_MetaDataDrawObj *pMetaDataDrawObj;
    UInt32 elaspedTime;
    System_BufferList bufferList;
    System_Buffer *pBuffer;
    System_MetaDataBuffer *pMetaBuffer;
    UInt32 radius;
    UInt32 offset;

    pMetaDataDrawObj = &pObj->metaDataDrawObj;

    if(pMetaDataDrawObj->isFirstTime)
    {
        pMetaDataDrawObj->isFirstTime = FALSE;
        pMetaDataDrawObj->startTime = Utils_getCurTimeInMsec();

        //zz (20150616): populate the sin table
        sin_table_fill();

        if (pObj->createArgs.radarParams.enable)
        {
            GrpxRadarLink_setInitTargetValues();

            Vps_printf("GrpxSrcLink: Initializing Radar Grid...");

            /* Save the background to a buffer */
            Draw2D_updateBufAddr(pObj->draw2DHndl, &pObj->metaDataDrawObj.backgroundBufAddr);

            if(!pObj->createArgs.radarParams.cameraOverlayEnable)
            {
                // draw labels
                GrpxRadarLink_drawLabels(pObj,
                        pObj->createArgs.radarParams.centerX,
                        pObj->createArgs.radarParams.centerY,
                        pObj->createArgs.radarParams.radius + 40,
                        0);
            }

            offset = 100 / pObj->createArgs.radarParams.scale;
            for (radius=offset; radius < pObj->createArgs.radarParams.radius; radius+=offset)
            {
                GrpxRadarLink_drawPartCircle(pObj,
                        pObj->createArgs.radarParams.centerX,
                        pObj->createArgs.radarParams.centerY,
                        radius,
                        pObj->createArgs.radarParams.angle
                        );
            }

            GrpxRadarLink_drawPartCircle(pObj,
                    pObj->createArgs.radarParams.centerX,
                    pObj->createArgs.radarParams.centerY,
                    pObj->createArgs.radarParams.radius,
                    pObj->createArgs.radarParams.angle
                    );

            // draw the sector lines
            GrpxRadarLink_drawSectorLines(pObj,
                    pObj->createArgs.radarParams.centerX,
                    pObj->createArgs.radarParams.centerY,
                    pObj->createArgs.radarParams.radius,
                    pObj->createArgs.radarParams.angle
                    );

            /* Display the background */
            pMetaDataDrawObj->backgroundDmaPrm.dataFormat   = SYSTEM_DF_RAW16;
            pMetaDataDrawObj->backgroundDmaPrm.destAddr[0]  = (Ptr) pObj->outObj.videoFrames[0].bufAddr[0];
            pMetaDataDrawObj->backgroundDmaPrm.destPitch[0] = pObj->info.queInfo[0].chInfo[0].pitch[0];
            pMetaDataDrawObj->backgroundDmaPrm.destAddr[1]  = NULL;
            pMetaDataDrawObj->backgroundDmaPrm.destPitch[1] = 0;
            if(pObj->createArgs.radarParams.cameraOverlayEnable)
            {
                pMetaDataDrawObj->backgroundDmaPrm.srcStartX    = 0;
                pMetaDataDrawObj->backgroundDmaPrm.srcStartY    = pObj->createArgs.radarParams.centerY -
                                                                    600;
                pMetaDataDrawObj->backgroundDmaPrm.destStartX   = 0;
                pMetaDataDrawObj->backgroundDmaPrm.destStartY   = pObj->createArgs.radarParams.centerY -
                                                                    600;
                pMetaDataDrawObj->backgroundDmaPrm.width        = 1920;
                pMetaDataDrawObj->backgroundDmaPrm.height       = 600 + offset;
            }
            else
            {
                pMetaDataDrawObj->backgroundDmaPrm.srcStartX    = pObj->createArgs.radarParams.centerX -
                                                                    pObj->createArgs.radarParams.radius;
                pMetaDataDrawObj->backgroundDmaPrm.srcStartY    = pObj->createArgs.radarParams.centerY -
                                                                    pObj->createArgs.radarParams.radius - offset/2;
                pMetaDataDrawObj->backgroundDmaPrm.destStartX   = pMetaDataDrawObj->backgroundDmaPrm.srcStartX;
                pMetaDataDrawObj->backgroundDmaPrm.destStartY   = pMetaDataDrawObj->backgroundDmaPrm.srcStartY;
                pMetaDataDrawObj->backgroundDmaPrm.width        = pObj->createArgs.radarParams.radius*2;
                pMetaDataDrawObj->backgroundDmaPrm.height       = pObj->createArgs.radarParams.radius + offset;
            }
            pMetaDataDrawObj->backgroundDmaPrm.srcAddr[0]   = (Ptr) pObj->metaDataDrawObj.backgroundBufAddr;
            pMetaDataDrawObj->backgroundDmaPrm.srcPitch[0]  = pObj->info.queInfo[0].chInfo[0].pitch[0];
            pMetaDataDrawObj->backgroundDmaPrm.srcAddr[1]   = NULL;
            pMetaDataDrawObj->backgroundDmaPrm.srcPitch[1]  = 0;

            Utils_dmaCopy2D(&pMetaDataDrawObj->dmaObj,
                            &pMetaDataDrawObj->backgroundDmaPrm,
                            1);

            Draw2D_updateBufAddr(pObj->draw2DHndl,
                    (UInt32 *) &pObj->outObj.videoFrames[0].bufAddr[0]);


        }

        return 0;
    }

    elaspedTime = Utils_getCurTimeInMsec() - pMetaDataDrawObj->startTime;

    if(elaspedTime < pMetaDataDrawObj->refreshInterval)
    {
        /* Not yet time to draw */
        return 0;
    }

    /* now draw */
    pMetaDataDrawObj->startTime = Utils_getCurTimeInMsec();

    System_getLinksFullBuffers(
        pObj->createArgs.inQueParams.prevLinkId,
        pObj->createArgs.inQueParams.prevLinkQueId,
        &bufferList
        );

    if(bufferList.numBuf)
    {
        /* pick last buffer or latest buffer to draw */
        pBuffer = bufferList.buffers[bufferList.numBuf-1];

        if(pBuffer
            && pBuffer->bufType == SYSTEM_BUFFER_TYPE_METADATA
            )
        {
            pMetaBuffer = (System_MetaDataBuffer*)pBuffer->payload;

            if (pObj->createArgs.ultrasonicParams.enable)
            {
                GrpxSrcLink_drawUltrasonicResults(pObj, pMetaBuffer);
            }
            else if (pObj->createArgs.radarParams.enable)
            {
                #ifdef RADAR_INCLUDE
                GrpxSrcLink_drawRadarResults(pObj, pMetaBuffer);
                #endif
            }
        }

        System_putLinksEmptyBuffers(
            pObj->createArgs.inQueParams.prevLinkId,
            pObj->createArgs.inQueParams.prevLinkQueId,
            &bufferList
            );
    }

    return 0;
}

