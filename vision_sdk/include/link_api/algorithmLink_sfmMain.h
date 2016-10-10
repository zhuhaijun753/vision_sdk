/*
 *******************************************************************************
 *
 * Copyright (C) 2015 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \ingroup  ALGORITHM_LINK_PLUGIN
 * \defgroup ALGORITHM_LINK_SFMMAIN_API  Sfm Main API
 *
 * \brief  This module has the interface for using SfM Main algorithm
 *
 *             -# Will take sparse optical flow tracks and multi-camera calibration
 *                results.
 *             -# Will interact with SfM Linear Two View (LTV) Algo to estimate
 *               camera poses and 3D points between two views.
 *             -# Will return vehicle pose, camera poses and 3D points in a
 *                world coordinate system.
 *
 * @{
 *
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \file algorithmLink_sfmMain.h
 *
 * \brief Algorithm Link API specific to Sfm Main algorithm
 *
 * \version 0.0 (Jun 2015) : [MM] First version
 *
 *******************************************************************************
 */

#ifndef ALGORITHM_LINK_SFMMAIN_H_
#define ALGORITHM_LINK_SFMMAIN_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */
#include <include/link_api/system.h>
#include <include/link_api/algorithmLink.h>

#include "../../examples/tda2xx/src/alg_plugins/structurefrommotion/include/iSfmMainAlgo.h"

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
    ALGLINK_SFMMAIN_IPQID_SOFTRACKS = 0x0,
    /**< QueueId for sparse optical flow tracks*/

    ALGLINK_SFMMAIN_IPQID_MAXIPQ,
    /**< Maximum number of input queues */

    ALGLINK_SFMMAIN_IPQID_FORCE32BITS = 0x7FFFFFFF
    /**< To make sure enum is 32 bits */

}AlgorithmLink_SfmMainInputQueId;

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
    ALGLINK_SFMMAIN_OPQID_POSES = 0x0,
    /**< QueueId for vehicle and camera poses */

    ALGLINK_SFMMAIN_OPQID_POINTS,
    /**< QueueId for 3D points and confidences*/

    ALGLINK_SFMMAIN_OPQID_LTVAPI,
    /* *<QueueId for API with Linear Two View (LTV) Algo */

    ALGLINK_SFMMAIN_OPQID_MAXOPQ,
    /**< Maximum number of output queues */

    ALGLINK_SFMMAIN_OPQID_FORCE32BITS = 0x7FFFFFFF
    /**< To make sure enum is 32 bits */

}AlgorithmLink_SfmMainOutputQueId;

/*******************************************************************************
 *  Data structures
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 *   \brief Structure containing create time parameters
 *
 *******************************************************************************
*/
typedef struct
{
    AlgorithmLink_CreateParams baseClassCreate;
    /**< Base class create params. This structure should be first element */
    SfmMain_CreateParams algCreateParams;
    /* Algorithm's create paramters defined in i<Algoname>Algo.h */
    System_LinkOutQueParams  outQueParams[ALGLINK_SFMMAIN_OPQID_MAXOPQ];
    /**< Output queue information */
    System_LinkInQueParams   inQueParams[ALGLINK_SFMMAIN_IPQID_MAXIPQ];
    /**< Input queue information */
    Int32 *calmatAddr;
    /**< DDR pointer for calibration matrices */
} AlgorithmLink_SfmMainCreateParams;

/**
 *******************************************************************************
 *
 *   \brief Structure containing control parameters
 *
 *******************************************************************************
*/
typedef struct
{
    AlgorithmLink_ControlParams baseClassControl;
    /**< Base class control params */
} AlgorithmLink_SfmMainControlParams;

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
 *        register plugins
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
 */
Int32 AlgorithmLink_sfmMain_initPlugin(Void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/* @} */

/* Nothing beyond this point */
