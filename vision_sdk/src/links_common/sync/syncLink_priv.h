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
 * \ingroup SYNC_LINK_API
 * \defgroup SYNC_LINK_IMPL Sync Link Implementation
 *
 * @{
 */

 /**
 *******************************************************************************
 *
 * \file syncLink_priv.h
 *
 * \brief Sync Link Private Header File
 *
 *        This file has the structures, enums, function prototypes
 *        for sync link, which are not exposed to the application
 *
 * \version 0.0 (Aug 2013) : [NN] First version
 *
 *******************************************************************************
 */

#ifndef SYNC_LINK_PRIV_H
#define SYNC_LINK_PRIV_H

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */
#include <src/links_common/system/system_priv_common.h>
#include <include/link_api/syncLink.h>

/**
 *******************************************************************************
 *
 * \brief Maximum number of sync link objects
 *
 * SUPPORTED in ALL platforms
 *
 *******************************************************************************
 */
#define SYNC_LINK_OBJ_MAX    (4U)

/**
 *******************************************************************************
 * \brief Max number of elemtns in sync link local queue
 *******************************************************************************
 */
#define SYNC_LINK_LOCAL_QUE_MAX_ELEMENTS        (10U)

/**
 *******************************************************************************
 * \brief Sync link max frames per out queue.
 *
 *******************************************************************************
 */
#define SYNC_LINK_MAX_FRAMES_PER_OUT_QUE        (8U)


/*******************************************************************************
 *  Data structures
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 *  \brief  Channel specific information
 *
 *******************************************************************************
 */
typedef struct {
    Utils_QueHandle    localQueHandle;
    /**< Handle to the queue for this channel */
    System_Buffer      *queMem[SYNC_LINK_LOCAL_QUE_MAX_ELEMENTS];
    /**< Queue memory */
} SyncLink_ChObj;


/**
 *******************************************************************************
 * \brief Structure to hold the original System Buffer pointers when a
 *        composite frame is constructed
 *
 *******************************************************************************
*/
typedef struct
{
    UInt32 numCh;
    System_Buffer *pOrgBuf[SYNC_LINK_MAX_CHANNELS];
} SyncLink_OrgBufferPtr;

/**
 *******************************************************************************
 *
 *  \brief  Sync Link Object
 *
 *******************************************************************************
*/
typedef struct {

    UInt32 tskId;
    /**< Placeholder to store sync link task id */

    Utils_TskHndl tsk;
    /**< Link task handle */

    UInt32 state;
    /**< Link state, one of SYSTEM_LINK_STATE_xxx */

    UInt32 linkInstId;
    /**< Instance Id of the link */

    UInt32 numCh;
    /**< Number of channels to sync */

    SyncLink_CreateParams  createArgs;
    /**< create time arguments */

    System_LinkInfo prevLinkInfo;
    /**< Previous link information */

    System_LinkInfo linkInfo;
    /**< Current link information */

    SyncLink_ChObj  chObj[SYNC_LINK_MAX_CHANNELS];
    /**< Instances for each channel */

    Utils_BufHndl outFrameQue;
    /**< Handle for the output queue */

    System_VideoFrameCompositeBuffer compBuf[SYNC_LINK_MAX_FRAMES_PER_OUT_QUE];
    /**< Composite buffers instances */

    System_Buffer outBuf[SYNC_LINK_MAX_FRAMES_PER_OUT_QUE];
    /**< Output System Buffer instances */

    SyncLink_OrgBufferPtr orgBufPtr[SYNC_LINK_MAX_FRAMES_PER_OUT_QUE];
    /**< Structure to hold the original System Buffer pointers when a composite
         frame is constructed */

    System_BufferList dropBufList;
    /**< Holds all dropped frames while sync logic iterates through */

    UInt64 masterTimeStamp;
    /**< Current master timestamp, which is the average of all timestamps
         of buffers in the channel or master channel timestamp */

    System_LinkStatistics   *linkStatsInfo;
    /**< Pointer to the Link statistics information,
         used to store below information
            1, min, max and average latency of the link
            2, min, max and average latency from source to this link
            3, links statistics like frames captured, dropped etc
        Pointer is assigned at the link create time from shared
        memory maintained by utils_link_stats layer */

    Bool isFirstFrame;
    /**< Flag to indicate first frame is received */

    BspOsal_ClockHandle timer;
    /**<Timer used to generate new data at a set interval*/

} SyncLink_Obj;

extern SyncLink_Obj gSyncLink_obj[SYNC_LINK_OBJ_MAX];

Void SyncLink_tskMain(struct Utils_TskHndl_t * pTsk, Utils_MsgHndl * pMsg);

Int32 SyncLink_tskCreate(UInt32 instId);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/* @} */

