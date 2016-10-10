/*
 *******************************************************************************
 *
 * Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */
#include "alg_filter2d.h"

Int32 Alg_claheCreate(
            Alg_ClaheObj *pObj,
            UInt32 inWidth,
            UInt32 inHeight,
            UInt32 inPitch,
            UInt32 outPitch
            )
{
    pObj->prms.visionParams.algParams.size =
        sizeof(pObj->prms);
    pObj->prms.visionParams.cacheWriteBack = NULL;
    pObj->prms.maxWidth = inWidth;
    pObj->prms.maxHeight = inHeight;
    pObj->prms.tileWidth = 160;
    pObj->prms.tileHeight = 90;

    pObj->handle = AlgIvision_create(
                &CLAHE_TI_VISION_FXNS,
                (IALG_Params*)&pObj->prms
                );
    UTILS_assert(pObj->handle!=NULL);

    pObj->inArgs.iVisionInArgs.size =
        sizeof(pObj->inArgs);

    pObj->inArgs.clipLimit = 2;

    pObj->outArgs.iVisionOutArgs.size =
        sizeof(pObj->outArgs);

    pObj->inBufs.size = sizeof(pObj->inBufs);
    pObj->inBufs.numBufs = CLAHE_TI_BUFDESC_IN_TOTAL;
    pObj->inBufs.bufDesc  = pObj->inBufDescList;

    pObj->outBufs.size = sizeof(pObj->outBufs);
    pObj->outBufs.numBufs = CLAHE_TI_BUFDESC_OUT_TOTAL;
    pObj->outBufs.bufDesc = pObj->outBufDescList;

    pObj->inBufDescList[CLAHE_TI_BUFDESC_IN_IMAGE_BUFFER] = &pObj->inBufDesc[CLAHE_TI_BUFDESC_IN_IMAGE_BUFFER];
    pObj->outBufDescList[CLAHE_TI_BUFDESC_IN_IMAGE_BUFFER] = &pObj->outBufDesc[CLAHE_TI_BUFDESC_OUT_IMAGE_BUFFER];

    pObj->inBufDescList[CLAHE_TI_BUFDESC_IN_LUT_BUFFER] = &pObj->inBufDesc[CLAHE_TI_BUFDESC_IN_LUT_BUFFER];
    pObj->outBufDescList[CLAHE_TI_BUFDESC_OUT_LUT_BUFFER] = &pObj->outBufDesc[CLAHE_TI_BUFDESC_OUT_LUT_BUFFER];

    pObj->inBufDesc[CLAHE_TI_BUFDESC_IN_IMAGE_BUFFER].numPlanes                          = 1;
    pObj->inBufDesc[CLAHE_TI_BUFDESC_IN_IMAGE_BUFFER].bufPlanes[0].frameROI.topLeft.x    = 0;
    pObj->inBufDesc[CLAHE_TI_BUFDESC_IN_IMAGE_BUFFER].bufPlanes[0].frameROI.topLeft.y    = 0;
    pObj->inBufDesc[CLAHE_TI_BUFDESC_IN_IMAGE_BUFFER].bufPlanes[0].width                 = inPitch;
    pObj->inBufDesc[CLAHE_TI_BUFDESC_IN_IMAGE_BUFFER].bufPlanes[0].height                = inHeight;
    pObj->inBufDesc[CLAHE_TI_BUFDESC_IN_IMAGE_BUFFER].bufPlanes[0].frameROI.width        = inWidth;
    pObj->inBufDesc[CLAHE_TI_BUFDESC_IN_IMAGE_BUFFER].bufPlanes[0].frameROI.height       = inHeight;
    pObj->inBufDesc[CLAHE_TI_BUFDESC_IN_IMAGE_BUFFER].bufPlanes[0].planeType             = 0;

    pObj->inBufDesc[CLAHE_TI_BUFDESC_IN_LUT_BUFFER].numPlanes                          = 1;
    pObj->inBufDesc[CLAHE_TI_BUFDESC_IN_LUT_BUFFER].bufPlanes[0].frameROI.topLeft.x    = 0;
    pObj->inBufDesc[CLAHE_TI_BUFDESC_IN_LUT_BUFFER].bufPlanes[0].frameROI.topLeft.y    = 0;
    pObj->inBufDesc[CLAHE_TI_BUFDESC_IN_LUT_BUFFER].bufPlanes[0].width                 = 0;
    pObj->inBufDesc[CLAHE_TI_BUFDESC_IN_LUT_BUFFER].bufPlanes[0].height                = 0;
    pObj->inBufDesc[CLAHE_TI_BUFDESC_IN_LUT_BUFFER].bufPlanes[0].frameROI.width        = 0;
    pObj->inBufDesc[CLAHE_TI_BUFDESC_IN_LUT_BUFFER].bufPlanes[0].frameROI.height       = 0;
    pObj->inBufDesc[CLAHE_TI_BUFDESC_IN_LUT_BUFFER].bufPlanes[0].planeType             = 0;
    pObj->inBufDesc[CLAHE_TI_BUFDESC_IN_LUT_BUFFER].bufPlanes[0].buf                   = NULL;

    pObj->outBufDesc[CLAHE_TI_BUFDESC_OUT_IMAGE_BUFFER].numPlanes                          = 1;
    pObj->outBufDesc[CLAHE_TI_BUFDESC_OUT_IMAGE_BUFFER].bufPlanes[0].frameROI.topLeft.x    = 0;
    pObj->outBufDesc[CLAHE_TI_BUFDESC_OUT_IMAGE_BUFFER].bufPlanes[0].frameROI.topLeft.y    = 0;
    pObj->outBufDesc[CLAHE_TI_BUFDESC_OUT_IMAGE_BUFFER].bufPlanes[0].width                 = outPitch;
    pObj->outBufDesc[CLAHE_TI_BUFDESC_OUT_IMAGE_BUFFER].bufPlanes[0].height                = inHeight;
    pObj->outBufDesc[CLAHE_TI_BUFDESC_OUT_IMAGE_BUFFER].bufPlanes[0].frameROI.width        = inWidth;
    pObj->outBufDesc[CLAHE_TI_BUFDESC_OUT_IMAGE_BUFFER].bufPlanes[0].frameROI.height       = inHeight;
    pObj->outBufDesc[CLAHE_TI_BUFDESC_OUT_IMAGE_BUFFER].bufPlanes[0].planeType             = 0;
    pObj->outBufDesc[CLAHE_TI_BUFDESC_OUT_IMAGE_BUFFER].bufPlanes[0].buf                   = NULL;

    pObj->outBufDesc[CLAHE_TI_BUFDESC_OUT_LUT_BUFFER].numPlanes                          = 1;
    pObj->outBufDesc[CLAHE_TI_BUFDESC_OUT_LUT_BUFFER].bufPlanes[0].frameROI.topLeft.x    = 0;
    pObj->outBufDesc[CLAHE_TI_BUFDESC_OUT_LUT_BUFFER].bufPlanes[0].frameROI.topLeft.y    = 0;
    pObj->outBufDesc[CLAHE_TI_BUFDESC_OUT_LUT_BUFFER].bufPlanes[0].width                 = 0;
    pObj->outBufDesc[CLAHE_TI_BUFDESC_OUT_LUT_BUFFER].bufPlanes[0].height                = 0;
    pObj->outBufDesc[CLAHE_TI_BUFDESC_OUT_LUT_BUFFER].bufPlanes[0].frameROI.width        = 0;
    pObj->outBufDesc[CLAHE_TI_BUFDESC_OUT_LUT_BUFFER].bufPlanes[0].frameROI.height       = 0;
    pObj->outBufDesc[CLAHE_TI_BUFDESC_OUT_LUT_BUFFER].bufPlanes[0].planeType             = 0;
    pObj->outBufDesc[CLAHE_TI_BUFDESC_OUT_LUT_BUFFER].bufPlanes[0].buf                   = NULL;

    pObj->lutBufSize = (256*inWidth*inHeight)/(pObj->prms.tileWidth*pObj->prms.tileHeight);

    pObj->lutBufAddr[0] =
        Utils_memAlloc(
            UTILS_HEAPID_DDR_CACHED_SR,
            pObj->lutBufSize,
            32
        );
    UTILS_assert(pObj->lutBufAddr[0]!=NULL);

    pObj->lutBufAddr[1] =
        Utils_memAlloc(
            UTILS_HEAPID_DDR_CACHED_SR,
            pObj->lutBufSize,
            32
        );
    UTILS_assert(pObj->lutBufAddr[1]!=NULL);

    pObj->lutBufId = 0;

    return 0;
}

Int32 Alg_claheProcess(
            Alg_ClaheObj *pObj,
            Void *inBufAddr,
            Void *outBufAddr
        )
{
    Int32 status = SYSTEM_LINK_STATUS_SOK;

    pObj->inBufDesc[CLAHE_TI_BUFDESC_IN_IMAGE_BUFFER].bufPlanes[0].buf = inBufAddr;

    pObj->outBufDesc[CLAHE_TI_BUFDESC_OUT_IMAGE_BUFFER].bufPlanes[0].buf = outBufAddr;

    pObj->inBufDesc[CLAHE_TI_BUFDESC_IN_LUT_BUFFER].bufPlanes[0].buf =
        pObj->lutBufAddr[pObj->lutBufId];

    pObj->outBufDesc[CLAHE_TI_BUFDESC_OUT_LUT_BUFFER].bufPlanes[0].buf =
        pObj->lutBufAddr[pObj->lutBufId ^ 1];

    status = AlgIvision_process(
                pObj->handle,
                &pObj->inBufs,
                &pObj->outBufs,
                (IVISION_InArgs*)&pObj->inArgs,
                (IVISION_OutArgs*)&pObj->outArgs
            );
    UTILS_assert(status==0);

    /* switch LUT buffers */
    pObj->lutBufId  = pObj->lutBufId ^ 1;

    return status;
}


Int32 Alg_claheDelete(Alg_ClaheObj *pObj)
{
    Int32 status = SYSTEM_LINK_STATUS_SOK;

    if(pObj->handle)
    {
        status = AlgIvision_delete(pObj->handle);
        UTILS_assert(status==0);
    }

    if(pObj->lutBufAddr[0])
    {
        status = Utils_memFree(
            UTILS_HEAPID_DDR_CACHED_SR,
            pObj->lutBufAddr[0],
            pObj->lutBufSize
        );
        UTILS_assert(status==0);
    }

    if(pObj->lutBufAddr[1])
    {
        status = Utils_memFree(
            UTILS_HEAPID_DDR_CACHED_SR,
            pObj->lutBufAddr[1],
            pObj->lutBufSize
        );
    }
    return status;
}

