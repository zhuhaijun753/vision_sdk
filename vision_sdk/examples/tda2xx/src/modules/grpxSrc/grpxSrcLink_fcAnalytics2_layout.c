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
 * \file grpxSrcLink_sved_layout.c
 *
 * \brief  This file has the implementation of GRPX layout for
 *         3D Surround view demo
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

Int32 GrpxSrcLink_drawFcAnalytics2DrawLegend(GrpxSrcLink_Obj *pObj)
{
    Draw2D_FontPrm fontPrm;
    Draw2D_FontProperty fontProp;
    Draw2D_RegionPrm regionPrm;
    Draw2D_BmpPrm bmpPrm;
    UInt16 startX, startY, lineHeight, boxWidth, boxHeight, tmpStartX;

    fontPrm.fontIdx = 5;

    Draw2D_getFontProperty(&fontPrm, &fontProp);

    lineHeight = fontProp.height;
    boxWidth   = fontProp.height-2;
    boxHeight  = fontProp.height-2;

    startX = pObj->info.queInfo[0].chInfo[0].width   - fontProp.width*22 - lineHeight;
    startY = pObj->info.queInfo[0].chInfo[0].height  - lineHeight*7;

    regionPrm.colorFormat = SYSTEM_DF_BGR16_565;
    regionPrm.width  = fontProp.width*22 + boxWidth + 4;
    regionPrm.height  = lineHeight*6 + 8;
    regionPrm.startX = startX - 4;
    regionPrm.startY = startY - 4;
    regionPrm.color = RGB888_TO_RGB565(16, 16, 16);
    Draw2D_fillRegion(pObj->draw2DHndl, &regionPrm);

    regionPrm.colorFormat = SYSTEM_DF_BGR16_565;
    regionPrm.width  = boxWidth;
    regionPrm.height  = boxHeight;

    regionPrm.startX = startX;
    regionPrm.startY = startY;
    regionPrm.color = RGB888_TO_RGB565(255, 0, 127);
    Draw2D_fillRegion(pObj->draw2DHndl, &regionPrm);

    regionPrm.startX = startX;
    regionPrm.startY = startY + lineHeight;
    regionPrm.color = RGB888_TO_RGB565(0, 255, 0);
    Draw2D_fillRegion(pObj->draw2DHndl, &regionPrm);

    regionPrm.startX = startX;
    regionPrm.startY = startY + lineHeight*2;
    regionPrm.color = RGB888_TO_RGB565(0, 255, 255);
    Draw2D_fillRegion(pObj->draw2DHndl, &regionPrm);

    regionPrm.startX = startX;
    regionPrm.startY = startY + lineHeight*3;
    regionPrm.color = RGB888_TO_RGB565(255, 255, 0);
    Draw2D_fillRegion(pObj->draw2DHndl, &regionPrm);

    bmpPrm.bmpIdx = DRAW2D_BMP_IDX_TL_RED;
    Draw2D_drawBmp(pObj->draw2DHndl,
        startX,
        startY + lineHeight*4,
        &bmpPrm
        );

    bmpPrm.bmpIdx = DRAW2D_BMP_IDX_TL_GREEN;
    Draw2D_drawBmp(pObj->draw2DHndl,
        startX,
        startY + lineHeight*5,
        &bmpPrm
        );

    tmpStartX = startX + boxWidth;

    Draw2D_drawString(pObj->draw2DHndl,
          tmpStartX,
          startY - lineHeight*2,
          " Camera     : Mono     ",
          &fontPrm
          );

    Draw2D_drawString(pObj->draw2DHndl,
          tmpStartX,
          startY - lineHeight,
          " Resolution : 1280x720 ",
          &fontPrm
          );


    Draw2D_drawString(pObj->draw2DHndl,
          tmpStartX,
          startY,
          " Pedestrian/Cyclist   ",
          &fontPrm
          );

    Draw2D_drawString(pObj->draw2DHndl,
          tmpStartX,
          startY + lineHeight,
          " Vehicle              ",
          &fontPrm
          );

    Draw2D_drawString(pObj->draw2DHndl,
          tmpStartX,
          startY + lineHeight*2,
          " Traffic Sign         ",
          &fontPrm
          );

    Draw2D_drawString(pObj->draw2DHndl,
          tmpStartX,
          startY + lineHeight*3,
          " Lane                 ",
          &fontPrm
          );

    Draw2D_drawString(pObj->draw2DHndl,
          tmpStartX,
          startY + lineHeight*4,
          " Traffic Light: Red   ",
          &fontPrm
          );

    Draw2D_drawString(pObj->draw2DHndl,
          tmpStartX,
          startY + lineHeight*5,
          " Traffic Light: Green ",
          &fontPrm
          );


    return SYSTEM_LINK_STATUS_SOK;
}

Int32 GrpxSrcLink_drawFcAnalytics2DrawInfo(GrpxSrcLink_Obj *pObj)
{
    Draw2D_BmpPrm bmpPrm;
    Draw2D_BmpProperty bmpProp;

    Draw2D_FontPrm fontPrm;
    Draw2D_FontProperty fontProp;
    UInt16 startX, lineHeight;

    fontPrm.fontIdx = 5;

    Draw2D_getFontProperty(&fontPrm, &fontProp);

    lineHeight = fontProp.height;

    startX = pObj->info.queInfo[0].chInfo[0].width   - fontProp.width*22 - lineHeight;

    /* Front cam analytics logo */
    bmpPrm.bmpIdx = DRAW2D_BMP_IDX_FRONT_CAM_ANALYTICS;
    Draw2D_getBmpProperty(&bmpPrm, &bmpProp);
    startX = (pObj->info.queInfo[0].chInfo[0].width - bmpProp.width)/2;
    Draw2D_drawBmp(pObj->draw2DHndl,
                   startX,
                   0,
                   &bmpPrm
                   );


    return SYSTEM_LINK_STATUS_SOK;
}

Int32 GrpxSrcLink_drawFcAnalytics2Layout(GrpxSrcLink_Obj *pObj)
{
    GrpxSrcLink_drawFcAnalytics2DrawInfo(pObj);
    GrpxSrcLink_drawFcAnalytics2DrawLegend(pObj);

    return SYSTEM_LINK_STATUS_SOK;
}
