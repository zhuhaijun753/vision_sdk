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
 * \ingroup DUP_LINK_API
 * \defgroup DUP_LINK_IMPL Dup Link Implementation
 *
 * @{
 */

/**
 *******************************************************************************
 *
 * \file dupLink_priv.h Dup Link private API/Data structures
 *
 * \brief  This file is a private header file for dup link implementation
 *         This file lists the data structures, function prototypes which are
 *         implemented and used as a part of dup link.
 *         Dup Link is used in cases where the output buffers from one link is
 *         to be sent to multiple links. Dup link simply duplicates the buffers
 *         without actually duplicating the video frame and send these buffers
 *         across all output queues. Dup link implements the logic to release
 *         buffers to it's previous link only when every link connected to it's
 *         output queue release the buffers.
 *
 * \version 0.0 (Apr 2016) : [YM] Ported to HLOS
 *
 *******************************************************************************
 */

#ifndef DUP_LINK_PRIV_H_
#define DUP_LINK_PRIV_H_

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
#include <include/link_api/dupLink.h>
#include <linux/src/system/system_priv_common.h>
#include <stdio.h>

/*******************************************************************************
 *  Defines
 *******************************************************************************
 */

 /**
 *******************************************************************************
 *
 * \brief Maximum number of dup link objects
 *
 * SUPPORTED in ALL platforms
 *
 *******************************************************************************
 */
#define DUP_LINK_OBJ_MAX    (5U)

/**
 *******************************************************************************
 *
 * \brief Maximum frmaes an output queue can support
 *
 * SUPPORTED in ALL platforms
 *
 *******************************************************************************
 */
#define DUP_LINK_MAX_FRAMES_PER_OUT_QUE    \
                           (SYSTEM_LINK_FRAMES_PER_CH*SYSTEM_MAX_CH_PER_OUT_QUE)

/**
 *******************************************************************************
 * \brief Size DUP link stack
 *******************************************************************************
 */
#define DUP_LINK_TSK_STACK_SIZE (OSA_TSK_STACK_SIZE_DEFAULT)


/*******************************************************************************
 *  Data structure's
 *******************************************************************************
 */


/**
 *******************************************************************************
 *
 * \brief Stat structure that stores information about the number of
 *  - Recevied buffers
 *  - Forwarded buffers ( Sent to next link )
 *  - Released buffers ( Sent back to the previous link )
 *
 *******************************************************************************
 */
typedef struct {
    UInt32 recvCount;
    UInt32 forwardCount[DUP_LINK_MAX_OUT_QUE];
    UInt32 releaseCount[DUP_LINK_MAX_OUT_QUE];
} DupLink_StatsObj;

/**
 *******************************************************************************
 *
 * \brief Structure to hold all Dup link related information
 *
 *******************************************************************************
 */
typedef struct {
    UInt32 tskId;
    /**< Placeholder to store dup link task id */

    UInt32 state;
    /**< Link state, one of SYSTEM_LINK_STATE_xxx */

    OSA_TskHndl tsk;
    /**< Handle to capture link task */

    DupLink_CreateParams createArgs;
    /**< Create params for dup link */

    UInt32 getFrameCount;
    /**< Count of incoming frames */

    UInt32 putFrameCount;
    /**< Count of outgoing frames */

    System_LinkInfo inTskInfo;
    /**< Output queue information of previous link */

    System_LinkInfo info;
    /**< Output queue information of this link */

    OSA_BufHndl outFrameQue[DUP_LINK_MAX_OUT_QUE];
    /**< Handles to each of the output queues */

    System_Buffer
                sysBufs[DUP_LINK_MAX_OUT_QUE * DUP_LINK_MAX_FRAMES_PER_OUT_QUE];
    /**< Placeholder to store the incoming buffers */

    System_BufferList inBufList;

    System_BufferList outBufList[DUP_LINK_MAX_OUT_QUE];

    OSA_MutexHndl lock;
    /**< Link level lock, used while updating the link params */

    DupLink_StatsObj stats;
    /**< To store statistics of the buffers */
} DupLink_Obj;

extern DupLink_Obj gDupLink_obj[DUP_LINK_OBJ_MAX];

Void DupLink_tskMain(struct OSA_TskHndl * pTsk, OSA_MsgHndl * pMsg, UInt32 curState);

Int32 DupLink_tskCreate(UInt32 instId);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/* @} */


