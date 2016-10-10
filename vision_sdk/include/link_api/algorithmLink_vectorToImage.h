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
 * \defgroup ALGORITHM_LINK_VECTORTOIMAGE_API  Vector to \
 *  Image API
 *
 *
 * \brief  This module has the interface for using vector to image algorithm
 *
 *         Vector to Image algorithm link -
 *
 *         Will take in optical flow vectors as input and generate a
 *         color image representing the flow vectors
 *          - Color will indicate the direction of flow vector
 *          - Intensity will indicate the magnitude of flow vector
 *
 * @{
 *
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \file algorithmLink_vectorToImage.h
 *
 * \brief Algorithm Link API specific to vector to image algorithm
 *
 * \version 0.0 (Nov 2013) : [PS] First version
 *
 *******************************************************************************
 */

#ifndef ALGORITHM_LINK_VECTORTOIMAGE_H_
#define ALGORITHM_LINK_VECTORTOIMAGE_H_

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
 *   \brief Structure containing create time parameters for Vector to Image
 *          algorithm
 *
 *******************************************************************************
*/
typedef struct
{
    AlgorithmLink_CreateParams baseClassCreate;
    /**< Base class create params. This structure should be first element */
    UInt32                   maxHeight;
    /**< Max height of the image (Input Flow vector / output color image) */
    UInt32                   maxWidth;
    /**< max width of the output (Input Flow vector / output color image) */
    UInt32                   numOutputFrames;
    /**< Number of output frames */
    System_LinkOutQueParams  outQueParams;
    /**< Output queue information */
    System_LinkInQueParams   inQueParams;
    /**< Input queue information */
    UInt32                   roiEnable;
    /**< Enable ROI based processing */
    UInt32                   numRoi;
    /**< Number of ROI's */
    AlgorithmLink_RoiParams  roiParams[4];
    /**< ROI params */
    UInt32                   lutId;
    /**< ID of LUT to use */

    Bool                     isLutSize_129x129;
    /**< Size info of the UT to be used */

    System_VideoDataFormat          dataFormat;
    /**< Output Data format, valid options are
     *   SYSTEM_DF_YUV422I_YUYV,<br>
     *   SYSTEM_DF_BGR16_565,<br>
     */

} AlgorithmLink_VectorToImageCreateParams;

/**
 *******************************************************************************
 *
 *   \brief Structure containing control parameters for Vector to Image
 *          algorithm
 *
 *******************************************************************************
*/
typedef struct
{
    AlgorithmLink_ControlParams baseClassControl;
    /**< Base class control params */
} AlgorithmLink_VectorToImageControlParams;

/*******************************************************************************
 *  Functions
 *******************************************************************************
 */
static inline void AlgorithmLink_VectorToImageCreateParams_Init(
                                AlgorithmLink_VectorToImageCreateParams *pPrm);

/**
 *******************************************************************************
 *
 * \brief Set defaults for plugin create parameters
 *
 * \param pPrm  [OUT] plugin create parameters
 *
 *******************************************************************************
 */
static inline void AlgorithmLink_VectorToImageCreateParams_Init(
                AlgorithmLink_VectorToImageCreateParams *pPrm)
{
/* MISRA.ONEDEFRULE.FUNC
 * MISRAC_2004 Rule 8.5
 * Function Definition in header file
 * KW State: Defer -> Waiver -> Case by case
 * MISRAC_WAIVER: This function initializes the create time argument to default.
 *  Defining this in the header file aids maintainability.
 */
    memset(pPrm, 0, sizeof(AlgorithmLink_VectorToImageCreateParams));

    pPrm->baseClassCreate.size = (UInt32)
                                sizeof(AlgorithmLink_VectorToImageCreateParams);
    pPrm->baseClassCreate.algId = ALGORITHM_LINK_DSP_ALG_VECTORTOIMAGE;

    pPrm->maxHeight = 1920U;
    pPrm->maxWidth  = 1080U;

    pPrm->numOutputFrames = 4U;

    pPrm->inQueParams.prevLinkId = SYSTEM_LINK_ID_INVALID;
    pPrm->inQueParams.prevLinkQueId = 0U;

    pPrm->outQueParams.nextLink = SYSTEM_LINK_ID_INVALID;

    pPrm->roiEnable = 0U;
    pPrm->lutId = 0U;

    pPrm->dataFormat = SYSTEM_DF_BGR16_565;
}

/**
 *******************************************************************************
 *
 * \brief Implementation of function to init plugins()
 *
 *        This function will be called by AlgorithmLink_initAlgPlugins, so as
 *        register plugins of vector to image algorithm
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
 */
Int32 AlgorithmLink_vectorToImage_initPlugin(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/* @} */

/* Nothing beyond this point */
