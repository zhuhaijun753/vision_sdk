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
 * \defgroup ALGORITHM_LINK_FCW  Forward Collision Warning API
 *
 * \brief  This module has the interface for using Forward Collision warning
 *         algorithm
 *
 * @{
 *
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \file algorithmLink_fcw.h
 *
 * \brief Algorithm Link API specific to forward collision warning algorithm
 *
 *******************************************************************************
 */

#ifndef ALGORITHM_LINK_FCW_H_
#define ALGORITHM_LINK_FCW_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */
#include <include/link_api/system.h>
#include <include/link_api/algorithmLink.h>
#include <include/link_api/algorithmLink_sfm.h>

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

typedef struct
{
    AlgorithmLink_CreateParams baseClassCreate;
    /**< Base class create params. This structure should be first element */
    System_LinkOutQueParams  outQueParams;
    /**< Output queue information */
    System_LinkInQueParams   inQueParams;
    /**< Input queue information */
    UInt32                   numOutBuffers;
    /**< Number of output Buffers */

    /*   Fcw Advanced parmaeters,
     *   refer algo user guide for more details
     */
    UInt32   imageHeight;
    UInt32   imageWidth;
    UInt32   frameRate;
    float    minObjDist        ;
    float    maxObjDist        ;
    float    minObjHeight      ;
    float    maxObjHeight      ;
    float    camHeightInMeter  ;
    float    camOffsetInMeter  ;
    float    cameraIntrinsicParams[ALG_SFM_CAMERA_INTRENSIC_PARAM_SIZE];
    /**< Intrinic parameters of the camera */

} AlgorithmLink_FcwCreateParams;

/**
 *******************************************************************************
 *
 *   \brief Structure containing control parameters for Fcw algorithm
 *
 *******************************************************************************
*/
typedef struct
{
    AlgorithmLink_ControlParams baseClassControl;
    /**< Base class control params */
} AlgorithmLink_FcwControlParams;

/*******************************************************************************
 *  Functions
 *******************************************************************************
 */
static inline void AlgorithmLink_Fcw_Init(
                            AlgorithmLink_FcwCreateParams *pPrm);

/**
 *******************************************************************************
 *
 * \brief Set defaults for plugin create parameters
 *
 * \param pPrm  [OUT] plugin create parameters
 *
 *******************************************************************************
 */
static inline void AlgorithmLink_Fcw_Init(
    AlgorithmLink_FcwCreateParams *pPrm)
{
/* MISRA.ONEDEFRULE.FUNC
 * MISRAC_2004 Rule 8.5
 * Function Definition in header file
 * KW State: Defer -> Waiver -> Case by case
 * MISRAC_WAIVER: This function initializes the create time argument to default.
 *  Defining this in the header file aids maintainability.
 */
    memset(pPrm, 0, sizeof(AlgorithmLink_FcwCreateParams));

    pPrm->baseClassCreate.size = (UInt32)
                                sizeof(AlgorithmLink_FcwCreateParams);
    pPrm->baseClassCreate.algId = ALGORITHM_LINK_DSP_ALG_FCW;

    pPrm->inQueParams.prevLinkId = SYSTEM_LINK_ID_INVALID;
    pPrm->inQueParams.prevLinkQueId = 0U;
    pPrm->numOutBuffers = 3U;
    pPrm->outQueParams.nextLink = SYSTEM_LINK_ID_INVALID;
    pPrm->minObjDist = 0;
    pPrm->maxObjDist = 60.0;
    pPrm->minObjHeight = 0.65;
    pPrm->maxObjHeight = 2.6;
    pPrm->imageHeight = 720;
    pPrm->frameRate = 25;
    pPrm->camHeightInMeter = 1.58;
    pPrm->camOffsetInMeter = 1.4;
}


/**
 *******************************************************************************
 *
 * \brief Implementation of function to init plugins()
 *
 *        This function will be called by AlgorithmLink_initAlgPlugins, so as
 *        register plugins of algorithm
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
 */
Int32 AlgorithmLink_fcw_initPlugin(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/* @} */

/* Nothing beyond this point */
