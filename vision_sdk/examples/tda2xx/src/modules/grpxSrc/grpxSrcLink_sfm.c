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
 * \file grpxSrcLink_sfm.c
 *
 * \brief  This file has the implementation of GRPX layout for
 *         3D Perception SfM Demo (CES 2016)
 *
 *******************************************************************************
 */

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */
#include "grpxSrcLink_priv.h"


/**
 *******************************************************************************
 *
 * \brief Background Color of the Graphics Buffer
 *
 * SUPPORTED in ALL platforms
 *
 *******************************************************************************
 */

#define GRPX_SFM_FRAME_THICKNESS  (10)


Int32 GrpxSrcLink_drawSfMLayout(GrpxSrcLink_Obj *pObj)
{
    Draw2D_BmpPrm bmpPrm;
    //Draw2D_FontPrm fontPrm;

    /* draw bitmap's */

    /* TI logo */
    /*bmpPrm.bmpIdx = DRAW2D_BMP_IDX_TI_LOGO;
    Draw2D_drawBmp(pObj->draw2DHndl,
                   350,  //25,
                   50, //980,
                   &bmpPrm
                   );*/

    /* 3D Perception */
    bmpPrm.bmpIdx = DRAW2D_BMP_IDX_3DPERCEPTION_SFM_LOGO;
    Draw2D_drawBmp(pObj->draw2DHndl,
                   450,
                   20, //30,
                   &bmpPrm
                   );

    /*fontPrm.fontIdx = 0;
    Draw2D_drawString(pObj->draw2DHndl,
    	  650,
          20,
          "3D PERCEPTION",
          &fontPrm
          );

    fontPrm.fontIdx = 1;
    Draw2D_drawString(pObj->draw2DHndl,
    	  650,
          80,
          "STRUCTURE FROM MOTION",
          &fontPrm
          );*/


    /* String for Live Camera 1 */
    /*fontPrm.fontIdx = 5;
    Draw2D_drawString(pObj->draw2DHndl,
          25+40,
          (200 + (440)*0),
          "  FRONT CAMERA  ",
          &fontPrm
          );*/

    /* String for Live Camera 2 */
    /*fontPrm.fontIdx = 5;
    Draw2D_drawString(pObj->draw2DHndl,
          25+40,
          (200 + (440)*1),
          "  RIGHT CAMERA  ",
          &fontPrm
          );*/

    /* String for Live Camera 3 */
    /*fontPrm.fontIdx = 5;
    Draw2D_drawString(pObj->draw2DHndl,
          25+40 + 880 + 520,
          (200 + (440)*0),
          "   REAR CAMERA  ",
          &fontPrm
          );*/

    /* String for Live Camera 4 */
    /*fontPrm.fontIdx = 5;
    Draw2D_drawString(pObj->draw2DHndl,
          25+40 + 880 + 520,
          (200 + (440)*1),
          "   LEFT CAMERA  ",
          &fontPrm
          );*/

    /* String for input resolution / frame-rate etc */
    /*fontPrm.fontIdx = 6;
    Draw2D_drawString(pObj->draw2DHndl,
          25,
          1080 - GRPX_SFM_FRAME_THICKNESS - 2*45,
          "RESOLUTION: 1280x720",
          &fontPrm
          );

    fontPrm.fontIdx = 6;
    Draw2D_drawString(pObj->draw2DHndl,
          25,
          1080 - GRPX_SFM_FRAME_THICKNESS - 45,
          "FRAME-RATE: 30fps   ",
          &fontPrm
          );*/

    //SFM STUFF
    /*fontPrm.fontIdx = 6;
    Draw2D_drawString(pObj->draw2DHndl,
          1400,
          1080 - 200,
          "MAP RESOLUTION:       2cm",
          &fontPrm
          );

    fontPrm.fontIdx = 6;
    Draw2D_drawString(pObj->draw2DHndl,
    	  1400,
          1080 - 200 + 50,
          "# TRACKED POINTS:   4x900 ",
          &fontPrm
          );

    fontPrm.fontIdx = 6;
    Draw2D_drawString(pObj->draw2DHndl,
    	  1400,
          1080 - 200 + 2*50,
          "TRACKING:           30fps",
          &fontPrm
          );

    fontPrm.fontIdx = 6;
    Draw2D_drawString(pObj->draw2DHndl,
    	  1400,
          1080 - 200 + 3*50,
          "TRIANGULATION:       5fps",
          &fontPrm
          );*/

    return SYSTEM_LINK_STATUS_SOK;
}

Void GrpxSrcLink_displaySfMDrawCpuLoadBar(
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

Int32 GrpxSrcLink_displaySfMStats(GrpxSrcLink_Obj *pObj)
{
    Draw2D_FontPrm fontPrm;
    UInt32 procId;
    Utils_SystemLoadStats *loadStats;
    char loadString[GRPX_SRC_LINK_STR_SZ];
    Draw2D_FontProperty fontProp;
    UInt32 startX, startY, startX1, startY1;
    UInt32 statsHeight;

    fontPrm.fontIdx = 14;

    Draw2D_getFontProperty(&fontPrm, &fontProp);

    statsHeight = 180;
    startY =  180 - fontProp.height;
    startX =  pObj->info.queInfo[0].chInfo[0].width - 545;
    startX1 = pObj->info.queInfo[0].chInfo[0].width - 540;
    startY1 = 0 + GRPX_SFM_FRAME_THICKNESS;


    for (procId = 0; procId < SYSTEM_PROC_MAX; procId++)
    {
            loadStats = &pObj->statsDisplayObj.systemLoadStats[procId];

            if(Bsp_platformIsTda3xxFamilyBuild()
                &&
                (procId == SYSTEM_PROC_A15_0
                    ||
                 procId == SYSTEM_PROC_EVE2
                    ||
                 procId == SYSTEM_PROC_EVE3
                    ||
                 procId == SYSTEM_PROC_EVE4
                    ||
                 procId == SYSTEM_PROC_IPU2
                )
              )
            {
                /* These CPUs dont exist in TDA3xx */
                continue;
            }

            if(BSP_PLATFORM_SOC_ID_TDA2EX == Bsp_platformGetSocId()
                &&
                (procId == SYSTEM_PROC_DSP2
                    ||
                 procId == SYSTEM_PROC_EVE1
                    ||
                 procId == SYSTEM_PROC_EVE2
                    ||
                 procId == SYSTEM_PROC_EVE3
                    ||
                 procId == SYSTEM_PROC_EVE4
                )
              )
            {
                /* These CPUs dont exist in TDA3xx */
                continue;
            }

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
            else if (SYSTEM_PROC_IPU2 == procId)
            {
               snprintf(loadString, GRPX_SRC_LINK_STR_SZ,
                          "%-4s\n",
                          "M4-2"
                          );
            }
            else if (SYSTEM_PROC_IPU1_1 == procId)
            {
#ifdef A15_TARGET_OS_LINUX
               /* In case of Linux build, VSDK do not use IPU1-1 and use the
                * same space in the GRPX for SGX load */
               snprintf(loadString, GRPX_SRC_LINK_STR_SZ,
                          "%-4s\n",
                          "SGX "
                          );
               System_linkControl(
                   SYSTEM_LINK_ID_SGX3DSFM_0,
                   SYSTEM_COMMON_CMD_GET_SGX_LOAD,
                   loadStats,
                   sizeof(Utils_SystemLoadStats),
                   TRUE);
#else
               snprintf(loadString, GRPX_SRC_LINK_STR_SZ,
                          "%-4s\n",
                          "M4-1"
                          );
#endif
            }
            else if (SYSTEM_PROC_A15_0 == procId)
            {
               snprintf(loadString, GRPX_SRC_LINK_STR_SZ,
                          "%-4s\n",
                          "A15"
                          );
               /* HACK for Demo, needs to fix properly */
#ifdef A15_TARGET_OS_LINUX
               loadStats->totalLoadParams.integerValue = 9;
               loadStats->totalLoadParams.fractionalValue = 5;
#endif
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

            //MM: DSP2 always at 100% hardcoded (averages do not make sense for DSP2)
#if 0
            if (procId==4)
            {
                GrpxSrcLink_displaySfMDrawCpuLoadBar
                    (
                        pObj,
                        100, /* CPU load integer value */
                        0, /* CPU load integer value */
                        startX,
                        fontProp.height + GRPX_SFM_FRAME_THICKNESS*2,
                        30,
                        (statsHeight - GRPX_SFM_FRAME_THICKNESS) - (fontProp.height + GRPX_SFM_FRAME_THICKNESS)*2
                    );

                /* draw CPU load as text */
                snprintf(loadString, GRPX_SRC_LINK_STR_SZ,
                              "%02d.%d%%\n",
                              99,
                              9
                              );
            }
            else
#endif
            {
            	GrpxSrcLink_displaySfMDrawCpuLoadBar
                (
                    pObj,
                    loadStats->totalLoadParams.integerValue, /* CPU load integer value */
                    loadStats->totalLoadParams.fractionalValue, /* CPU load integer value */
                    startX1,
                    fontProp.height + GRPX_SFM_FRAME_THICKNESS*2,
                    30,
                    (statsHeight - GRPX_SFM_FRAME_THICKNESS) - (fontProp.height + GRPX_SFM_FRAME_THICKNESS)*2
                );

                /* draw CPU load as text */
                snprintf(loadString, GRPX_SRC_LINK_STR_SZ,
                              "%02d.%d%%\n",
                              loadStats->totalLoadParams.integerValue,
                              loadStats->totalLoadParams.fractionalValue
                              );
            }


            Draw2D_clearString(pObj->draw2DHndl,
                      startX,
                      startY1,
                      strlen(loadString),
                      &fontPrm
                      );

            Draw2D_drawString(pObj->draw2DHndl,
                      startX,
                      startY1,
                      loadString,
                      &fontPrm
                      );
           startX1 = startX1 + fontProp.width*5 + 0;
           startX = startX+fontProp.width*5 + 0;
    }
    return 0;
}
