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

#define FILTER_COEF_WIDTH   3
#define FILTER_COEF_HEIGHT  3

static uint8_t filterCoeff[FILTER_COEF_WIDTH * FILTER_COEF_HEIGHT]=
{
  1, 2, 1,
  1, 2, 1,
  0, 0, 0,
};

Int32 Alg_filter2dCreate(
            Alg_Filter2dObj *pObj,
            UInt32 inWidth,
            UInt32 inHeight,
            UInt32 inPitch[],
            Bool allocTmpBuf,
            Bool enableClahe,
            Bool enableFilter2d
)
{
    memset(pObj, 0, sizeof(*pObj));

    if(enableClahe==FALSE && enableFilter2d==FALSE)
    {
        /* both cannot be FALSE, so force enable filter2d */
        enableFilter2d = TRUE;
    }

    pObj->enableClahe = enableClahe;
    pObj->enableFilter2d = enableFilter2d;

    pObj->prms.visionParams.algParams.size =
        sizeof(pObj->prms);

    pObj->prms.visionParams.cacheWriteBack = NULL;
    pObj->prms.filterCoefWidth = FILTER_COEF_WIDTH;
    pObj->prms.filterCoefHeight = FILTER_COEF_HEIGHT;
    pObj->prms.filterCoef = filterCoeff;
    pObj->prms.imageFormat = FILTER_2D_TI_IMAGE_FORMAT_YUV420;
    pObj->prms.separableFilter = 1;
    if(pObj->enableClahe)
    {
        pObj->prms.enableContrastStretching = FALSE;
    }
    else
    {
        pObj->prms.enableContrastStretching = TRUE;
    }
    pObj->prms.enableFilter = TRUE;
    pObj->prms.minVal = 0;
    pObj->prms.maxVal = 255;
    pObj->prms.minPercentileThreshold = 1;
    pObj->prms.maxPercentileThreshold = 99;

    if(pObj->enableFilter2d)
    {
        pObj->handle = AlgIvision_create(
                &FILTER_2D_TI_VISION_FXNS,
                (IALG_Params*)&pObj->prms
                );
        UTILS_assert(pObj->handle!=NULL);
    }

    pObj->inArgs.iVisionInArgs.size =
        sizeof(pObj->inArgs);

    pObj->inArgs.minVal = 0;
    pObj->inArgs.maxVal = 255;

    pObj->outArgs.iVisionOutArgs.size =
        sizeof(pObj->outArgs);

    pObj->inBufs.size = sizeof(pObj->inBufs);
    pObj->inBufs.numBufs = FILTER_2D_TI_BUFDESC_IN_TOTAL;
    pObj->inBufs.bufDesc  = pObj->inBufDescList;

    pObj->outBufs.size = sizeof(pObj->outBufs);
    pObj->outBufs.numBufs = FILTER_2D_TI_BUFDESC_OUT_TOTAL;
    pObj->outBufs.bufDesc = pObj->outBufDescList;

    pObj->inBufDescList[FILTER_2D_TI_BUFDESC_IN_IMAGEBUFFER] = &pObj->inBufDesc;
    pObj->outBufDescList[FILTER_2D_TI_BUFDESC_OUT_IMAGE_BUFFER] = &pObj->outBufDesc;

    pObj->inBufDesc.numPlanes                          = 2;
    pObj->inBufDesc.bufPlanes[0].frameROI.topLeft.x    = 0;
    pObj->inBufDesc.bufPlanes[0].frameROI.topLeft.y    = 0;
    pObj->inBufDesc.bufPlanes[0].width                 = inPitch[0];
    pObj->inBufDesc.bufPlanes[0].height                = inHeight;
    pObj->inBufDesc.bufPlanes[0].frameROI.width        = inWidth;
    pObj->inBufDesc.bufPlanes[0].frameROI.height       = inHeight;
    pObj->inBufDesc.bufPlanes[0].planeType             = 0;

    pObj->inBufDesc.bufPlanes[1].frameROI.topLeft.x    = 0;
    pObj->inBufDesc.bufPlanes[1].frameROI.topLeft.y    = 0;
    pObj->inBufDesc.bufPlanes[1].width                 = inPitch[1];
    pObj->inBufDesc.bufPlanes[1].height                = inHeight/2;
    pObj->inBufDesc.bufPlanes[1].frameROI.width        = inWidth;
    pObj->inBufDesc.bufPlanes[1].frameROI.height       = inHeight/2;
    pObj->inBufDesc.bufPlanes[1].planeType             = 1;

    pObj->outWidth  = inWidth;
    pObj->outHeight = inHeight;

    pObj->outPitch[0] = SystemUtils_align(inWidth, 128);
    pObj->outPitch[1] = pObj->outPitch[0];
    pObj->outBufSize[0] = pObj->outPitch[0]*(inHeight);
    pObj->outBufSize[1] = pObj->outPitch[1]*(inHeight)/2;

    pObj->outBufAddr[0] = NULL;
    pObj->outBufAddr[1] = NULL;

    pObj->tmpBufAddr = NULL;

    pObj->outBufDesc.numPlanes                          = 2;
    pObj->outBufDesc.bufPlanes[0].frameROI.topLeft.x    = 0;
    pObj->outBufDesc.bufPlanes[0].frameROI.topLeft.y    = 0;
    pObj->outBufDesc.bufPlanes[0].width                 = pObj->outPitch[0];
    pObj->outBufDesc.bufPlanes[0].height                = inHeight;
    pObj->outBufDesc.bufPlanes[0].frameROI.width        = inWidth;
    pObj->outBufDesc.bufPlanes[0].frameROI.height       = inHeight;
    pObj->outBufDesc.bufPlanes[0].planeType             = 0;
    pObj->outBufDesc.bufPlanes[0].buf                   = NULL;

    pObj->outBufDesc.bufPlanes[1].frameROI.topLeft.x    = 0;
    pObj->outBufDesc.bufPlanes[1].frameROI.topLeft.y    = 0;
    pObj->outBufDesc.bufPlanes[1].width                 = pObj->outPitch[1];
    pObj->outBufDesc.bufPlanes[1].height                = inHeight/2;
    pObj->outBufDesc.bufPlanes[1].frameROI.width        = inWidth;
    pObj->outBufDesc.bufPlanes[1].frameROI.height       = inHeight/2;
    pObj->outBufDesc.bufPlanes[1].planeType             = 1;
    pObj->outBufDesc.bufPlanes[1].buf                   = NULL;

    if(allocTmpBuf)
    {
        pObj->outBufAddr[0] =
            Utils_memAlloc(
                UTILS_HEAPID_DDR_CACHED_SR,
                pObj->outBufSize[0],
                32
            );
        UTILS_assert(pObj->outBufAddr[0]!=NULL);

        pObj->outBufAddr[1] =
            Utils_memAlloc(
                UTILS_HEAPID_DDR_CACHED_SR,
                pObj->outBufSize[1],
                32
            );
        UTILS_assert(pObj->outBufAddr[1]!=NULL);

        pObj->outBufDesc.bufPlanes[0].buf                   =
            (UInt8*)pObj->outBufAddr[0];

        pObj->outBufDesc.bufPlanes[1].buf                   =
            (UInt8*)pObj->outBufAddr[1];
    }

    if(pObj->enableClahe)
    {
        UInt32 claheInPitch;

        if(pObj->enableFilter2d)
        {
            pObj->tmpBufAddr =
                Utils_memAlloc(
                    UTILS_HEAPID_DDR_CACHED_SR,
                    pObj->outBufSize[0],
                    32
                );
            UTILS_assert(pObj->tmpBufAddr!=NULL);

            claheInPitch = pObj->outPitch[0];
        }
        else
        {
            claheInPitch = inPitch[0];
        }

        Alg_claheCreate(
            &pObj->claheObj,
            inWidth,
            inHeight,
            claheInPitch,
            pObj->outPitch[0]
            );
    }

    return 0;
}

Int32 Alg_filter2dProcess(
            Alg_Filter2dObj *pObj,
            Void *inBufAddr[],
            Void *outBufAddr[]
        )
{
    Int32 status = SYSTEM_LINK_STATUS_SOK;

    if(pObj->enableFilter2d)
    {
        pObj->inBufDesc.bufPlanes[0].buf = inBufAddr[0];
        pObj->inBufDesc.bufPlanes[1].buf = inBufAddr[1];

        if(pObj->enableClahe)
        {
            pObj->outBufDesc.bufPlanes[0].buf = pObj->tmpBufAddr;

            if(pObj->outBufAddr[1]==NULL)
            {
                pObj->outBufDesc.bufPlanes[1].buf = outBufAddr[1];
            }
            else
            {
                pObj->outBufDesc.bufPlanes[1].buf = pObj->outBufAddr[1];
            }
        }
        else
        {
            if(pObj->outBufAddr[0]==NULL && pObj->outBufAddr[1]==NULL)
            {
                pObj->outBufDesc.bufPlanes[0].buf = outBufAddr[0];
                pObj->outBufDesc.bufPlanes[1].buf = outBufAddr[1];
            }
            else
            {
                pObj->outBufDesc.bufPlanes[0].buf = pObj->outBufAddr[0];
                pObj->outBufDesc.bufPlanes[1].buf = pObj->outBufAddr[1];
            }
        }

        status = AlgIvision_process(
            pObj->handle,
            &pObj->inBufs,
            &pObj->outBufs,
            (IVISION_InArgs*)&pObj->inArgs,
            (IVISION_OutArgs*)&pObj->outArgs
        );
        UTILS_assert(status==0);

        pObj->inArgs.minVal = pObj->outArgs.minVal;
        pObj->inArgs.maxVal = pObj->outArgs.maxVal;
    }

    if(pObj->enableClahe)
    {
        Void *claheOutBufAddr;
        Void *claheInBufAddr;

        if(pObj->tmpBufAddr==NULL)
        {
            claheInBufAddr = inBufAddr[0];
        }
        else
        {
            claheInBufAddr = pObj->tmpBufAddr;
        }

        if(pObj->outBufAddr[0]==NULL)
        {
            claheOutBufAddr = outBufAddr[0];
        }
        else
        {
            claheOutBufAddr = pObj->outBufAddr[0];
        }

        Alg_claheProcess(
            &pObj->claheObj,
            claheInBufAddr,
            claheOutBufAddr
            );
    }

    return status;
}

Int32 Alg_filter2dDelete(Alg_Filter2dObj *pObj)
{
    Int32 status = SYSTEM_LINK_STATUS_SOK;

    if(pObj->handle)
    {
        status = AlgIvision_delete(pObj->handle);
        UTILS_assert(status==0);
    }

    if(pObj->outBufAddr[0])
    {
        status = Utils_memFree(
            UTILS_HEAPID_DDR_CACHED_SR,
            pObj->outBufAddr[0],
            pObj->outBufSize[0]
        );
        UTILS_assert(status==0);
    }

    if(pObj->outBufAddr[1])
    {
        status = Utils_memFree(
            UTILS_HEAPID_DDR_CACHED_SR,
            pObj->outBufAddr[1],
            pObj->outBufSize[1]
        );
    }

    if(pObj->tmpBufAddr)
    {
        status = Utils_memFree(
            UTILS_HEAPID_DDR_CACHED_SR,
            pObj->tmpBufAddr,
            pObj->outBufSize[0]
        );
        UTILS_assert(status==0);
    }

    if(pObj->enableClahe)
    {
        Alg_claheDelete(&pObj->claheObj);
    }

    return status;
}
