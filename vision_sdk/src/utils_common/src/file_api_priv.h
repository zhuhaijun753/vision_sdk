 /*
 *******************************************************************************
 *
 * Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */


#ifndef FILE_API_PRIV_H_
#define FILE_API_PRIV_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *  Include files
 *******************************************************************************
 */

#include <include/link_api/system.h>
#include <src/utils_common/include/file_api.h>
#include <src/utils_common/include/utils_dma.h>
#include <osal/bsp_osal.h>
#include <hsmmcsd_API.h>
#include <platform.h>
#include <stdint.h>
#include <soc.h>
#include <soc_defines.h>
#include <platform.h>
#include <hsmmcsd_edma.h>
#ifdef TDA2XX_FAMILY_BUILD
#include <dma_xbar.h>
#endif
#include "hw_ctrl_core_pad.h"
#include "hw_types.h"
#include "hw_l3init_cm_core.h"
#include "pmhal_prcm.h"
#include "pm/pmhal/pmhal_mm.h"
#include "hw_l4per_cm_core.h"

/* This is for calling file read/write API
 */
#include <fatfs/ff.h>

/*******************************************************************************
 *  Define's
 *******************************************************************************
 */
/* #define UTILS_FILE_API_DEBUG */


#define UTILS_FILE_MAX  (10U)

/*******************************************************************************
 *  Function's
 *******************************************************************************
 */

/*******************************************************************************
 *  Static Globals
 *******************************************************************************
 */
typedef struct
{
    FIL  fileObj[UTILS_FILE_MAX];
    Bool isAlloc[UTILS_FILE_MAX];
    BspOsal_SemHandle lock;
    BspOsal_SemHandle edmaDone;
    EDMA3_DRV_Handle hEdma;
} FileSys_Obj;

extern FileSys_Obj gFileSys_obj;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/* @} */


