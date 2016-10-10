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
 * \ingroup  ALGORITHM_LINK_PLUGIN
 * \defgroup ALGORITHM_LINK_OCL_OBJECTDETECTION Object detection API
 *
 * \brief  This module has the interface for using feature plane classification
 *         algorithm
 *         Feature Plane classification algorithm link -
 *             1. This alg takes input from feature plane compute link
 *                and generates co-ordinates of the rectangles where
 *                objects are present in the frame
 *
 * @{
 *
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \file algorithmLink_objectDetection.h
 *
 * \brief Algorithm Link API specific to feature plane classification algorithm
 *
 * \version 0.0 (Feb 2013) : [NN] First version
 *
 *******************************************************************************
 */

#ifndef ALGORITHM_LINK_OBJECTDETECTION_H_
#define ALGORITHM_LINK_OBJECTDETECTION_H_

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


/**
 *******************************************************************************
 *
 * \ingroup LINK_API_CMD
 * \addtogroup ALGORITHM_LINK_OCL_OBJECT_DETECT_CMD Object Detect Control
 *             Commands
 *
 * @{
 *
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 *   \brief Alg Link Config CMD: Enable/Disable algorithm
 *
 *   \param AlgorithmLink_ObjectDetectEnableAlgParams [IN]
 *
 *   \return SYSTEM_STATUS_SOK on success
 *
 *******************************************************************************
 */
#define ALGORITHM_LINK_OBJECT_DETECT_CMD_ENABLE_ALG     (0x2000)

/* @} */

/*******************************************************************************
 *  Data structures
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 *   \brief Parameters to set specific ROI
 *
 *******************************************************************************
*/
typedef struct
{
    AlgorithmLink_ControlParams baseClassControl;
    /**< Base class control params */

    UInt32                   enablePD;
    /**< Enabled detection of Pedestrians */
    UInt32                   enableTSR;
    /**< Enabled detection of Traffic signs */
    UInt32                   enableVD;
    /**< Enabled detection of Vehicle's */

    UInt32                   enableAdvConfig;
    /**< When TRUE, parameters specified below are used,
     *   When FALSE, parameters specified below are not used
     */
    UInt32 detectorTypePD;
    /**< Advanced parameter, used for tuning algorithm */
    UInt32 trackingMethodPD;
    /**< Advanced parameter, used for tuning algorithm */
    UInt32 softCascadeThPD;
    /**< Advanced parameter, used for tuning algorithm */
    UInt32 strongCascadeThPD;
    /**< Advanced parameter, used for tuning algorithm */

    UInt32 detectorTypeTSR;
    /**< Advanced parameter, used for tuning algorithm */
    UInt32 trackingMethodTSR;
    /**< Advanced parameter, used for tuning algorithm */
    UInt32 recognitionMethodTSR;
    /**< Advanced parameter, used for tuning algorithm */
    UInt32 softCascadeThTSR;
    /**< Advanced parameter, used for tuning algorithm */
    UInt32 strongCascadeThTSR;
    /**< Advanced parameter, used for tuning algorithm */

    UInt32 detectorTypeVD;
    /**< Advanced parameter, used for tuning algorithm */
    UInt32 trackingMethodVD;
    /**< Advanced parameter, used for tuning algorithm */
    UInt32 softCascadeThVD;
    /**< Advanced parameter, used for tuning algorithm */
    UInt32 strongCascadeThVD;
    /**< Advanced parameter, used for tuning algorithm */

} AlgorithmLink_ObjectDetectEnableAlgParams;

/**
 *******************************************************************************
 *
 *   \brief Structure containing create time parameters for feature plane
 *          classification algorithm
 *
 *******************************************************************************
*/
typedef struct
{
    AlgorithmLink_CreateParams baseClassCreate;
    /**< Base class create params. This structure should be first element */
    System_LinkOutQueParams  outQueParams;
    /**< Output queue information */
    System_LinkInQueParams   inQueParams;
    /**< Input queue information */
    UInt32                   numOutBuffers;
    /**< Number of output buffers */
    UInt32                   enablePD;
    /**< Enabled detection of Pedestrians */
    UInt32                   enableTSR;
    /**< Enabled detection of Traffic signs */
    UInt32                   enableVD;
    /**< Enabled detection of Vehicles */
    UInt32                   imgFrameWidth;
    /**< Width of the input frame */
    UInt32                   imgFrameHeight;
    /**< Height of the input frame */
} AlgorithmLink_ObjectDetectionCreateParams;

/**
 *******************************************************************************
 *
 *   \brief Structure containing control parameters for Geometric Alignment
 *          algorithm
 *
 *******************************************************************************
*/
typedef struct
{
    AlgorithmLink_ControlParams baseClassControl;
    /**< Base class control params */
} AlgorithmLink_ObjectDetectionControlParams;

/*******************************************************************************
 *  Function Prototypes
 *******************************************************************************
 */
static inline void AlgorithmLink_ObjectDetection_Init(
                        AlgorithmLink_ObjectDetectionCreateParams *pPrm);

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
static inline void AlgorithmLink_ObjectDetection_Init(
    AlgorithmLink_ObjectDetectionCreateParams *pPrm)
{
/* MISRA.ONEDEFRULE.FUNC
 * MISRAC_2004 Rule 8.5
 * Function Definition in header file
 * KW State: Defer -> Waiver -> Case by case
 * MISRAC_WAIVER: This function initializes the create time argument to default.
 *  Defining this in the header file aids maintainability.
 */
    memset(pPrm, 0, sizeof(AlgorithmLink_ObjectDetectionCreateParams));

    pPrm->baseClassCreate.size      = (UInt32)
                            sizeof(AlgorithmLink_ObjectDetectionCreateParams);
    pPrm->baseClassCreate.algId     = ALGORITHM_LINK_DSP_ALG_OBJECTDETECTION;
    pPrm->inQueParams.prevLinkId    = SYSTEM_LINK_ID_INVALID;
    pPrm->inQueParams.prevLinkQueId = 0U;
    pPrm->outQueParams.nextLink     = SYSTEM_LINK_ID_INVALID;
    pPrm->numOutBuffers             = 4U;
    pPrm->enablePD                  = (UInt32) TRUE;
    pPrm->enableTSR                 = (UInt32) FALSE;
    pPrm->enableVD                  = (UInt32) FALSE;
    pPrm->imgFrameWidth             = 640U;
    pPrm->imgFrameHeight            = 360U;

}


/**
 *******************************************************************************
 *
 * \brief Implementation of function to init plugins()
 *
 *        This function will be called by AlgorithmLink_initAlgPlugins, so as
 *        register plugins of feature plane classification algorithm
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
 */
Int32 AlgorithmLink_ObjectDetection_initPlugin(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/* @} */

/* Nothing beyond this point */
