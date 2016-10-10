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
 * \defgroup ALGORITHM_LINK_IMAGEPREPROCESS Image Pre-process Algorithm API
 *
 * \brief  This module has the interface for using feature plane comp algorithm
 *
 *         Image preprocess algorithm link -
 *             1. This alg takes input from previous link applies filter 2d , yuv
 *                padding to it.This is pre-cursor to image pyramid Algorithm.
 *
 *
 * @{
 *
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \file algorithmLink_imagePreProcess.h
 *
 * \brief Algorithm Link API specific to feature plane computation algorithm
 *
 * \version 0.0 (Feb 2013) : [NN] First version
 *
 *******************************************************************************
 */

#ifndef ALGORITHM_LINK_IMAGEPREPROCESS_H_
#define ALGORITHM_LINK_IMAGEPREPROCESS_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */
#include <include/link_api/system.h>
#include <include/link_api/algorithmLink.h>


/* @} */

/*******************************************************************************
 *  Defines
 *******************************************************************************
 */

#define  ALGORITHM_LINK_IMAGEPREPROCESS_PAD_X  (64)
#define  ALGORITHM_LINK_IMAGEPREPROCESS_PAD_Y  (64)



/*******************************************************************************
 *  Data structures
 *******************************************************************************
 */

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
} AlgorithmLink_ImagePreProcessCreateParams;



/*******************************************************************************
 *  Function Prototype
 *******************************************************************************
 */
static inline void AlgorithmLink_ImagePreProcess_Init(
                    AlgorithmLink_ImagePreProcessCreateParams *pPrm);

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
static inline void AlgorithmLink_ImagePreProcess_Init(
    AlgorithmLink_ImagePreProcessCreateParams *pPrm)
{
    memset(pPrm, 0, sizeof(AlgorithmLink_ImagePreProcessCreateParams));

    pPrm->baseClassCreate.size = (UInt32)
                    sizeof(AlgorithmLink_ImagePreProcessCreateParams);
    pPrm->baseClassCreate.algId = ALGORITHM_LINK_EVE_ALG_IMAGEPREPROCESS;

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
Int32 AlgorithmLink_imagePreProcess_initPlugin(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/* @} */

/* Nothing beyond this point */
