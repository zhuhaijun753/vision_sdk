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
 * \ingroup SYSTEM_EVE_LINK_API
 *
 * \defgroup SYSTEM_EVE_LINK_IMPL Processor Link API: EVE Core0 Implementation
 *
 * @{
 */

/**
 *******************************************************************************
 *
 * \file systemLink_priv_eve.h EVE private file containing all
 *                                the header files util files required by EVE
 *
 * \version 0.0 (Jun 2013) : [SS] First version
 *
 *******************************************************************************
 */

#ifndef SYSTEMLINK_PRIV_EVE_H_
#define SYSTEMLINK_PRIV_EVE_H_

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/*******************************************************************************
 *  Include files
 *******************************************************************************
 */
#include <src/links_common/system/system_priv_common.h>
#include <include/link_api/system.h>
#include <src/utils_common/include/utils_timer_reconfig.h>

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
 *  \brief  system link object handle
 *          Contains LinkObject Handle and TaskId
 *
 *******************************************************************************
*/
typedef struct {
    UInt32 tskId;
    /**< SystemLink Task Id */

    Utils_TskHndl tsk;
    /**< SystemLink Task Handle */

} SystemLink_Obj;

extern SystemLink_Obj gSystemLink_obj;

Void SystemLink_tskMain(struct Utils_TskHndl_t * pTsk, Utils_MsgHndl * pMsg);

Int32 SystemLink_tskCreate(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/* @} */

/* Nothing beyond this point */

