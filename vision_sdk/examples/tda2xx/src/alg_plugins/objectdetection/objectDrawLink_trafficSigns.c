/*
 *******************************************************************************
 *
 * Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */

#include "objectDrawLink_priv.h"

UInt8 gObjectDraw_trafficSigns_1[] = {
    #include "traffic_signs/000_1.h"
    #include "traffic_signs/001_1.h"
    #include "traffic_signs/002_1.h"
    #include "traffic_signs/003_1.h"
    #include "traffic_signs/004_1.h"
    #include "traffic_signs/005_1.h"
    #include "traffic_signs/006_1.h"
    #include "traffic_signs/007_1.h"
    #include "traffic_signs/008_1.h"
    #include "traffic_signs/009_1.h"
    #include "traffic_signs/010_1.h"
    #include "traffic_signs/011_1.h"
    #include "traffic_signs/012_1.h"
    #include "traffic_signs/013_1.h"
    #include "traffic_signs/014_1.h"
    #include "traffic_signs/015_1.h"
    #include "traffic_signs/016_1.h"
    #include "traffic_signs/017_1.h"
    #include "traffic_signs/018_1.h"
    #include "traffic_signs/019_1.h"
    #include "traffic_signs/020_1.h"
    #include "traffic_signs/021_1.h"
    #include "traffic_signs/022_1.h"
    #include "traffic_signs/023_1.h"
    #include "traffic_signs/024_1.h"
    #include "traffic_signs/025_1.h"
    #include "traffic_signs/026_1.h"
    #include "traffic_signs/027_1.h"
    #include "traffic_signs/028_1.h"
    #include "traffic_signs/029_1.h"
    #include "traffic_signs/030_1.h"
    #include "traffic_signs/031_1.h"

};

Int32 AlgorithmLink_objectDrawCopyTrafficSign(
    UInt8 *bufAddrY,
    UInt8 *bufAddrC,
    UInt32 pitchY,
    UInt32 pitchC,
    UInt32 bufWidth,
    UInt32 bufHeight,
    UInt32 startX,
    UInt32 startY,
    UInt32 trafficSignId,
    UInt32 trafficSignType
    )
{
    UInt8 *pTrafficSign;
    UInt8 *pTrafficSignList;
    UInt32 copyWidth, copyHeight, i;
    UInt32 trafficSignSize;
    UInt32 trafficSignWidth, trafficSignHeight;

    if(trafficSignId>=OBJECTDRAW_TRAFFIC_SIGN_MAX)
        return -1;

    trafficSignWidth = OBJECTDRAW_TRAFFIC_SIGN_WIDTH_1;
    trafficSignHeight = OBJECTDRAW_TRAFFIC_SIGN_HEIGHT_1;
    pTrafficSignList = gObjectDraw_trafficSigns_1;

    trafficSignSize = trafficSignWidth*trafficSignHeight*3/2;

    /* align to multiple of 2, since data format is YUV420 */
    startX = SystemUtils_floor(startX, 2);
    startY = SystemUtils_floor(startY, 2);

    /* clip the copy area to limit within buffer area */
    copyWidth = trafficSignWidth;
    copyHeight = trafficSignHeight;

    if(startX > bufWidth
        ||
       startY > bufHeight
        )
    {
        /* Nothing to copy in this case */
        return 0;
    }

    if(startX+copyWidth > bufWidth)
    {
        copyWidth = bufWidth - startX;
    }

    if(startY+copyHeight > bufHeight)
    {
        copyHeight = bufHeight - startY;
    }

    /* adjust input buffer pointer to start location */
    bufAddrY = bufAddrY + pitchY*startY + startX;
    bufAddrC = bufAddrC + pitchC*startY/2 + startX;

    /* Copy Y */
    pTrafficSign = pTrafficSignList
                + trafficSignId*trafficSignSize;

    for(i=0; i<copyHeight; i++)
    {
        memcpy(bufAddrY, pTrafficSign, copyWidth);

        bufAddrY+=pitchY;
        pTrafficSign+=trafficSignWidth;
    }

    /* Copy C */
    pTrafficSign = pTrafficSignList
                + trafficSignId*trafficSignSize;

    pTrafficSign += trafficSignWidth*trafficSignHeight;

    for(i=0; i<copyHeight/2; i++)
    {
        memcpy(bufAddrC, pTrafficSign, copyWidth);

        bufAddrC+=pitchC;
        pTrafficSign+=trafficSignWidth;
    }

    return 0;
}












































