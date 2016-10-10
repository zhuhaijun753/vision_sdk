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
 * \file pEstimateLink_priv.h Pose Estimate Algorithm Link private
 *                            API/Data structures
 *
 * \brief  This link private header file has defined
 *         - Algorithm link instance/handle object
 *         - All the local data structures
 *         - Algorithm plug in function interfaces
 *
 * \version 0.0 (Oct 2013) : [PS] First version
 *
 *******************************************************************************
 */

#ifndef _PESTIMATE_LINK_PRIV_H_
#define _PESTIMATE_LINK_PRIV_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *  Include files
 *******************************************************************************
 */
#include <include/link_api/system.h>
#include <include/link_api/algorithmLink.h>
#include <include/link_api/algorithmLink_poseEstimate.h>
#include <include/link_api/algorithmLink_algPluginSupport.h>
#include <src/utils_common/include/utils_prf.h>
#include <src/utils_common/include/utils_que.h>
#include "./include/iPoseEstimateAlgo.h"
#include <src/utils_common/include/utils_link_stats_if.h>

/*******************************************************************************
 *  Enums
 *******************************************************************************
 */

/*******************************************************************************
 *  Defines
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 *   \brief Max number of output
 *
 *   This macro defines the number of outputs.
 *
 *   SUPPORTED in ALL platforms
 *
 *******************************************************************************
 */
#define PESTIMATE_LINK_MAX_NUM_OUTPUT (8)


#define PESTIMATE_LINK_INPUT_FRAME_WIDTH (1280)

#define PESTIMATE_LINK_INPUT_FRAME_HEIGHT (720)

#define PESTIMATE_LINK_MAX_NUM_VIEWS (4)

#define PESTIMATE_LINK_CALMAT_SIZE ((PESTIMATE_LINK_MAX_NUM_VIEWS) *         \
                                    12 * 4 + 4)


/*******************************************************************************
 *  Data structures
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 *   \brief Structure containing pose estimate algorithm link parameters
 *
 *          This structure holds any algorithm parameters specific to this link.
 *
 *******************************************************************************
*/
typedef  struct
{
    void                      * algHandle;
    /**< Handle of the algorithm */


    UInt32                      dataFormat;
    /**< Data format of the video to operate on */
    UInt32                      inPitch[SYSTEM_MAX_PLANES];
    /**< Pitch of the input video buffer, support only YUV420 Data format */
    UInt32                      numInputChannels;
    /**< Number of input channels on input Q (Prev link output Q) */
    System_LinkChInfo           inputChInfo[SYSTEM_MAX_CH_PER_OUT_QUE];
    /**< channel info of input */
    AlgorithmLink_PEstimateCreateParams             algLinkCreateParams;
    /**< Create params of the pose estimate algorithm Link*/
    SV_PEstimate_CreationParamsStruct               algCreateParams;
    /**< Create params of the pose estimate algorithm */
    SV_PEstimate_ControlParams                      controlParams;
    /**< Control params of the pose estimate algorithm */
    System_Buffer                *sysBufferFeaturePts;
    /**< Place holder for the Feature Pts sysBuffer. Only one will be held
     * inside Pose estimate link at any point in time.
     */
    System_Buffer buffers[ALGLINK_PESTIMATE_OPQID_MAXOPQ]
                         [PESTIMATE_LINK_MAX_NUM_OUTPUT];
    /**< System buffer data structure to exchange buffers between links */
    System_MetaDataBuffer calibMat[ALGLINK_PESTIMATE_OPQID_MAXOPQ]
                                  [PESTIMATE_LINK_MAX_NUM_OUTPUT];
    /**< Payload for System buffers */
    AlgorithmLink_OutputQueueInfo outputQInfo[ALGLINK_PESTIMATE_OPQID_MAXOPQ];
    /**< All the information about output Queues used */
    AlgorithmLink_InputQueueInfo  inputQInfo[ALGLINK_PESTIMATE_IPQID_MAXIPQ];
    /**< All the information about input Queues used */
    UInt32                        frameDropCounter;
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
    Bool receivedFeaturePtsFlag;
    /**< Flag to indicate if feature Pts has been received. At any point in time
     *   Pose Estimate link keeps only one copy of feature Pts. So no queue is used.
     *   Just a bool is used for tracking.
     */
    Bool isFirstOPGenerated;
    /**< Flag to indicate if first output is generated. This flag
     *   can be used to control any special processing for first time
     */

} AlgorithmLink_PEstimateObj;

/*******************************************************************************
 *  Algorithm Link Private Functions
 *******************************************************************************
 */
Int32 AlgorithmLink_pEstimateCreate(void * pObj, void * pCreateParams);
Int32 AlgorithmLink_pEstimateProcess(void * pObj);
Int32 AlgorithmLink_pEstimateControl(void * pObj, void * pControlParams);
Int32 AlgorithmLink_pEstimateStop(void * pObj);
Int32 AlgorithmLink_pEstimateDelete(void * pObj);
Int32 AlgorithmLink_pEstimatePrintStatistics(void *pObj,
                       AlgorithmLink_PEstimateObj *pPEstimateObj);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/*@}*/

/* Nothing beyond this point */
