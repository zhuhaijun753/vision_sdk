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
 * \ingroup ALGORITHM_LINK_PLUGIN
 * \defgroup ALGORITHM_LINK_DEWARP : Algorithm implementation private include
 *
 * @{
 */


#ifndef DEWARP_ALG_PLUGIN_PRIV_H_
#define DEWARP_ALG_PLUGIN_PRIV_H_

/*******************************************************************************
 *  Include files
 *******************************************************************************
 */
#include <include/link_api/algorithmLink_deWarp.h>
#include <include/link_api/algorithmLink_algPluginSupport.h>
#include <src/links_ipu/system/system_priv_ipu1_0.h>
#include <vps/iss/vps_m2mIss.h>
#include <vps/vps_m2mIntf.h>
#include <src/utils_common/include/utils_cbuf_ocmc.h>

/*******************************************************************************
 *  Defines
 *******************************************************************************
 */

/**
 *******************************************************************************
 * \brief Indicates the default number of output buffers to be used
 *******************************************************************************
*/
#define ALG_LINK_DEWARP_NUM_BUFS_PER_CH_DEFAULT (3U)

/**
 ******************************************************************************
 * \brief Maximum number of output frames allocated per channel
 ******************************************************************************
 */
#define ALG_PLUGIN_DEWARP_MAX_FRAMES_PER_CH     (SYSTEM_LINK_MAX_FRAMES_PER_CH)


/**
 ******************************************************************************
 *
 * \brief Maximum number of output frames that could be allocated
 *
 ******************************************************************************
 */
#define ALG_PLUGIN_DEWARP_MAX_FRAMES (ALG_LINK_DEWARP_MAX_CH * \
                                      ALG_PLUGIN_DEWARP_MAX_FRAMES_PER_CH)

/**
 *******************************************************************************
 * \brief In Valid View ID marker.
 *******************************************************************************
 */
#define ALG_PLUGIN_DEWARP_INVALID_VIEW_ID (0x7FFFFFFFU)

/**
 *******************************************************************************
 * \brief Maximum Output Width.
 *******************************************************************************
 */
#define ALG_PLUGIN_DEWARP_MAX_OUT_WIDTH (1280U)

/*******************************************************************************
 *  Enum's
 *******************************************************************************
 */


/*******************************************************************************
 *  Data structure's
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 *  \brief  Structure containing attributes of each channel
 *
 *******************************************************************************
*/
typedef struct {

    vpsissSimcopCfgPrms_t   drvSimcopCfg;
    /**< SIMCOP configuration parameters */
    Fvid2_Handle            drvHandle;
    /**< FVID2 driver handle. */

    UInt32                  outBufSize;
    /**< Size of output buffer, used to track the allocated size per channel */
    /* Used, when applications require to DUMP a output frame into local memory,
        Apps, later could fetch it */
    UInt8                   *saveFrameBufAddr;
    /**< Frame buffer used for saving captured frame */
    volatile UInt32         saveFrame;
    /**< Flag to indicate dumping of frame is in progress */

    /* Input Frame parameters */
    UInt32          inputFrameWidth;
    /** < Width */
    UInt32          inputFrameHeight;
    /** < Height */
    UInt32          inputFramePitch[SYSTEM_MAX_PLANES];
    /** < Pitch */
    /* Output Frame parameters */
    UInt32          outputFrameWidth;
    /** < Width */
    UInt32          outputFrameHeight;
    /** < Height */
    UInt32          outputFramePitch[SYSTEM_MAX_PLANES];
    /** < Pitch */

    UInt32          xPad;
    /** < x pad */

    UInt32          yPad;
    /** < y pad */

    UInt32 ocmcLdcLutAddr;
    /** < This is the OCMC RAM address where LDC LUTs are copied into */

} AlgPluginDeWarpChObj;

/**
 *******************************************************************************
 *
 *  \brief  Structure containing information for each instance of DeWarp Plugin
 *
 *******************************************************************************
*/
typedef struct {

    BspOsal_SemHandle           drvFrameProcessed;
    /**< Semaphore for tracking process call of the driver */

    AlgLink_DeWarpCreateParams  createArgs;
    /**< Create params link */


    AlgorithmLink_InputQueueInfo    inputQInfo;
    /**< All the information about input Queue*/
    AlgorithmLink_OutputQueueInfo   outputQInfo;
    /**< All the information about output Queue*/


    AlgPluginDeWarpChObj            chObj[ALG_LINK_DEWARP_MAX_CH];
    /**< Attributes for operating each channel */
    UInt32                          numInputCh;
    /**< Number of input channels */
    System_VideoFrameCompositeBuffer *pPrevOutBuffer;
    /**< Previous frame's output to be given as reference input frame,
            to process next frame. This will hold a composite buffers describing
            previous output */

    System_LinkStatistics   *linkStatsInfo;
    /**< Pointer to the Link statistics information,
         used to store below information
            1, min, max and average latency of the link
            2, min, max and average latency from source to this link
            3, links statistics like frames captured, dropped etc
        Pointer is assigned at the link create time from shared
        memory maintained by utils_link_stats layer */

    Bool    isFirstFrameRecv;
    /**< Flag to activate stats collection*/
    UInt32  activeViewPointId;
    /**< Used to track the current selected view point */

    System_Buffer sysBuffers[ALG_PLUGIN_DEWARP_MAX_FRAMES_PER_CH];
    /**< System buffer that would hold composite output buffers. */
    System_VideoFrameCompositeBuffer
                            compBuffers[ALG_PLUGIN_DEWARP_MAX_FRAMES_PER_CH];
    /**< Composite Buffer that would hold output of each channel */
    UInt32 outBufSize[ALG_PLUGIN_DEWARP_MAX_FRAMES_PER_CH];
    /**< Keep track of size of allocated memory */

    UInt32 processedFrameCount;
    /**< Number of frames processed by this plugin */

    UInt32 fillBuf;
    /**< Fill Buffer address */

    UInt32 fillBufUV;
    /**< Fill Buffer UV address */

    AlgorithmLink_SrvCommonViewPointParams *curViewParams;
    /**< Pointer to the current view point parameter structure */

    Utils_DmaChObj          dumpFramesDmaObj;
    /**< DMA parameters used to dump a frame. Essentially a output frame is
            copied (DMA) into spare buffer */

} AlgLink_DeWarpObj;


#endif /* DEWARP_ALG_PLUGIN_PRIV_H_ */

/* @} */
