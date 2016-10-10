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
 * \file iResizer.h
 *
 * \brief Interface for resizer driver,
 *        Defines function pointers and interface for resizer drier,
 *        These functions must be implemented by the resizer driver
 *
 * \version 0.0
 *
 *******************************************************************************
 */

#ifndef IRESIZER_H_
#define IRESIZER_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */

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
 *   \brief Function pointer for creating driver
 *
 *******************************************************************************
*/
typedef Void * (*Alg_imgPyramidCreateDrv) (
            AlgorithmLink_ImgPmdObj *pImgPmdObj,
            Void *data);

/**
 *******************************************************************************
 *
 *   \brief Function pointer for deleting driver
 *
 *******************************************************************************
*/
typedef Void (*Alg_imgPyramidDeleteDrv) (
            Void *pHndl,
            Void *data);

/**
 *******************************************************************************
 *
 *   \brief Function pointer for processing frame
 *
 *******************************************************************************
*/
typedef Void (*Alg_imgPyramidDrvProcessFrame) (
            Void *pHndl,
            AlgorithmLink_ImgPyramidFrameDesc *inFrmDesc1,
            AlgorithmLink_ImgPyramidFrameDesc *outFrmDesc1,
            AlgorithmLink_ImgPyramidFrameDesc *inFrmDesc2,
            AlgorithmLink_ImgPyramidFrameDesc *outFrmDesc2,
            Void *data);

/**
 *******************************************************************************
 *
 *   \brief Structure containing function pointer for the resizer driver.
 *          the driver implementing resizer functionality can add
 *          their function so that they can be used for generating image
 *          pyramid.
 *
 *******************************************************************************
*/
struct Alg_ImgPyramidRszDrvFxns_t
{
    AlgorithmLink_ImgPyramidRszModules  rszModule;
    /**< ID of the resizer driver */
    Alg_imgPyramidCreateDrv             createDrv;
    /**< Function to create driver */
    Alg_imgPyramidDeleteDrv             deleteDrv;
    /**< Function to delete driver */
    Alg_imgPyramidDrvProcessFrame       processFrame;
    /**< Function to process frame,
         this pointer supports single input and two outputs */
} ;


/*******************************************************************************
 *  Global Variable
 *******************************************************************************
 */

/* Structure containing pointer to the function to the resizer module */
Alg_ImgPyramidRszDrvFxns gImgPyramidRszModule
    [ALGORITHM_LINK_IMG_PYRAMID_RSZ_MODULE_MAX] =
{
    #ifdef ISS_INCLUDE
    {
        ALGORITHM_LINK_IMG_PYRAMID_RSZ_MODULE_ISS,
        Alg_imgPyramidIssRszCreateDrv,
        Alg_imgPyramidIssRszDeleteDrv,
        Alg_imgPyramidIssRszProcessFrame
    },
    #endif
    #ifdef VPE_INCLUDE
    {
        ALGORITHM_LINK_IMG_PYRAMID_RSZ_MODULE_VPE,
        Alg_imgPyramidVpeRszCreateDrv,
        Alg_imgPyramidVpeRszDeleteDrv,
        Alg_imgPyramidVpeRszProcessFrame
    },
    #endif
    {
        ALGORITHM_LINK_IMG_PYRAMID_FORCE32BIT, NULL, NULL, NULL
    }
};

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/* Nothing beyond this point */
