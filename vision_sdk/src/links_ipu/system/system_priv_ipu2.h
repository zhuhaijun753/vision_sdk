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
 * \ingroup SYSTEM_IMPL
 *
 * \defgroup SYSTEM_IPU2_IMPL System implementation for IPU2 Core 0 & 1
 *
 * @{
 */

/**
 *******************************************************************************
 *
 * \file system_priv_ipu2.h IPU2 private file containing all the header files
 *                          util files required by IPU2
 *
 *
 * \version 0.0 (Jun 2013) : [SS] First version
 *
 *******************************************************************************
 */
#ifndef SYSTEM_PRIV_IPU2_H_
#define SYSTEM_PRIV_IPU2_H_

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
#include <include/link_api/dupLink.h>
#include <include/link_api/gateLink.h>
#include <include/link_api/selectLink.h>
#include <include/link_api/syncLink.h>
#include <include/link_api/mergeLink.h>
#include <include/link_api/ipcLink.h>
#include <include/link_api/algorithmLink.h>
#include <include/link_api/nullLink.h>
#include <include/link_api/nullSrcLink.h>

/*******************************************************************************
 *  Data structure's
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 *  \brief  Structure to hold the IPU2 global objects.
 *          Any Link in the IPU2 core can use these objects.
 *
 *******************************************************************************
*/
typedef struct {
    UInt32 reserved;

} System_Ipu2_Obj;

extern System_Ipu2_Obj gSystem_objIpu2;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/* @} */

/* Nothing beyond this point */
