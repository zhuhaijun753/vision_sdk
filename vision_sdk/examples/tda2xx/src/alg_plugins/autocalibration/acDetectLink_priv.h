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
 * \file acDetectLink_priv.h automatic chart detect Algorithm Link private
 *                            API/Data structures
 *
 * \brief  This link private header file has defined
 *         - Algorithm link instance/handle object
 *         - All the local data structures
 *         - Algorithm plug in function interfaces
 *
 *
 *******************************************************************************
 */

#ifndef _ACDETECT_LINK_PRIV_H_
#define _ACDETECT_LINK_PRIV_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *  Include files
 *******************************************************************************
 */
#include <include/link_api/system.h>
#include <include/link_api/algorithmLink.h>
#include <include/link_api/algorithmLink_autoChartDetect.h>
#include <include/link_api/algorithmLink_algPluginSupport.h>
#include <src/utils_common/include/utils_prf.h>
#include <src/utils_common/include/utils_que.h>
#include "./include/iAutoChartDetectAlgo.h"
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
 *   This macro defines the number of outputs
 *
 *   SUPPORTED in ALL platforms
 *
 *******************************************************************************
 */
#define ACDETECT_LINK_MAX_NUM_OUTPUT (8)


#define ACDETECT_LINK_INPUT_FRAME_WIDTH (1280)

#define ACDETECT_LINK_INPUT_FRAME_HEIGHT (720)

#define ACDETECT_LINK_MAX_NUM_VIEWS (4)



#define ACDETECT_LINK_FEATUREPOINT_SIZE ((NUM_CHART_CORNERS) *         \
                                      (ACDETECT_LINK_MAX_NUM_VIEWS) *  \
                                      2 * 4 * 4 + 4)



/*******************************************************************************
 *  Data structures
 *******************************************************************************
 */


/**
 *******************************************************************************
 *
 *   \brief Structure containing automatic chart detect algorithm link parameters
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
    AlgorithmLink_ACDetectCreateParams            algLinkCreateParams;
    /**< Create params of the automatic chart detect algorithm Link*/
    SV_ACDetect_CreationParamsStruct              algCreateParams;
    /**< Create params of the automatic chart detect algorithm */
    SV_ACDetect_ControlParams                     controlParams;
    /**< Control params of the automatic chart detect algorithm */
    System_Buffer buffers[ALGLINK_ACDETECT_OPQID_MAXOPQ]
                         [ACDETECT_LINK_MAX_NUM_OUTPUT];
    /**< System buffer data structure to exchange buffers between links */
    System_MetaDataBuffer featurePoints[ALGLINK_ACDETECT_OPQID_MAXOPQ]
                                     [ACDETECT_LINK_MAX_NUM_OUTPUT];
    /**< Payload for System buffers */
    AlgorithmLink_OutputQueueInfo outputQInfo[ALGLINK_ACDETECT_OPQID_MAXOPQ];
    /**< All the information about output Queues used */
    AlgorithmLink_InputQueueInfo  inputQInfo[ALGLINK_ACDETECT_IPQID_MAXIPQ];
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
    Bool isFirstOPGenerated;
    /**< Flag to indicate if first output is generated. This flag
     *   can be used to control any special processing for first time
     */
} AlgorithmLink_ACDetectObj;

/*******************************************************************************
 *  Algorithm Link Private Functions
 *******************************************************************************
 */
Int32 AlgorithmLink_acDetectCreate(void * pObj, void * pCreateParams);
Int32 AlgorithmLink_acDetectProcess(void * pObj);
Int32 AlgorithmLink_acDetectControl(void * pObj, void * pControlParams);
Int32 AlgorithmLink_acDetectStop(void * pObj);
Int32 AlgorithmLink_acDetectDelete(void * pObj);
Int32 AlgorithmLink_acDetectPrintStatistics(void *pObj,
                       AlgorithmLink_ACDetectObj *pACDetectObj);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/*@}*/

/* Nothing beyond this point */
