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
 * \ingroup NULLSRC_LINK_API
 * \defgroup NULLSRC_LINK_IMPL Null Link Implementation
 *
 * @{
 */

/**
 *******************************************************************************
 *
 * \file nullSrcLink_priv.h Null Source Link private API/Data structures
 *
 * \brief  This file is a private header file for null Source link implementation
 *         This file lists the data structures, function prototypes which are
 *         implemented and used as a part of null link.
 *
 * \version 0.0 (Dec 2015) : [BJ] First version ported to Linux
 *
 *******************************************************************************
 */

#ifndef _NULL_SRC_LINK_PRIV_H_
#define _NULL_SRC_LINK_PRIV_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *  Include files
 *******************************************************************************
 */
#include <linux/src/osa/include/osa.h>
#include <linux/src/osa/include/osa_tsk.h>
#include <linux/src/osa/include/osa_mem.h>
#include <linux/src/osa/include/osa_buf.h>
#include <linux/src/osa/include/osa_prf.h>
#include <include/link_api/nullSrcLink.h>
#include <linux/src/system/system_priv_common.h>

#include <stdio.h>
#include <signal.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

/*******************************************************************************
 *  Defines
 *******************************************************************************
 */

/**
 *******************************************************************************
 * \brief Size NULL link stack
 *******************************************************************************
 */
#define NULL_SRC_LINK_TSK_STACK_SIZE (OSA_TSK_STACK_SIZE_DEFAULT)

/**
 ******************************************************************************
 *
 * \brief Maximum number of output buffers in null source link
 *
 * SUPPORTED in ALL platforms
 *
 ******************************************************************************
 */
#define NULL_SRC_LINK_MAX_OUT_BUFFERS   (NULL_SRC_LINK_MAX_OUT_BUFS_PER_CH \
                                            * NULL_SRC_LINK_MAX_CH)


/*******************************************************************************
 *  Data structure's
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \brief Structure to hold all Dup link related information
 *
 *******************************************************************************
 */

typedef struct {
    UInt32 tskId;
    /**< Placeholder to store null link task id */

    OSA_TskHndl tsk;
    /**< Handle to null link task */

    NullSrcLink_CreateParams createArgs;
    /**< Create params for null link */

    System_LinkInfo linkInfo;
    /* Null source link info that is returned when queried by next link */

    OSA_BufHndl outBufQue;
    /**< Null Source link output side full buffer queue */

    System_Buffer *pBufferOnFullQ[NULL_SRC_LINK_MAX_OUT_BUFFERS];
    /** Holds full buffers */

    System_Buffer *pBufferOnEmptyQ[NULL_SRC_LINK_MAX_CH]
                                  [NULL_SRC_LINK_MAX_OUT_BUFS_PER_CH];
   /** Holds individual channel empty buffers */

    System_Buffer buffers[NULL_SRC_LINK_MAX_CH]
                         [NULL_SRC_LINK_MAX_OUT_BUFS_PER_CH];
    /**< System buffer data structure to exchange buffers between links */

    System_VideoFrameBuffer videoFrames[NULL_SRC_LINK_MAX_CH]
                                       [NULL_SRC_LINK_MAX_OUT_BUFS_PER_CH];
    /**< Payload for System buffers in case of YUV data*/

    System_BitstreamBuffer bitstreamBuf[NULL_SRC_LINK_MAX_CH]
                                       [NULL_SRC_LINK_MAX_OUT_BUFS_PER_CH];
    /*< Payload for System buffers in case of compressed data*/

    timer_t timerid;
    /**<Timer used to generate new data at a set interval*/

    FILE *fpDataStream[NULL_SRC_LINK_MAX_CH];
    /**< Binary File containing the stream data.*/

    FILE *fpIndexFile[NULL_SRC_LINK_MAX_CH];
    /**< File used to index into fpDataStream.
    * It will contain frame sizes in bytes.
    */

    OSA_LatencyStats linkLatency;
    /**< Structure to find out min, max and average latency of the link */
    OSA_LatencyStats srcToLinkLatency;
    /**< Structure to find out min, max and average latency from
     *   source to this link
     */
    OSA_LinkStatistics linkStats;
    /* link specific statistics */

    Bool isFirstBufferSent;
    /**< flag indicates if Null Src Link has sent any buffers to next link yet*/

    UInt32 numPendingCmds;
    /**< Number of pending NEW_DATA_CMD that are yet to handled */

    Ptr allocatedAddr[NULL_SRC_LINK_MAX_CH]
                     [NULL_SRC_LINK_MAX_OUT_BUFS_PER_CH];

    Ptr bufferAddr[NULL_SRC_LINK_MAX_OUT_BUFS_PER_CH];
} NullSrcLink_Obj;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/* @} */
