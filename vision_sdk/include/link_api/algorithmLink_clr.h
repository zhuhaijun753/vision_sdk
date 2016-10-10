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
 * \defgroup ALGORITHM_LINK_CLR  Circular Light Recognition API
 *
 * \brief  This module has the interface for using Circular Light Recognition
 *         algorithm
 *
 * @{
 *
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \file algorithmLink_clr.h
 *
 * \brief Algorithm Link API specific to algorithm
 *
 * \version 0.0 (Feb 2013) : [NN] First version
 *
 *******************************************************************************
 */

#ifndef ALGORITHM_LINK_CLR_H_
#define ALGORITHM_LINK_CLR_H_

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

#define LD_FILTER_TAP_X     (3U)

/**
 * Maximum number of color lights to be dected
 */
#define ALGORITHM_LINK_CLR_MAX_NUM_COLOR_LIGHTS   (2)


/**
 * Maximum number of radii for which CLR will be performed
 */
#define ALGORITHM_LINK_CLR_MAX_NUM_RADIUS   (32)


/**
 *******************************************************************************
 *
 *   \ingroup LINK_API_CMD
 *   \addtogroup ALGORITHM_LINK_CLR_CMD CLR Control Commands
 *   @{
 *
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 *   \brief Alg Link Config CMD: Enable/Disable algorithm
 *
 *   \param AlgorithmLink_ClrAlgParams [IN]
 *
 *   \return SYSTEM_STATUS_SOK on success
 *
 *******************************************************************************
 */
#define ALGORITHM_LINK_CLR_CMD_SET_PARAMS     (0x3000)

/* @} */

/*******************************************************************************
 *  Enum's
 *******************************************************************************
 */

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

    UInt32 lightBrightnessThr[ALGORITHM_LINK_CLR_MAX_NUM_COLOR_LIGHTS];
    /**< Light Brightness Threshold */
    UInt32 lightColor[ALGORITHM_LINK_CLR_MAX_NUM_COLOR_LIGHTS];
    /**< Light color Threshold */
    UInt32 lightThr1[ALGORITHM_LINK_CLR_MAX_NUM_COLOR_LIGHTS];
    /**< Light Threshold1 */
    UInt32 lightThr2[ALGORITHM_LINK_CLR_MAX_NUM_COLOR_LIGHTS];
    /**< Light Threshold2 */
    UInt32 falseFilterThr[ALGORITHM_LINK_CLR_MAX_NUM_COLOR_LIGHTS];
    /**< False Filter Threshold */
    UInt32 numRadius;
    /**< Number of Radius  */
    UInt32 radius[ALGORITHM_LINK_CLR_MAX_NUM_RADIUS];
    /**< Radius */
    UInt32 circleDetectionThr[ALGORITHM_LINK_CLR_MAX_NUM_RADIUS];
    /**< Circle Detection Threshold */
    UInt32 scalingFactor[ALGORITHM_LINK_CLR_MAX_NUM_RADIUS];
    /**< Scaling Factor */
    UInt32 morphologyMethod;
    /**< Morphology Method */
    UInt32 groupingWindowSize;
    /**< Grouping Window Size */

    UInt32 roiStartX;
    /**< ROI to process relative within imgFrameWidthximgFrameHeight */
    UInt32 roiStartY;
    /**< ROI to process relative within imgFrameWidthximgFrameHeight */
    UInt32 roiWidth;
    /**< ROI to process relative within imgFrameWidthximgFrameHeight */
    UInt32 roiHeight;
    /**< ROI to process relative within imgFrameWidthximgFrameHeight */
} AlgorithmLink_ClrAlgParams;


typedef struct
{
    AlgorithmLink_CreateParams baseClassCreate;
    /**< Base class create params. This structure should be first element */
    System_LinkOutQueParams  outQueParams;
    /**< Output queue information */
    System_LinkInQueParams   inQueParams;
    /**< Input queue information */
    UInt32                   numOutBuffers;
    /**< Number of output Buffers */
    UInt32                   imgFrameStartX;
    /**< Start of actual video relative to start of input buffer */
    UInt32                   imgFrameStartY;
    /**< Start of actual video relative to start of input buffer */
    UInt32                   imgFrameWidth;
    /**< Width of the input frame */
    UInt32                   imgFrameHeight;
    /**< Height of the input frame */

    UInt32                   roiStartX;
    /**< ROI to process relative within imgFrameWidthximgFrameHeight */
    UInt32                   roiStartY;
    /**< ROI to process relative within imgFrameWidthximgFrameHeight */
    UInt32                   roiWidth;
    /**< ROI to process relative within imgFrameWidthximgFrameHeight */
    UInt32                   roiHeight;
    /**< ROI to process relative within imgFrameWidthximgFrameHeight */

} AlgorithmLink_ClrCreateParams;

/**
 *******************************************************************************
 *
 *   \brief Structure containing control parameters for algorithm
 *
 *******************************************************************************
*/
typedef struct
{
    AlgorithmLink_ControlParams baseClassControl;
    /**< Base class control params */
} AlgorithmLink_ClrControlParams;

/*******************************************************************************
 *  Functions
 *******************************************************************************
 */
static inline void AlgorithmLink_Clr_Init(
                            AlgorithmLink_ClrCreateParams *pPrm);

/**
 *******************************************************************************
 *
 * \brief Set defaults for plugin create parameters
 *
 * \param pPrm  [OUT] plugin create parameters
 *
 *******************************************************************************
 */
static inline void AlgorithmLink_Clr_Init(
    AlgorithmLink_ClrCreateParams *pPrm)
{
/* MISRA.ONEDEFRULE.FUNC
 * MISRAC_2004 Rule 8.5
 * Function Definition in header file
 * KW State: Defer -> Waiver -> Case by case
 * MISRAC_WAIVER: This function initializes the create time argument to default.
 *  Defining this in the header file aids maintainability.
 */
    memset(pPrm, 0, sizeof(AlgorithmLink_ClrCreateParams));

    pPrm->baseClassCreate.size = (UInt32)
                                sizeof(AlgorithmLink_ClrCreateParams);
    pPrm->baseClassCreate.algId = ALGORITHM_LINK_DSP_ALG_CLR;

    pPrm->imgFrameStartX    = 0U;
    pPrm->imgFrameStartY    = 0U;
    pPrm->imgFrameWidth     = 1280U;
    pPrm->imgFrameHeight    = 720U;

    pPrm->roiStartX    = 0U;
    pPrm->roiStartY    = 0U;
    pPrm->roiWidth     = 1280U;
    pPrm->roiHeight    = 720U;

    pPrm->inQueParams.prevLinkId = SYSTEM_LINK_ID_INVALID;
    pPrm->inQueParams.prevLinkQueId = 0U;
    pPrm->numOutBuffers = 3U;
    pPrm->outQueParams.nextLink = SYSTEM_LINK_ID_INVALID;
}


/**
 *******************************************************************************
 *
 * \brief Implementation of function to init plugins()
 *
 *        This function will be called by AlgorithmLink_initAlgPlugins, so as
 *        register plugins of algorithm
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
 */
Int32 AlgorithmLink_clr_initPlugin(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/* @} */

/* Nothing beyond this point */
