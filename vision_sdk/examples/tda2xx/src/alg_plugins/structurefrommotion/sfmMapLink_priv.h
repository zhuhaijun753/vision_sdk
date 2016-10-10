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
 * \file sfmMapLink_priv.h SfM Map Algorithm Link private
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

#ifndef _SFM_MAP_LINK_PRIV_H_
#define _SFM_MAP_LINK_PRIV_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *  Include files
 *******************************************************************************
 */
#include <include/link_api/system.h>
#include <include/link_api/algorithmLink.h>
#include <include/link_api/algorithmLink_sfmMap.h>
#include <include/link_api/algorithmLink_algPluginSupport.h>
#include <src/utils_common/include/utils_prf.h>
#include <src/utils_common/include/utils_que.h>
#include <src/utils_common/include/utils_link_stats_if.h>
#include "./include/iSfmMapAlgo.h"

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
#define SFMMAP_LINK_MAX_NUM_OUTPUT (6)
//output buffer sizes
#define SFMMAP_BOXES_BUFFER_SIZE (sizeof(ObjectBox) * SFM_MAP_NUM_BOXES_LIMIT)
#define SFMMAP_LABELMAP_BUFFER_SIZE (3*sizeof(UInt8) * SFM_MAP_NUM_OG_CELLS_LIMIT)

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
    AlgorithmLink_SfmMapCreateParams linkCreateParams;
    /**< Create params (link level)*/
    SfmMap_ControlParams			  algControlParams;
    /**< Control params */
    System_Buffer buffers[ALGLINK_SFMMAP_OPQID_MAXOPQ]
                         [SFMMAP_LINK_MAX_NUM_OUTPUT];
    /**< System buffer data structure to exchange buffers between links */
    System_MetaDataBuffer   metaDataBuffers[ALGLINK_SFMMAP_OPQID_MAXOPQ]
                                     [SFMMAP_LINK_MAX_NUM_OUTPUT];
    /**< Payload for System buffers */
    AlgorithmLink_OutputQueueInfo outputQInfo[ALGLINK_SFMMAP_OPQID_MAXOPQ];
    /**< All the information about output Queues used */
    AlgorithmLink_InputQueueInfo  inputQInfo[ALGLINK_SFMMAP_IPQID_MAXIPQ];
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
    System_Buffer  *sysBufferPoses;
    /*always keep latest pose*/
    System_MetaDataBuffer * metaBufferPoses;
    /*always keep latest pose*/

    Bool receivedFirstPoseFlag;
    /*true if first pose input was received */

    Bool isFirstFrameRecv;
    /**< Flag to indicate if first frame is received, this is used as trigger
     *   to start stats counting
     */
    Bool isFirstOPGenerated;
    /**< Flag to indicate if first output is generated. This flag
     *   can be used to control any special processing for first time
     */
} AlgorithmLink_SfmMapObj;

/*******************************************************************************
 *  Algorithm Link Private Functions
 *******************************************************************************
 */
Int32 AlgorithmLink_sfmMapCreate(void * pObj, void * pCreateParams);
Int32 AlgorithmLink_sfmMapProcess(void * pObj);
Int32 AlgorithmLink_sfmMapControl(void * pObj, void * pControlParams);
Int32 AlgorithmLink_sfmMapStop(void * pObj);
Int32 AlgorithmLink_sfmMapDelete(void * pObj);
Int32 AlgorithmLink_sfmMapPrintStatistics(void *pObj, void *pAlgLinkObj);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/*@}*/

/* Nothing beyond this point */
