/*
 *******************************************************************************
 *
 * Copyright (C) 2016 Texas Instruments Incorporated - http://www.ti.com/
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
 * \file opencvCannyLink_priv.h OpenCV Canny Algorithm Link private
 *                            API/Data structures
 *
 * \brief  This link private header file has defined
 *         - Algorithm link instance/handle object
 *         - All the local data structures
 *         - Algorithm plug in function interfaces
 *
 * \version 0.0 (March 2016) : [SN] First version
 *
 *******************************************************************************
 */

#ifndef _OPENCVCANNY_LINK_PRIV_H_
#define _OPENCVCANNY_LINK_PRIV_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *  Include files
 *******************************************************************************
 */
#include <include/link_api/system.h>
#include <include/link_api/algorithmLink.h>
#include <include/link_api/algorithmLink_opencvCanny.h>
#include <include/link_api/algorithmLink_algPluginSupport.h>
#include <src/osa/include/osa_prf.h>
#include <src/osa/include/osa_mem.h>
#include "iOpenCVCannyAlgo.h"

/*******************************************************************************
 *  Defines
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 *   \brief Max number of video frames needed for OpenCV Canny algorithm
 *
 *   SUPPORTED in ALL platforms
 *
 *******************************************************************************
 */
#define OPENCVCANNY_LINK_MAX_FRAMES (10)

/*******************************************************************************
 *  Data structures
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 *   \brief Structure containing OpenCV Canny algorithm link specific parameters
 *
 *          This structure holds any algorithm parameters specific to this link.
 *
 *******************************************************************************
*/
typedef  struct
{
    Alg_OpenCVCanny_Obj         * algHandle;
    /**< Handle of the algorithm */
    UInt32                      pitch;
    /**< Pitch of output video frames. This is kept same for all channels*/
    System_LinkOutQueParams     outQueParams;
    /**< Output queue information */
    System_LinkInQueParams      inQueParams;
    /**< Input queue information */
    UInt32                      numInputChannels;
    /**< Number of input channels on input Q (Prev link output Q) */
    System_LinkChInfo           inputChInfo[SYSTEM_MAX_CH_PER_OUT_QUE];
    /**< channel info of input */
    AlgorithmLink_OpenCVCannyCreateParams algLinkCreateParams;
    /**< Create params of the OpenCV Canny algorithm Link*/
    Alg_OpenCVCannyCreateParams   createParams;
    /**< Create params of the OpenCV Canny algorithm */
    Alg_OpenCVCannyControlParams  controlParams;
    /**< Control params of the OpenCV Canny algorithm */
    System_Buffer buffers[SYSTEM_MAX_CH_PER_OUT_QUE]
                         [OPENCVCANNY_LINK_MAX_FRAMES];
    /**< System buffer data structure to exchange buffers between links */
    System_VideoFrameBuffer videoFrames[SYSTEM_MAX_CH_PER_OUT_QUE]
                                       [OPENCVCANNY_LINK_MAX_FRAMES];
    /**< Payload for System buffers */
    AlgorithmLink_OutputQueueInfo outputQInfo;
    /**< All the information about output Q */
    AlgorithmLink_InputQueueInfo  inputQInfo;
    /**< All the information about input Q */
    UInt32                      frameDropCounter;
    /**< Counter to keep track of number of frame drops */

    OSA_LatencyStats linkLatency;
    /**< Structure to find out min, max and average latency of the link */

    OSA_LatencyStats srcToLinkLatency;
    /**< Structure to find out min, max and average latency from
     *   source to this link
     */

    OSA_LinkStatistics   linkStatsInfo;

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
} AlgorithmLink_OpenCVCannyObj;

/*******************************************************************************
 *  Algorithm Link Private Functions
 *******************************************************************************
 */
Int32 AlgorithmLink_opencvCannyCreate(void * pObj, void * pCreateParams);
Int32 AlgorithmLink_opencvCannyProcess(void * pObj);
Int32 AlgorithmLink_opencvCannyControl(void * pObj, void * pControlParams);
Int32 AlgorithmLink_opencvCannyStop(void * pObj);
Int32 AlgorithmLink_opencvCannyDelete(void * pObj);
Int32 AlgorithmLink_opencvCannyPrintStatistics(void *pObj,
                       AlgorithmLink_OpenCVCannyObj *pOpenCVCannyObj);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/*@}*/

/* Nothing beyond this point */
