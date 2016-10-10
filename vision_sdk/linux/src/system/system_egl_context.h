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
 * \defgroup SYSTEM_IMPL   System framework implementation
 *
 * @{
 *
 *******************************************************************************
 */

/**
 *******************************************************************************
 * \file system_egl_context.h
 *
 * \brief  Header file for system egl context sharing APIs.
 *
 *******************************************************************************
 */
#ifndef __SYSTEM_EGL_CONTEXT_H__
#define __SYSTEM_EGL_CONTEXT_H__

#include <EGL/egl.h>

struct __EGLCompatBuffer {
    EGLNativePixmapType eglPixmap;
    void *pixmapNative;
    UInt32 width;
    UInt32 height;
    UInt32 stride;
};

typedef struct __EGLCompatBuffer EGLCompatBuffer;

struct control_srv_egl_ctx {
    EGLNativeDisplayType (*get_egl_native_display) (void);
    EGLCompatBuffer (*get_egl_native_buffer) (
                uint32_t width, uint32_t height);
    void (*destroy_egl_native_buffer) (
                EGLCompatBuffer buffer);
};

#endif
