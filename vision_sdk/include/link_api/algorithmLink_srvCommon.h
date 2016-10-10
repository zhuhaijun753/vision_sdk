/*
 *******************************************************************************
 *
 * Copyright (C) 2016 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \ingroup  ALGORITHM_LINK_PLUGIN
 * \defgroup ALGORITHM_LINK_SRV_COMMON_API  Common structures\
 * Defines
 *
 * \brief  This module has the common interface that would be used in Geometric
 *          Alignment, Photometric Alignment & Synthesis
 *
 *          Currently supported for TDA3x 3D SRV based on LDC H/W
 *
 * @{
 *
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \file algorithmLink_srvCommon.h
 *
 * \brief Common data type and constants used in surround view algorithm plugins
 *
 * \version 0.0 (Nov 2016) : [Suj] First version
 *
 *******************************************************************************
 */

#ifndef ALGORITHM_LINK_SRVCOMMON_H_
#define ALGORITHM_LINK_SRVCOMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */
#include <include/link_api/system.h>
#include <include/link_api/algorithmLink.h>
#include <include/link_api/system_buffer.h>
#include <include/link_api/issM2mSimcopLink.h>

/*******************************************************************************
 *  Defines
 *******************************************************************************
 */

/**
 *******************************************************************************
 * \brief Configures the number of distinct view points supported.
 *
 *  SUPPORTED in TDA3xx and when LDC HARDWARE accelerator is used.
 *
 *  Defines the total number of cameras, from which video streams is captured
 *  and processed.
 *
 *
 *******************************************************************************
*/
#define ALGLINK_SRV_COMMON_MAX_CAMERAS  (4U)

/**
 *******************************************************************************
 * \brief Marker used to identify non valid view identifier
 *
 *  SUPPORTED in TDA3xx and when LDC HARDWARE accelerator is used.
 *
 *******************************************************************************
*/
#define ALGLINK_SRV_COMMON_INVALID_VIEW_ID  (0x7FFFFFFFU)

/**
 *******************************************************************************
 * \brief Maximum number of slices for LDC
 *
 *  SUPPORTED in TDA3xx and when LDC HARDWARE accelerator is used.
 *
 *******************************************************************************
*/
#define ALGLINK_SRV_LDC_MAX_SLICES      (4U)

/*******************************************************************************
 *  Enum's
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 *   \brief This enum defines the slice parameter type
 *
 *******************************************************************************
*/
typedef enum
{
    ALGLINK_SRV_LDC_SLICE_PRMTYPE_ABS,
    /**< The Slice parameters are absolute */

    ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT
    /**< The slice parameters are in percent */

}ALGLINK_SRV_LDC_SLICE_PRMTYPE;

/*******************************************************************************
 *  Data structures
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 *   \brief This structure defines the parameters for each slice
 *
 *******************************************************************************
*/
typedef struct
{
    ALGLINK_SRV_LDC_SLICE_PRMTYPE slicePrmType;
    /**< Slice Parameter Type */

    UInt32 sliceX;
    /**< StartX of the slice */

    UInt32 sliceY;
    /**< StartY of the slice */

    UInt32 sliceW;
    /**< Width of the slice */

    UInt32 sliceH;
    /**< Height of the slice */

    UInt32 blockWidth;
    /**< Block width for the slice */

    UInt32 blockHeight;
    /**< Block height for the slice */

} AlgorithmLink_slicePrm;

/**
 *******************************************************************************
 *
 *   \brief This structure defines the LDC multi slice parameters.
 *
 *******************************************************************************
*/
typedef struct
{
    UInt32  numSlices;
    /**< Num of slices */

    AlgorithmLink_slicePrm slice[ALGLINK_SRV_LDC_MAX_SLICES];
    /**< Slice parameters */

} AlgorithmLink_ldcSlicePrm;

/**
 *******************************************************************************
 *
 *   \brief Surround view is constructed by stiching using more than one video
 *          stream. This structure defines all the parameters required to
 *          construct a surround.
 *
 *          The algorithm plugins such as DeWarp & Synthesis uses this to
 *          communicate view point parameters.
 *******************************************************************************
*/
typedef struct
{
    UInt32  viewPointId;
    /**< An unique ID identifying the view point */
    UInt32  numCameras;
    /**< Number of cameras from which video is being captured */
    UInt32  ldcOutFrameWidth[ALGLINK_SRV_COMMON_MAX_CAMERAS];
    /**< Expected width of output frame, after LDC correction
            for each view point */
    UInt32  ldcOutFrameHeight[ALGLINK_SRV_COMMON_MAX_CAMERAS];
    /**< Expected height of output frame, after LDC correction
            for each view point */
    UInt32  ldcOutPadX[ALGLINK_SRV_COMMON_MAX_CAMERAS];
    /**< Applications could use this to ignore / pad pixels on output.
            e.g. if output width was 1280 and pad was 10
                    actual output width would be 1270 with a 1280 pitch */
    UInt32  ldcOutPadY[ALGLINK_SRV_COMMON_MAX_CAMERAS];
    /**< Applications could use this to ignore / pad lines on output.
            e.g. if output height was 800 and pad was 10
                    actual output width would be 790 lines */
    UInt32  ldcOutBlockWidth[ALGLINK_SRV_COMMON_MAX_CAMERAS];
    /**< Block Width to be used by LDC */
    UInt32  ldcOutBlockHeight[ALGLINK_SRV_COMMON_MAX_CAMERAS];
    /**< Block Height to be used by LDC */

    float   *viewToWorldMeshTable;
    /**< Describes a map from real world to mesh table */

    /* This is specific to synth plugin, viewPointId should also be included */
    UInt32  carPosX;
    /**< Expected position of the car to be rendered at, X co-ordinate, for each
            view point */
    UInt32  carPosY;
    /**< Expected position of the car to be rendered at, X co-ordinate, for each
            view point */
    UInt32  *carImgPtr;
    /**< Pointer describing the CAR. This plugin will not use this, required
            here to aid applications */
    UInt32  sizeOfcarImg;
    /**< Size of the CAR image, expressed in bytes. This plugin will not use
            this, required here to aid applications */

    System_VideoFrameCompositeBuffer *pCompBuf;
    /**< Composite buffer, Caller allocated memory required for this structures
            and will use the following members as input and output
            numFrames   [IN]  Will be used to specify the number of cameras
            metaBufAddr [IN/OUT]    The space for this array of pointer is
                                    allocated by caller and this function will
                                    update the same with LDC LUTs
                                    metaBufAddr[0] will hold the memory required
                                    for channel 0 (camera 0) and so on...
            metaBufSize [OUT]   Will specify the total size in bytes for all
                                    channels */
    UInt32 *baseBufAddr;
    /**< Base address of the super buffer, this buffer will hold all the
            required data for a given view point */

    UInt32  remapChannels;
    /**< Flag used to remap video input channels to output channel. When set to
            TRUE, remappedCh will hold valid remap table. */

    UInt32  remappedCh[ALGLINK_SRV_COMMON_MAX_CAMERAS];
    /**< Define remapping for all valid view points. If remap is not desired
            for an channel, set the value to channel id as show below.

            e.g. 2 valid view point and view point 0 : no remap required and
                view point 1 require channel remap. Channels 0,3 are inter
                changed & channels 1 & 2 are interchaned.

            remapChannels = FALSE;
            remappedCh[0U] = 0U;
            remappedCh[1U] = 1U;
            remappedCh[2U] = 2U;
            remappedCh[3U] = 3U;

            remapChannels = TRUE;
            remappedCh[0U] = 3U;
            remappedCh[1U] = 2U;
            remappedCh[2U] = 1U;
            remappedCh[3U] = 0U;
            */
    AlgorithmLink_ldcSlicePrm slicePrm[ALGLINK_SRV_COMMON_MAX_CAMERAS];
    /**< LDC Slice parameters */
} AlgorithmLink_SrvCommonViewPointParams;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ALGORITHM_LINK_SRVCOMMON_H_ */

/* @} */

/* Nothing beyond this point */
