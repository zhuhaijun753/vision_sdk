/*
 *******************************************************************************
 *
 * Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */

/**
  ******************************************************************************
 * \file grpxSrcLink_sv_sof_layout.c
 *
 * \brief  This file has the implementation of GRPX layout for
 *         2D Surround view + SOF demo on TDA3xx
 *
 * \version 0.0 (Oct 2013) : [NN] First version
 *
 *******************************************************************************
 */

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */
#include "grpxSrcLink_priv.h"
#include <examples/tda2xx/src/alg_plugins/autocalibration/include/svACDCommonDefs.h>

#define SRV_NUM_CORNER_PTS  (8U)
#define SRV_CORNERPT_SIZE   ((SRV_NUM_CORNER_PTS * 2U * 4U) + 1)
#define SRV_RECT_WIDTH      (10U)
#define SRV_RECT_HEIGHT     (10U)

static UInt32 gDisWidth;
static UInt32 gDisHeight;
static UInt32 gWinWidth;
static UInt32 gWinHeight;
static Int32 gPrevCornerPts[SRV_CORNERPT_SIZE];

Int32 *chains_srvCalibration_Display_getCornerPoints(Void);
Void chains_srvCalibration_Display_getCaptureSize(
                                            UInt32 *pWidth, UInt32 *pHeight);
Bool chains_srvCalibration_Display_getDrawCornerPts();
Void chains_srvCalibration_Display_setDrawCornerPts(Bool drawCornerPts);
Bool chains_srvCalibration_Display_getClearCornerPts();
Void chains_srvCalibration_Display_setClearCornerPts(Bool clearCornerPts);

Void getStringPosn(
            UInt32 winStartX,
            UInt32 winStartY,
            UInt32 winWidth,
            UInt32 winHeight,
            UInt32 stringLen,
            Draw2D_FontProperty *pFontProp,
            UInt32 *pStrX,
            UInt32 *pStrY)
{
    UInt32 winOfstX;
    UInt32 winOfstY;

    winOfstX = (winWidth/2) - (stringLen * pFontProp->width/2);
    winOfstY = winHeight - (2 * pFontProp->height);

    *pStrX = winOfstX + winStartX;
    *pStrY = winOfstY + winStartY;
}

Int32 GrpxSrcLink_drawSrvCalibLayout(
                    GrpxSrcLink_Obj *pObj,
                    System_LinkChInfo *pChInfo)
{
    UInt32 strStartX, strStartY;
    Draw2D_FontPrm fontPrm;
    Draw2D_FontProperty fontProp;
    char string[128U];
    Draw2D_LinePrm linePrm;
    UInt32 firstStartX;
    UInt32 firstStartY;
    UInt32 firstWidth;
    UInt32 firstHeight;

    UInt32 secondStartX;
    UInt32 secondStartY;
    UInt32 secondWidth;
    UInt32 secondHeight;

    UInt32 capWidth, capHeight;

    gDisWidth = pChInfo->width;
    gDisHeight = pChInfo->height;
    gWinWidth = gDisWidth/2;
    gWinHeight = gDisHeight/2;

    fontPrm.fontIdx = 1U;
    Draw2D_getFontProperty(
            &fontPrm,
            &fontProp);

    strcpy(string,"CH 0:FRONT");
    getStringPosn(
            0U,
            0U,
            gWinWidth,
            gWinHeight,
            strlen(string),
            &fontProp,
            &strStartX,
            &strStartY);

    Draw2D_drawString(
            pObj->draw2DHndl,
            strStartX,
            strStartY,
            string,
            &fontPrm);

    strcpy(string,"CH 1:RIGHT");
    getStringPosn(
            gWinWidth,
            0U,
            gWinWidth,
            gWinHeight,
            strlen(string),
            &fontProp,
            &strStartX,
            &strStartY);

    Draw2D_drawString(
            pObj->draw2DHndl,
            strStartX,
            strStartY,
            string,
            &fontPrm);

    strcpy(string,"CH 2:BACK");
    getStringPosn(
            0U,
            gWinHeight,
            gWinWidth,
            gWinHeight,
            strlen(string),
            &fontProp,
            &strStartX,
            &strStartY);

    Draw2D_drawString(
            pObj->draw2DHndl,
            strStartX,
            strStartY,
            string,
            &fontPrm);

    strcpy(string,"CH 3:LEFT");
    getStringPosn(
            gWinWidth,
            gWinHeight,
            gWinWidth,
            gWinHeight,
            strlen(string),
            &fontProp,
            &strStartX,
            &strStartY);

    Draw2D_drawString(
            pObj->draw2DHndl,
            strStartX,
            strStartY,
            string,
            &fontPrm);

    /*  Calibration ROI */
    linePrm.lineColorFormat = SYSTEM_DF_BGR16_565;
    linePrm.lineColor = 0xF800;
    linePrm.lineSize = 1;

    chains_srvCalibration_Display_getCaptureSize(&capWidth, &capHeight);

    firstStartX = (FIRST_ROI_LEFT * gWinWidth)/capWidth;
    firstStartY = (FIRST_ROI_TOP * gWinHeight)/capHeight;
    firstWidth = ((FIRST_ROI_RIGHT - FIRST_ROI_LEFT) * gWinWidth)/capWidth;
    firstHeight = ((FIRST_ROI_BOTTOM - FIRST_ROI_TOP) * gWinHeight)/capHeight;

    secondStartX = (SECOND_ROI_LEFT * gWinWidth)/capWidth;
    secondStartY = (SECOND_ROI_TOP * gWinHeight)/capHeight;
    secondWidth = ((SECOND_ROI_RIGHT - SECOND_ROI_LEFT) * gWinWidth)/capWidth;
    secondHeight = ((SECOND_ROI_BOTTOM - SECOND_ROI_TOP) * gWinHeight)/capHeight;

    /* FRONT */
    /* Left ROI */
    Draw2D_drawRect(
            pObj->draw2DHndl,
            firstStartX,
            firstStartY,
            firstWidth,
            firstHeight,
            &linePrm);

    /* Right ROI */
    Draw2D_drawRect(
            pObj->draw2DHndl,
            secondStartX,
            secondStartY,
            secondWidth,
            secondHeight,
            &linePrm);

    /* RIGHT */
    /* Left ROI */
    Draw2D_drawRect(
            pObj->draw2DHndl,
            gWinWidth + firstStartX,
            firstStartY,
            firstWidth,
            firstHeight,
            &linePrm);

    /* Right ROI */
    Draw2D_drawRect(
            pObj->draw2DHndl,
            gWinWidth + secondStartX,
            secondStartY,
            secondWidth,
            secondHeight,
            &linePrm);

    /* BACK */
    /* Left ROI */
    Draw2D_drawRect(
            pObj->draw2DHndl,
            firstStartX,
            gWinHeight + firstStartY,
            firstWidth,
            firstHeight,
            &linePrm);

    /* Right ROI */
    Draw2D_drawRect(
            pObj->draw2DHndl,
            secondStartX,
            gWinHeight + secondStartY,
            secondWidth,
            secondHeight,
            &linePrm);

    /* LEFT */
    /* Left ROI */
    Draw2D_drawRect(
            pObj->draw2DHndl,
            gWinWidth + firstStartX,
            gWinHeight + firstStartY,
            firstWidth,
            firstHeight,
            &linePrm);

    /* Right ROI */
    Draw2D_drawRect(
            pObj->draw2DHndl,
            gWinWidth + secondStartX,
            gWinHeight + secondStartY,
            secondWidth,
            secondHeight,
            &linePrm);

    return 0;
}

Void scaleCornerPoints(
            Int32 origX,
            Int32 origY,
            UInt32 *pScaledX,
            UInt32 *pScaledY)
{
    UInt32 capWidth, capHeight;

    chains_srvCalibration_Display_getCaptureSize(&capWidth, &capHeight);

    *pScaledX = ((origX >> 4) * gWinWidth)/capWidth;
    *pScaledY = ((origY >> 4) * gWinHeight)/capHeight;
}

Int32 drawCornerPoints(
            GrpxSrcLink_Obj *pObj,
            UInt32 winStartX,
            UInt32 winStartY,
            UInt32 numPoints,
            Int32 *pPoint,
            Bool clear)
{
    UInt32 cnt;
    UInt32 color;
    Draw2D_RegionPrm region;
    UInt32 scaledX, scaledY;

    color = RGB888_TO_RGB565(160, 0, 0);

    for (cnt = 0;cnt < numPoints;cnt ++)
    {
        scaleCornerPoints(
                pPoint[cnt * 2],
                pPoint[cnt * 2 + 1],
                &scaledX,
                &scaledY);

        region.color  = color;
        region.colorFormat = SYSTEM_DF_BGR16_565;
        region.startX = winStartX + scaledX - (SRV_RECT_WIDTH >> 1);
        region.startY = winStartY + scaledY - (SRV_RECT_HEIGHT >> 1);
        region.width = SRV_RECT_WIDTH;
        region.height  = SRV_RECT_HEIGHT;

        if (clear == TRUE)
        {
            Draw2D_clearRegion(
                    pObj->draw2DHndl,
                    region.startX,
                    region.startY,
                    region.width,
                    region.height);
        }
        else
        {
            Draw2D_fillRegion(
                    pObj->draw2DHndl,
                    &region);
        }
    }

    return 0;
}

Int32 GrpxSrcLink_displaySrvCalibStats(
                        GrpxSrcLink_Obj *pObj)
{
    Int32 *pCornerPoints;

    if (chains_srvCalibration_Display_getClearCornerPts() == TRUE)
    {
        /*
         *  New corner points are available, erase preious points
         *  and draw the new corner points
         */

        pCornerPoints = gPrevCornerPts;

        if (*pCornerPoints == 1)
        {
            pCornerPoints ++;

            // Front Cam
            drawCornerPoints(
                        pObj,
                        0U,
                        0U,
                        SRV_NUM_CORNER_PTS,
                        pCornerPoints,
                        TRUE);

            pCornerPoints += SRV_NUM_CORNER_PTS << 1;

            // Right Cam
            drawCornerPoints(
                        pObj,
                        gWinWidth,
                        0U,
                        SRV_NUM_CORNER_PTS,
                        pCornerPoints,
                        TRUE);

            pCornerPoints += SRV_NUM_CORNER_PTS << 1;

            // Back Cam
            drawCornerPoints(
                        pObj,
                        0U,
                        gWinHeight,
                        SRV_NUM_CORNER_PTS,
                        pCornerPoints,
                        TRUE);

            pCornerPoints += SRV_NUM_CORNER_PTS << 1;

            // Left Cam
            drawCornerPoints(
                        pObj,
                        gWinWidth,
                        gWinHeight,
                        SRV_NUM_CORNER_PTS,
                        pCornerPoints,
                        TRUE);
        }

        chains_srvCalibration_Display_setClearCornerPts(FALSE);
    }

    if (chains_srvCalibration_Display_getDrawCornerPts() == TRUE)
    {
        pCornerPoints = chains_srvCalibration_Display_getCornerPoints();

        if (*pCornerPoints == 1)
        {
            pCornerPoints ++;

            // Front Cam
            drawCornerPoints(
                        pObj,
                        0U,
                        0U,
                        SRV_NUM_CORNER_PTS,
                        pCornerPoints,
                        FALSE);

            pCornerPoints += SRV_NUM_CORNER_PTS << 1;

            // Right Cam
            drawCornerPoints(
                        pObj,
                        gWinWidth,
                        0U,
                        SRV_NUM_CORNER_PTS,
                        pCornerPoints,
                        FALSE);

            pCornerPoints += SRV_NUM_CORNER_PTS << 1;

            // Back Cam
            drawCornerPoints(
                        pObj,
                        0U,
                        gWinHeight,
                        SRV_NUM_CORNER_PTS,
                        pCornerPoints,
                        FALSE);

            pCornerPoints += SRV_NUM_CORNER_PTS << 1;

            // Left Cam
            drawCornerPoints(
                        pObj,
                        gWinWidth,
                        gWinHeight,
                        SRV_NUM_CORNER_PTS,
                        pCornerPoints,
                        FALSE);

            memcpy(
                gPrevCornerPts,
                chains_srvCalibration_Display_getCornerPoints(),
                SRV_CORNERPT_SIZE << 2);
        }

        chains_srvCalibration_Display_setDrawCornerPts(FALSE);
    }

    return 0;
}
