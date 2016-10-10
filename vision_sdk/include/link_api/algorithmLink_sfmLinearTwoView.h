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
 * \defgroup ALGORITHM_LINK_SFMLTV_API  Sfm Linear Two View API
 *
 * \brief  This module has the interface for using SfM Linear Two View algorithm
 *
 *             -# Will take tracked, normalized image points from two camera views as input
 *             -# Will estimate camera motion and triangulate 3D points from
 *                these image correspondences
 *             -# see SfM Main algo for reference how to call SfM LTV
 *
 * @{
 *
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \file algorithmLink_sfmLinearTwoView.h
 *
 * \brief Algorithm Link API specific to Sfm LTV algorithm
 *
 * \version 0.0 (Jun 2015) : [MM] First version
 *
 *******************************************************************************
 */

#ifndef ALGORITHM_LINK_SFMLTV_H_
#define ALGORITHM_LINK_SFMLTV_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */
#include <include/link_api/system.h>
#include <include/link_api/algorithmLink.h>

#include "../../examples/tda2xx/src/alg_plugins/structurefrommotion/include/iSfmLinearTwoViewAlgo.h"

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
    ALGLINK_SFMLTV_IPQID_LTVAPI = 0x0,
    /**< QueueId for pointers to data that LTV will operate on */

    ALGLINK_SFMLTV_IPQID_MAXIPQ,
    /**< Maximum number of input queues */

    ALGLINK_SFMLTV_IPQID_FORCE32BITS = 0x7FFFFFFF
    /**< To make sure enum is 32 bits */

}AlgorithmLink_SfmLinearTwoViewInputQueId;

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
    ALGLINK_SFMLTV_OPQID_MAXOPQ = 0x0,
    /**< Maximum number of output queues */

    ALGLINK_SFMLTV_OPQID_FORCE32BITS = 0x7FFFFFFF
    /**< To make sure enum is 32 bits */

}AlgorithmLink_SfmLinearTwoViewOutputQueId;

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
    SfmLinearTwoView_CreateParams algCreateParams;
    /* Algorithm's create paramters defined in i<Algoname>Algo.h */
    System_LinkInQueParams   inQueParams[ALGLINK_SFMLTV_IPQID_MAXIPQ];
    /**< Input queue information */
} AlgorithmLink_SfmLinearTwoViewCreateParams;

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
} AlgorithmLink_SfmLinearTwoViewControlParams;

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
Int32 AlgorithmLink_sfmLinearTwoView_initPlugin(Void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/* @} */

/* Nothing beyond this point */
