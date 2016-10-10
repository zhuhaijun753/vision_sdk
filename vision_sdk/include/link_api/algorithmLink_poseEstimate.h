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
 * \defgroup ALGORITHM_LINK_POSE_ESTIMATE_API Pose Estimation Algorithm APIs
 *
 * \brief  This module has the interface for using pose estimate algorithm
 *
 *         Pose Estimate algorithm link -
 *           -# Will take feature points generate by automatic chart detect and
 *              generate calibration matrix, which will get used by the geometric alignment stage.
 *
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
 * \file algorithmLink_poseEstimate.h
 *
 * \brief Algorithm Link API specific to pose estimate algorithm
 *
 *
 *******************************************************************************
 */

#ifndef ALGORITHM_LINK_POSEESTIMATE_H_
#define ALGORITHM_LINK_POSEESTIMATE_H_

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
    ALGLINK_PESTIMATE_IPQID_FEATURE = 0x0,
    /**< QueueId for multiview images */

    ALGLINK_PESTIMATE_IPQID_MAXIPQ,
    /**< Maximum number of input queues */

    ALGLINK_PESTIMATE_IPQID_FORCE32BITS = 0x7FFFFFFF
    /**< To make sure enum is 32 bits */

}AlgorithmLink_PEstimateInputQueId;

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
    ALGLINK_PESTIMATE_OPQID_CALMAT = 0x0,
    /**< QueueId for calibration matrix output */

    ALGLINK_PESTIMATE_OPQID_MAXOPQ,
    /**< Maximum number of output queues */

    ALGLINK_PESTIMATE_OPQID_FORCE32BITS = 0x7FFFFFFF
    /**< To make sure enum is 32 bits */

}AlgorithmLink_PEstimateOutputQueId;

/**
 *******************************************************************************
 * \brief Enum for mode of pose estimate
 *
 * SUPPORTED in ALL platforms
 *
 *******************************************************************************
*/
typedef enum
{
    ALGLINK_PESTIMATE_MODE_SKIP = 0x0,
    /**< skip extrinsic pose estimation  */

    ALGLINK_PESTIMATE_MODE_ENABLE,
    /**< enable pose estimation */

    ALGLINK_PESTIMATE_MODE_DEFAULT,
    /**< use default calibration matrix */

    ALGLINK_PESTIMATE_MODE_FORCE32BITS = 0x7FFFFFFF
    /**< To make sure enum is 32 bits */

}AlgorithmLink_PEstimateMode;

/*******************************************************************************
 *  Data structures
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 *   \brief Structure containing create time parameters for Pose Estimate
 *          algorithm
 *
 *******************************************************************************
*/
typedef struct
{
	AlgorithmLink_PEstimateMode  pEstimateMode;
    /**< pose estimation mode to be used */
    void *                       pEstCalMatDRPtr;
    /**< DDR pointer for calibration matrix */
    void *                       featurePtDDRPtr;
    /**< DDR pointer for feature pts */
} AlgorithmLink_PEstimateParams;

/**
 *******************************************************************************
 *
 *   \brief Structure containing create time parameters for Pose Estimate
 *          algorithm
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
    /**< number of input views from which output will be synthesized */
    UInt32                   numOutput;
    /**< Number of output calibration matrix */
    System_LinkOutQueParams  outQueParams[ALGLINK_PESTIMATE_OPQID_MAXOPQ];
    /**< Output queue information */
    System_LinkInQueParams   inQueParams[ALGLINK_PESTIMATE_IPQID_MAXIPQ];
    /**< Input queue information */
    AlgorithmLink_PEstimateParams pEstimateParams;
    /**< Pose Estimate parameters */
    UInt32                  ignoreFirstNFrames;
    /**< The first set of frames received should be visible.
            In cases where Auto White balance is enabled and cannot grantee
            first good frame.
            Use this to configure the algorithm to skip, until valid frame is
            received. */
    UInt32                  defaultFocalLength;
    /**< Focal length of the lens used */
} AlgorithmLink_PEstimateCreateParams;

/**
 *******************************************************************************
 *
 *   \brief Structure containing control parameters for Pose Estimate
 *          algorithm
 *
 *******************************************************************************
*/
typedef struct
{
    AlgorithmLink_ControlParams baseClassControl;
    /**< Base class control params */
} AlgorithmLink_PEstimateControlParams;

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
 *        register plugins of geometric alignment algorithm
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
 */
Int32 AlgorithmLink_pEstimate_initPlugin(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/* @} */

/* Nothing beyond this point */
