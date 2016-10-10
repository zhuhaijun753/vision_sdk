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
 * \ingroup SYSTEM_IPU2_LINK_API
 *
 * \defgroup SYSTEM_IPU2_LINK_IMPL Processor Link API: IPU2 Cores Implementation
 *
 * @{
 */

/**
 *******************************************************************************
 *
 * \file systemLink_priv_ipu2.h IPU2 private file containing all
 *                              the header files util files required by ipu2
 *
 * \version 0.0 (Jun 2013) : [CM] First version
 * \version 0.1 (Jul 2013) : [CM] Updates as per code review comments
 *
 *******************************************************************************
 */

#ifndef SYSTEM_LINK_PRIV_IPU2_H_
#define SYSTEM_LINK_PRIV_IPU2_H_

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
#include <src/utils_common/include/utils_stat_collector.h>
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
    /**< SystemLink Task Id */
    UInt32 tskId;

    /**< SystemLink Task Handle */
    Utils_TskHndl tsk;

} SystemLink_Obj;

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

/* @} */
