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
 * \defgroup ALGORITHM_LINK_FPCOMPUTE Feature Plane Computation v2 API
 *
 * \brief  This module has the interface for using feature plane comp algorithm
 *
 *         Feature Plane Computation algorithm link -
 *             1. This alg takes input from previous link and computes
 *                feature planes for luma and chroma components
 *
 * @{
 *
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \file algorithmLink_fpCompute.h
 *
 * \brief Algorithm Link API specific to feature plane computation algorithm
 *
 * \version 0.0 (Nov 2015) : [YM] First version
 *
 *******************************************************************************
 */

#ifndef ALGORITHM_LINK_FPCOMPUTE_H_
#define ALGORITHM_LINK_FPCOMPUTE_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */
#include <include/link_api/system.h>
#include <include/link_api/algorithmLink.h>

/**
 *******************************************************************************
 *
 *   \ingroup LINK_API_CMD
 *   \addtogroup ALGORITHM_LINK_FPCOMPUTE_CMD \
 *                   Feature Plane Compute v2 Control Commands
 *   @{
 *
 *******************************************************************************
 */


/**
 *******************************************************************************
 *
 *   \brief Alg Link Config CMD: Create FP compute alg
 *                               This is a compulsory ioctl to instantiate Alg.
 *
 *   \param AlgorithmLink_FPComputeSetROIParams [IN]
 *
 *   \return SYSTEM_STATUS_SOK on success
 *
 *******************************************************************************
 */
#define ALGORITHM_LINK_FPCOMPUTE_CMD_CREATE_ALG               (0x1000)

/**
 *******************************************************************************
 *
 *   \brief Alg Link Config CMD: Set ROI parameters
 *
 *   \param AlgorithmLink_FPComputeSetROIParams [IN]
 *
 *   \return SYSTEM_STATUS_SOK on success
 *
 *******************************************************************************
 */
#define ALGORITHM_LINK_FPCOMPUTE_CMD_SET_ROI                 (0x1001)


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

    UInt32                   imgFrameStartX;
    /**< Start of actual video relative to start of input buffer */
    UInt32                   imgFrameStartY;
    /**< Start of actual video relative to start of input buffer */
    UInt32                   imgFrameWidth;
    /**< Width of the input frame */
    UInt32                   imgFrameHeight;
    /**< Height of the input frame */
} AlgorithmLink_FPComputeSetROIParams;

/**
 *******************************************************************************
 *
 *   \brief Structure containing create time parameters for feature plane
 *          computation algorithm
 *
 *******************************************************************************
*/
typedef struct
{
    AlgorithmLink_CreateParams baseClassCreate;
    /**< Base class create params. This structure should be first element */
    UInt32                   imgFrameStartX;
    /**< Start of actual video relative to start of input buffer */
    UInt32                   imgFrameStartY;
    /**< Start of actual video relative to start of input buffer */
    UInt32                   imgFrameWidth;
    /**< Width of the input frame */
    UInt32                   imgFrameHeight;
    /**< Height of the input frame */
    System_LinkOutQueParams  outQueParams;
    /**< Output queue information */
    System_LinkInQueParams   inQueParams;
    /**< Input queue information */
    UInt32                   numOutBuffers;
    /**< Number of output buffers */
} AlgorithmLink_FPComputeCreateParams;

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
} AlgorithmLink_FPComputeControlParams;

/*******************************************************************************
 *  Function Prototype
 *******************************************************************************
 */
static inline void AlgorithmLink_FPCompute_Init(
                    AlgorithmLink_FPComputeCreateParams *pPrm);

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
static inline void AlgorithmLink_FPCompute_Init(
    AlgorithmLink_FPComputeCreateParams *pPrm)
{
    memset(pPrm, 0, sizeof(AlgorithmLink_FPComputeCreateParams));

    pPrm->baseClassCreate.size = (UInt32)
                    sizeof(AlgorithmLink_FPComputeCreateParams);
    pPrm->baseClassCreate.algId = ALGORITHM_LINK_EVE_ALG_FEATUREPLANECOMPUTE;

    pPrm->imgFrameStartX = 0U;
    pPrm->imgFrameStartY = 0U;
    pPrm->imgFrameHeight = 768U;
    pPrm->imgFrameWidth  = 488U;
    pPrm->inQueParams.prevLinkId    = SYSTEM_LINK_ID_INVALID;
    pPrm->inQueParams.prevLinkQueId = 0U;
    pPrm->outQueParams.nextLink     = SYSTEM_LINK_ID_INVALID;
    pPrm->numOutBuffers             = 4U;
}


/**
 *******************************************************************************
 *
 * \brief Implementation of function to init plugins()
 *
 *        This function will be called by AlgorithmLink_initAlgPlugins, so as
 *        register plugins of feature plane computation algorithm
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
 */
Int32 AlgorithmLink_fpCompute_initPlugin(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/* @} */

/* Nothing beyond this point */
