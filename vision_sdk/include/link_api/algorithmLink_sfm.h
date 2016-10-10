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
 * \defgroup ALGORITHM_LINK_SFM  Sfm API
 *
 * \brief  This module has the interface for using Sfm algorithm
 *
 * @{
 *
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \file algorithmLink_sfm.h
 *
 * \brief Algorithm Link API specific to sfm algorithm
 *
 *******************************************************************************
 */

#ifndef ALGORITHM_LINK_SFM_H_
#define ALGORITHM_LINK_SFM_H_

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

/*
 *******************************************************************************
 * \brief Number of camera intrinsic parameters
 *******************************************************************************
 */
#define ALG_SFM_CAMERA_INTRENSIC_PARAM_SIZE         (9U)

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

    /*   Sfm Advanced parmaeters,
     *   refer Sfm algo user guide for more details
     */
    UInt32                   maxTrackPoints;
    /**< Max points to track in SOF */

    UInt32 cameraExtrinsicParamsBufAddr;
    /**< Pointer to extrinic parameters of the camera */

    UInt32 cameraExtrinsicParamsBufSize;
    /**< Size of cameraExtrinsicParamsBuf */

    UInt32 cameraExtrinsicParamsDataSize;
    /**< Size of valid data present in cameraExtrinsicParamsBuf */

    float cameraIntrinsicParams[ALG_SFM_CAMERA_INTRENSIC_PARAM_SIZE];
    /**< Intrinic parameters of the camera */

} AlgorithmLink_SfmCreateParams;

/**
 *******************************************************************************
 *
 *   \brief Structure containing control parameters for Sfm algorithm
 *
 *******************************************************************************
*/
typedef struct
{
    AlgorithmLink_ControlParams baseClassControl;
    /**< Base class control params */
} AlgorithmLink_SfmControlParams;

/*******************************************************************************
 *  Functions
 *******************************************************************************
 */
static inline void AlgorithmLink_Sfm_Init(
                            AlgorithmLink_SfmCreateParams *pPrm);

/**
 *******************************************************************************
 *
 * \brief Set defaults for plugin create parameters
 *
 * \param pPrm  [OUT] plugin create parameters
 *
 *******************************************************************************
 */
static inline void AlgorithmLink_Sfm_Init(
    AlgorithmLink_SfmCreateParams *pPrm)
{
/* MISRA.ONEDEFRULE.FUNC
 * MISRAC_2004 Rule 8.5
 * Function Definition in header file
 * KW State: Defer -> Waiver -> Case by case
 * MISRAC_WAIVER: This function initializes the create time argument to default.
 *  Defining this in the header file aids maintainability.
 */
    memset(pPrm, 0, sizeof(AlgorithmLink_SfmCreateParams));

    pPrm->baseClassCreate.size = (UInt32)
                                sizeof(AlgorithmLink_SfmCreateParams);
    pPrm->baseClassCreate.algId = ALGORITHM_LINK_DSP_ALG_SFM;

    pPrm->maxTrackPoints    = 600;

    pPrm->inQueParams.prevLinkId = SYSTEM_LINK_ID_INVALID;
    pPrm->inQueParams.prevLinkQueId = 0U;
    pPrm->numOutBuffers = 3U;
    pPrm->outQueParams.nextLink = SYSTEM_LINK_ID_INVALID;

}


/**
 *******************************************************************************
 *
 * \brief Implementation of function to init plugins()
 *
 *        This function will be called by AlgorithmLink_initAlgPlugins, so as
 *        register plugins of sfm algorithm
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
 */
Int32 AlgorithmLink_sfm_initPlugin(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/* @} */

/* Nothing beyond this point */
