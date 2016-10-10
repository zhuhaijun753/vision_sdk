 /*
 *******************************************************************************
 *
 * Copyright (C) 2015 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \ingroup RTI_LINK_API
 * \defgroup RTI_LINK_IMPL RTI link Implementation
 *
 * @{
 */

/**
 *******************************************************************************
 *
 * \file rtiLink_priv.h RTI link private API/Data structures
 *
 * \brief  This file is a private header file for RTI link implementation
 *         This file lists the data structures, function prototypes which are
 *         implemented and used as a part of RTI link.
 *
 * \version 0.1 (Jul 2015) : [CSG] First version
 *
 *******************************************************************************
 */

#ifndef _RTI_LINK_PRIV_H_
#define _RTI_LINK_PRIV_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *  Include files
 *******************************************************************************
 */
#include <src/links_ipu/system/system_priv_ipu1_0.h>
#include <include/link_api/rtiLink.h>

/*******************************************************************************
 *  Defines
 *******************************************************************************
 */

 /**
 *******************************************************************************
 *
 * \brief Stack size for RTI link tasks
 *
 * SUPPORTED in TDA3x
 *
 *******************************************************************************
 */
#define RTI_LINK_TSK_STACK_SIZE    (SYSTEM_DEFAULT_TSK_STACK_SIZE)

 /**
 *******************************************************************************
 *
 * \brief Priority for RTI link tasks
 *
 * SUPPORTED in TDA3x
 *
 *******************************************************************************
 */
#define RTI_LINK_TSK_PRI    (2U)

 /**
 *******************************************************************************
 *
 * \brief Maximum number of RTI link objects
 *
 * SUPPORTED in TDA3x
 *
 *******************************************************************************
 */
#define RTI_LINK_OBJ_MAX    (1)

 /**
 *******************************************************************************
 *
 * \brief Maximum number of RTI modules supported
 *
 * SUPPORTED in TDA3x
 * There are 5 instances in TDA3x
 *
 *******************************************************************************
 */
#define RTI_LINK_NUM_RTI_MODULES    (5)

/*******************************************************************************
 *  Data structure's
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \brief Structure to hold all RTI link related information
 *
 *******************************************************************************
 */

typedef struct {
    Utils_TskHndl tsk;
    /**< Handle to RTI WWDT proces task */

    UInt32 rtiServiceEnabled;
    /**< Maintain whether RTI servicing is enabled */

    UInt32 rtiMonitorEnabled[RTI_LINK_NUM_RTI_MODULES];
    /**< Maintain whether RTI expiry mointoring is enabled */

    UInt32 state;
    /**< Link state, one of SYSTEM_LINK_STATE_xxx */

    UInt32 tskId;
    /**< Placeholder to store RTI link task id */

    BspOsal_SemHandle lock;
    /**< Link level lock, used while updating the link params */

    BspOsal_IntrHandle  rtiHwiHandle[RTI_LINK_NUM_RTI_MODULES];
    /**< HWI handles */

    UInt32  rtiExpired[RTI_LINK_NUM_RTI_MODULES];
    /**< RTI expiry status */

    UInt32  procMap[RTI_LINK_NUM_RTI_MODULES];
    /**< Mapping of RTI module to CPU */
} RtiLink_Obj;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/* @} */
