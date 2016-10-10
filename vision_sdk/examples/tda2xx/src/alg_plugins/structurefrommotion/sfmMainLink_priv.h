/*
 *******************************************************************************
 *
 * Copyright (C) 2015 Texas Instruments Incorporated - http://www.ti.com/
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
 * \file sfmMainLink_priv.h SfM Main Algorithm Link private
 *                            API/Data structures
 *
 * \brief  This link private header file has defined
 *         - Algorithm link instance/handle object
 *         - All the local data structures
 *         - Algorithm plug in function interfaces
 *
 * \version 0.0 (Jun 2015) : [MM] First version
 *
 *******************************************************************************
 */

#ifndef _SFM_MAIN_LINK_PRIV_H_
#define _SFM_MAIN_LINK_PRIV_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *  Include files
 *******************************************************************************
 */
#include <include/link_api/system.h>
#include <include/link_api/algorithmLink.h>
#include <include/link_api/algorithmLink_sfmMain.h>
#include <include/link_api/algorithmLink_algPluginSupport.h>
#include <src/utils_common/include/utils_prf.h>
#include <src/utils_common/include/utils_que.h>
#include <src/utils_common/include/utils_link_stats_if.h>
#include "./include/iSfmMainAlgo.h"

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
 *   \brief Max number of output LUT tables
 *
 *   GA generates three tables - Simple stitch LUT, Blend LUT1, Blend LUT2.
 *   All the three are considered together as one unit. This macro defines
 *   number of such units.
 *
 *   SUPPORTED in ALL platforms
 *
 *******************************************************************************
 */
#define SFMMAIN_LINK_MAX_NUM_OUTPUT (16)
//output buffer sizes
#define SFMMAIN_POSES0_BUFFER_SIZE (sizeof(Pose3D_f))
#define SFMMAIN_POSES1_BUFFER_SIZE (sizeof(Pose3D_f) * SFM_NUM_CAMS_LIMIT)
#define SFMMAIN_POINTS0_BUFFER_SIZE (sizeof(Point3D_f) * SFM_NUM_CAMS_LIMIT * SFM_NUM_POINTS_LIMIT)
#define SFMMAIN_POINTS1_BUFFER_SIZE (sizeof(Flouble) * SFM_NUM_CAMS_LIMIT * SFM_NUM_POINTS_LIMIT)
#define SFMMAIN_LTVAPI_BUFFER_SIZE (sizeof(SfmLtv_API))

/*******************************************************************************
 *  Data structures
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 *   \brief Structure containing algorithm link parameters
 *
 *          This structure holds any algorithm parameters specific to this link.
 *
 *******************************************************************************
*/
typedef  struct
{
    void                   			 *algHandle;
    /**< Handle of the algorithm */
    AlgorithmLink_SfmMainCreateParams linkCreateParams;
    /**< Create params (link level)*/
    //SfmMain_CreateParams			  algCreateParams;
    /**< Create params (algo level) */
    SfmMain_ControlParams			  algControlParams;
    /**< Control params */
    System_Buffer buffers[ALGLINK_SFMMAIN_OPQID_MAXOPQ]
                         [SFMMAIN_LINK_MAX_NUM_OUTPUT];
    /**< System buffer data structure to exchange buffers between links */
    System_MetaDataBuffer   metaDataBuffers[ALGLINK_SFMMAIN_OPQID_MAXOPQ]
                                     [SFMMAIN_LINK_MAX_NUM_OUTPUT];
    /**< Payload for System buffers */
    System_Buffer *inputBufferMultiCamCalibData;
    /**< Input buffer that holds current multi-camera calibration data*/
    AlgorithmLink_OutputQueueInfo outputQInfo[ALGLINK_SFMMAIN_OPQID_MAXOPQ];
    /**< All the information about output Queues used */
    AlgorithmLink_InputQueueInfo  inputQInfo[ALGLINK_SFMMAIN_IPQID_MAXIPQ];
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

    Bool isLTVBufferAvailable;
    /**< Flag to indicate if LTV is available (TRUE) or LTV is still busy (FALSE) */

    UInt32 algoCallCounter;
    /**< Counter for number of process function calls */

    Int32 delayCounter;
    /**< Counter for delaying first call to process function */

} AlgorithmLink_SfmMainObj;

/*******************************************************************************
 *  Algorithm Link Private Functions
 *******************************************************************************
 */
Int32 AlgorithmLink_sfmMainCreate(void * pObj, void * pCreateParams);
Int32 AlgorithmLink_sfmMainProcess(void * pObj);
Int32 AlgorithmLink_sfmMainControl(void * pObj, void * pControlParams);
Int32 AlgorithmLink_sfmMainStop(void * pObj);
Int32 AlgorithmLink_sfmMainDelete(void * pObj);
Int32 AlgorithmLink_sfmMainPrintStatistics(void *pObj, void *pAlgLinkObj);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/*@}*/

/* Nothing beyond this point */
