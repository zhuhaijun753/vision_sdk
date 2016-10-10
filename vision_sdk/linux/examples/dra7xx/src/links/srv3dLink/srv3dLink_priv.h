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
 * \ingroup SRV3D_LINK_API
 * \defgroup SRV3D_LINK_IMPL Srv3D Link Implementation
 *
 * @{
 */

/**
 *******************************************************************************
 *
 * \file srv3DLink_priv.h Srv3D Link private API/Data structures
 *
 * \brief  This link private header file has defined
 *         - Srv3D link instance/handle object
 *         - All the local data structures
 *         - Srv3D driver interfaces
 *
 * \version 0.0 (Aug 2015) : [SS] First version
 *
 *******************************************************************************
 */

#ifndef _SRV3D_LINK_PRIV_H_
#define _SRV3D_LINK_PRIV_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *  Include files
 *******************************************************************************
 */
#include <include/link_api/system.h>
#include <linux/src/system/system_priv_common.h>
#include <osa.h>
#include <osa_que.h>
#include <include/link_api/srv3dLink.h>
#include <linux/src/links/sgx3Dsrv/sgxRender3DSRV.h>
#include <vivi_srv.h>

#include <EGL/egl.h>
#include <GLES2/gl2.h>

/*******************************************************************************
 *  Defines
 *******************************************************************************
 */


/**
 *******************************************************************************
 *
 *   \brief Max Number of srv3D link instances supported
 *
 *   SUPPORTED in ALL platforms
 *
 *******************************************************************************
 */
#define SRV3D_LINK_OBJ_MAX                     (1)

/**
 *******************************************************************************
 *
 * \brief Task size for SRV3D link task
 *
 *******************************************************************************
 */
#define SRV3D_LINK_TSK_STACK_SIZE              (OSA_TSK_STACK_SIZE_DEFAULT)

/**
 *******************************************************************************
 *
 *   \brief SV Algorithm Input Width
 *
 *   SUPPORTED in ALL platforms
 *
 *******************************************************************************
 */
#define SRV3D_LINK_INPUT_FRAME_WIDTH           (1280)

/**
 *******************************************************************************
 *
 *   \brief SV Algorithm Input Height
 *
 *   SUPPORTED in ALL platforms
 *
 *******************************************************************************
 */
#define SRV3D_LINK_INPUT_FRAME_HEIGHT          (720)

/**
 *******************************************************************************
 *
 *   \brief SV Algorithm Output Height
 *
 *   SUPPORTED in ALL platforms
 *
 *******************************************************************************
 */
#define SRV3D_LINK_OUTPUT_FRAME_HEIGHT           (1080)

/**
 *******************************************************************************
 *
 *   \brief SV Algorithm Output Width
 *
 *   SUPPORTED in ALL platforms
 *
 *******************************************************************************
 */
#define SRV3D_LINK_OUTPUT_FRAME_WIDTH            (880)
/**
 *******************************************************************************
 *
 *   \brief Number of EGL buffers requested
 *
 *   SUPPORTED in ALL platforms
 *
 *******************************************************************************
 */

#define SRV3D_LINK_MAXNUM_EGL_BUFFERS               (10)

/**
 *******************************************************************************
 *
 *   \brief Number of EGL textures stored
 *
 *   SUPPORTED in ALL platforms
 *
 *******************************************************************************
 */

#define SRV3DLINK_EGL_MAX_TEXTURES                  (64)


/*******************************************************************************
 *  Data structures
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 *   \brief Srv3D link instance object
 *
 *          This structure contains
 *          - All the local data structures
 *          - VPS Data structures required for Srv3D driver interfaces
 *          - All fields to support the Link stats and status information
 *
 *******************************************************************************
*/
typedef struct
{
    UInt32  linkId;
    /**< placeholder to store the Srv3D link Id */

    OSA_TskHndl  tsk;
    /**< placeholder to store the Srv3D link task handler */

    srv3dLink_CreateParams createArgs;
    /**< placeholder to store the Srv3D link create parameters */

    System_LinkInfo inTskInfo[SRV3D_LINK_NUM_IPQ];
    /**< Specifies a place holder that describe the LINK information */

    System_LinkInfo tskInfo;
    /**< Output queue information of this link */

    System_Buffer  *sysBufferGALUT;
    /**< Place holder for the GA LUT sysBuffer. Only one will be held
     * inside Synthesis link at any point in time.
     */
    System_Buffer  *sysBufferBlendLUT;
    /**< Place holder for the Blend LUT sysBuffer. Only one will be held
     * inside Synthesis link at any point in time.
     */
    System_Buffer  *sysBufferPALUT;
    /**< Place holder for the last PA LUT sysBuffer. Only one will be held
     * inside Synthesis link at any point in time.
     */

    System_Buffer *eglBuffers[SRV3D_LINK_MAXNUM_EGL_BUFFERS];

    OSA_QueHndl eglEmptyBufQue;
    /**< The queue that will hold the empty buffers */

    OSA_QueHndl eglFullBufQue;
    /**< The queue that will hold the rendered buffers */

    struct control_srv_egl_ctx eglInfo;
    /**< EGL functions to obtain pixmap buffers */

    EGLDisplay eglDisplay;
    /**< EGL display */

    EGLConfig eglConfig;
    /**< EGL render specific configuration */

    EGLContext eglContext;
    /**< EGL render context */

    EGLNativeDisplayType nativeDisplay;
    /**< Native display device */

    SgxRender3DSRV_Obj render3DSRVObj;
    /**< Reusable 3D surround view render object */

    PFNEGLCREATEIMAGEKHRPROC eglCreateImageKHR;
    /**< EGL proc to create EGLImage*/

    PFNGLEGLIMAGETARGETTEXTURE2DOESPROC eglEGLImageTargetTexture2DOES;
    /**< EGL proc to bind EGL Image to texture*/

    PFNEGLCREATESYNCKHRPROC  eglCreateSyncKHR;
    /**< EGL proc to create sync*/

    PFNEGLDESTROYSYNCKHRPROC eglDestroySyncKHR;
    /**< EGL proc to destroy sync*/

    PFNEGLCLIENTWAITSYNCKHRPROC eglClientWaitSyncKHR;
    /**< EGL proc to wait for sync completion*/

    PFNEGLGETSYNCATTRIBKHRPROC eglGetSyncAttribKHR;
    /**< EGL proc to get sync attributes*/

    UInt32 numTexture;
    /**< Number of pre-stored textures*/

    GLuint textureId[SRV3DLINK_EGL_MAX_TEXTURES];
    /**< GL texture IDs*/

    EGLImageKHR textureEglImage[SRV3DLINK_EGL_MAX_TEXTURES];
    /**< EGL image bound to the texture*/

    Void *textureBufAddr[SRV3DLINK_EGL_MAX_TEXTURES];
    /**< frame address for the egl Image*/

} srv3dLink_Obj;

/*******************************************************************************
 *  Srv3D Link Private Functions
 *******************************************************************************
 */
Int32 srv3dLink_drvCreate(srv3dLink_Obj *pObj,
                               srv3dLink_CreateParams *pPrm);
Int32 srv3dLink_drvStart(srv3dLink_Obj *pObj);
Int32 srv3dLink_drvDoProcessFrames(srv3dLink_Obj *pObj);
Int32 srv3dLink_drvStop(srv3dLink_Obj *pObj);
Int32 srv3dLink_drvDelete(srv3dLink_Obj *pObj);
Int32 srv3dLink_drvPrintStatistics(srv3dLink_Obj *pObj);
Int32 srv3dLink_drvGetLinkInfo(Void *pTsk, System_LinkInfo *info);
Int32 srv3dLink_drvGetFullBuffers(Void * ptr, UInt16 queId, System_BufferList * pBufList);
Int32 srv3dLink_drvPutEmptyBuffers(Void * ptr, UInt16 queId, System_BufferList * pBufList);
Int32 srv3dLink_drvDoProcessEglInfo(srv3dLink_Obj *pObj, struct control_srv_egl_ctx *pPrm);

void get_matrix_output_car(float x, float y, float z, float angleX, float angleY, float angleZ, float scaleX, float scaleY, float scaleZ, float* matrix, int viewid);
void get_matrix_output_mesh(float x, float y, float z, float angleX, float angleY, float angleZ, float scaleX, float scaleY, float scaleZ, float* matrix);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/*@}*/

/* Nothing beyond this point */
