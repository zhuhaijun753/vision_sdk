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
#include "system_gl_egl_utils.h"

#ifndef EGL_TI_raw_video
#  define EGL_TI_raw_video             1
#  define EGL_RAW_VIDEO_TI             0x333A   /* eglCreateImageKHR target */
#  define EGL_GL_VIDEO_FOURCC_TI       0x3331   /* eglCreateImageKHR attribute */
#  define EGL_GL_VIDEO_WIDTH_TI        0x3332   /* eglCreateImageKHR attribute */
#  define EGL_GL_VIDEO_HEIGHT_TI       0x3333   /* eglCreateImageKHR attribute */
#  define EGL_GL_VIDEO_BYTE_STRIDE_TI  0x3334   /* eglCreateImageKHR attribute */
#  define EGL_GL_VIDEO_BYTE_SIZE_TI    0x3335   /* eglCreateImageKHR attribute */
#  define EGL_GL_VIDEO_YUV_FLAGS_TI    0x3336   /* eglCreateImageKHR attribute */
#endif

#ifndef EGLIMAGE_FLAGS_YUV_CONFORMANT_RANGE
#  define EGLIMAGE_FLAGS_YUV_CONFORMANT_RANGE (0 << 0)
#  define EGLIMAGE_FLAGS_YUV_FULL_RANGE       (1 << 0)
#  define EGLIMAGE_FLAGS_YUV_BT601            (0 << 1)
#  define EGLIMAGE_FLAGS_YUV_BT709            (1 << 1)
#endif

#define FOURCC(a, b, c, d) ((uint32_t)(uint8_t)(a) | ((uint32_t)(uint8_t)(b) << 8) | ((uint32_t)(uint8_t)(c) << 16) | ((uint32_t)(uint8_t)(d) << 24 ))
#define FOURCC_STR(str)    FOURCC(str[0], str[1], str[2], str[3])


static EGLSyncKHR (*eglCreateSyncKHR)(EGLDisplay dpy, EGLenum type, const EGLint *attrib_list);
static EGLBoolean (*eglDestroySyncKHR)(EGLDisplay dpy, EGLSyncKHR sync);
static EGLint (*eglClientWaitSyncKHR)(EGLDisplay dpy, EGLSyncKHR sync, EGLint flags, EGLTimeKHR timeout);

void System_eglPrintGLString(const char *name, GLenum s) {

   const char *v = (const char *) glGetString(s);

   Vps_printf(" EGL: GL %s = %s\n", name, v);
}

void System_eglCheckGlError(const char* op) {
   GLint error;

   for (error = glGetError(); error; error = glGetError()) {
       fprintf(stderr, "GL: after %s() glError (0x%x)\n", op, error);
   }
}

void System_eglCheckEglError(const char* op, EGLBoolean returnVal) {
   EGLint error;

   if (returnVal != EGL_TRUE) {
       fprintf(stderr, " EGL: %s() returned %d\n", op, returnVal);
   }

   for (error = eglGetError(); error != EGL_SUCCESS; error = eglGetError()) {
       fprintf(stderr, " EGL: after %s() eglError (0x%x)\n", op, error);
   }
}


int System_eglOpen(System_EglObj *pEglObj, struct control_srv_egl_ctx *eglInfo)
{
    EGLint num_configs;
    EGLint majorVersion;
    EGLint minorVersion;
    int ret, count;
    System_Buffer *nextBuf;
    int status;
    System_EglPixmapVideoFrameBuffer *eglBuf;    
    EGLCompatBuffer eglCBuf;
    UInt32 flags = pEglObj->chInfo.flags;

    const EGLint attribs[] = {
       EGL_RED_SIZE, 1,
       EGL_GREEN_SIZE, 1,
       EGL_BLUE_SIZE, 1,
       EGL_ALPHA_SIZE, 0,
       EGL_SURFACE_TYPE, EGL_PIXMAP_BIT,
       EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
       EGL_DEPTH_SIZE, 8,
       EGL_NONE
    };
    EGLint context_attribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };

    memcpy(&(pEglObj->eglInfo), eglInfo, sizeof(pEglObj->eglInfo));    

    pEglObj->nativeDisplay = pEglObj->eglInfo.get_egl_native_display();
    pEglObj->display = eglGetDisplay((EGLNativeDisplayType)pEglObj->nativeDisplay);
    System_eglCheckEglError("eglGetDisplay", EGL_TRUE);
    if (pEglObj->display == EGL_NO_DISPLAY) {
       Vps_printf(" EGL: ERROR: eglGetDisplay() returned EGL_NO_DISPLAY !!!\n");
       return -1;
    }

    ret = eglInitialize(pEglObj->display, &majorVersion, &minorVersion);
    System_eglCheckEglError("eglInitialize", ret);
    Vps_printf(" EGL: version %d.%d\n", majorVersion, minorVersion);
    if (ret != EGL_TRUE) {
       Vps_printf(" EGL: eglInitialize() failed !!!\n");
       return -1;
    }

    if (!eglBindAPI(EGL_OPENGL_ES_API)) { // Shiju - add in VSDK
        Vps_printf(" EGL: ERROR - failed to bind api EGL_OPENGL_ES_API\n");
        return -1;
    }

    if (!eglChooseConfig(pEglObj->display, attribs, &pEglObj->config, 1, &num_configs))
    {
       Vps_printf(" EGL: ERROR: eglChooseConfig() failed. Couldn't get an EGL visual config !!!\n");
       return -1;
    }

    pEglObj->context = eglCreateContext(pEglObj->display, pEglObj->config, EGL_NO_CONTEXT, context_attribs);
    System_eglCheckEglError("eglCreateContext", EGL_TRUE);
    if (pEglObj->context == EGL_NO_CONTEXT) {
       Vps_printf(" EGL: eglCreateContext() failed !!!\n");
       return -1;
    }

    ret = eglMakeCurrent(pEglObj->display, EGL_NO_SURFACE, EGL_NO_SURFACE, pEglObj->context);
    System_eglCheckEglError("eglMakeCurrent", ret);
    if (ret != EGL_TRUE) {
       Vps_printf(" EGL: eglMakeCurrent() failed !!!\n");
       return -1;
    }

    System_eglPrintGLString("Version", GL_VERSION);
    System_eglPrintGLString("Vendor", GL_VENDOR);
    System_eglPrintGLString("Renderer", GL_RENDERER);
    System_eglPrintGLString("Extensions", GL_EXTENSIONS);

    status  = OSA_queCreate(&(pEglObj->eglEmptyBufQue), SYSTEM_MAXNUM_EGL_BUFFERS);
    OSA_assert(status == SYSTEM_LINK_STATUS_SOK);

    status  = OSA_queCreate(&(pEglObj->eglFullBufQue), SYSTEM_MAXNUM_EGL_BUFFERS);
    OSA_assert(status == SYSTEM_LINK_STATUS_SOK);

    for (count = 0; count < SYSTEM_MAXNUM_EGL_BUFFERS; count++)
    {
        nextBuf = OSA_memAlloc(sizeof(System_Buffer));
        OSA_assert(nextBuf != NULL);

        nextBuf->payload = OSA_memAlloc(sizeof(System_EglPixmapVideoFrameBuffer));
        OSA_assert(nextBuf->payload != NULL);

        nextBuf->payloadSize = sizeof(System_EglPixmapVideoFrameBuffer);
        nextBuf->bufType = SYSTEM_BUFFER_TYPE_EGLPIXMAP_VIDEO_FRAME;
        nextBuf->chNum = 0;
        nextBuf->frameId = 0;

        eglBuf = nextBuf->payload;
        eglCBuf = pEglObj->eglInfo.get_egl_native_buffer(pEglObj->width, pEglObj->height);
        eglBuf->eglPixmap = eglCBuf.eglPixmap;
        eglBuf->pixmapNative = eglCBuf.pixmapNative;
        eglBuf->bufAddr[0] = eglCBuf.pixmapNative;
        eglBuf->eglSurface = (UInt32)eglCreatePixmapSurface(pEglObj->display, pEglObj->config, eglBuf->eglPixmap, NULL);
        OSA_assert(eglBuf->eglSurface != (UInt32)EGL_NO_SURFACE);

        eglBuf->chInfo.flags = System_Link_Ch_Info_Set_Flag_Data_Format(flags, SYSTEM_DF_ABGR32_8888);	
        eglBuf->chInfo.pitch[0] = eglCBuf.stride;
        eglBuf->chInfo.pitch[1] = eglCBuf.stride;
        eglBuf->chInfo.pitch[2] = eglCBuf.stride;
        eglBuf->chInfo.width = eglCBuf.width;
        eglBuf->chInfo.height= eglCBuf.height;
        eglBuf->chInfo.startX= 0;
        eglBuf->chInfo.startY= 0;
        eglBuf->flags = System_Link_Ch_Info_Set_Flag_Data_Format(flags, SYSTEM_DF_ABGR32_8888);	

        pEglObj->chInfo.width = eglCBuf.width;
        pEglObj->chInfo.height= eglCBuf.height;
        pEglObj->chInfo.pitch[0] = eglCBuf.stride;
        pEglObj->chInfo.pitch[1] = eglCBuf.stride;
        pEglObj->chInfo.pitch[2] = eglCBuf.stride;
        pEglObj->chInfo.startX   = 0;
        pEglObj->chInfo.startY   = 0;
        pEglObj->chInfo.flags = System_Link_Ch_Info_Set_Flag_Data_Format(flags, SYSTEM_DF_ABGR32_8888);

        //Vps_printf("System Buffer created = %p, contains = %p (eglpixmap = %p)\n", nextBuf, eglBuf, eglBuf->eglPixmap);

        OSA_quePut(&(pEglObj->eglEmptyBufQue), (Int32)nextBuf, OSA_TIMEOUT_FOREVER);
        pEglObj->eglBuffers[count] = nextBuf;
    }

    return 0;
}

void System_eglWaitSync(System_EglObj *pEglObj)
{
    int ret;
    EGLSyncKHR sync = NULL;

    if (eglCreateSyncKHR)
    {
        sync = eglCreateSyncKHR(pEglObj->display, EGL_SYNC_FENCE_KHR, NULL);
    }
    if(sync != EGL_NO_SYNC_KHR)
    {
        ret = eglClientWaitSyncKHR(pEglObj->display, sync,
                           EGL_SYNC_FLUSH_COMMANDS_BIT_KHR, 1000 * 1000 * 32);
        if (ret == EGL_TIMEOUT_EXPIRED_KHR)
        {
        Vps_printf(" SGXDISPLAY: wait for egl sync timed out\n");
        }
        else if (ret != EGL_CONDITION_SATISFIED_KHR)
        {
        Vps_printf(" SGXDISPLAY: failed to wait for egl sync: %x\n", eglGetError());
        }
        if (!eglDestroySyncKHR(pEglObj->display, sync))
        {
        Vps_printf(" SGXDISPLAY: error at eglDestroySyncKHR: %x\n", eglGetError());
        }
    }
}

static GLuint System_eglSetupYuvTexSurface(System_EglObj *pObj, System_EglTexProperty *pProp, void *bufAddr, int texIndex)
{
    EGLint attr[32];
    int attrIdx;
    PFNEGLCREATEIMAGEKHRPROC eglCreateImageKHR;
    PFNGLEGLIMAGETARGETTEXTURE2DOESPROC glEGLImageTargetTexture2DOES;

    attrIdx = 0;

    attr[attrIdx++] = EGL_GL_VIDEO_FOURCC_TI;
    attr[attrIdx++] = FOURCC_STR("NV12");

    attr[attrIdx++] = EGL_GL_VIDEO_WIDTH_TI;
    attr[attrIdx++] = pProp->width;

    attr[attrIdx++] = EGL_GL_VIDEO_HEIGHT_TI;
    attr[attrIdx++] = pProp->height;

    attr[attrIdx++] = EGL_GL_VIDEO_BYTE_STRIDE_TI;
    attr[attrIdx++] = pProp->pitch[0];

    attr[attrIdx++] = EGL_GL_VIDEO_BYTE_SIZE_TI;
    if(pProp->dataFormat==SYSTEM_DF_YUV420SP_UV)
    {
        attr[attrIdx++] = (pProp->pitch[0] * pProp->height * 3)/2;
    }
    else
    {
        Vps_printf(" EGL: ERROR: Unsupported data format (%d) !!!\n", pProp->dataFormat);
        OSA_assert(0);
    }

    attr[attrIdx++] = EGL_GL_VIDEO_YUV_FLAGS_TI;
    attr[attrIdx++] = EGLIMAGE_FLAGS_YUV_CONFORMANT_RANGE | EGLIMAGE_FLAGS_YUV_BT601;

    attr[attrIdx++] = EGL_NONE;


    eglCreateImageKHR =
        (PFNEGLCREATEIMAGEKHRPROC)eglGetProcAddress("eglCreateImageKHR");
    glEGLImageTargetTexture2DOES =
        (PFNGLEGLIMAGETARGETTEXTURE2DOESPROC)eglGetProcAddress("glEGLImageTargetTexture2DOES");

    pObj->texImg[texIndex] = eglCreateImageKHR(
                                pObj->display,
                                EGL_NO_CONTEXT,
                                EGL_RAW_VIDEO_TI,
                                bufAddr,
                                attr
                              );

    System_eglCheckEglError("eglCreateImageKHR", EGL_TRUE);
    if (pObj->texImg[texIndex] == EGL_NO_IMAGE_KHR) {
        Vps_printf(" EGL: ERROR: eglCreateImageKHR failed !!!\n");
        return -1;
    }

    glGenTextures(1, &pObj->texYuv[texIndex]);
    System_eglCheckGlError("eglCreateImageKHR");

    glBindTexture(GL_TEXTURE_EXTERNAL_OES, pObj->texYuv[texIndex]);
    System_eglCheckGlError("glBindTexture");

    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    System_eglCheckGlError("glTexParameteri");

    glEGLImageTargetTexture2DOES(GL_TEXTURE_EXTERNAL_OES, (GLeglImageOES)pObj->texImg[texIndex]);
    System_eglCheckGlError("glEGLImageTargetTexture2DOES");

    pObj->bufAddr[texIndex] = bufAddr;

    return 0;
}

GLuint System_eglGetTexYuv(System_EglObj *pEglObj, System_EglTexProperty *pProp, void *bufAddr)
{
    GLuint texYuv = 0;
    int texFound = 0, i, status;

    for(i=0; i<pEglObj->numBuf; i++)
    {
        if(pEglObj->bufAddr[i]==bufAddr)
        {
            texYuv = pEglObj->texYuv[i];
            texFound = 1;
            break;
        }
    }
    if(texFound==0)
    {
        OSA_assert(i<SYSTEM_EGL_MAX_TEXTURES);

        status = System_eglSetupYuvTexSurface(
                        pEglObj,
                        pProp,
                        bufAddr,
                        i
                        );
        if(status!=0)
        {
            Vps_printf(" EGL: ERROR: Unable to bind texture[%d] to address [0x%08x] !!!\n", i, (unsigned int)bufAddr);
        }
        OSA_assert(status==0);

        texYuv = pEglObj->texYuv[i];

        pEglObj->numBuf++;
    }

    return texYuv;
}

int System_eglMakeCurrentNill(System_EglObj *pEglObj)
{
    int ret = eglMakeCurrent(pEglObj->display, EGL_NO_SURFACE, EGL_NO_SURFACE, pEglObj->context);
    System_eglCheckEglError("eglMakeCurrent", ret);
    if (ret != EGL_TRUE) {
       Vps_printf(" EGL: eglMakeCurrent() failed !!!\n");
       return -1;
    }

    return 0;
}

int System_eglMakeCurrentBuffer(System_EglObj *pEglObj, EGLSurface surface)
{
    int ret = eglMakeCurrent(pEglObj->display, surface, surface, pEglObj->context);
    System_eglCheckEglError("eglMakeCurrent", ret);
    if (ret != EGL_TRUE) {
       Vps_printf(" EGL: eglMakeCurrent() failed !!!\n");
       return -1;
    }

    return 0;
}

int System_eglClose(System_EglObj *pEglObj)
{
    int count;
    System_Buffer *nextBuf;
    System_EglPixmapVideoFrameBuffer *eglBuf;
    EGLCompatBuffer eglCBuf;


    eglMakeCurrent(pEglObj->display,EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    for (count = 0; count < SYSTEM_MAXNUM_EGL_BUFFERS; count++)
    {
        nextBuf = pEglObj->eglBuffers[count];
        eglBuf = nextBuf->payload;
        eglDestroySurface(pEglObj->display, (EGLSurface)eglBuf->eglSurface);
        eglCBuf.eglPixmap = eglBuf->eglPixmap;
        eglCBuf.pixmapNative = eglBuf->pixmapNative;
        pEglObj->eglInfo.destroy_egl_native_buffer(eglCBuf);
        OSA_memFree(eglBuf);
        OSA_memFree(nextBuf);
    }

    eglDestroyContext(pEglObj->display, pEglObj->context);
#ifdef BUILD_INFOADAS
    eglTerminate(pEglObj->display);
#endif
    OSA_queDelete(&(pEglObj->eglEmptyBufQue));
    OSA_queDelete(&(pEglObj->eglFullBufQue));

    return 0;

}
