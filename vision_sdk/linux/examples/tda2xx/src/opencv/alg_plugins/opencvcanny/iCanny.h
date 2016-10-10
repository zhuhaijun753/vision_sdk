/*
 *******************************************************************************
 *
 * Copyright (C) 2016 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \file iCanny.h
 *
 * \brief Interface file for OpenCV Canny algo in cpp
 *
 *        This OpenCV Canny algorithm is only for demonstrative purpose. 
 *        It is NOT product quality.
 *
 * \version 0.0 (March 2016) : [SN] First version
 *
 *******************************************************************************
 */

#ifndef _ICANNY_H_
#define _ICANNY_H_

/* OpenCV headers */
#include <opencv/highgui.h>
#include <opencv/cv.h>
#include <opencv/cxcore.h>

#include <include/link_api/system.h>

#ifdef __cplusplus
extern "C" {
#endif

void iCanny(IplImage *inputImage, IplImage *outputImage);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
