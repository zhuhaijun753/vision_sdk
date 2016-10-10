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
 * \ingroup SGXFRMCPY_LINK_API
 * \defgroup SGXFRMCPY_LINK_IMPL SgxFrmcpy Link Implementation
 *
 * @{
 */

/**
 *******************************************************************************
 *
 * \file sgxFrmcpyLink_priv.h SgxFrmcpy Link private API/Data structures
 *
 * \brief  This link private header file has defined
 *         - SgxFrmcpy link instance/handle object
 *         - All the local data structures
 *         - SgxFrmcpy driver interfaces
 *
 * \version 0.0 (Jun 2014) : [SS] First version
 *
 *******************************************************************************
 */

#ifndef _SGXFRMCPY_LINK_PRIV_H_
#define _SGXFRMCPY_LINK_PRIV_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *  Include files
 *******************************************************************************
 */
#include <include/link_api/system.h>
#include <linux/src/system/system_priv_common.h>
#include <linux/src/system/system_gl_egl_utils.h>
#include <osa.h>
#include <osa_mutex.h>
#include <osa_que.h>
#include <osa_prf.h>
#include <osa_mem_shared.h>
#include <osa_mem.h>
#include <osa_buf.h>
#include <include/link_api/sgxFrmcpyLink.h>
#include "sgxRender1x1.h"
#include "sgxRender2x2.h"
#include "sgxRenderKmsCube.h"
#include <linux/src/system/system_pvrscope_if.h>

/*******************************************************************************
 *  Defines
 *******************************************************************************
 */


/**
 *******************************************************************************
 *
 *   \brief Max Number of sgxFrmcpy link instances supported
 *
 *   SUPPORTED in ALL platforms
 *
 *******************************************************************************
 */
#define SGXFRMCPY_LINK_OBJ_MAX                     (1)

/**
 *******************************************************************************
 *
 * \brief Task size for SGXFRMCPY link task
 *
 *******************************************************************************
 */
#define SGXFRMCPY_LINK_TSK_STACK_SIZE              (OSA_TSK_STACK_SIZE_DEFAULT)

/*******************************************************************************
 *  Data structures
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 *   \brief SgxFrmcpy link instance object
 *
 *          This structure contains
 *          - All the local data structures
 *          - VPS Data structures required for SgxFrmcpy driver interfaces
 *          - All fields to support the Link stats and status information
 *
 *******************************************************************************
*/
typedef struct
{
    UInt32 linkId;
    /**< placeholder to store the SgxFrmcpy link Id */

    OSA_TskHndl tsk;
    /**< placeholder to store the SgxFrmcpy link task handler */

    SgxFrmcpyLink_CreateParams createArgs;
    /**< placeholder to store the SgxFrmcpy link create parameters */

    System_LinkInfo inTskInfo;
    /**< Specifies a place holder that describe the LINK information */

    System_LinkQueInfo inQueInfo;
    /**< place holder that describe the output information of the LINK */

    OSA_LatencyStats linkLatency;
    /**< Structure to find out min, max and average latency of the link */

    OSA_LatencyStats srcToLinkLatency;
    /**< Structure to find out min, max and average latency from
     *   source to this link
     */

    OSA_LinkStatistics linkStats;
    /* link specific statistics */

    Bool isFirstFrameRecv;
    /**< Flag to indicate if first frame is received, this is used as trigger
     *   to start stats counting
     */

    System_EglObj eglObj;
    /**< EGL object information */

    SgxRender1x1_Obj render1x1Obj;
    /**< 1x1 rendering prgram obj */

    SgxRender2x2_Obj render2x2Obj;
    /**< 2x2 rendering prgram obj */

    SgxRenderKmsCube_Obj renderKmsCubeObj;
    /**< KMS Cube rendering prgram obj */

    System_LinkInfo tskInfo;
    /**< Link output queue info, the next link query & use this info */

    System_PvrScope_Obj pvrscopeObj;
    /**< object to hold the PVR SCOPE FIFO delails and profile Info */

} SgxFrmcpyLink_Obj;

/*******************************************************************************
 *  SgxFrmcpy Link Private Functions
 *******************************************************************************
 */
Int32 SgxFrmcpyLink_drvCreate(SgxFrmcpyLink_Obj *pObj,
                               SgxFrmcpyLink_CreateParams *pPrm);
Int32 SgxFrmcpyLink_drvStart(SgxFrmcpyLink_Obj *pObj);
Int32 SgxFrmcpyLink_drvDoProcessFrames(SgxFrmcpyLink_Obj *pObj);
Int32 SgxFrmcpyLink_drvStop(SgxFrmcpyLink_Obj *pObj);
Int32 SgxFrmcpyLink_drvDelete(SgxFrmcpyLink_Obj *pObj);
Int32 SgxFrmcpyLink_drvPrintStatistics(SgxFrmcpyLink_Obj *pObj);
Int32 SgxFrmcpyLink_drvDoProcessEglInfo(SgxFrmcpyLink_Obj *pObj,
			       struct control_srv_egl_ctx *pPrm);
Int32 SgxFrmcpyLink_getLinkInfo(Void * ptr, System_LinkInfo * info);
Int32 SgxFrmcpyLink_getFullBuffers(Void * ptr, UInt16 queId,
                                    System_BufferList * pBufList);
Int32 SgxFrmcpyLink_putEmptyBuffers(Void * ptr, UInt16 queId,
                                     System_BufferList * pBufList);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/*@}*/

/* Nothing beyond this point */
