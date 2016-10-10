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
 * \ingroup SGX3DSFM_LINK_API
 * \defgroup SGX3DSFM_LINK_IMPL Sgx3DSFM Link Implementation
 *
 * @{
 */

/**
 *******************************************************************************
 *
 * \file sgx3DsfmLink_priv.h Sgx3Dsfm Link private API/Data structures
 *
 * \brief  This link private header file has defined
 *         - Sgx3Dsfm link instance/handle object
 *         - All the local data structures
 *         - Sgx3Dsfm driver interfaces
 *
 * \version 0.0 (Dec 2015) : [MM] First version
 *
 *******************************************************************************
 */

#ifndef _SGX3DSFM_LINK_PRIV_H_
#define _SGX3DSFM_LINK_PRIV_H_

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
#include <include/link_api/sgx3DsfmLink.h>
#include <include/link_api/algorithmLink_sfmMain.h>
#include <include/link_api/algorithmLink_sfmMap.h>
#include <linux/src/system/sgxRenderUtils/sgxRender3DSRV.h>
#include <linux/src/system/system_pvrscope_if.h>

/*******************************************************************************
 *  Defines
 *******************************************************************************
 */


/**
 *******************************************************************************
 *
 *   \brief Max Number of SGX3DSFM link instances supported
 *
 *   SUPPORTED in ALL platforms
 *
 *******************************************************************************
 */
#define SGX3DSFM_LINK_OBJ_MAX                     (1)

/**
 *******************************************************************************
 *
 * \brief Task size for SGX3DSFM link task
 *
 *******************************************************************************
 */
#define SGX3DSFM_LINK_TSK_STACK_SIZE              (OSA_TSK_STACK_SIZE_DEFAULT)

/**
 *******************************************************************************
 *
 *   \brief Max number of elements for local queues
 *
 *   SUPPORTED in ALL platforms
 *
 *******************************************************************************
 */
#define SGX3DSFM_LINK_MAX_LOCALQUEUELENGTH        (16)


/*******************************************************************************
 *  Data structures
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 *   \brief Data Structure for the Que of system buffers.
 *
 *   Que handle and the associated memory for queue elements are grouped.
 *
 *******************************************************************************
*/
typedef struct {
    OSA_QueHndl     queHandle;
    /**< Handle to the queue for this channel */
} Sgx3DsfmLink_SysBufferQue;


/**
 *******************************************************************************
 *
 *   \brief Sgx3Dsfm link instance object
 *
 *          This structure contains
 *          - All the local data structures
 *          - VPS Data structures required for Sgx3Dsfm driver interfaces
 *          - All fields to support the Link stats and status information
 *
 *******************************************************************************
*/
typedef struct
{
    UInt32  linkId;
    /**< placeholder to store the Sgx3Dsfm link Id */
    OSA_TskHndl  tsk;
    /**< placeholder to store the Sgx3Dsfm link task handler */
    UInt32  inPitch[SYSTEM_MAX_PLANES];
    /**< Pitch of the input video buffer, This is kept same for all channels */
    UInt32  inDataFormat;
    /**< Data format of the video to operate on */
    UInt32  numInputChannels;
    /**< Number of input channels on input Q (Prev link output Q) */
    Sgx3DsfmLink_CreateParams createArgs;
    /**< placeholder to store the Sgx3Dsfm link create parameters */
    System_LinkInfo inTskInfo[SGX3DSFM_LINK_IPQID_MAXIPQ];
    /**< Specifies a place holder that describe the LINK information */
    System_LinkQueInfo inQueInfo[SGX3DSFM_LINK_IPQID_MAXIPQ];
    /**< place holder that describe the output information of the LINK */
    /**< Payload for System buffers */
    System_LinkInfo tskInfo;
    /**< Output queue information of this link */
    Sgx3DsfmLink_SysBufferQue localInputQ[SGX3DSFM_LINK_IPQID_MAXIPQ];
    /**< Local Qs to hold input */
    /**< Counter to keep track of number of frame drops */
    System_Buffer  *sysBufferEGOPOSE;
    /**< Place holder for the last egopose sysBuffer. Only one will be held
     * inside Synthesis link at any point in time.
     */
    System_Buffer  *sysBufferBOXES;
    /**< Place holder for the last boxes sysBuffer. Only one will be held
     * inside Synthesis link at any point in time.
     */
    Bool receivedFirstEGOPOSEFlag;
    /**< Flag to check the availabilty of the GA LUT*/
    Bool receivedFirstBOXFlag;   
    /**< Flag to check the availabilty of the GA LUT*/ 
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
    SgxRender3DSRV_Obj render3DSRVObj;
    /**< 3D rendering prgram obj */

    System_PvrScope_Obj pvrscopeObj;
    /**< object to hold the PVR SCOPE FIFO delails and profile Info */

} Sgx3DsfmLink_Obj;

/*******************************************************************************
 *  Sgx3Dsfm Link Private Functions
 *******************************************************************************
 */
Int32 Sgx3DsfmLink_drvCreate(Sgx3DsfmLink_Obj *pObj,
                               Sgx3DsfmLink_CreateParams *pPrm);
Int32 Sgx3DsfmLink_drvStart(Sgx3DsfmLink_Obj *pObj);
Int32 Sgx3DsfmLink_drvDoProcessFrames(Sgx3DsfmLink_Obj *pObj);
Int32 Sgx3DsfmLink_drvStop(Sgx3DsfmLink_Obj *pObj);
Int32 Sgx3DsfmLink_drvDelete(Sgx3DsfmLink_Obj *pObj);
Int32 Sgx3DsfmLink_drvPrintStatistics(Sgx3DsfmLink_Obj *pObj);
Int32 Sgx3DsfmLink_drvDoProcessEglInfo(Sgx3DsfmLink_Obj *pObj,
			       struct control_srv_egl_ctx *pPrm);
Int32 Sgx3DsfmLink_getLinkInfo(Void *pTsk, System_LinkInfo *info);
Int32 Sgx3DsfmLink_getFullBuffers(Void * ptr, UInt16 queId, System_BufferList * pBufList);
Int32 Sgx3DsfmLink_putEmptyBuffers(Void * ptr, UInt16 queId, System_BufferList * pBufList);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/*@}*/

/* Nothing beyond this point */
