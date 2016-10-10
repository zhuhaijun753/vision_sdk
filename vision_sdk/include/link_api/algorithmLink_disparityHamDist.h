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
 * \defgroup ALGORITHM_LINK_DISPARITY_HAMDIST  Disparity Haming Distance Computation API
 *
 * \brief  This module has the interface for Disparity Haming distance algorithm
 *         used in Stereo processing
 *         This alg takes input from Census transform Link and does the disparity
 *         computation
 * @{
 *
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \file algorithmLink_disparityHamDist.h
 *
 * \brief Algorithm Link API specific to feature plane computation algorithm
 *
 * \version 0.0 (Sept 2014) : [SR] First version
 *
 *******************************************************************************
 */

#ifndef ALGORITHM_LINK_DISPARITY_HAMDIST_H_
#define ALGORITHM_LINK_DISPARITY_HAMDIST_H_

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
 *   \brief Structure containing create time parameters for feature plane
 *          computation algorithm
 *
 *******************************************************************************
 */
typedef struct
{
    AlgorithmLink_CreateParams baseClassCreate;
    /**< Base class create params. This structure should be first element */
    UInt32                   srcImageWidth;
    /**< Width of the input frame's ROI */
    UInt32                   srcImageHeight;
    /**< Height of the input frame's ROI */
    UInt32                   imageRoiWidth;
    /**< Width of the input frame */
    UInt32                   imageRoiHeight;
    /**< Height of the input frame */
    UInt32                  inputBitDepth;
    /**< Input bit depth length */
    UInt32                  winWidth;
    /**< Window width size */
    UInt32                  winHeight;
    /**< Window height size */
    UInt32                  numDisparities;
    /**< Number of disparities */
    UInt32                  disparityStep;
    /**< Disparity step size */
    UInt32                  searchDir;
    /**< Search direction */
    UInt32                  extraRightLeftMap;
    /**< Extra Right Left map for left-right check */
    UInt32                  inputLeftImageStartX;
    /**< X offset for left frame */
    UInt32                  inputLeftImageStartY;
    /**< Y offset for left frame */
    UInt32                  inputRightImageStartX;
    /**< X offset for right frame */
    UInt32                  inputRightImageStartY;
    /**< Y offset for right frame */
    System_LinkOutQueParams  outQueParams;
    /**< Output queue information */
    System_LinkInQueParams   inQueParams;
    /**< Input queue information */
    UInt32                   numOutBuffers;
    /**< Number of output Buffers */
} AlgorithmLink_DisparityHamDistCreateParams;

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
} AlgorithmLink_DisparityHamDistControlParams;

/*******************************************************************************
 *  Function Prototypes
 *******************************************************************************
 */
static inline void AlgorithmLink_DisparityHamDist_Init(
                            AlgorithmLink_DisparityHamDistCreateParams *pPrm);

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
static inline void AlgorithmLink_DisparityHamDist_Init(
        AlgorithmLink_DisparityHamDistCreateParams *pPrm)
{
/* MISRA.ONEDEFRULE.FUNC
 * MISRAC_2004 Rule 8.5
 * Function Definition in header file
 * KW State: Defer -> Waiver -> Case by case
 * MISRAC_WAIVER: This function initializes the create time argument to default.
 *  Defining this in the header file aids maintainability.
 */
    UInt32 finalRoiWidth= 640U; /* VC */
    UInt32 finalRoiHeight= 360U; /* VC */

    memset(pPrm, 0, sizeof(AlgorithmLink_DisparityHamDistCreateParams));

    pPrm->baseClassCreate.size = (UInt32)
                            sizeof(AlgorithmLink_DisparityHamDistCreateParams);
    pPrm->baseClassCreate.algId = ALGORITHM_LINK_EVE_ALG_DISPARITY_HAMDIST;

    pPrm->imageRoiWidth       = finalRoiWidth; /* VC */
    pPrm->imageRoiHeight      = finalRoiHeight; /* VC */
    pPrm->inputBitDepth     = 32U;
    pPrm->winWidth          = 15U;
    pPrm->winHeight         = 15U;
    pPrm->numDisparities    = 64U;
    pPrm->disparityStep     = 2U;

    pPrm->srcImageWidth = ((finalRoiWidth + (pPrm->numDisparities - 1U) +
                            (pPrm->winWidth - 1U) + 15U) >> 4U) <<4U; /* VC */
    pPrm->srcImageHeight  = ((finalRoiHeight + (pPrm->winHeight - 1U) + 7U)
                                >> 3U) <<3U; /* VC */

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
 *        register plugins of soft isp algorithm
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
 */
Int32 AlgorithmLink_disparityHamDist_initPlugin(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/* @} */

/* Nothing beyond this point */
