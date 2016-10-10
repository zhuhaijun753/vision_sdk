/*
 *******************************************************************************
 *
 * Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */

#include "objectDrawLink_priv.h"


#define OBJECTDRAW_TRAFFIC_LIGHT_WIDTH_1  (32)
#define OBJECTDRAW_TRAFFIC_LIGHT_HEIGHT_1 (32)

#define OBJECTDRAW_TRAFFIC_LIGHT_MAX    (2)

UInt8 gObjectDraw_trafficLights_1[] = {
    #include "traffic_lights/Red_TL_32x32.h"
    #include "traffic_lights/Green_TL_32x32.h"
};

Int32 AlgorithmLink_objectDrawCopyTrafficLight(
    UInt8 *bufAddrY,
    UInt8 *bufAddrC,
    UInt32 pitchY,
    UInt32 pitchC,
    UInt32 bufWidth,
    UInt32 bufHeight,
    UInt32 startX,
    UInt32 startY,
    UInt32 trafficLightId,
    UInt32 trafficLightType
    )
{
    UInt8 *pTrafficLight;
    UInt8 *pTrafficLightList;
    UInt32 copyWidth, copyHeight, i;
    UInt32 trafficLightSize;
    UInt32 trafficLightWidth, trafficLightHeight;

    if(trafficLightId>=OBJECTDRAW_TRAFFIC_LIGHT_MAX)
        return -1;

    trafficLightWidth = OBJECTDRAW_TRAFFIC_LIGHT_WIDTH_1;
    trafficLightHeight = OBJECTDRAW_TRAFFIC_LIGHT_HEIGHT_1;
    pTrafficLightList = gObjectDraw_trafficLights_1;

    trafficLightSize = trafficLightWidth*trafficLightHeight*3/2;

    /* align to multiple of 2, since data format is YUV420 */
    startX = SystemUtils_floor(startX, 2);
    startY = SystemUtils_floor(startY, 2);

    /* clip the copy area to limit within buffer area */
    copyWidth = trafficLightWidth;
    copyHeight = trafficLightHeight;

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
    pTrafficLight = pTrafficLightList
                + trafficLightId*trafficLightSize;

    for(i=0; i<copyHeight; i++)
    {
        memcpy(bufAddrY, pTrafficLight, copyWidth);

        bufAddrY+=pitchY;
        pTrafficLight+=trafficLightWidth;
    }

    /* Copy C */
    pTrafficLight = pTrafficLightList
                + trafficLightId*trafficLightSize;

    pTrafficLight += trafficLightWidth*trafficLightHeight;

    for(i=0; i<copyHeight/2; i++)
    {
        memcpy(bufAddrC, pTrafficLight, copyWidth);

        bufAddrC+=pitchC;
        pTrafficLight+=trafficLightWidth;
    }

    return 0;
}
