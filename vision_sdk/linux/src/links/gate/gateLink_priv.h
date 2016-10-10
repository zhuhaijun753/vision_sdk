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
 * \ingroup GATE_LINK_API
 * \defgroup GATE_LINK_IMPL Gate Link Implementation
 *
 * @{
 */

/**
 *******************************************************************************
 *
 * \file gateLink_priv.h Gate Link private API/Data structures
 *
 * \brief  This file is a private header file for gate link implementation
 *         This file lists the data structures, function prototypes which are
 *         implemented and used as a part of gate link.
 *
 * \version 0.0 (Apr 2015) : [YM] First version ported to HLOS
 *
 *******************************************************************************
 */

#ifndef GATE_LINK_PRIV_H_
#define GATE_LINK_PRIV_H_

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
#include <include/link_api/gateLink.h>
#include <linux/src/system/system_priv_common.h>
#include <stdio.h>


/*******************************************************************************
 *  Defines
 *******************************************************************************
 */

 /**
 *******************************************************************************
 *
 * \brief Maximum number of gate link objects
 *
 * SUPPORTED in ALL platforms
 *
 *******************************************************************************
 */
#define GATE_LINK_OBJ_MAX    (4U)

/**
 *******************************************************************************
 * \brief Size GATE link stack
 *******************************************************************************
 */
#define GATE_LINK_TSK_STACK_SIZE (OSA_TSK_STACK_SIZE_DEFAULT)


/*******************************************************************************
 *  Enum's
 *******************************************************************************
 */

/**
 *******************************************************************************
 *  \brief Enumerations for AVB Src notification types
 *
 *  SUPPORTED in ALL platforms
 *
 *******************************************************************************
 */
typedef enum
{
    GATE_LINK_OPERATION_STATUS_ON = 0U,
    /**< Switches the operation on at runtime
     *   It it indicates gate link can forward the data to next link */

    GATE_LINK_OPERATION_STATUS_OFF = 1U,
    /**< Switches the operation off at runtime
     *   It it indicates gate link can not forward the data to next link
     */

    GATE_LINK_OPERATION_STATUE_FORCE32BITS = 0x7FFFFFFFU
     /**< This should be the last value after the max enumeration value.
      *   This is to make sure enum size defaults to 32 bits always regardless
      *   of compiler.
      */
} GateLink_OperationStatus;



/*******************************************************************************
 *  Data structure's
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \brief Structure to hold all Gate link related information
 *
 *******************************************************************************
 */
typedef struct {
    UInt32 tskId;
    /**< Placeholder to store gate link task id */

    UInt32 state;
    /**< Link state, one of SYSTEM_LINK_STATE_xxx */

    OSA_TskHndl tsk;
    /**< Handle to capture link task */

    GateLink_OperationStatus opStatus;
    /**< Decision on received buffer is taken based on this,
     *   Buffers are forwarded as it is or returned back
     */

    Int32 bufCount;
    /**< This signifies number of buffers to be released by
      *  gate link, this is useful while turning opStatus on->off
      *  or off->on */

    OSA_MutexHndl lock;
    /**< Link level lock, used while updating the link params */

    GateLink_CreateParams createArgs;
    /**< Create params for gate link */

} GateLink_Obj;

extern GateLink_Obj gGateLink_obj[GATE_LINK_OBJ_MAX];

/**
 *******************************************************************************
 *
 *   \brief Gate link's task
 *
 *   \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
*/
Void GateLink_tskMain(struct OSA_TskHndl * pTsk, OSA_MsgHndl * pMsg, UInt32 curState);

/**
 *******************************************************************************
 *
 *   \brief Gate link's task creation fuciton
 *
 *   \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
*/
Int32 GateLink_tskCreate(UInt32 instId);

/**
 *******************************************************************************
 *
 *   \brief Gate link's driver creation fuction
 *
 *   \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
*/

Int32 GateLink_drvCreate(GateLink_Obj * pObj, const GateLink_CreateParams * pPrm);

/**
 *******************************************************************************
 *
 *   \brief Gate link's Process Data functions actually process buffers at
 *          runtime
 *
 *   \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
*/
Int32 GateLink_drvProcessData(GateLink_Obj * pObj);

/**
 *******************************************************************************
 *
 *   \brief Gets buffers to be processed from input queue
 *
 *   \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
*/
Int32 GateLink_getFullBuffers(Void * ptr, UInt16 queId,
                            System_BufferList * pBufList);

/**
 *******************************************************************************
 *
 *   \brief Provides Link info to the caller
 *
 *   \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
*/
Int32 GateLink_getLinkInfo(Void * ptr, System_LinkInfo * info);

/**
 *******************************************************************************
 *
 *   \brief Releases processed buffers
 *
 *   \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
*/
Int32 GateLink_putEmptyBuffers(Void * ptr, UInt16 queId,
                              System_BufferList * pBufList);
/**
 *******************************************************************************
 *
 *   \brief Gate link's driver delete fuciton
 *
 *   \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
*/
Int32 GateLink_drvDelete(GateLink_Obj * pObj);



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/* @} */


