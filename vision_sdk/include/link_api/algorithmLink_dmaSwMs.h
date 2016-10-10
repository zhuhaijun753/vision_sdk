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
 * \defgroup ALGORITHM_LINK_DMA_SW_MS_API  DMA Software Mosaic \
 *                                        API
 *
 * \brief  This module has the interface for the algorithm plugin which uses
 *         DMA to perform the function of Software mosaic
 *
 *         This plugin can run on DSP, A15 and IPU1
 *
 * @{
 *
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \file algorithmLink_dmaSwMs.h
 *
 * \brief  DMA Software Mosaic API
 *
 * \version 0.0 (Aug 2013) : [KC] First version
 *
 *******************************************************************************
 */

#ifndef ALGORITHM_LINK_DMA_SW_MS_H_
#define ALGORITHM_LINK_DMA_SW_MS_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */
#include <include/link_api/system.h>
#include <include/link_api/algorithmLink.h>

/**
 *******************************************************************************
 *
 *   \brief Maximum number of windows in Mosaic layout
 *
 *******************************************************************************
 */
#define ALGORITHM_LINK_DMA_SW_MS_MAX_WINDOWS    (8)

/**
 *******************************************************************************
 *
 *   \brief Invalid channel ID
 *
 *******************************************************************************
 */
#define ALGORITHM_LINK_DMA_SW_MS_INVALID_CH_ID  ((UInt32)-1)
/**
 *******************************************************************************
 *
 *   \ingroup LINK_API_CMD
 *   \addtogroup ALGORITHM_LINK_DMA_SW_MS_API_CMD \
 *                  DMA Software Mosaic Control Commands
 *
 *   @{
 *
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 *   \brief Alg Link Config CMD: Set SW Mosaic Layout parameters
 *
 *   \param AlgorithmLink_DmaSwMsLayoutParams [IN]
 *
 *   \return SYSTEM_STATUS_SOK on success
 *
 *******************************************************************************
 */
#define ALGORITHM_LINK_DMA_SW_MS_CONFIG_CMD_SET_LAYOUT_PARAMS     (0x0001)

/**
 *******************************************************************************
 *
 *   \brief Alg Link Config CMD: Get SW Mosaic Layout parameters
 *
 *   \param AlgorithmLink_DmaSwMsLayoutParams [OUT]
 *
 *   \return SYSTEM_STATUS_SOK on success
 *
 *******************************************************************************
 */
#define ALGORITHM_LINK_DMA_SW_MS_CONFIG_CMD_GET_LAYOUT_PARAMS     (0x0002)

/* @} */


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
 *   \brief SW Mosaic window information
 *
 *******************************************************************************
*/
typedef struct
{
    UInt32 chId;
    /**< Channel associated with this window
     *
     *   If chId is \ref ALGORITHM_LINK_DMA_SW_MS_INVALID_CH_ID blank data
     *   is copied into the window
     */

    UInt32 inStartX;
    /**< X-position in input frame from where to copy */

    UInt32 inStartY;
    /**< Y-position in input frame from where to copy */

    UInt32 outStartX;
    /**< X-position in output frame to where to copy */

    UInt32 outStartY;
    /**< Y-position in output frame to where to copy */

    UInt32 width;
    /**< Window width,
     *   if window width < input width, cropped input is copied
     *   if window width > input width, rest of window is filled with blank data
     */

    UInt32 height;
    /**< Window height,
     *   if window height < input height, cropped input is copied
     *   if window height > input height, rest of window is filled with blank
     *      data
     */

} AlgorithmLink_DmaSwMsLayoutWinInfo;

/**
 *******************************************************************************
 *
 *   \brief SW Mosaic layout parameters
 *
 *******************************************************************************
*/
typedef struct
{
    AlgorithmLink_ControlParams baseClassControl;
    /**< Base class control params */

    UInt32 numWin;
    /**< Number of windows in the SW Mosaic */

    AlgorithmLink_DmaSwMsLayoutWinInfo winInfo
                [ALGORITHM_LINK_DMA_SW_MS_MAX_WINDOWS];
    /**< Information of individual window's in the output */

    UInt32 outBufWidth;
    /**< MUST be <= AlgorithmLink_DmaSwMsCreateParams.maxOutBufWidth */

    UInt32 outBufHeight;
    /**< MUST be <= AlgorithmLink_DmaSwMsCreateParams.maxOutBufHeight */

} AlgorithmLink_DmaSwMsLayoutParams;

/**
 *******************************************************************************
 *
 *   \brief Structure containing create time parameters for DMA SW Mosaic
 *
 *******************************************************************************
 */
typedef struct
{
    AlgorithmLink_CreateParams baseClassCreate;
    /**< Base class create params */

    System_LinkInQueParams    inQueParams;
    /**< Input queue information */

    System_LinkOutQueParams   outQueParams;
    /**< Output queue information */

    AlgorithmLink_DmaSwMsLayoutParams initLayoutParams;
    /**< Initial window layout parameters */

    UInt32 maxOutBufWidth;
    /**< Max possible output buffer width */

    UInt32 maxOutBufHeight;
    /**< Max possible output buffer height */

    UInt32 numOutBuf;
    /**< Number of buffer to allocate for the output */

    UInt32 useLocalEdma;
    /**< Flag to control if system DMA controller should be used or local DMA
     *   controller should be used
     *   FALSE, use system DMA controller
     *   TRUE, use local EDMA controller
     *
     *   NOTE: local EDMA controller is available for DSP only
     *         for M4, A15 always system DMA controller will be used
     */

} AlgorithmLink_DmaSwMsCreateParams;


/*******************************************************************************
 *  Function Prototypes
 *******************************************************************************
 */
static inline Void AlgorithmLink_DmaSwMsLayoutParams_Init(
                                    AlgorithmLink_DmaSwMsLayoutParams *pPrm);
static inline Void AlgorithmLink_DmaSwMsCreateParams_Init(
                                    AlgorithmLink_DmaSwMsCreateParams *pPrm);

/*******************************************************************************
 *  Functions
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \brief Set default values for Layout parameters
 *
 *******************************************************************************
 */
static inline Void AlgorithmLink_DmaSwMsLayoutParams_Init(
                    AlgorithmLink_DmaSwMsLayoutParams *pPrm)
{
/* MISRA.ONEDEFRULE.FUNC
 * MISRAC_2004 Rule 8.5
 * Function Definition in header file
 * KW State: Defer -> Waiver -> Case by case
 * MISRAC_WAIVER: This function initializes the create time argument to default.
 *  Defining this in the header file aids maintainability.
 */
    memset(pPrm, 0, sizeof(AlgorithmLink_DmaSwMsLayoutParams));

    pPrm->baseClassControl.size = (UInt32)
                                    sizeof(AlgorithmLink_DmaSwMsLayoutParams);
    pPrm->baseClassControl.controlCmd
            = ALGORITHM_LINK_DMA_SW_MS_CONFIG_CMD_SET_LAYOUT_PARAMS;

    pPrm->numWin = 0U;
    pPrm->outBufWidth  = 1920U;
    pPrm->outBufHeight = 1080U;
}

/**
 *******************************************************************************
 *
 * \brief Set default values for create parameters
 *
 *******************************************************************************
 */
static inline Void AlgorithmLink_DmaSwMsCreateParams_Init(
                    AlgorithmLink_DmaSwMsCreateParams *pPrm)
{
/* MISRA.ONEDEFRULE.FUNC
 * MISRAC_2004 Rule 8.5
 * Function Definition in header file
 * KW State: Defer -> Waiver -> Case by case
 * MISRAC_WAIVER: This function initializes the create time argument to default.
 *  Defining this in the header file aids maintainability.
 */
    memset(pPrm, 0, sizeof(AlgorithmLink_DmaSwMsCreateParams));

    pPrm->baseClassCreate.size = (UInt32)
                                    sizeof(AlgorithmLink_DmaSwMsCreateParams);
    pPrm->baseClassCreate.algId = ALGORITHM_LINK_IPU_ALG_DMA_SWMS;
    pPrm->maxOutBufWidth = 1920U;
    pPrm->maxOutBufHeight = 1080U;
    pPrm->numOutBuf = 4U;
    pPrm->useLocalEdma = (UInt32)FALSE;

    AlgorithmLink_DmaSwMsLayoutParams_Init(&pPrm->initLayoutParams);
}

/**
 *******************************************************************************
 *
 * \brief Implementation of function to init plugins()
 *
 *        This function will be called by AlgorithmLink_initAlgPlugins, so as
 *        register plugins of frame copy algorithm
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
 */
Int32 AlgorithmLink_DmaSwMs_initPlugin(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/* @} */

/* Nothing beyond this point */
