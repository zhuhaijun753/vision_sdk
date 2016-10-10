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
 * \file system_gl_egl_utils.c
 *
 * \brief   System level helper functions for GL/EGL.
 *
 */
#ifndef _SYSTEM_GL_EGL_UTILS_H_
#define _SYSTEM_GL_EGL_UTILS_H_

#include <osa.h>
#include <osa_que.h>
#include <include/link_api/system_const.h>

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#ifdef BUILD_INFOADAS
#include <vivi_egl_context.h>
#else
#include "system_egl_context.h"
#endif

#define SYSTEM_MAXNUM_EGL_BUFFERS               (4)
#define SYSTEM_EGL_MAX_TEXTURES                 (100)

/**
 *******************************************************************************
 *
 * \brief Maximum number of output queues that SGX link supports.
 *
 * SUPPORTED in ALL platforms
 *
 *******************************************************************************
 */
#define  SGX_LINK_MAX_OUT_QUE                   (1U)

typedef struct {

    System_VideoDataFormat dataFormat;
    /**< SUPPORTED Formats,
     *    SYSTEM_DF_YUV420SP_UV
     */

    UInt32 width;
    /**< in pixels */

    UInt32 height;
    /**< in lines */

    UInt32 pitch[SYSTEM_MAX_PLANES];
    /**< in bytes, only pitch[0] used right now */

} System_EglTexProperty;

typedef struct
{
    EGLDisplay display;
    EGLNativeDisplayType nativeDisplay;
    /**< Native display device */
    EGLConfig config;
    EGLContext context;
    System_Buffer *eglBuffers[SYSTEM_MAXNUM_EGL_BUFFERS];

    OSA_QueHndl eglEmptyBufQue;
    /**< The queue that will hold the empty buffers */

    OSA_QueHndl eglFullBufQue;
    /**< The queue that will hold the rendered buffers */

    struct control_srv_egl_ctx eglInfo;
    /**< EGL functions to obtain pixmap buffers */

    GLuint      texYuv[SYSTEM_EGL_MAX_TEXTURES];
    EGLImageKHR texImg[SYSTEM_EGL_MAX_TEXTURES];
    Void        *bufAddr[SYSTEM_EGL_MAX_TEXTURES];
    int numBuf;

    int width;
    int height;

    System_LinkChInfo chInfo;

} System_EglObj;


void System_eglCheckGlError(const char* op);
void System_eglCheckEglError(const char* op, EGLBoolean returnVal);
void System_eglPrintGLString(const char *name, GLenum s);
int System_eglOpen(System_EglObj *pEglObj, struct control_srv_egl_ctx *eglInfo);
int System_eglMakeCurrentBuffer(System_EglObj *pEglObj, EGLSurface surface);
int System_eglMakeCurrentNill(System_EglObj *pEglObj);
GLuint System_eglGetTexYuv(System_EglObj *pEglObj, System_EglTexProperty *pProp, void *bufAddr);
void System_eglWaitSync(System_EglObj *pEglObj);
int System_eglClose(System_EglObj *pEglObj);

#endif
