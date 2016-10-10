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
 * \defgroup ALGORITHM_LINK_OBJECTDRAW  Object Draw Link API
 *
 * \brief  This file contains plug in functions for rectangle drawing over
 *         a object. This plugin does the following
 *         - Takes the composite buffer from the previous link. The composite \
 *            buffer has two channels, one containing the original video \
 *            and the other containing metadata (rectangles to be drawn).
 *         - Scales co-ordinates as per the requirement
 *         - Optionally copies the input video into an output buffer \
 *            using EDMA
 *         - Draws rectangles on the video and sends to the next link
 *
 * @{
 *
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \file algorithmLink_objectDraw.h
 *
 * \brief Plugin that draws rectangles over the objects
 *
 * \version 0.0 (Mar 2013) : [NN] First version
 *
 *******************************************************************************
 */

#ifndef ALGORITHM_LINK_OBJECTDRAW_H_
#define ALGORITHM_LINK_OBJECTDRAW_H_

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

/**
 *******************************************************************************
 *
 *   \ingroup LINK_API_CMD
 *   \addtogroup ALGORITHM_LINK_OBJECT_DRAW_CMD \
 *                  Object Detect Draw Control Commands
 *   @{
 *
 *******************************************************************************
 */




/**
 *******************************************************************************
 *
 *   \brief Max nummber of input algos for drawing
 *
 *******************************************************************************
 */
#define ALGORITHM_LINK_OBJECT_DRAW_MAX_DRAW_ALG     (8)

/**
 *******************************************************************************
 *
 *   \brief Alg Link Config CMD: Set ROI parameters
 *
 *   \param AlgorithmLink_ObjectDrawSetROIParams [IN]
 *
 *   \return SYSTEM_STATUS_SOK on success
 *
 *******************************************************************************
 */
#define ALGORITHM_LINK_OBJECT_DRAW_CMD_SET_ROI      (0x1000)

/**
 *******************************************************************************
 *
 *   \brief Alg Link Config CMD: Control drawing related parameters at runtime
 *
 *   \param AlgorithmLink_ObjectDrawSetDrawParams [IN]
 *
 *   \return SYSTEM_STATUS_SOK on success
 *
 *******************************************************************************
 */
#define ALGORITHM_LINK_OBJECT_DRAW_CMD_SET_DRAW_PRMS      (0x1001)

/* @} */

/*******************************************************************************
 *  Enum's
 *******************************************************************************
 */
/**
 *******************************************************************************
 *
 *  \brief  Enumerations for which object type the algorithm should draw.
 *
 *
 *******************************************************************************
*/
typedef enum
{
    ALGORITHM_LINK_OBJECT_DETECT_DRAW_ALL = 0x0FFFFFFF,
    /**< Draw PD and TSR both */
    ALGORITHM_LINK_OBJECT_DETECT_DRAW_PD  = 0x00000001,
    /**< Draw PD only */
    ALGORITHM_LINK_OBJECT_DETECT_DRAW_TSR = 0x00000002,
    /**< Draw TSR only */
    ALGORITHM_LINK_OBJECT_DETECT_DRAW_VD  = 0x00000004,
    /**< Draw VD only */
    ALGORITHM_LINK_OBJECT_DETECT_DRAW_CLR = 0x00000008,
    /**< Draw Circular Light Recognition only */
    ALGORITHM_LINK_OBJECT_DETECT_DRAW_LD  = 0x00000010,
    /**< Draw Lane Detection only */
    ALGORITHM_LINK_OBJECT_DETECT_DRAW_SFM = 0x00000020,
    /**< Draw Lane Detection only */
    ALGORITHM_LINK_OBJECT_DETECT_DRAW_SOF = 0x00000040,
    /**< Draw Lane Detection only */
    ALGORITHM_LINK_OBJECT_DETECT_DRAW_VD_NIGHT  = 0x00000100,
    /**< Draw VD only */
    ALGORITHM_LINK_OBJECT_DETECT_DRAW_FORCE32BITS = 0x7FFFFFFF
    /**< This should be the last value after the max enumeration value.
     *   This is to make sure enum size defaults to 32 bits always regardless
     *   of compiler.
     */
} AlgorithmLink_ObjectDrawOption;


/**
 *******************************************************************************
 *
 *  \brief  Enumerations for which object type the algorithm should draw.
 *
 *
 *******************************************************************************
*/
typedef enum
{
    ALGORITHM_LINK_OBJECT_DETECT_DRAW_ALG_OD = 0,
    /**< Algorithm providing input is object detect */

    ALGORITHM_LINK_OBJECT_DETECT_DRAW_ALG_CLR = 1,
    /**< Algorithm providing input is Circular Light Recognition */

    ALGORITHM_LINK_OBJECT_DETECT_DRAW_ALG_LD = 2,
    /**< Algorithm providing input is Lane detection */

    ALGORITHM_LINK_OBJECT_DETECT_DRAW_ALG_SOF = 3,
    /**< Algorithm providing input is Lane detection */

    ALGORITHM_LINK_OBJECT_DETECT_DRAW_ALG_SFM = 4,
    /**< Algorithm providing input is Lane detection */

    ALGORITHM_LINK_OBJECT_DETECT_DRAW_ALG_FORCE32BITS = 0x7FFFFFFF
    /**< This should be the last value after the max enumeration value.
     *   This is to make sure enum size defaults to 32 bits always regardless
     *   of compiler.
     */
} AlgorithmLink_ObjectDrawAlgId;

/*******************************************************************************
 *  Data structures
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 *   \brief Parameters to set specific ROI
 *
 *******************************************************************************
*/
typedef struct
{
    AlgorithmLink_ControlParams baseClassControl;
    /**< Base class control params */

    UInt32                   imgFrameStartX;
    /**< Start of actual video relative to start of input buffer */
    UInt32                   imgFrameStartY;
    /**< Start of actual video relative to start of input buffer */
    UInt32                   imgFrameWidth;
    /**< Width of the input frame */
    UInt32                   imgFrameHeight;
    /**< Height of the input frame */
} AlgorithmLink_ObjectDrawSetROIParams;


/**
 *******************************************************************************
 *
 *   \brief Parameters to set drawing parameters
 *
 *******************************************************************************
*/
typedef struct
{
    AlgorithmLink_ControlParams baseClassControl;
    /**< Base class control params */

    AlgorithmLink_ObjectDrawOption drawOption;
    /**< Algo information to draw */

} AlgorithmLink_ObjectDrawSetDrawParams;

/**
 *******************************************************************************
 *
 *   \brief Structure containing create time parameters for algorithm
 *
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
    UInt32                   imgFrameStartX;
    /**< Start of actual input to algo relative to start of input buffer */
    UInt32                   imgFrameStartY;
    /**< Start of actual input to algo relative to start of input buffer */
    UInt32                   imgFrameWidth;
    /**< Width of frame given to the PD algo */
    UInt32                   imgFrameHeight;
    /**< Height of the frame given to the PD algo */
    UInt32                   numOutBuffers;
    /**< Number of output Buffers, optional if copy of input is not needed */
    AlgorithmLink_ObjectDrawOption drawOption;
    /**< Option of Object to Draw */
    UInt32                   numDrawAlg;
    /**< Number of different algorithms for which drawing is required */
    AlgorithmLink_ObjectDrawAlgId drawAlgId[ALGORITHM_LINK_OBJECT_DRAW_MAX_DRAW_ALG];
    /**< ID of the algorithm for which drawing is required */
    UInt32                   objectRectThickness;
    /**< Thickness of Object Rectangle */
    UInt32                   objectDrawDistance;
    /**< Draw object distace if available */
    UInt32                   objectTsrDrawTrafficSigns;
    /**< Draw traffic signs */
    UInt32                   laneThickness;
    /**< Thickness in pixels of the lane */
    UInt32                   laneDrawLines;
    /**< Set TRUE for draw connected lines,
      *  Set as FALSE for draw individual Pixel points */
    float                    laneScaleX;
    /**< lane co-ordinates should be scaled by this factor in X-direction */
    float                    laneScaleY;
    /**< lane co-ordinates should be scaled by this factor in Y-direction */
    UInt32                   laneRoiStartY;
    /**< Start of lane ROI in Y direction */
    UInt32                   laneRoiHeight;
    /**< Height of lane ROI in Y direction */
    UInt32                   laneRoiWidth;
    /**< Width of lane ROI in Y direction */
    UInt32                   sofMaxTrackPoints;
    /**< Max number of points to track for SOF */
} AlgorithmLink_ObjectDrawCreateParams;

/**
 *******************************************************************************
 *
 *   \brief Structure containing control parameters for Object Draw
 *          algorithm
 *
 *******************************************************************************
*/
typedef struct
{
    AlgorithmLink_ControlParams baseClassControl;
    /**< Base class control params */
} AlgorithmLink_ObjectDrawControlParams;

/*******************************************************************************
 *  Functions
 *******************************************************************************
 */
static inline void AlgorithmLink_ObjectDraw_Init(
                                AlgorithmLink_ObjectDrawCreateParams *pPrm);

/**
 *******************************************************************************
 *
 * \brief Set defaults for plugin create parameters
 *
 * \param pPrm  [OUT] plugin create parameters
 *
 *******************************************************************************
 */
static inline void AlgorithmLink_ObjectDraw_Init(
    AlgorithmLink_ObjectDrawCreateParams *pPrm)
{
/* MISRA.ONEDEFRULE.FUNC
 * MISRAC_2004 Rule 8.5
 * Function Definition in header file
 * KW State: Defer -> Waiver -> Case by case
 * MISRAC_WAIVER: This function initializes the create time argument to default.
 *  Defining this in the header file aids maintainability.
 */
    memset(pPrm, 0, sizeof(AlgorithmLink_ObjectDrawCreateParams));

    pPrm->baseClassCreate.size = (UInt32)
                                sizeof(AlgorithmLink_ObjectDrawCreateParams);
    pPrm->baseClassCreate.algId = ALGORITHM_LINK_IPU_ALG_OBJECT_DRAW;
    pPrm->inQueParams.prevLinkId    = SYSTEM_LINK_ID_INVALID;
    pPrm->inQueParams.prevLinkQueId = 0U;
    pPrm->outQueParams.nextLink     = SYSTEM_LINK_ID_INVALID;
    pPrm->imgFrameStartX    = 0U;
    pPrm->imgFrameStartY    = 0U;
    pPrm->imgFrameWidth     = 640U;
    pPrm->imgFrameHeight    = 360U;
    pPrm->numOutBuffers  = 4U;

    pPrm->objectRectThickness = 3U;
    pPrm->objectDrawDistance = FALSE;
    pPrm->objectTsrDrawTrafficSigns = FALSE;
    pPrm->laneThickness = 2;
    pPrm->laneDrawLines = TRUE;
    pPrm->laneScaleX = 1.0;
    pPrm->laneScaleY = 1.0;
    pPrm->laneRoiStartY = 120;
    pPrm->laneRoiHeight = 240;
    pPrm->sofMaxTrackPoints = 600;

    pPrm->drawOption = ALGORITHM_LINK_OBJECT_DETECT_DRAW_ALL;
    pPrm->numDrawAlg = 1;
    pPrm->drawAlgId[0] = ALGORITHM_LINK_OBJECT_DETECT_DRAW_ALG_OD;
    pPrm->drawAlgId[1] = ALGORITHM_LINK_OBJECT_DETECT_DRAW_ALG_CLR;
    pPrm->drawAlgId[2] = ALGORITHM_LINK_OBJECT_DETECT_DRAW_ALG_LD;
    pPrm->drawAlgId[3] = ALGORITHM_LINK_OBJECT_DETECT_DRAW_ALG_SOF;
    pPrm->drawAlgId[4] = ALGORITHM_LINK_OBJECT_DETECT_DRAW_ALG_SFM;
}


/**
 *******************************************************************************
 *
 * \brief Implementation of function to init plugins()
 *
 *        This function will be called by AlgorithmLink_initAlgPlugins, so as
 *        register plugins of object draw algorithm
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
 */
Int32 AlgorithmLink_objectDraw_initPlugin(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/* @} */

/* Nothing beyond this point */
