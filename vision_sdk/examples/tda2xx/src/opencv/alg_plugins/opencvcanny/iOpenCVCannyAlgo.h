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
 * \file iOpenCVCannyAlgo.h
 *
 * \brief Interface file for Alg_OpenCVCanny algorithm on A15
 *
 *        This Alg_OpenCVCanny algorithm is only for demonstrative purpose. 
 *        It is NOT product quality.
 *
 * \version 0.0 (March 2016) : [SN] First version
 *
 *******************************************************************************
 */

#ifndef _IOPENCVCANNYALGO_H_
#define _IOPENCVCANNYALGO_H_

/* opencv headers */

#include <opencv/highgui.h>

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */
#include <include/link_api/system.h>

/*******************************************************************************
 *  Defines
 *******************************************************************************
 */

/*******************************************************************************
 *  Enum's
 *******************************************************************************
 */

/*******************************************************************************
 *  Data structures
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 *   \brief Structure for OpenCV Canny algoirthm object
 *
 *******************************************************************************
*/
typedef struct
{
    UInt32                   maxHeight;
    /**< Max height of the frame */
    UInt32                   maxWidth;
    /**< max width of the frame */
    IplImage*                inputImage;
    /* opencv input image */
    IplImage*                outputImage;
    /* opencv output image */
} Alg_OpenCVCanny_Obj;

/**
 *******************************************************************************
 *
 *   \brief Structure containing the OpenCV Canny create time parameters
 *
 *******************************************************************************
*/
typedef struct
{
    UInt32                   maxHeight;
    /**< Max height of the frame */
    UInt32                   maxWidth;
    /**< max width of the frame */ 
} Alg_OpenCVCannyCreateParams;

/**
 *******************************************************************************
 *
 *   \brief Structure containing the OpenCV Canny control parameters
 *
 *******************************************************************************
*/
typedef struct
{
    UInt32                   xxx;
    /**< Any parameters which can be used to alter the algorithm behavior */
} Alg_OpenCVCannyControlParams;

/*******************************************************************************
 *  Functions
 *******************************************************************************
 */

 /**
 *******************************************************************************
 *
 * \brief Implementation of create for OpenCV Canny algo
 *
 * \param  pCreateParams    [IN] Creation parameters for OpenCV Canny Algorithm
 *
 * \return  Handle to algorithm
 *
 *******************************************************************************
 */
Alg_OpenCVCanny_Obj * Alg_OpenCVCannyCreate(
                        Alg_OpenCVCannyCreateParams *pCreateParams);

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
                           Uint32             copyMode
                          );

/**
 *******************************************************************************
 *
 * \brief Implementation of Control for OpenCV Canny algo
 *
 * \param  algHandle             [IN] Algorithm object handle
 * \param  pControlParams        [IN] Pointer to Control Params
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
 */
Int32 Alg_OpenCVCannyControl(Alg_OpenCVCanny_Obj          *pAlgHandle,
                           Alg_OpenCVCannyControlParams *pControlParams);

/**
 *******************************************************************************
 *
 * \brief Implementation of Delete for OpenCV Canny algo
 *
 * \param  algHandle             [IN] Algorithm object handle
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
 */
Int32 Alg_OpenCVCannyDelete(Alg_OpenCVCanny_Obj *pAlgHandle);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/* Nothing beyond this point */
