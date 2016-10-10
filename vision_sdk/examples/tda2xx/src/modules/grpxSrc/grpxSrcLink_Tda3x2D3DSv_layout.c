/*
 *******************************************************************************
 *
 * Copyright (C) 2016 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */

/**
  ******************************************************************************
 * \file grpxSrcLink_Tda3x3DSv_layout.c
 *
 * \brief  This file has the implementation of GRPX layout for
 *         TDA3x 2D 3D Surround view demo
 *
 * \version 0.0 (Feb 2016) : [Suj] First version
 *
 *******************************************************************************
 */

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */
#include "grpxSrcLink_priv.h"

/*******************************************************************************
 *  FUNCTION PROTOTYPES
 *******************************************************************************
 */
Void chains_issMultCaptIsp2d3dSv_get2DVideoPrm(
                                UInt32 *pStartX,
                                UInt32 *pStartY,
                                UInt32 *pWidth,
                                UInt32 *pHeight);

Void chains_issMultCaptIsp2d3dSv_get3DVideoPrm(
                                UInt32 *pStartX,
                                UInt32 *pStartY,
                                UInt32 *pWidth,
                                UInt32 *pHeight);
/**
 *******************************************************************************
 *
 * \brief Background Color of the Graphics Buffer
 *
 * SUPPORTED in ALL platforms
 *
 *******************************************************************************
 */
#define SVED_BACKGROUND_COLOR ((UInt16)(RGB888_TO_RGB565(8,8,8)))

#define SVED_FRAME_THICKNESS  (10)


Int32 GrpxSrcLink_drawTDA3x2D3DSVLayout(GrpxSrcLink_Obj *pObj)
{
    Draw2D_RegionPrm region;
    Draw2D_BmpPrm bmpPrm;
    Draw2D_BmpProperty bmpProp;
    Draw2D_FontPrm fontPrm;
    Draw2D_FontProperty fontProp;
    UInt32 startX2d, startY2d, width2d, height2d, rot;
    UInt32 startX3d, startY3d, width3d, height3d;
    GrpxSrcLink_CreateParams *pCreatePrms = &pObj->createArgs;

    /* Get the 2D SRV video window parameters */
    chains_issMultCaptIsp2d3dSv_get2DVideoPrm(
                        &startX2d,
                        &startY2d,
                        &width2d,
                        &height2d);

    /* Get the 3D SRV video window parameters */
    chains_issMultCaptIsp2d3dSv_get3DVideoPrm(
                        &startX3d,
                        &startY3d,
                        &width3d,
                        &height3d);

    /* fill full buffer with background color */
    region.color  = SVED_BACKGROUND_COLOR;
    region.startX = 0;
    region.startY = 0;
    region.height = pObj->info.queInfo[0].chInfo[0].height;
    region.width  = pObj->info.queInfo[0].chInfo[0].width;
    Draw2D_fillRegion(pObj->draw2DHndl,&region);

    /* 2D Surround view video */
    region.color  = DRAW2D_TRANSPARENT_COLOR;
    region.startX = startX2d;
    region.startY = startY2d;
    region.height = height2d;
    region.width  = width2d;
    Draw2D_fillRegion(pObj->draw2DHndl,&region);

    /* 3D Surround view video */
    region.color  = DRAW2D_TRANSPARENT_COLOR;
    region.startX = startX3d;
    region.startY = startY3d;
    region.height = height2d; /* This is intentionally 2d to match height */
    region.width  = width3d;
    Draw2D_fillRegion(pObj->draw2DHndl,&region);

    /* draw bitmap's */

    /* TI logo */
    bmpPrm.bmpIdx = DRAW2D_BMP_IDX_TI_LOGO_1;
    Draw2D_getBmpProperty(&bmpPrm, &bmpProp);
    Draw2D_drawBmp(pObj->draw2DHndl,
                   pObj->info.queInfo[0].chInfo[0].width - (bmpProp.width*3/2) - SVED_FRAME_THICKNESS*4,
                   height2d - bmpProp.height,
                   &bmpPrm
                   );

    /* Surround view logo */
    bmpPrm.bmpIdx = DRAW2D_BMP_IDX_SURROUND_VIEW;
    Draw2D_drawBmp(pObj->draw2DHndl,
                   520+SVED_FRAME_THICKNESS+80,
                   0,
                   &bmpPrm
                   );

    if (pObj->createArgs.enableJeepOverlay == TRUE)
    {
        /* Jeep Image */
        bmpPrm.bmpIdx = DRAW2D_BMP_IDX_JEEP_220_X_330;
        rot = 0;

        Draw2D_drawBmp_rot(
                pObj->draw2DHndl,
                startX2d + (width2d - 220)/2,
                startY2d + (height2d - 330)/2,
                &bmpPrm,
                rot);
    }

    /* String for input resolution / frame-rate etc */
    fontPrm.fontIdx = 5;
    Draw2D_getFontProperty(&fontPrm, &fontProp);
    if (SYSTEM_STD_WXGA_30 == pCreatePrms->tda3x3DSvSrcResolution)
    {
        Draw2D_drawString(pObj->draw2DHndl,
                25,
                200 - 120 + 15,
                "RESOLUTION: 1280x800",
                &fontPrm);
    }
    else
    {
        Draw2D_getFontProperty(&fontPrm, &fontProp);
        Draw2D_drawString(pObj->draw2DHndl,
                (1920-400-fontProp.width/2),
                (250),
                "RESOLUTION: 1280x720",
                &fontPrm);
    }

    Draw2D_drawString(pObj->draw2DHndl,
                (1920-400-fontProp.width/2),
                (250 +30),
                "FRAME-RATE: 30fps   ",
                &fontPrm);

    Draw2D_drawString(pObj->draw2DHndl,
                (1920-400-fontProp.width/2),
                (250 +60),
                "   NETWORK: FPDLink ",
                &fontPrm);

    /* String for output resolution */
    fontPrm.fontIdx = 4;
    Draw2D_getFontProperty(&fontPrm, &fontProp);
    Draw2D_drawString(pObj->draw2DHndl,
                (width2d/2) - fontProp.width*10,
                pObj->info.queInfo[0].chInfo[0].height - 120 + 15 + 30 - 40,
                "RESOLUTION : 720x960",
                &fontPrm);

    fontPrm.fontIdx = 4;
    Draw2D_drawString(pObj->draw2DHndl,
                width2d + (width3d/2) - fontProp.width*10,
                pObj->info.queInfo[0].chInfo[0].height - 120 + 15 + 30 - 40,
                "RESOLUTION : 752x960",
                &fontPrm);


    return SYSTEM_LINK_STATUS_SOK;
}

Void GrpxSrcLink_displayTDA3x2D3DSVDrawCpuLoadBar(
                    GrpxSrcLink_Obj *pObj,
                    UInt32 cpuLoadInt,
                    UInt32 cpuLoadFract,
                    UInt32 startX,
                    UInt32 startY,
                    UInt32 width,
                    UInt32 height
                )
{
    Draw2D_RegionPrm region;
    UInt32 color[2];
    UInt32 barHeight[2];

    color[0] = RGB888_TO_RGB565(40, 40, 40);
    color[1] = RGB888_TO_RGB565(0, 160, 0);

    if(cpuLoadFract>=5)
        cpuLoadInt++;

    barHeight[0] = (height * (100 - cpuLoadInt))/100;

    if(barHeight[0] > height)
        barHeight[0] = height;

    barHeight[1] = height - barHeight[0];

    /* fill in in active load color */
    region.color  = color[0];
    region.startX = startX;
    region.startY = startY;
    region.height = barHeight[0];
    region.width  = width;

    Draw2D_fillRegion(pObj->draw2DHndl,&region);

    /* fill active load color */
    region.color  = color[1];
    region.startX = startX;
    region.startY = startY + barHeight[0];
    region.height = barHeight[1];
    region.width  = width;

    Draw2D_fillRegion(pObj->draw2DHndl,&region);

}

Int32 GrpxSrcLink_displayTDA3x2D3DSVStats(GrpxSrcLink_Obj *pObj)
{
    Draw2D_FontPrm fontPrm;
    UInt32 procId;
    Utils_SystemLoadStats *loadStats;
    char loadString[GRPX_SRC_LINK_STR_SZ];
    Draw2D_FontProperty fontProp;
    UInt32 startX, startY, startX1, startY1;
    UInt32 statsHeight;
    Bsp_PlatformSocId socId;

    socId = Bsp_platformGetSocId();

    fontPrm.fontIdx = 3;

    Draw2D_getFontProperty(&fontPrm, &fontProp);

    statsHeight = 180;
    startY =  200 - fontProp.height;
    startX =  pObj->info.queInfo[0].chInfo[0].width - 400;
    startX1 = pObj->info.queInfo[0].chInfo[0].width - 400;
    startY1 = 20 + SVED_FRAME_THICKNESS;


    for (procId = 0; ((procId < SYSTEM_PROC_MAX) &&
                      (BSP_PLATFORM_SOC_ID_TDA3XX == socId)); procId++)
    {
        if ((((SYSTEM_PROC_A15_0 != procId) && (SYSTEM_PROC_EVE2 != procId)) &&
             ((SYSTEM_PROC_EVE3 != procId) && (SYSTEM_PROC_EVE4 != procId))) &&
            (SYSTEM_PROC_IPU2 != procId))
        {
            loadStats = &pObj->statsDisplayObj.systemLoadStats[procId];

            snprintf(loadString, GRPX_SRC_LINK_STR_SZ,
                          "%-4s\n",
                          System_getProcName(procId)
                          );

            if (SYSTEM_PROC_IPU1_0 == procId)
            {
               snprintf(loadString, GRPX_SRC_LINK_STR_SZ,
                          "%-4s\n",
                          "M4-0"
                          );
            }
            else if (SYSTEM_PROC_IPU1_1 == procId)
            {
               snprintf(loadString, GRPX_SRC_LINK_STR_SZ,
                          "%-4s\n",
                          "M4-1"
                          );
            }
            else
            {
                /*
                 * Avoid MISRA C Warnings
                 */
            }

            /* draw CPU name */
            Draw2D_clearString(pObj->draw2DHndl,
                      startX,
                      startY,
                      strlen(loadString),
                      &fontPrm
                      );

            Draw2D_drawString(pObj->draw2DHndl,
                      startX,
                      startY,
                      loadString,
                      &fontPrm
                      );

            GrpxSrcLink_displayTDA3x2D3DSVDrawCpuLoadBar
                (
                    pObj,
                    loadStats->totalLoadParams.integerValue, /* CPU load integer value */
                    loadStats->totalLoadParams.fractionalValue, /* CPU load integer value */
                    startX,
                    fontProp.height + SVED_FRAME_THICKNESS*2,
                    30,
                    (statsHeight - SVED_FRAME_THICKNESS) - (fontProp.height + SVED_FRAME_THICKNESS)*2
                );

            /* draw CPU load as text */
            snprintf(loadString, GRPX_SRC_LINK_STR_SZ,
                          "%02d.%d%%\n",
                          loadStats->totalLoadParams.integerValue,
                          loadStats->totalLoadParams.fractionalValue
                          );
            Draw2D_clearString(pObj->draw2DHndl,
                      startX1,
                      startY1,
                      strlen(loadString),
                      &fontPrm
                      );

            Draw2D_drawString(pObj->draw2DHndl,
                      startX1,
                      startY1,
                      loadString,
                      &fontPrm
                      );
           startX1 = startX1 + fontProp.width*6 + 0;
           startX = startX+fontProp.width*6 + 0;
        }
    }
    return 0;
}
