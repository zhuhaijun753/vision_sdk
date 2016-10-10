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
 * \defgroup ALGORITHM_LINK_OBJECTCLASSIFICATION Object classification API
 *
 * \brief  This module has the interface for using feature plane classification
 *         algorithm
 *         Feature Plane classification algorithm link -
 *             1. This alg takes input from Object Detect link and confirms
 *                the detection to be TRUE or FALSE, giving more accurate output
 *                for object detection usecase.
 *
 * @{
 *
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \file algorithmLink_objectClassification.h
 *
 * \brief Algorithm Link API specific to object classification algorithm
 *
 * \version 0.0 (Nov 2015) : [YM] First version
 *
 *******************************************************************************
 */

#ifndef ALGORITHM_LINK_OBJECTCLASSICIFICATION_H_
#define ALGORITHM_LINK_OBJECTCLASSICIFICATION_H_

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
 *   \ingroup LINK_API_CMD
 *   \addtogroup ALGORITHM_LINK_OBJECTCLASSIFICATION_CMD \
 *                   Object Classifcation Algorithm Control Commands
 *   @{
 *
 *******************************************************************************
 */

/**
 *******************************************************************************
 * \brief Link CMD: Command to set the pyramid information.
 *
 *   \param AlgorithmLink_ImgPyramidImgPyramidInfoControlParams *pPrm [In] Image
 *          Pyramid Information.
 *
 *   Supported only on TDA3xx Iss UseCase
 *
 *******************************************************************************
*/
#define ALGORITHM_LINK_OBJECTCLASSIFICATION_CMD_CREATE_ALG   (0x1000)


/* @} */

/*******************************************************************************
 *  Data structures
 *******************************************************************************
 */

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
    UInt32                   imgFrameWidth;
    /**< Width of the input frame */
    UInt32                   imgFrameHeight;
    /**< Height of the input frame */
    UInt32                   numScales;
    /**< Height of the input frame */

} AlgorithmLink_ObjectClassificationCreateParams;

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
} AlgorithmLink_ObjectClassificationControlParams;

/*******************************************************************************
 *  Function Prototypes
 *******************************************************************************
 */
static inline void AlgorithmLink_ObjectClassification_Init(
                        AlgorithmLink_ObjectClassificationCreateParams *pPrm);

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
static inline void AlgorithmLink_ObjectClassification_Init(
    AlgorithmLink_ObjectClassificationCreateParams *pPrm)
{
/* MISRA.ONEDEFRULE.FUNC
 * MISRAC_2004 Rule 8.5
 * Function Definition in header file
 * KW State: Defer -> Waiver -> Case by case
 * MISRAC_WAIVER: This function initializes the create time argument to default.
 *  Defining this in the header file aids maintainability.
 */
    memset(pPrm, 0, sizeof(AlgorithmLink_ObjectClassificationCreateParams));

    pPrm->baseClassCreate.size      = (UInt32)
                            sizeof(AlgorithmLink_ObjectClassificationCreateParams);
    pPrm->baseClassCreate.algId     = ALGORITHM_LINK_DSP_ALG_OBJECTCLASSIFICATION;
    pPrm->inQueParams.prevLinkId    = SYSTEM_LINK_ID_INVALID;
    pPrm->inQueParams.prevLinkQueId = 0U;
    pPrm->outQueParams.nextLink     = SYSTEM_LINK_ID_INVALID;
    pPrm->numOutBuffers             = 4U;
    pPrm->imgFrameWidth             = 640U;
    pPrm->imgFrameHeight            = 360U;
    pPrm->numScales                 = 17U;
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
Int32 AlgorithmLink_ObjectClassification_initPlugin(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/* @} */

/* Nothing beyond this point */
