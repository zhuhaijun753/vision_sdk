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
 * \file sceneObstructionDetectLink_priv.h Scene Obstruction Detect Algorithm Link private
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

#ifndef _SCENE_OBSTRUCTION_DETECT_LINK_PRIV_H_
#define _SCENE_OBSTRUCTION_DETECT_LINK_PRIV_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *  Include files
 *******************************************************************************
 */
#include <include/link_api/system.h>
#include <include/link_api/algorithmLink.h>
#include <include/link_api/algorithmLink_sceneObstructionDetect.h>
#include <include/link_api/algorithmLink_algPluginSupport.h>
#include <src/utils_common/include/utils_prf.h>
#include "iSceneObstructionDetect_ti.h"
#include <examples/tda2xx/src/alg_plugins/common/include/alg_ivision.h>
#include <src/utils_common/include/utils_link_stats_if.h>


/*******************************************************************************
 *  Defines
 *******************************************************************************
 */

/**
 *******************************************************************************
 *   \brief Max number of buffers
 *******************************************************************************
 */
#define SCENE_OBSTRUCTION_DETECT_LINK_MAX_NUM_OUTPUT    (3)


/**
 *******************************************************************************
 *   \brief Max number of input channels
 *******************************************************************************
 */
#define SCENE_OBSTRUCTION_DETECT_LINK_MAX_CH            (4)

/**
 *******************************************************************************
 *   \brief Max size of Scene obstruction detect results
 *******************************************************************************
 */
#define SCENE_OBSTRUCTION_DETECT_LINK_MAX_BUF_SIZE      (SystemUtils_align(4, 128))


/*******************************************************************************
 *  Data structures
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 *   \brief Structure containing scene detection algorithm link specific parameters
 *
 *          This structure holds any algorithm parameters specific to this link.
 *
 *******************************************************************************
*/
typedef  struct
{
    Void         * algHandle;
    /**< Handle of the algorithm */
    UInt32                      numInputChannels;
    /**< Number of input channels on input Q (Prev link output Q) */
    System_LinkChInfo           inputChInfo[SYSTEM_MAX_CH_PER_OUT_QUE];
    /**< channel info of input */
    AlgorithmLink_SceneObstructionDetectCreateParams algLinkCreateParams;
    /**< Create params of the frame copy algorithm Link*/
    SCENE_OBSTRUCTION_DETECT_TI_CreateParams   createParams;
    /**< Create parameters for the algorithm */
    IVISION_InArgs                      inArgs;
    /**< inArgs for the algorithm */
    IVISION_OutArgs                   outArgs;
    /**< outArgs for the algorithm */
    IVISION_InBufs                      inBufs;
    /**< input buffers for the algorithm */
    IVISION_OutBufs                     outBufs;
    /**< output buffers for the algorithm */
    IVISION_BufDesc                     inBufDesc;
    /**< input buffer descriptor */
    IVISION_BufDesc                     outBufDesc;
    /**< output buffer descriptor */
    IVISION_BufDesc                     *inBufDescList[SCENE_OBSTRUCTION_DETECT_TI_BUFDESC_IN_TOTAL];
    /**< list of input buffer descriptors */
    IVISION_BufDesc                 *outBufDescList[SCENE_OBSTRUCTION_DETECT_TI_BUFDESC_OUT_TOTAL];
    /**< list of input buffer descriptors */
    System_Buffer buffers[SCENE_OBSTRUCTION_DETECT_LINK_MAX_CH*SCENE_OBSTRUCTION_DETECT_LINK_MAX_NUM_OUTPUT];
    /**< System buffer data structure to exchange buffers between links */
    System_MetaDataBuffer metaDataBuffers[SCENE_OBSTRUCTION_DETECT_LINK_MAX_CH*SCENE_OBSTRUCTION_DETECT_LINK_MAX_NUM_OUTPUT];
    System_LinkQueInfo inQueInfo;
    /**< Payload for the meta buffers */
    AlgorithmLink_OutputQueueInfo outputQInfo;
    /**< All the information about output Q */
    AlgorithmLink_InputQueueInfo  inputQInfo;
    /**< All the information about input Q */
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
   Bool sceneObstructionState;
    /**< Flag to indicate whether the scene is currently obstructed 
        TRUE: scene is obstructed
        FALSE: scene is not obstructed
        */
} AlgorithmLink_SceneObstructionDetectObj;

/*******************************************************************************
 *  Algorithm Link Private Functions
 *******************************************************************************
 */
Int32 AlgorithmLink_sceneObstructionDetectCreate(void * pObj, void * pCreateParams);
Int32 AlgorithmLink_sceneObstructionDetectProcess(void * pObj);
Int32 AlgorithmLink_sceneObstructionDetectControl(void * pObj, void * pControlParams);
Int32 AlgorithmLink_sceneObstructionDetectStop(void * pObj);
Int32 AlgorithmLink_sceneObstructionDetectDelete(void * pObj);
Int32 AlgorithmLink_sceneObstructionDetectPrintStatistics(void *pObj,
                       AlgorithmLink_SceneObstructionDetectObj *pSceneObstructionDetectObj);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/*@}*/

/* Nothing beyond this point */
