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
 * \ingroup SELECT_LINK_API
 * \defgroup SELECT_LINK_IMPL Select Link Implementation
 *
 * @{
 */

/**
 *******************************************************************************
 *
 * \file selectLink_priv.h Select Link private API/Data structures
 *
 * \brief  This file is a private header file for Select link implementation
 *         This file lists the data structures, function prototypes which are
 *         implemented and used as a part of Select link.
 *         Select Link is used in cases where the input buffers are selectively
 *         sent to next links.
 *
 * \version 0.0 (APR 2016) : [YM] Ported to LHLOS
 *
 *******************************************************************************
 */

#ifndef SELECT_LINK_PRIV_H_
#define SELECT_LINK_PRIV_H_

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
#include <include/link_api/selectLink.h>
#include <linux/src/system/system_priv_common.h>
#include <stdio.h>


/*******************************************************************************
 *  Defines
 *******************************************************************************
 */

 /**
 *******************************************************************************
 *
 * \brief Maximum number of SELECT link objects
 *
 * SUPPORTED in ALL platforms
 *
 *******************************************************************************
 */
#define SELECT_LINK_OBJ_MAX    (2U)

/**
 *******************************************************************************
 *
 * \brief Select Link channels not mapped
 *
 * SUPPORTED in ALL platforms
 *
 *******************************************************************************
 */
#define SELECT_LINK_CH_NOT_MAPPED   (0xFFFFU)

/**
 *******************************************************************************
 * \brief Size SELECT link stack
 *******************************************************************************
 */
#define SELECT_LINK_TSK_STACK_SIZE (OSA_TSK_STACK_SIZE_DEFAULT)


/*******************************************************************************
 *  Data structure's
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \brief
 *
 *  Channel link info to hold the information of the perticular channel
 *
 *
 *
 *******************************************************************************
 */
typedef struct {

    UInt32 queId;
    /**< Holds the queue Id of the channel */

    UInt32 outChNum;
    /**< Channel number of the output channel */

    Bool   rtChInfoUpdate;
    /**< Flag to indicate if the channel info has updated */

    System_LinkChInfo rtChInfo;
    /**< Link Channel Info */
} SelectLink_ChInfo;

/**
 *******************************************************************************
 *
 * \brief
 *
 * Structure to hold all Select link related information
 *
 *
 *******************************************************************************
 */
typedef struct {
    UInt32 tskId;
    /**< Placeholder to store select link task id */

    UInt32 state;
    /**< Link state, one of SYSTEM_LINK_STATE_xxx */

    OSA_TskHndl tsk;
    /**< Handle to select link task */

    SelectLink_CreateParams createArgs;
    /**< Create params for select link */

    System_LinkInfo inTskInfo;
    /**< Output queue information of previous link */

    System_LinkInfo info;
    /**< Output queue information of this link */

    OSA_BufHndl outFrameQue[SELECT_LINK_MAX_OUT_QUE];
    /**< Handles to the output queue */

    SelectLink_ChInfo   inChInfo[SYSTEM_MAX_CH_PER_OUT_QUE];
    /**< Input Channel info */

    SelectLink_OutQueChInfo   prevOutQueChInfo[SELECT_LINK_MAX_OUT_QUE];
    /**< Previous Channel Queue Info*/

    OSA_LinkStatistics linkStats;
    /**< Statistics related to this link */

    UInt32 getFrameCount;
    /**< Count of incoming frames */

    UInt32 putFrameCount;
    /**< Count of outgoing frames */

} SelectLink_Obj;


extern SelectLink_Obj gSelectLink_obj[SELECT_LINK_OBJ_MAX];

Void SelectLink_tskMain(struct OSA_TskHndl * pTsk, OSA_MsgHndl * pMsg, UInt32 curState);
Int32 SelectLink_tskCreate(UInt32 instId);
Int32 SelectLink_drvSetOutQueChInfo(SelectLink_Obj * pObj, const SelectLink_OutQueChInfo *pPrm);
Int32 SelectLink_drvGetOutQueChInfo(SelectLink_Obj * pObj, SelectLink_OutQueChInfo *pPrm);
Int32 SelectLink_drvCreate(SelectLink_Obj * pObj, const SelectLink_CreateParams * pPrm);
Int32 SelectLink_drvProcessData(SelectLink_Obj * pObj);
Int32 SelectLink_getFullBuffers(Void * ptr, UInt16 queId, System_BufferList * pBufList);
Int32 SelectLink_getLinkInfo(Void * ptr, System_LinkInfo * info);
Int32 SelectLink_putEmptyBuffers(Void * ptr, UInt16 queId, System_BufferList * pBufList);
Int32 SelectLink_drvDelete(SelectLink_Obj * pObj);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/* @} */


