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
 * \file system_gbm_allocator.h
 *
 * \brief   System level allocator for GBM buffers
 *          These buffers are shared between IPU, GPU, and A15.
 *
 *******************************************************************************
 */
#ifndef __SYSTEM_GBM_ALLOCATOR_H__
#define __SYSTEM_GBM_ALLOCATOR_H__

#include "system_gl_egl_utils.h"


EGLNativeDisplayType gbm_allocator_get_native_display (void);
EGLCompatBuffer gbm_allocator_get_native_buffer (uint32_t width, uint32_t height);
void gbm_allocator_destroy_native_buffer (EGLCompatBuffer buffer);

#endif
