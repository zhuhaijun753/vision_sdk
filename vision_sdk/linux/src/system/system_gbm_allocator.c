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
 * \file system_gbm_allocator.c
 *
 * \brief   System level allocator for GBM buffers
 *          These buffers are shared between IPU, GPU, and A15.
 *
 *******************************************************************************
 */
#include <stdlib.h>
#include <drm.h>

#include <omap_drmif.h>

#include <xf86drm.h>
#include <gbm.h>

#include <linux/src/osa/include/osa_mem.h>


#include "system_gl_egl_utils.h"
#include "system_gbm_allocator.h"

int fd = -1;
struct gbm_device *dev = NULL;
struct omap_device *odev = NULL;

EGLNativeDisplayType gbm_allocator_get_native_display ()
{
   if(fd == -1) {
       fd = drmOpen("omapdrm", NULL);
   }
   if(fd > 0 && dev == NULL) {
       dev = gbm_create_device(fd);
       odev = omap_device_new(fd);
   }

   return (EGLNativeDisplayType)dev;
} 

EGLCompatBuffer gbm_allocator_get_native_buffer (uint32_t width, uint32_t height)
{
   EGLCompatBuffer eglCBuf;
   Void *buffer, *pbuffer;
   uint32_t stride;
   uint32_t gemname;
   struct omap_bo *obo;


   gbm_device_precalculate_stride(dev, width, GBM_FORMAT_XRGB8888, &stride);
   buffer = OSA_memAllocSR(OSA_HEAPID_DDR_CACHED_SR1, stride * height, 32);	
   pbuffer = (Void *)OSA_memVirt2Phys((unsigned int)buffer, OSA_MEM_REGION_TYPE_SR1);
   obo = omap_bo_new_paddr(odev, stride * height, (uint32_t)pbuffer, OMAP_BO_WC);
   omap_bo_get_name(obo, &gemname);

   struct alloc_bufname_import data = {
       .width = width,
       .height = height,
       .stride = stride,
       .format = GBM_FORMAT_XRGB8888,
       .buffer_name = gemname,
   };


   struct gbm_bo *bo = gbm_bo_import(dev,
   GBM_BO_IMPORT_ALLOC_HANDLE,
   &data,
   GBM_BO_USE_SCANOUT | GBM_BO_USE_RENDERING);

   omap_bo_del(obo);

   eglCBuf.width = gbm_bo_get_width(bo);
   eglCBuf.height = gbm_bo_get_height(bo);
   eglCBuf.stride = gbm_bo_get_stride(bo);
   eglCBuf.eglPixmap = (EGLNativePixmapType) bo;
   eglCBuf.pixmapNative = (void *)buffer;

   return eglCBuf;
}

void gbm_allocator_destroy_native_buffer (EGLCompatBuffer buffer)
{
   uint32_t stride, height;
   struct gbm_bo *bo = (struct gbm_bo *)buffer.eglPixmap;

   height = gbm_bo_get_height(bo);
   stride = gbm_bo_get_stride(bo);

   gbm_bo_destroy(bo);
   OSA_memFreeSR(OSA_HEAPID_DDR_CACHED_SR1, buffer.pixmapNative, stride * height);
}
