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
 * \defgroup ALGORITHM_LINK_AUTO_CHART_DETECT_API  Auto Chart Detection \
 *           Algo APIs
 *
 * \brief  This module has the interface for using automatic chart detection algorithm
 *
 *         Automatic Chart Detect algorithm link -
 *           -# Will take in images from multiple views and generate
 *              feature points, which will get used by the pose estimation

 *           -# Will call the algorithm process function once in K frames.
 *              For the remaining (K-1) frames, the link shall immediately
 *              release back the input video buffers.
 *
 * @{
 *
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \file algorithmLink_autoChartDetect.h
 *
 * \brief Algorithm Link API specific to automatic chart detection algorithm
 *
 *
 *******************************************************************************
 */

#ifndef ALGORITHM_LINK_AUTOCHARTDETECT_H_
#define ALGORITHM_LINK_AUTOCHARTDETECT_H_

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
/**
 *******************************************************************************
 * \brief Enum for the input Q IDs
 *
 * SUPPORTED in ALL platforms
 *
 *******************************************************************************
*/
typedef enum
{
    ALGLINK_ACDETECT_IPQID_MULTIVIEW = 0x0,
    /**< QueueId for multiview images */

    ALGLINK_ACDETECT_IPQID_MAXIPQ,
    /**< Maximum number of input queues */

    ALGLINK_ACDETECT_IPQID_FORCE32BITS = 0x7FFFFFFF
    /**< To make sure enum is 32 bits */

} AlgorithmLink_ACDetectInputQueId;

/**
 *******************************************************************************
 * \brief Enum for the output Q IDs
 *
 * SUPPORTED in ALL platforms
 *
 *******************************************************************************
*/
typedef enum
{
    ALGLINK_ACDETECT_OPQID_FEATURE = 0x0,
    /**< QueueId for feature points output */

    ALGLINK_ACDETECT_OPQID_MAXOPQ,
    /**< Maximum number of output queues */

    ALGLINK_ACDETECT_OPQID_FORCE32BITS = 0x7FFFFFFF
    /**< To make sure enum is 32 bits */

}AlgorithmLink_ACDetectOutputQueId;

/**
 *******************************************************************************
 * \brief Enum for mode of automatic chart detection
 *
 * SUPPORTED in ALL platforms
 *
 *******************************************************************************
*/
typedef enum
{
    ALGLINK_ACDETECT_MODE_SKIP = 0x0,
    /**< skip automatic chart detection */

    ALGLINK_ACDETECT_MODE_ENABLE,
    /**< use automatic chart detection  */

    ALGLINK_ACDETECT_MODE_DEFAULT,
    /**< Use default feature locations */

    ALGLINK_ACDETECT_MODE_FORCE32BITS = 0x7FFFFFFF
    /**< To make sure enum is 32 bits */

}AlgorithmLink_ACDetectMode;

/*******************************************************************************
 *  Data structures
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 *   \brief Structure containing create time parameters for Automatic Chart
 *          Detect algorithm
 *
 *******************************************************************************
*/
typedef struct
{
	AlgorithmLink_ACDetectMode  acDetectMode;
    /**< automatic chart detection mode to be used */

    void *                   featurePtDDRPtr;
    /**< DDR pointer for feature pts */
} AlgorithmLink_ACDetectParams;

/**
 *******************************************************************************
 *
 *   \brief Structure containing create time parameters for Automatic Chart
 *          Detect algorithm
 *
 *******************************************************************************
*/
typedef struct
{
    AlgorithmLink_CreateParams baseClassCreate;
    /**< Base class create params. This structure should be first element */
    UInt32                   maxInputHeight;
    /**< Max height of the input (captured) frame */
    UInt32                   maxInputWidth;
    /**< Max width of the input (captured) frame */
    UInt32                   numViews;
    /**< number of input views from which feature points will be detected */
    UInt32                   numOutput;
    /**< Number of output feature points */
    System_LinkOutQueParams  outQueParams[ALGLINK_ACDETECT_OPQID_MAXOPQ];
    /**< Output queue information */
    System_LinkInQueParams   inQueParams[ALGLINK_ACDETECT_IPQID_MAXIPQ];
    /**< Input queue information */
    AlgorithmLink_ACDetectParams acDetectParams;
    /**< Automatic Chart Detect parameters */
    UInt32                  ignoreFirstNFrames;
    /**< The first set of frames received should be visible.
            In cases where Auto White balance is enabled and cannot grantee
            first good frame.
            Use this to configure the algorithm to skip, until valid frame is
            received. */
    UInt32                  defaultFocalLength;
    /**< Focal length of the lens used */
} AlgorithmLink_ACDetectCreateParams;

/**
 *******************************************************************************
 *
 *   \brief Structure containing control parameters for Automatic Chart Detect
 *          algorithm
 *
 *******************************************************************************
*/
typedef struct
{
    AlgorithmLink_ControlParams baseClassControl;
    /**< Base class control params */
} AlgorithmLink_ACDetectControlParams;

/*******************************************************************************
 *  Functions
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \brief Implementation of function to init plugins()
 *
 *        This function will be called by AlgorithmLink_initAlgPlugins, so as
 *        register plugins of automatic chart detect algorithm
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
 */
Int32 AlgorithmLink_acDetect_initPlugin(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/* @} */

/* Nothing beyond this point */
