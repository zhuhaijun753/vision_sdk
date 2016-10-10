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

#ifndef ALGORITHM_LINK_GEOMETRICALIGNMENT_H_
#define ALGORITHM_LINK_GEOMETRICALIGNMENT_H_

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
 *  \brief TDA3X CMD: Generate LDC LUT for 3D SRV
 *
 *  \param NONE
 *
 *******************************************************************************
*/
#define TDA3X_SV_3D_GEN_LDCLUT  (0xA001U)

/**
 *******************************************************************************
 *
 *  \brief TDA2X CMD: Generate GPU LUT for 3D SRV
 *
 *  \param NONE
 *
 *******************************************************************************
*/
#define TDA2X_SV_3D_GEN_GPULUT  (0xA002U)

/**
 *******************************************************************************
 *
 *  \brief TDA3X CMD: Generate Calibration Matrix for 3D SRV
 *
 *  \param NONE
 *
 *******************************************************************************
*/
#define TDA3X_SV_3D_GEN_CALMAT  (0xA003U)

/**
 *******************************************************************************
 *
 *  \brief TDA2X CMD: Auto Calibration Create
 *
 *  \param NONE
 *
 *******************************************************************************
*/
#define TDA2X_SV_3D_AUTO_CALIB_CREATE           (0xA004U)

/**
 *******************************************************************************
 *
 *  \brief TDA2X CMD: Auto Calibration Get Buffers
 *
 *  \param NONE
 *
 *******************************************************************************
*/
#define TDA2X_SV_3D_AUTO_CALIB_GET_BUF     (0xA005U)

/**
 *******************************************************************************
 *
 *  \brief TDA2X CMD: Auto Calibration Process
 *
 *  \param NONE
 *
 *******************************************************************************
*/
#define TDA2X_SV_3D_AUTO_CALIB_PROCESS          (0xA006U)

/**
 *******************************************************************************
 *
 *  \brief TDA2X CMD: Auto Calibration Process
 *
 *  \param NONE
 *
 *******************************************************************************
*/
#define TDA2X_SV_3D_CALMAT_CALIB_PROCESS        (0xA007U)

/**
 *******************************************************************************
 *
 *  \brief TDA2X CMD: Auto Calibration Print Statistics
 *
 *  \param NONE
 *
 *******************************************************************************
*/
#define TDA2X_SV_3D_AUTO_CALIB_PRINTSTAT        (0xA008U)

/**
 *******************************************************************************
 *
 *  \brief TDA2X CMD: Auto Calibration Dump Frames
 *
 *  \param NONE
 *
 *******************************************************************************
*/
#define TDA2X_SV_3D_AUTO_CALIB_DUMPFRAMES       (0xA009U)

/**
 *******************************************************************************
 *
 *  \brief TDA2X CMD: Auto Calibration Delete
 *
 *  \param NONE
 *
 *******************************************************************************
*/
#define TDA2X_SV_3D_AUTO_CALIB_DELETE           (0xA00AU)

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
    ALGLINK_GALIGN_IPQID_MULTIVIEW = 0,
    /**< QueueId for multiview images */

    ALGLINK_GALIGN_IPQID_MAXIPQ = 1,
    /**< Maximum number of input queues */

    ALGLINK_GALIGN_IPQID_FORCE32BITS = 0x7FFFFFFF
    /**< To make sure enum is 32 bits */

}AlgorithmLink_GAlignInputQueId;

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
    ALGLINK_GALIGN_OPQID_GALUT = 0,
    /**< QueueId for GA LUT output */

    ALGLINK_GALIGN_OPQID_GASGXLUT = 1,
    /**< QueueId for SGX GA LUT */
    /**< 3D - connected to SGX link */
    /**< 2D - Not used */

    ALGLINK_GALIGN_OPQID_PIXELSPERCM = 2,
    /*new output for GAlign for chart center detection*/

    ALGLINK_GALIGN_OPQID_MAXOPQ = 3,
    /**< Maximum number of output queues */

    ALGLINK_GALIGN_OPQID_FORCE32BITS = 0x7FFFFFFF
    /**< To make sure enum is 32 bits */

}AlgorithmLink_GAlignOutputQueId;

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
    ALGLINK_GALIGN_CALMODE_DEFAULT = 0,
    /**< Use default tables for GALUT */

    ALGLINK_GALIGN_CALMODE_USERGALUT = 1,
    /**< User provided GALUT table */

    ALGLINK_GALIGN_CALMODE_FORCE_DEFAULTPERSMATRIX = 2,
    /**< Force GALUT generation with default perspective matrix */

    ALGLINK_GALIGN_CALMODE_FORCE_USERPERSMATRIX = 3,
    /**< Force GALUT generation with user provided perspective matrix */

    ALGLINK_GALIGN_CALMODE_FORCE32BITS = 0x7FFFFFFF
    /**< To make sure enum is 32 bits */

}AlgorithmLink_GAlignCalibrationMode;

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
    AlgorithmLink_GAlignCalibrationMode  calMode;
    /**< Calibration mode to be used */
    void *                   gaLUTDDRPtr;
    /**< DDR pointer for GALUT */
    void *                   persMatDDRPtr;
    /**< DDR pointer for perspective matrix */
} AlgorithmLink_GAlignCalibrationParams;

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
    System_LinkOutQueParams  outQueParams[ALGLINK_GALIGN_OPQID_MAXOPQ];
    /**< Output queue information */
    System_LinkInQueParams   inQueParams[ALGLINK_GALIGN_IPQID_MAXIPQ];
    /**< Input queue information */
    AlgorithmLink_GAlignCalibrationParams calParams;
    /**< GA Calibration parameters */
    UInt32  enablePixelsPerCm;
    /**< enable computation of pixelsPerCm output?*/
    AlgorithmLink_SrvOutputModes  svOutputMode;
    /**< Surround view Alg can support either 2D or 3D output modes */
    UInt32                  ignoreFirstNFrames;
    /**< The first set of frames received should be visible.
            In cases where Auto White balance is enabled and cannot grantee
            first good frame.
            Use this to configure the algorithm to skip, until valid frame is
            received. */
    UInt32                  defaultFocalLength;
    /**< Focal length of the lens used */

    UInt32                  ldcLutOnlyMode;
    /**< TRUE: It generates the LUT, but does not correct
               distortion in the input frame, In this case,
               distortion correction will be done by the
               hardware/other modules
         FALSE: Default Value, it generate the distortion correction LUT
                and use it to correct input frame */
    UInt32                  ldcOutFrmWidth;
    /**< LDC frame Width,
         used to calculate buffer size
         used only when #ldcLutOnlyMode is set to TRUE */
    UInt32                  ldcOutFrmHeight;
    /**< LDC frame height,
         used to calculate buffer size
         used only when #ldcLutOnlyMode is set to TRUE */
    UInt32                  ldcDownScaleFactor;
    /**< LDC DownScaling factor, used only when #ldcLutOnlyMode is
         set to TRUE */
} AlgorithmLink_GAlignCreateParams;

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
} AlgorithmLink_GAlignControlParams;

/**
 *******************************************************************************
 *
 *   \brief Structure containing parameters to generate LDC LUT for 3D SRV
 *
 *******************************************************************************
*/
typedef struct
{
    Int8 numCameras;
    /**< Number of cameras used */
    Int8 subsampleratio;
    /**< Down sampling ration to be used */
    Int16 SVOutDisplayHeight;
    /**< Required surround view output height */
    Int16 SVOutDisplayWidth;
    /**< Required surround view output width */
    Int16 SVInCamFrmHeight;
    /**< Input frame frame height */
    Int16 SVInCamFrmWidth;
    /**< Input frame frame height */
    Int32 carPosx;
    /**< Location of the CAR */
    Int32 carPosy;
    /**< Location of the CAR */
    Int32 *calmat;
    /**< Calibaration matrix input */
    Int32 LDC3DWidth[6U];
    /**< Width of individual cameras, output */
    Int32 LDC3DHeight[6U];
    /**< Height of individual cameras, output */
    float *view2worldmesh;
    /**< View to world table for this view point */
    Int16 *LDCLUT3D[6U];
    /**< LDC LUTs for each camera */
    UInt32 calMatSize;
    /**< Size of calibaration matrix in bytes */
    UInt32 v2wMeshSize;
    /**< Size of view to world table in bytes */
    UInt32 ldcLutSize;
    /**< Size of LDC LUTs in bytes */
    UInt16 *GPULUT3D;
    /**< GPU LUT address */
    UInt32 gpuLutSize;
    /**< Size of GPU LUT in bytes */

}AlgorithmLink_lutGenPrm_3DSRV;

/**
 *******************************************************************************
 *
 *   \brief Structure containing parameters to generate Calibration Matrix
 *          for 3D SRV
 *
 *******************************************************************************
*/
typedef struct
{
    Int8 numCameras;
    /**< Number of cameras used */
    UInt32 imgPitch[2U];
    /**< Input Image Pitch */
    UInt32 imgWidth;
    /**< Input Image Height */
    UInt32 imgHeight;
    /**< Input Image Height */
    Int32 *calMatBuf;
    /**< Output Calibration Matrix Buffer Address */
    Int8 *inChartPosBuf;
    /**< Input Chart Position Buffer Address */
    UInt32 inChartPosSize;
    /**< Size of the Input Chart Position Buffer */
    Void *inImgPtr[6U];
    /**< Array of input image buffer addresses */
    Void *persistentBaseAddr;
    /**< Persistent Buffer Address */
    Void *scratchBaseAddr;
    /**< Scratch Buffer Address */
    UInt32 persistentMemSize;
    /**< Persistent Buffer Size */
    UInt32 srcatchMemSize;
    /**< Scratch Buffer Size */
    Int32 *cornerPointsBuf;
    /**< Corner Points Buffer */
    Int32 retStatus;
    /**< Return status of the Auto Calibration */
    UInt32 displayWidth;
    /**< 2D SV Output Display Width */
    UInt32 displayHeight;
    /**< 2D SV Output Display Height */
    Int32 *persMatBuf;
    /**< Output Persmat Matrix for 2D SRV Buffer Address */

}AlgorithmLink_calMatGenPrm_3DSRV;

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
Int32 AlgorithmLink_gAlign_initPlugin(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/* @} */

/* Nothing beyond this point */
