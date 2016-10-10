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
 * \file canny.c
 *
 * \brief OpenCV Canny algo on A15
 *
 *        This OpenCV Canny algorithm is only for demonstrative purpose.
 *        It is NOT product quality.
 *        This algorithm does a OpenCV Canny. Height and width gets decided during
 *        Create. If height / width needs to be altered, then control call
 *        needs to be done.
 *
 * \version 0.0 (March 2016) : [SN] First version
 *
 *******************************************************************************
 */

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */
 
#include "iCanny.h"
#include "opencv2/imgproc.hpp"

void iCanny(IplImage *inputImage, IplImage *outputImage)
{
    UInt32 lowThresh;
    UInt32 highThresh;

    cv::Mat inputMat  = cv::cvarrToMat(inputImage, false);
    cv::Mat outputMat = cv::cvarrToMat(outputImage, false);
    
    lowThresh                       = 30;
    highThresh                      = lowThresh * 3;
    
    cv::Canny(inputMat, outputMat, lowThresh, highThresh, 3);
}
