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
 * \file openclCannyEdgeLink_priv.h Frame Copy Algorithm Link private
 *                            API/Data structures
 *
 * \brief  This link private header file has defined
 *         - Algorithm link instance/handle object
 *         - All the local data structures
 *         - Algorithm plug in function interfaces
 *
 * \version 0.0 (Aug 2013) : [PS] First version
 *
 *******************************************************************************
 */

#ifndef _OPENCLCannyEdge_LINK_PRIV_H_
#define _OPENCLCannyEdge_LINK_PRIV_H_

#ifdef __cplusplus
extern "C" {
#endif
/*******************************************************************************
 *  OpenCL Include files
 *******************************************************************************
 */



/*******************************************************************************
 *  Include files
 *******************************************************************************
 */
#include <include/link_api/system.h>
#include <include/link_api/algorithmLink.h>
#include <include/link_api/algorithmLink_openClCannyEdge.h>
#include <include/link_api/algorithmLink_algPluginSupport.h>
#include <src/utils_common/include/utils_prf.h>
#include "iOpenClCannyEdgeAlgo.h"
#include <src/utils_common/include/utils_link_stats_if.h>


/*******************************************************************************
 *  Defines
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 *   \brief Max number of video frames needed for frame copy algorithm
 *
 *   SUPPORTED in ALL platforms
 *
 *******************************************************************************
 */
#define CannyEdge_LINK_MAX_FRAMES (10)

/*******************************************************************************
 *  Data structures
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 *   \brief Structure containing frame copy algorithm link specific parameters
 *
 *          This structure holds any algorithm parameters specific to this link.
 *
 *******************************************************************************
*/
typedef  struct
{
    Alg_OpenCLCannyEdge_Obj     * algHandle;
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
    AlgorithmLink_OpenCLCannyEdgeCreateParams algLinkCreateParams;
    /**< Create params of the frame copy algorithm Link*/
    Alg_OpenCLCannyEdgeCreateParams   createParams;
    /**< Create params of the frame copy algorithm */
    Alg_OpenCLCannyEdgeControlParams  controlParams;
    /**< Control params of the frame copy algorithm */
    System_Buffer buffers[SYSTEM_MAX_CH_PER_OUT_QUE]
                         [CannyEdge_LINK_MAX_FRAMES];
    /**< System buffer data structure to exchange buffers between links */
    System_VideoFrameBuffer videoFrames[SYSTEM_MAX_CH_PER_OUT_QUE]
                                       [CannyEdge_LINK_MAX_FRAMES];
    /**< Payload for System buffers */
    AlgorithmLink_OutputQueueInfo outputQInfo;
    /**< All the information about output Q */
    AlgorithmLink_InputQueueInfo  inputQInfo;
    /**< All the information about input Q */
    UInt32                      frameDropCounter;
    /**< Counter to keep track of number of frame drops */

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
} AlgorithmLink_OpenCLCannyEdgeObj;

/*******************************************************************************
 *  Algorithm Link Private Functions
 *******************************************************************************
 */
Int32 AlgorithmLink_openclCannyEdgeCreate(void * pObj, void * pCreateParams);
Int32 AlgorithmLink_openclCannyEdgeProcess(void * pObj);
Int32 AlgorithmLink_openclCannyEdgeControl(void * pObj, void * pControlParams);
Int32 AlgorithmLink_openclCannyEdgeStop(void * pObj);
Int32 AlgorithmLink_openclCannyEdgeDelete(void * pObj);
Int32 AlgorithmLink_openclCannyEdgePrintStatistics(void *pObj,
                       AlgorithmLink_OpenCLCannyEdgeObj *pCannyEdgeObj);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/*@}*/

/* Nothing beyond this point */
