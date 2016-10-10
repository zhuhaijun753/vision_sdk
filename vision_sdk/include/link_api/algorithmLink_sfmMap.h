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
 * \defgroup ALGORITHM_LINK_SFMMAP_API  Sfm Map API
 *
 * \brief  This module has the interface for using SfM Map algorithm
 *
 *             -# Will take 3D points in world coordinates
 *             -# Will create an occupancy grid map and extract object boxes
 *
 * @{
 *
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \file algorithmLink_sfmMap.h
 *
 * \brief Algorithm Link API specific to Sfm Map algorithm
 *
 * \version 0.0 (Jun 2015) : [MM] First version
 *
 *******************************************************************************
 */

#ifndef ALGORITHM_LINK_SFMMAP_H_
#define ALGORITHM_LINK_SFMMAP_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */
#include <include/link_api/system.h>
#include <include/link_api/algorithmLink.h>

#include "../../examples/tda2xx/src/alg_plugins/structurefrommotion/include/iSfmMapAlgo.h"

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
    ALGLINK_SFMMAP_IPQID_3DPOINTS = 0x0,
    /**< QueueId for 3D points */

    ALGLINK_SFMMAP_IPQID_POSES,
    /**< QueueId for poses */

    ALGLINK_SFMMAP_IPQID_MAXIPQ,
    /**< Maximum number of input queues */

    ALGLINK_SFMMAP_IPQID_FORCE32BITS = 0x7FFFFFFF
    /**< To make sure enum is 32 bits */

}AlgorithmLink_SfmMapInputQueId;

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
    ALGLINK_SFMMAP_OPQID_BOXES = 0x0,
    /**< QueueId for object boxes */

    ALGLINK_SFMMAP_OPQID_MAXOPQ,
    /**< Maximum number of output queues */

    ALGLINK_SFMMAP_OPQID_FORCE32BITS = 0x7FFFFFFF
    /**< To make sure enum is 32 bits */

}AlgorithmLink_SfmMapOutputQueId;

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
    SfmMap_CreateParams algCreateParams;
    /* Algorithm's create paramters defined in i<Algoname>Algo.h */
    System_LinkOutQueParams  outQueParams[ALGLINK_SFMMAP_OPQID_MAXOPQ];
    /**< Output queue information */
    System_LinkInQueParams   inQueParams[ALGLINK_SFMMAP_IPQID_MAXIPQ];
    /**< Input queue information */
} AlgorithmLink_SfmMapCreateParams;

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
} AlgorithmLink_SfmMapControlParams;

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
Int32 AlgorithmLink_sfmMap_initPlugin(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/* @} */

/* Nothing beyond this point */
