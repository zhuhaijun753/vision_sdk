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
 *
 * \ingroup ALGORITHM_LINK_API
 * \defgroup ALGORITHM_LINK_IMPL Algorithm Link Implementation
 *
 * @{
 */

/**
 *******************************************************************************
 *
 * \file objectDrawLink_priv.h Object Draw Link
 *       private API/Data structures
 *
 * \brief  This link private header file has defined
 *         - Algorithm link instance/handle object
 *         - All the local data structures
 *         - Algorithm plug in function interfaces
 *
 * \version 0.0 (Feb 2013) : [NN] First version
 *
 *******************************************************************************
 */

#ifndef _OBJECTDRAW_LINK_PRIV_H_
#define _OBJECTDRAW_LINK_PRIV_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *  Include files
 *******************************************************************************
 */
#include <include/link_api/system.h>
#include <include/link_api/algorithmLink.h>
#include <include/link_api/algorithmLink_objectDraw.h>
#include "examples/tda2xx/include/draw2d.h"
#include <include/link_api/algorithmLink_algPluginSupport.h>
#include <include/link_api/algorithmLink_objectDetection.h>
#include <src/utils_common/include/utils_prf.h>
#include <src/utils_common/include/utils_dma.h>
#include "iobjdet_ti.h"
#include "iclr_ti.h"
#include <src/utils_common/include/utils_link_stats_if.h>
#include <examples/tda2xx/src/alg_plugins/lanedetection/laneDetectLink_priv.h>
#include <examples/tda2xx/src/alg_plugins/sparseopticalflow/sparseOpticalFlowLink_priv.h>
#include <examples/tda2xx/src/alg_plugins/sfm/sfmLink_priv.h>
#include "math.h"

/*******************************************************************************
 *  Enums
 *******************************************************************************
 */

/*******************************************************************************
 *  Defines
 *******************************************************************************
 */

/* Size of different traffic sign icons */
#define OBJECTDRAW_TRAFFIC_SIGN_32x32   0

#define COLOR_RED       (0x4C34FF)
#define COLOR_GREEN     (0x960000)
#define COLOR_BLUE      (0xB3CC00)
#define COLOR_YELLOW    (0xE2009D)
#define COLOR_PURPLE    (0x5EA1F3)

/**
 *******************************************************************************
 *
 *   \brief traffic signs width
 *
 *   SUPPORTED in ALL platforms
 *
 *******************************************************************************
 */
#define OBJECTDRAW_TRAFFIC_SIGN_WIDTH_1  (32)
/**
 *******************************************************************************
 *
 *   \brief traffic signs height
 *
 *   SUPPORTED in ALL platforms
 *
 *******************************************************************************
 */
#define OBJECTDRAW_TRAFFIC_SIGN_HEIGHT_1 (32)
/**
 *******************************************************************************
 *
 *   \brief Max number of valid traffic signs
 *
 *   SUPPORTED in ALL platforms
 *
 *******************************************************************************
 */
#define OBJECTDRAW_TRAFFIC_SIGN_MAX    (27)

/**
 *******************************************************************************
 *
 *   \brief Max number of buffers
 *
 *   SUPPORTED in ALL platforms
 *
 *******************************************************************************
 */
#define OBJECTDRAW_LINK_MAX_NUM_OUTPUT    (8)

/**
 *******************************************************************************
 *
 *   \brief Thickness of the rectangles drawn around objects
 *
 *   SUPPORTED in ALL platforms
 *
 *******************************************************************************
 */
#define OBJECT_RECTANGLE_THICKNESS         (1)

/*******************************************************************************
 *  Data structures
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 *   \brief Structure containing feature plane computation algorithm link
 *          parameters
 *
 *          This structure holds any algorithm parameters specific to this link.
 *
 *******************************************************************************
*/
typedef struct
{
    AlgorithmLink_ObjectDrawCreateParams algLinkCreateParams;
    /**< Create params of object draw algorithm link */

    System_Buffer buffers[OBJECTDRAW_LINK_MAX_NUM_OUTPUT];
    /**< System buffers to exchange data with next link */

    System_VideoFrameBuffer videoBuffers[OBJECTDRAW_LINK_MAX_NUM_OUTPUT];
    /**< Payload for the system buffers */

    UInt32 outBufferSize_y;
    /**< output buffer size for Y-plane */

    UInt32 outBufferSize_uv;
    /**< output buffer size for C-plane */

    AlgorithmLink_InputQueueInfo  inputQInfo;
    /**< All the information about input Queue*/

    AlgorithmLink_OutputQueueInfo outputQInfo;
    /**< All the information about output Queue*/

    Draw2D_BufInfo draw2DBufInfo;
    /**< Buffer info used for drawing */

    Draw2D_Handle draw2DHndl;
    /**< Handle to the draw object */

    Draw2D_LinePrm linePrm;
    /**< Line params for drawing rectangles */

    UInt32                        frameDropCounter;
    /**< Counter to keep track of number of frame drops */

    Utils_DmaChObj  copyFramesDmaObj;
    /**< DMA object to use when copying input frame to output frame */

    System_LinkStatistics   *linkStatsInfo;
    /**< Pointer to the Link statistics information,
         used to store below information
            1, min, max and average latency of the link
            2, min, max and average latency from source to this link
            3, links statistics like frames captured, dropped etc
        Pointer is assigned at the link create time from shared
        memory maintained by utils_link_stats layer */

    Bool isFirstFrameRecv;
    /**< Flag to indicate if first frame is received, this is used as trigger
     *   to start stats counting
     */

    TI_OD_outputList *pOdOutput;

    TI_OD_outputList *pClrOutput;

    AlgorithmLink_LaneDetectOutput *pLdOutput;

    AlgorithmLink_SfmOutput *pSfmOutput;

    strackInfo *pSofOutput;

    UInt32 drawBufAddr[SYSTEM_MAX_PLANES];

    UInt32 laneMinWidth;

    UInt32 laneMaxWidth;

} AlgorithmLink_ObjectDrawObj;

/*******************************************************************************
 *  Algorithm Link Private Functions
 *******************************************************************************
 */
Int32 AlgorithmLink_objectDrawCreate(void * pObj,
                                              void * pCreateParams);
Int32 AlgorithmLink_objectDrawProcess(void * pObj);
Int32 AlgorithmLink_objectDrawControl(void * pObj,
                                               void * pControlParams);
Int32 AlgorithmLink_objectDrawStop(void * pObj);
Int32 AlgorithmLink_objectDrawDelete(void * pObj);
Int32 AlgorithmLink_objectDrawPrintStatistics(void *pObj,
                AlgorithmLink_ObjectDrawObj *pObjectDrawObj);

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
    );

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
    );

void AlgorithmLink_objectDrawConvertHsvToYuv();

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/*@}*/

/* Nothing beyond this point */
