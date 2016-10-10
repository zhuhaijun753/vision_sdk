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
 * \defgroup ALGORITHM_LINK_OPENCVCANNY_API OpenCV Canny Edge API
 *
 * \brief  This module has the interface for using opencv canny algorithm
 *
 *         OpenCV Canny is a sample algorithm, which just performs canny 
 *         edge detection on input buffer
 *
 * @{
 *
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \file algorithmLink_opencvCanny.h
 *
 * \brief Algorithm Link API specific to opencv canny algorithm on DSP
 *
 * \version 0.0 (Aug 2013) : [PS] First version
 *
 *******************************************************************************
 */

#ifndef ALGORITHM_LINK_OPENCVCANNY_H_
#define ALGORITHM_LINK_OPENCVCANNY_H_

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
 *   \brief Structure containing create time parameters for OpenCV Canny algorithm
 *
 *          This structure is a replica of create time parameters of opencv canny
 *          algorithm
 *
 *******************************************************************************
*/
typedef struct
{
    AlgorithmLink_CreateParams baseClassCreate;
    /**< Base class create params */
    UInt32                    maxHeight;
    /**< Max height of the frame */
    UInt32                    maxWidth;
    /**< max width of the frame */
    UInt32                    numOutputFrames;
    /**< Number of output frames to be created for this link per channel*/
    System_LinkOutQueParams   outQueParams;
    /**< Output queue information */
    System_LinkInQueParams    inQueParams;
    /**< Input queue information */
} AlgorithmLink_OpenCVCannyCreateParams;

/**
 *******************************************************************************
 *
 *   \brief Structure containing control parameters for OpenCV Canny algorithm
 *
 *          This structure is a replica of control parameters of opencv canny
 *          algorithm
 *
 *******************************************************************************
*/
typedef struct
{
    AlgorithmLink_ControlParams baseClassControl;
    /**< Base class control params */
} AlgorithmLink_opencvCannyControlParams;

/*******************************************************************************
 *  Functions
 *******************************************************************************
 */
 static inline void AlgorithmLink_OpenCVCanny_Init(
                            AlgorithmLink_OpenCVCannyCreateParams *pPrm);

/**
 *******************************************************************************
 *
 * \brief Set defaults for plugin create parameters
 *
 * \param pPrm  [OUT] plugin create parameters
 *
 *******************************************************************************
 */
static inline void AlgorithmLink_OpenCVCanny_Init(
                            AlgorithmLink_OpenCVCannyCreateParams *pPrm)
{

/* Do nothing */
}

/**
 *******************************************************************************
 *
 * \brief Implementation of function to init plugins()
 *
 *        This function will be called by AlgorithmLink_initAlgPlugins, so as
 *        register plugins of opencv canny algorithm
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
 */
Int32 AlgorithmLink_OpenCVCanny_initPlugin(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/* @} */

/* Nothing beyond this point */
