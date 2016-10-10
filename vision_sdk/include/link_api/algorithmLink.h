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
 * \ingroup FRAMEWORK_MODULE_API
 * \defgroup ALGORITHM_LINK_API Algorithm Link API
 *
 * \brief  This module has the interface for using Algorithm Link
 *
 *         Algorithm Link is used to perform processing operations, which
 *         are executed on DSP / EVE / IPU
 *
 * @{
 *
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \file algorithmLink.h
 *
 * \brief Algorithm Link API common to all algorithm links
 *
 * \version 0.0 (Jul 2013) : [PS] First version
 *
 *******************************************************************************
 */

#ifndef ALGORITHM_LINK_H_
#define ALGORITHM_LINK_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */
#include <include/link_api/system.h>

/*******************************************************************************
 *  Defines
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 *   \ingroup LINK_API_CMD
 *   \addtogroup ALGORITHM_LINK_API_CMD Algorithm Link Control Commands
 *
 *   @{
 *
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 *   \brief Link CMD: Set / get the control / status of algorithm
 *
 *   SUPPORTED in ALL platforms
 *
 *   \param AlgorithmLink_ControlParams *pPrm [IN]
 *
 *   \return SYSTEM_STATUS_SOK on success
 *
 *******************************************************************************
 */
#define ALGORITHM_LINK_CMD_CONFIG                      (0x7000U)

/**
 *******************************************************************************
 *
 *   \brief Link CMD: Get the Algorithm Id of the algorithm
 *
 *   SUPPORTED in ALL platforms
 *
 *   \param AlgorithmLink_ControlParams *pPrm [IN]
 *
 *   \return SYSTEM_STATUS_SOK on success
 *
 *******************************************************************************
 */
#define ALGORITHM_LINK_CMD_GET_ALGID                   (0x7001U)

/* @} */

/*******************************************************************************
 *  Enum's
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 *  \brief  Enumerations for the algorithms supported on DSP.
 *
 *          List of the supported algorithms. Alg ID to be configured by
 *          user/App while creating a specific algorithm link instance.
 *          Note that it is fine for more than one algorithm link instance
 *          to have the same algorithm Id.
 *******************************************************************************
*/
typedef enum
{
    ALGORITHM_LINK_DSP_ALG_FRAMECOPY = 0,
    /**< Alg to copy i/p frame to o/p frame */

    ALGORITHM_LINK_DSP_ALG_COLORTOGRAY = 1,
    /**< Alg to convert color image to gray image */

    ALGORITHM_LINK_DSP_ALG_DMA_SWMS = 2,
    /**< Alg to DMA based SW Mosaic */

    ALGORITHM_LINK_DSP_ALG_SYNTHESIS = 3,
    /**< Alg to perform synthesis for surround view */

    ALGORITHM_LINK_DSP_ALG_GALIGNMENT = 4,
    /**< Alg to generate Geometric alignment LUTs for surroundview synthesis */

    ALGORITHM_LINK_DSP_ALG_ULTRASONICFUSION = 5,
    /**< Alg to ultrasonic detections with surroundview */

    ALGORITHM_LINK_DSP_ALG_PALIGNMENT = 6,
    /**< Alg to generate Photometric alignment LUTs for surroundview synthesis*/

    ALGORITHM_LINK_DSP_ALG_VECTORTOIMAGE = 7,
    /**< Alg to generate Color image from optical flow vectors*/

    ALGORITHM_LINK_DSP_ALG_OBJECTDETECTION = 8,
    /**< Alg to compute feature planes for Object detection */

    ALGORITHM_LINK_DSP_ALG_LANE_DETECT = 9,
    /**< Alg to detect lanes in a video */

    ALGORITHM_LINK_DSP_ALG_STEREO_POST_PROCESS = 10,
    /**< Alg to do post processing on disparity output*/

    ALGORITHM_LINK_DSP_ALG_RADAR_PROCESS = 11,
    /**< Alg to perform radar signal processing*/

    ALGORITHM_LINK_DSP_ALG_CRFALIGNMENT = 12,
    /**< Alg to perform camera radar alignment*/

    ALGORITHM_LINK_DSP_ALG_SFM = 13,
    /**< Alg to perform SFN */

    ALGORITHM_LINK_DSP_ALG_CLR = 14,
    /**< Alg to perform Circular Light Recognition */

    ALGORITHM_LINK_DSP_ALG_OBJECT_DRAW = 15,
    /**< Alg to perform Circular Light Recognition */

    ALGORITHM_LINK_DSP_ALG_FCW = 16,
    /**< Alg to perform Forward Collision Warning */

    ALGORITHM_LINK_DSP_ALG_OBJECTCLASSIFICATION = 17,
    /**< Alg to compute feature planes for Object detection */

    ALGORITHM_LINK_DSP_ALG_SCENE_OBSTRUCTION_DETECT = 18,
    /** < Alg to perform SCENE obstruction detection */

    ALGORITHM_LINK_DSP_ALG_GALIGNMENT_3D = 19,
        /**< Alg to generate Geometric alignment LUTs for surroundview synthesis */

    ALGORITHM_LINK_DSP_ALG_SFMLTV = 20,
    /**< Alg to estimate structure and motion from two views*/

    ALGORITHM_LINK_DSP_ALG_SFMMAIN = 21,
    /**< Alg to perform structure from motion pipeline (main function)*/

    ALGORITHM_LINK_DSP_ALG_SFMMAP = 22,
    /**< Alg to create map and object boxes from 3D points*/

    ALGORITHM_LINK_DSP_ALG_AUTO_CHART_DETECT = 23,
    /**< Alg to do automatic chart detection */

    ALGORITHM_LINK_DSP_ALG_POSE_ESTIMATE = 24,
    /**< Alg to do extrinsic pose estimate */

    ALGORITHM_LINK_DSP_ALG_SAFEFRAMECOPY = 25,
    /**< Alg to copy i/p frame to o/p frame with FFI example */

    ALGORITHM_LINK_DSP_ALG_MAXNUM = 26,
    /**< Should be the last value of this enumeration.
     *   Will be used by Link/driver for validating the input parameters. */
    ALGORITHM_LINK_DSP_ALG_FORCE32BITS = 0x7FFFFFFF
    /**< This should be the last value after the max enumeration value.
     *   This is to make sure enum size defaults to 32 bits always regardless
     *   of compiler.
     */
} AlgorithmLink_DspAlgorithmId;

/**
 *******************************************************************************
 *
 *  \brief  Enumerations for the algorithms supported on EVE
 *
 *          List of the supported algorithms. Alg ID to be configured by
 *          user/App while creating a specific algorithm link instance.
 *          Note that it is fine for more than one algorithm link instance
 *          to have the same algorithm Id.
 *******************************************************************************
*/
typedef enum
{
    ALGORITHM_LINK_EVE_ALG_FRAMECOPY = 0,
    /**< Alg to copy i/p frame to o/p frame */

    ALGORITHM_LINK_EVE_ALG_DENSE_OPTICAL_FLOW = 1,
    /**< Alg to generate LUT to show optical Flow */

    ALGORITHM_LINK_EVE_ALG_EDGEDETECTION = 2,
    /**< Alg to detect edge in image */

    ALGORITHM_LINK_EVE_ALG_FEATUREPLANECOMPUTE = 3,
    /**< Alg to compute feature planes for Object detection */

    ALGORITHM_LINK_EVE_ALG_SPARSE_OPTICAL_FLOW = 4,
    /**< Alg to get flow vectors(Sparse Optical Flow) */

    ALGORITHM_LINK_EVE_ALG_SOFTISP = 5,
    /**< Alg to process RCCC->CFA->Y(RCCC Soft ISP) */

    ALGORITHM_LINK_EVE_ALG_CENSUS = 6,
    /**< Alg to process RCCC->CFA->Y(RCCC Soft ISP) */

    ALGORITHM_LINK_EVE_ALG_DISPARITY_HAMDIST = 7,
    /**< Alg to process RCCC->CFA->Y(RCCC Soft ISP) */

    ALGORITHM_LINK_EVE_ALG_SUBFRAMECOPY = 8,
    /**< Alg to copy subframe*/

    ALGORITHM_LINK_EVE_ALG_REMAPMERGE = 9,
    /**< Alg to copy subframe*/

    ALGORITHM_LINK_EVE_ALG_IMAGEPREPROCESS = 10,
    /**< Alg to image pre process for image pyramid*/

    ALGORITHM_LINK_EVE_ALG_FPCOMPUTE = 11,
    /**< Alg to compute feature planes based on image pyramid generated using
         scalar / resizer
     */

    ALGORITHM_LINK_EVE_ALG_SAFEFRAMECOPY = 12,
    /**< Alg to copy i/p frame to o/p frame with FFI example */

    ALGORITHM_LINK_EVE_ALG_MAXNUM = 13,
    /**< Should be the last value of this enumeration.
     *   Will be used by Link/driver for validating the input parameters. */

    ALGORITHM_LINK_EVE_ALG_FORCE32BITS = 0x7FFFFFFF
    /**< This should be the last value after the max enumeration value.
     *   This is to make sure enum size defaults to 32 bits always regardless
     *   of compiler.
     */
} AlgorithmLink_EveAlgorithmId;

/**
 *******************************************************************************
 *
 *  \brief  Enumerations for the algorithms supported on A15
 *
 *          List of the supported algorithms. Alg ID to be configured by
 *          user/App while creating a specific algorithm link instance.
 *          Note that it is fine for more than one algorithm link instance
 *          to have the same algorithm Id.
 *******************************************************************************
*/
typedef enum
{
    ALGORITHM_LINK_A15_ALG_FRAMECOPY = 0,
    /**< Alg to copy i/p frame to o/p frame */

    ALGORITHM_LINK_A15_ALG_COLORTOGRAY = 1,
    /**< Alg to convert color image to gray image */

    ALGORITHM_LINK_A15_ALG_DMA_SWMS = 2,
    /**< Alg to DMA based SW Mosaic */

    ALGORITHM_LINK_A15_ALG_OBJECT_DRAW = 3,
    /**< Alg to draw objects */

    ALGORITHM_LINK_A15_ALG_SAFEFRAMECOPY = 4,
    /**< Alg to copy i/p frame to o/p frame for ECC example */

    ALGORITHM_LINK_A15_ALG_OPENCVCANNY = 5,
    /**< Alg to perform opencv canny on i/p frame*/

    ALGORITHM_LINK_A15_ALG_OPENCLFRAMECOPY = 6,
    /**< Alg to copy i/p frame to o/p frame for OpenCL example */

    ALGORITHM_LINK_A15_ALG_OPENCLCANNYEDGE = 7,
    /**< Alg to do Canny edge detection for OpenCL example */

    ALGORITHM_LINK_A15_ALG_OPENCLOBJECTDETECT = 8,
    /**< Alg to do Object detection for OpenCL example */

    ALGORITHM_LINK_A15_ALG_MAXNUM = 9,
    /**< Should be the last value of this enumeration.
     *   Will be used by Link/driver for validating the input parameters. */

    ALGORITHM_LINK_A15_ALG_FORCE32BITS = 0x7FFFFFFF
    /**< This should be the last value after the max enumeration value.
     *   This is to make sure enum size defaults to 32 bits always regardless
     *   of compiler.
     */
} AlgorithmLink_A15AlgorithmId;

/**
 *******************************************************************************
 *
 *  \brief  Enumerations for the algorithms supported on M4
 *
 *          List of the supported algorithms. Alg ID to be configured by
 *          user/App while creating a specific algorithm link instance.
 *          Note that it is fine for more than one algorithm link instance
 *          to have the same algorithm Id.
 *******************************************************************************
*/
typedef enum
{
    ALGORITHM_LINK_IPU_ALG_DMA_SWMS = 0,
    /**< Alg to DMA based SW Mosaic */

    ALGORITHM_LINK_IPU_ALG_OBJECT_DRAW = 1,
    /**< Alg to draw rectangles on the image (Needed by PD) */

    ALGORITHM_LINK_IPU_ALG_ISS_AEWB = 2,
    /**< AEWB for ISS running on IPU1-0. Only valid for TDA3x */

    ALGORITHM_LINK_IPU_ALG_ISS_AEWB1 = 3,
    /**< AEWB for ISS running on IPU1-0. Only valid for TDA3x */

    ALGORITHM_LINK_IPU_ALG_HW_CRC = 4,
    /**< CRC for checking Frame Freeze Detect on IPU1-0. Only valid for TDA3x */

    ALGORITHM_LINK_IPU_ALG_IMG_PYRAMID = 5,
    /**< Image pyramid algorithm link. Only valid for TDA3x */

    ALGORITHM_LINK_IPU_ALG_SCENE_OBSTRUCTION_DETECT = 6,
    /** < Alg to perform SCENE obstruction detection */

    ALGORITHM_LINK_IPU_ALG_DEWARP = 7,
    /**< Plugin to that supports DeWarpping of images, depends on SIMCOP/TDA3x*/

    ALGORITHM_LINK_IPU_ALG_MAXNUM = 8,
    /**< Should be the last value of this enumeration.
     *   Will be used by Link/driver for validating the input parameters. */

    ALGORITHM_LINK_IPU_ALG_FORCE32BITS = 0x7FFFFFFF
    /**< This should be the last value after the max enumeration value.
     *   This is to make sure enum size defaults to 32 bits always regardless
     *   of compiler.
     */
} AlgorithmLink_IpuAlgorithmId;

/**
 *******************************************************************************
 *
 *  \brief  Enumerations for the SRV algorithms ouput modes supported
 *
 *          Surround view Alg can support either 2D or 3D ouput modes.
 *          user/App while creating need to specify one of this modes.
 *******************************************************************************
*/
typedef enum
{
    ALGORITHM_LINK_SRV_OUTPUT_2D = 0,
    /**< 2D surround view stitching/creation on DSP */

    ALGORITHM_LINK_SRV_OUTPUT_3D = 1,
    /**< 3D surround view stitching/creation on A15-SGX */

    ALGORITHM_LINK_SRV_OUTPUT_2D_LDC = 2,
    /**< 2D surround view with HW LDC enabled
         Only available on TDA3xx */

    ALGORITHM_LINK_SRV_OUTPUT_3D_LDC = 3,
    /**< 3D Surround view with HW LDC enabled
            Available only on TDA3xx */

    ALGORITHM_LINK_SRV_OUTPUT_MAXNUM = 4,
    /**< Should be the last value of this enumeration.
     *   Will be used by Link/driver for validating the input parameters. */

    ALGORITHM_LINK_SRV_OUTPUT_FORCE32BITS = 0x7FFFFFFF
    /**< This should be the last value after the max enumeration value.
     *   This is to make sure enum size defaults to 32 bits always regardless
     *   of compiler.
     */
} AlgorithmLink_SrvOutputModes;
/*******************************************************************************
 *  Data structures
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 *   \brief Structure containing the Algorithm link create time parameters
 *
 *          Create parameters for an algorithm will be defined based on the
 *          algorithm. This structure is the base class and algorithm
 *          specific parameters will be extended class elements. First element
 *          of extended structure should be base class structure and first
 *          element in base class has to be size element, which indicates
 *          size of extended or base structure, depending on what is used.
 *
 *******************************************************************************
*/
typedef struct
{
    UInt32                   size;
    /**< Size of the structure */
    UInt32                   algId;
    /**< Used to select the algorithm to be used in this link instance.
     *   See AlgorithmLink_algorithmID for the supported values */
} AlgorithmLink_CreateParams;

/**
 *******************************************************************************
 *
 *   \brief Structure containing the Algorithm link control parameters
 *
 *          Control parameters for an algorithm will be defined based on the
 *          algorithm. This structure is the base class and algorithm
 *          specific parameters will be extended class elements. First element
 *          of extended structure should be base class structure and first
 *          element in base class has to be size element, which indicates
 *          size of extended or base structure, depending on what is used.
 *
 *******************************************************************************
*/
typedef struct
{
    UInt32                   size;
    /**< Size of the structure */

    UInt32                   controlCmd;
    /**< Plugin defined control command */

} AlgorithmLink_ControlParams;

/**
 *******************************************************************************
 *
 *   \brief ROI Parameters
 *
 *******************************************************************************
*/
typedef struct {

    UInt32 startX;
    /**< ROI start X */

    UInt32 startY;
    /**< ROI start Y */

    UInt32 width;
    /**< ROI width */

    UInt32 height;
    /**< ROI height  */

} AlgorithmLink_RoiParams;

/*******************************************************************************
 *  Functions
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 *   \brief Algorithm link register and init function
 *
 *          For each algorithm link instance
 *          - Creates link task
 *          - Registers as a link with the system API
 *
 *   \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
*/
Int32 AlgorithmLink_init(void);

/**
 *******************************************************************************
 *
 *   \brief Algorithm link de-register and de-init function
 *
 *          For each algorithm link instance
 *          - Deletes link task
 *          - De-registers as a link with the system API
 *
 *   \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
*/
Int32 AlgorithmLink_deInit(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/* @} */

/* Nothing beyond this point */
