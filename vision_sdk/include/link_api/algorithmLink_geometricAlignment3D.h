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
 * \defgroup ALGORITHM_LINK_GEOMETRICALIGNMENT_API  Geometric\
 * Alignment API
 *
 * \brief  This module has the interface for using geometric alignment algorithm
 *
 *         Geometric Alignment algorithm link -
 *           -# Will take in images from multiple views and generate
 *              LUTs (calibration params) for Geometric alignment of views,
 *              which will get used by the synthesis stage.
 *           -# Will generate two types of LUTs:
 *               - Single LUT for Simple synthesis operation
 *               - Dual LUTs for blending type of synthesis
 *           -# Will always generate all the above 3 LUTs independent of
 *              the mode of operation of synthesis link
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
 * \file algorithmLink_geometricAlignment.h
 *
 * \brief Algorithm Link API specific to geometric alignment algorithm
 *
 * \version 0.0 (Oct 2013) : [PS] First version
 *          0.1 (Dec 2013) : [PS] Added support for GA calibration support
 *
 *******************************************************************************
 */

#ifndef ALGORITHM_LINK_GEOMETRICALIGNMENT3D_H_
#define ALGORITHM_LINK_GEOMETRICALIGNMENT3D_H_

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
    ALGLINK_GALIGN3D_IPQID_MULTIVIEW = 0x0,
    /**< QueueId for multiview images */

    ALGLINK_GALIGN3D_IPQID_CALMAT,
    /**< QueueId for multiview images */

    ALGLINK_GALIGN3D_IPQID_MAXIPQ,
    /**< Maximum number of input queues */

    ALGLINK_GALIGN3D_IPQID_FORCE32BITS = 0x7FFFFFFF
    /**< To make sure enum is 32 bits */

}AlgorithmLink_GAlign3DInputQueId;

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
    ALGLINK_GALIGN3D_OPQID_GALUT = 0x0,
    /**< QueueId for GA LUT output */

    ALGLINK_GALIGN3D_OPQID_GASGXLUT,
    /**< QueueId for SGX GA LUT */
    /**< 3D - connected to SGX link */
    /**< 2D - Not used */

    ALGLINK_GALIGN3D_OPQID_PIXELSPERCM,
    /*new output for GAlign for chart center detection*/

    ALGLINK_GALIGN3D_OPQID_MAXOPQ,
    /**< Maximum number of output queues */

    ALGLINK_GALIGN3D_OPQID_FORCE32BITS = 0x7FFFFFFF
    /**< To make sure enum is 32 bits */

}AlgorithmLink_GAlign3DOutputQueId;

/**
 *******************************************************************************
 * \brief Enum for mode of GA calibration
 *
 * SUPPORTED in ALL platforms
 *
 *******************************************************************************
*/
typedef enum
{
    ALGLINK_GALIGN3D_CALMODE_DEFAULT = 0x0,
    /**< Use default tables for GALUT */

    ALGLINK_GALIGN3D_CALMODE_USERGALUT,
    /**< User provided GALUT table */

    ALGLINK_GALIGN3D_CALMODE_FORCE_DEFAULTPERSMATRIX,
    /**< Force GALUT generation with default perspective matrix */

    ALGLINK_GALIGN3D_CALMODE_FORCE_USERPERSMATRIX,
    /**< Force GALUT generation with user provided perspective matrix */

    ALGLINK_GALIGN3D_CALMODE_FORCE_USERGASGXLUT,
    /**< User provided GA SGX LUT table */

    ALGLINK_GALIGN3D_CALMODE_FORCE32BITS = 0x7FFFFFFF
    /**< To make sure enum is 32 bits */

}AlgorithmLink_GAlign3DCalibrationMode;

/*******************************************************************************
 *  Data structures
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 *   \brief Structure containing create time parameters for Geometric Alignment
 *          algorithm
 *
 *******************************************************************************
*/
typedef struct
{
    AlgorithmLink_GAlign3DCalibrationMode  calMode;
    /**< Calibration mode to be used */
    void *                   gaLUTDDRPtr;
    /**< DDR pointer for GALUT */
    void *                   persMatDDRPtr;
    /**< DDR pointer for perspective matrix */
    void *                   autoPersMatDDRPtr;
    /**< DDR pointer for automatic perspective matrix */
} AlgorithmLink_GAlign3DCalibrationParams;

/**
 *******************************************************************************
 *
 *   \brief Structure containing create time parameters for Geometric Alignment
 *          algorithm
 *
 *******************************************************************************
*/
typedef struct
{
    AlgorithmLink_CreateParams baseClassCreate;
    /**< Base class create params. This structure should be first element */
    UInt32                   maxOutputHeight;
    /**< Max height of the output (stiched) frame */
    UInt32                   maxOutputWidth;
    /**< max width of the output (stiched) frame */
    UInt32                   maxInputHeight;
    /**< Max height of the input (captured) frame */
    UInt32                   maxInputWidth;
    /**< Max width of the input (captured) frame */
    UInt32                   numViews;
    /**< number of input views from which output will be synthesized */
    UInt32                   numOutputTables;
    /**< Number of output GA LUT tables. All the 3 LUT tables are considered
     * as one unit. numOutputTables defines number of such units.*/
    Int16                    carBoxWidth;
    /**< Width of the car box > */
    Int16                    carBoxHeight;
    /**< Height of the car box > */
    System_LinkOutQueParams  outQueParams[ALGLINK_GALIGN3D_OPQID_MAXOPQ];
    /**< Output queue information */
    System_LinkInQueParams   inQueParams[ALGLINK_GALIGN3D_IPQID_MAXIPQ];
    /**< Input queue information */
    AlgorithmLink_GAlign3DCalibrationParams calParams;
    /**< GA Calibration parameters */
    UInt32  enablePixelsPerCm;
    /**< enable computation of pixelsPerCm output?*/
    AlgorithmLink_SrvOutputModes  svOutputMode;
    /**< Surround view Alg can support either 2D or 3D ouput modes */
    UInt32 calmatInputBufferConnected;
    /**<is calmat input buffer connected? 0=no, 1=yes*/
    UInt32                  ignoreFirstNFrames;
    /**< The first set of frames received should be visible.
            In cases where Auto White balance is enabled and cannot grantee
            first good frame.
            Use this to configure the algorithm to skip, until valid frame is
            received. */
    UInt32                  defaultFocalLength;
    /*<>*/
} AlgorithmLink_GAlign3DCreateParams;

/**
 *******************************************************************************
 *
 *   \brief Structure containing control parameters for Geometric Alignment
 *          algorithm
 *
 *******************************************************************************
*/
typedef struct
{
    AlgorithmLink_ControlParams baseClassControl;
    /**< Base class control params */
} AlgorithmLink_GAlign3DControlParams;

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
Int32 AlgorithmLink_gAlign3D_initPlugin(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/* @} */

/* Nothing beyond this point */
