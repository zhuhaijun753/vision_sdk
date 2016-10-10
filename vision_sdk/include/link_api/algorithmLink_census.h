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
 * \defgroup ALGORITHM_LINK_CENSUS  Census API
 *
 * \brief  This module has the interface for census algorithm used in Stereo
 *         processing
 * @{
 *
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \file algorithmLink_census.h
 *
 * \brief Algorithm Link API specific to feature plane computation algorithm
 *
 * \version 0.0 (Sept 2014) : [SR] First version
 *
 *******************************************************************************
 */

#ifndef ALGORITHM_LINK_CENSUS_H_
#define ALGORITHM_LINK_CENSUS_H_

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
    /**< Width of the input frame */
    UInt32                   srcImageHeight;
    /**< Height of the input frame */
    UInt32                   imageRoiWidth;
    /**< Width of the input frame */
    UInt32                   imageRoiHeight;
    /**< Height of the input frame */
    UInt32                   inputBitDepth;
    /**< Input frame bit depth */
    UInt32                   winWidth;
    /**< Window Width */
    UInt32                   winHeight;
    /**< Window Height */
    UInt32                   winHorzStep;
    /**< Window Horizontal Step size */
    UInt32                   winVertStep;
    /**< Window Vertical Step size */
    System_LinkOutQueParams  outQueParams;
    /**< Output queue information */
    System_LinkInQueParams   inQueParams;
    /**< Input queue information */
    UInt32                   numOutBuffers;
    /**< Number of output Buffers */
} AlgorithmLink_CensusCreateParams;

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
} AlgorithmLink_CensusControlParams;

/*******************************************************************************
 *  Function Prototypes
 *******************************************************************************
 */
static inline void AlgorithmLink_Census_Init(
                                AlgorithmLink_CensusCreateParams *pPrm);

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
static inline void AlgorithmLink_Census_Init(
                                AlgorithmLink_CensusCreateParams *pPrm)
{
/* MISRA.ONEDEFRULE.FUNC
 * MISRAC_2004 Rule 8.5
 * Function Definition in header file
 * KW State: Defer -> Waiver -> Case by case
 * MISRAC_WAIVER: This function initializes the create time argument to default.
 *  Defining this in the header file aids maintainability.
 */
 
   /* VC: If we can pass AlgorithmLink_DisparityHamDistCreateParams as a second argument then below values can be taken from it instead of being hardcoded */
   UInt32 finalRoiWidth= 640U; /* VC, ideally should be disparityHamDistCreateParams->imageRoiWidth */
   UInt32 finalRoiHeight= 360U; /* VC, ideally should be disparityHamDistCreateParams->imageRoiHeight */
   UInt32 numDisparities= 64U; /* VC, ideally should be disparityHamDistCreateParams->numDisparities */
   UInt32 disparityWinWidth= 15U; /* VC, ideally should be disparityHamDistCreateParams->winWidth */
   UInt32 disparityWinHeight= 15U; /* VC, ideally should be disparityHamDistCreateParams->winHeight */

    memset(pPrm, 0, sizeof(AlgorithmLink_CensusCreateParams));

    pPrm->baseClassCreate.size = (UInt32) sizeof(AlgorithmLink_CensusCreateParams);
    pPrm->baseClassCreate.algId = ALGORITHM_LINK_EVE_ALG_CENSUS;

    pPrm->imageRoiWidth     = ((finalRoiWidth + (numDisparities - 1U) + /* VC */
                                (disparityWinWidth - 1U) + 15U) >> 4U) << 4U;
    pPrm->imageRoiHeight     = ((finalRoiHeight + (disparityWinHeight - 1U) +
                                7U) >> 3U) <<3U; /* VC */

    pPrm->inputBitDepth     = 8U; /*YUV420 format*/
    pPrm->winWidth          = 9U;
    pPrm->winHeight         = 9U;
    pPrm->winHorzStep       = 2U;
    pPrm->winVertStep       = 2U;

    pPrm->srcImageWidth     = 800U; /* VC: must match ISP output's width and must be >= pPrm->imageRoiWidth + pPrm->winWidth - 1; */
    pPrm->srcImageHeight    = 450; /* VC: must match ISP output's height and must be >= pPrm->imageRoiHeight + pPrm->winHeight - 1; */

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
Int32 AlgorithmLink_census_initPlugin(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/* @} */

/* Nothing beyond this point */
