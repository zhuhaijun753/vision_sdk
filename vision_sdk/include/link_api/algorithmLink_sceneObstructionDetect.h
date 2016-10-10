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
 * \ingroup  ALGORITHM_LINK_PLUGIN
 * \defgroup ALGORITHM_LINK_SCENE_OBSTRUCTION_DETECT  \
 *           Scene obstruction detection API
 *
 * \brief  This module has the interface for using SCENE_OBSTRUCTION_DETECT algorithm
 *
 * @{
 *
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \file algorithmLink_sceneObstructionDetect.h
 *
 * \brief Algorithm Link API specific to Scene obstruction detect algorithm
 *
 * \version 0.0 (Nov 2015) : [NN] First version
 *
 *******************************************************************************
 */

#ifndef ALGORITHM_LINK_SCENE_OBSTRUCTION_DETECT_H_
#define ALGORITHM_LINK_SCENE_OBSTRUCTION_DETECT_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */
#include <include/link_api/system.h>
#include <include/link_api/algorithmLink.h>

/*******************************************************************************
 *  Defines
 *******************************************************************************
 */

/* Max number of H3A Planes/buffers and thus output parameters supported */
#define ALGORITHM_SCENE_OBSTRUCTION_DETECT_MAX_PLANES              (1U)
#define SCENE_OBSTRUCTION_DETECT_NUM_COEFS (11U)

/**
    \ingroup LINK_API_CMD
    \addtogroup ALGORITHM_SCENE_OBSTRUCTION_DETECT_LINK_API_CMD   Scene Obstruction Detect Control Commands

    @{
*/

/**
 *******************************************************************************
 * \brief Link CMD: Command to set the Scene Obstruction Dynamic Params
 *
 *   \param AlgorithmLink_SceneObstructionDynamicParams *pPrm [IN] Dynamic Parameters.
 *
 *   Supported only on TDA3xx Iss UseCase
 *
 *******************************************************************************
*/
#define ALGORITHM_SCENE_OBSTRUCTION_DETRC_LINK_CMD_SET_SCENE_OBSTRUCTION_DYNAMIC_PARAMS   (0x1000)

/* @} */


/*******************************************************************************
 *  Data structures
 *******************************************************************************
 */

/*******************************************************************************
 *  \brief Alg Link create params
 *******************************************************************************
 */
typedef struct
{
    AlgorithmLink_CreateParams  baseClassCreate;
    /**< Base class create params. This structure should be first element */
    UInt32                      channelId;
    /**< Algorithm Mode */
    System_LinkOutQueParams  outQueParams;
    /**< Output queue information */
    System_LinkInQueParams      inQueParams;
    /**< Input queue information */
    UInt32                      numOutBuffers;
    /**< Number of output Buffers */
    UInt32 wdrEnable;
    /**< Is WDR enabled, if yes then AF data of second/long exposure follows the first exposure */
    UInt32 paxNumH;
    /**< Number of AF paxels in H-direction */
    UInt32 paxNumV;
    /**< Number of AF pacxels in V-direction */
    UInt32 vfEnable;
    /**< 0: vertical focus disabled, 1: vertical focus enabled */
    float classifierCoef[SCENE_OBSTRUCTION_DETECT_NUM_COEFS];
    /**< Coefficients of the classifier */
    float scaleFactor;
    /**< Scale factor that will be applied to the feature vector */
    float sensitivity;
    /**< Sensitivity value, 0: less sensitive, 10: most sensitive */
    UInt32 frameSkipInterval;
    /**< Number of frames to be skipped between each execution of the algorithm
         * 0 means the algorithm runs everyframe
         * 1 means the algorithm runs every other frame
         * etc
         */
    UInt32 numBlockedFramesThreshold;
    /** < Number of frames that have to be obstructed before outputting the scene obstruction event
         */
    System_LinkMemAllocInfo memAllocInfo;
    /**< Memory alloc region info, used to pass user alloc memory address */
} AlgorithmLink_SceneObstructionDetectCreateParams;


/*******************************************************************************
 *  Function Prototypes
 *******************************************************************************
 */
static inline void AlgorithmLink_SceneObstructionDetect_Init(
                            AlgorithmLink_SceneObstructionDetectCreateParams *pPrm);

/*******************************************************************************
 *  Functions
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \brief Set defaults for plugin create parameters
 *
 * \param pPrm  [OUT] plugin create parameters
 *
 *******************************************************************************
 */
static inline void AlgorithmLink_SceneObstructionDetect_Init(
    AlgorithmLink_SceneObstructionDetectCreateParams *pPrm)
{
/* MISRA.ONEDEFRULE.FUNC
 * MISRAC_2004 Rule 8.5
 * Function Definition in header file
 * KW State: Defer -> Waiver -> Case by case
 * MISRAC_WAIVER: This function initializes the create time argument to default.
 *  Defining this in the header file aids maintainability.
 */
    memset(pPrm, 0, sizeof(AlgorithmLink_SceneObstructionDetectCreateParams));

    pPrm->baseClassCreate.size = (UInt32)
                                    sizeof(AlgorithmLink_SceneObstructionDetectCreateParams);
    pPrm->baseClassCreate.algId = ALGORITHM_LINK_DSP_ALG_SCENE_OBSTRUCTION_DETECT;

    pPrm->channelId = 0U;

    pPrm->inQueParams.prevLinkId = SYSTEM_LINK_ID_INVALID;
    pPrm->inQueParams.prevLinkQueId = 0U;
    pPrm->numOutBuffers = 3U;

    pPrm->paxNumH = 3U;
    pPrm->paxNumV = 3U;
    pPrm->vfEnable= 1;
    pPrm->scaleFactor= 10.0;
    pPrm->classifierCoef[0]= 6.010115;
    pPrm->classifierCoef[1]= 1.562322;
    pPrm->classifierCoef[2]= -33.339242;
    pPrm->classifierCoef[3]= 1.963373;
    pPrm->classifierCoef[4]= -39.755250;
    pPrm->classifierCoef[5]= 0.0;
    pPrm->classifierCoef[6]= 0.0;
    pPrm->classifierCoef[7]= 0.0;
    pPrm->classifierCoef[8]= 0.0;
    pPrm->classifierCoef[9]= 0.0;
    pPrm->classifierCoef[10]= 0.0;
    pPrm->sensitivity= -2.0;
    pPrm->frameSkipInterval= 10;
    pPrm->numBlockedFramesThreshold= 3;

}


/**
 *******************************************************************************
 *
 * \brief Implementation of function to init plugins()
 *
 *        This function will be called by AlgorithmLink_initAlgPlugins, so as
 *        register plugins of Scene obstruction detect algorithm
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
 */
Int32 AlgorithmLink_sceneObstructionDetect_initPlugin(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/* @} */

/* Nothing beyond this point */
