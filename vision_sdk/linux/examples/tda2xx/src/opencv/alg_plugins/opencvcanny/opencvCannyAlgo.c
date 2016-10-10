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
 * \file opencvCannyAlgo.c
 *
 * \brief Algorithm for Alg_OpenCVCanny on A15
 *
 *        This Alg_OpenCVCanny algorithm is only for demonstrative purpose.
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
#include "iOpenCVCannyAlgo.h"

#include "iCanny.h"

/**
 *******************************************************************************
 *
 * \brief Implementation of create for OpenCVCanny algo
 *
 * \param  pCreateParams    [IN] Creation parameters for OpenCVCanny Algorithm
 *
 * \return  Handle to algorithm
 *
 *******************************************************************************
 */
Alg_OpenCVCanny_Obj * Alg_OpenCVCannyCreate(
                        Alg_OpenCVCannyCreateParams *pCreateParams)
{
    CvSize imageSize;

    Alg_OpenCVCanny_Obj * pAlgHandle;

    pAlgHandle = (Alg_OpenCVCanny_Obj *) malloc(sizeof(Alg_OpenCVCanny_Obj));

    OSA_assert(pAlgHandle != NULL);

    pAlgHandle->maxHeight           = pCreateParams->maxHeight;
    pAlgHandle->maxWidth            = pCreateParams->maxWidth;
    
    imageSize.height                = pCreateParams->maxHeight;
    imageSize.width                 = pCreateParams->maxWidth;
    pAlgHandle->inputImage          = cvCreateImageHeader(imageSize, IPL_DEPTH_8U, 1);
    pAlgHandle->outputImage         = cvCreateImageHeader(imageSize, IPL_DEPTH_8U, 1);

    return pAlgHandle;
}

/**
 *******************************************************************************
 *
 * \brief Implementation of Process for OpenCVCanny algo
 *
 *        Supported formats are SYSTEM_DF_YUV422I_YUYV, SYSTEM_DF_YUV420SP_UV
 *        It is assumed that the width of the image will
 *        be multiple of 4 and buffer pointers are 32-bit aligned.
 *
 * \param  algHandle    [IN] Algorithm object handle
 * \param  inPtr[]      [IN] Array of input pointers
 *                           Index 0 - Pointer to Y data in case of YUV420SP,
 *                                   - Single pointer for YUV422IL or RGB
 *                           Index 1 - Pointer to UV data in case of YUV420SP
 * \param  outPtr[]     [IN] Array of output pointers. Indexing similar to
 *                           array of input pointers
 * \param  width        [IN] width of image
 * \param  height       [IN] height of image
 * \param  inPitch[]    [IN] Array of pitch of input image (Address offset
 *                           b.n. two  consecutive lines, interms of bytes)
 *                           Indexing similar to array of input pointers
 * \param  outPitch[]   [IN] Array of pitch of output image (Address offset
 *                           b.n. two  consecutive lines, interms of bytes)
 *                           Indexing similar to array of input pointers
 * \param  dataFormat   [IN] Different image data formats. Refer
 *                           System_VideoDataFormat
 * \param  copyMode     [IN] 0 - copy by CPU, 1 - copy by DMA
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
 */
Int32 Alg_OpenCVCannyProcess(Alg_OpenCVCanny_Obj *algHandle,
                           UInt32            *inPtr[],
                           UInt32            *outPtr[],
                           UInt32             width,
                           UInt32             height,
                           UInt32             inPitch[],
                           UInt32             outPitch[],
                           UInt32             dataFormat,
                           UInt32             copyMode
                          )
{
    Int32 rowIdx;
    Int32 colIdx;

    UInt32 wordWidth;
    UInt32 numPlanes;

    UInt32 *inputPtr;
    UInt32 *outputPtr;

    if((width > algHandle->maxWidth) ||
       (height > algHandle->maxHeight) ||
       (copyMode != 0))
    {
        return SYSTEM_LINK_STATUS_EFAIL;
    }

    if(dataFormat == SYSTEM_DF_YUV422I_YUYV)
    {
        numPlanes = 1;
        wordWidth = (width*2)>>2;
    }
    else if(dataFormat == SYSTEM_DF_YUV420SP_UV)
    {
        numPlanes = 2;
        wordWidth = (width)>>2;
    }
    else
    {
        return SYSTEM_LINK_STATUS_EFAIL;
    } 

    /*
     * For Luma plane of 420SP OR RGB OR 422IL
     */
     
    inputPtr  = inPtr[0];
    outputPtr = outPtr[0];

    cvSetData(algHandle->inputImage, (uchar *)inputPtr, inPitch[0]);
    cvSetData(algHandle->outputImage, (uchar *)outputPtr, outPitch[0]);

    iCanny(algHandle->inputImage, algHandle->outputImage);

    /*
     * For Chroma plane of 420SP
     */

    if(numPlanes == 2)
    {
        inputPtr  = inPtr[1];
        outputPtr = outPtr[1];
        for(rowIdx = 0; rowIdx < (height >> 1) ; rowIdx++)
        {
            for(colIdx = 0; colIdx < wordWidth ; colIdx++)
            {
                *(outputPtr+colIdx) = 0x80808080;
            }
            inputPtr += (inPitch[1] >> 2);
            outputPtr += (outPitch[1] >> 2);
        }
    }

    return SYSTEM_LINK_STATUS_SOK;
}

/**
 *******************************************************************************
 *
 * \brief Implementation of Control for OpenCVCanny algo
 *
 * \param  algHandle             [IN] Algorithm object handle
 * \param  pControlParams        [IN] Pointer to Control Params
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
 */
Int32 Alg_OpenCVCannyControl(Alg_OpenCVCanny_Obj          *pAlgHandle,
                           Alg_OpenCVCannyControlParams *pControlParams)
{
    /*
     * Any alteration of algorithm behavior
     */
    return SYSTEM_LINK_STATUS_SOK;
}

/**
 *******************************************************************************
 *
 * \brief Implementation of Stop for OpenCVCanny algo
 *
 * \param  algHandle    [IN] Algorithm object handle
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
 */
Int32 Alg_OpenCVCannyStop(Alg_OpenCVCanny_Obj *algHandle)
{
      return SYSTEM_LINK_STATUS_SOK;
}

/**
 *******************************************************************************
 *
 * \brief Implementation of Delete for OpenCVCanny algo
 *
 * \param  algHandle    [IN] Algorithm object handle
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
 */
Int32 Alg_OpenCVCannyDelete(Alg_OpenCVCanny_Obj *algHandle)
{
    cvReleaseImageHeader(&algHandle->inputImage);
    cvReleaseImageHeader(&algHandle->outputImage);
    free(algHandle);
    return SYSTEM_LINK_STATUS_SOK;
}

/* Nothing beyond this point */
