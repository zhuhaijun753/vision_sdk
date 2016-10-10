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
#include "fpComputeLink_priv.h"

Int32 Alg_fpComputeCreate(
            Alg_FPComputeObj *pObj,
            AlgorithmLink_ImgPyramidInfo * pImgPmdInfo,
            UInt32 *outBufSize
      )
{
    Int32 status;
    UInt16 i;

    memset(pObj, 0, sizeof(*pObj));

    pObj->prms.visionParams.algParams.size =
        sizeof(pObj->prms);

    pObj->prms.visionParams.cacheWriteBack = NULL;

    pObj->prms.imgFrameWidth    = pImgPmdInfo->frmDesc[0].orgWidth;
    pObj->prms.imgFrameHeight   = pImgPmdInfo->frmDesc[0].orgHeight;
    pObj->prms.leftPadPels      = 0;
    pObj->prms.topPadPels       = 0;
    pObj->prms.cellSize         = 4;
    pObj->prms.blockSize        = 4;
    pObj->prms.blockOverlap     = 0;
    pObj->prms.sreachStep       = pImgPmdInfo->sreachStep;
    pObj->prms.maxNumScales     = pImgPmdInfo->numScales;
    pObj->prms.numBins          = 6;
    pObj->prms.gradientMethod   = 0;
    pObj->prms.enableCellSum    = 0;
    pObj->prms.scaleRatioQ12    = 0; /* NOT USED */
    pObj->prms.additionalPlaneFLag = (1<<0)|(1<<1)|(1<<2);
    pObj->prms.outPutBufSize    = 0;
    pObj->prms.outFormat        = 0;

    for(i = 0; i<pImgPmdInfo->numScales; i++)
    {
        pObj->prms.scaleParams[i].orgWidth   = pImgPmdInfo->frmDesc[i].orgWidth;
        pObj->prms.scaleParams[i].orgHeight  = pImgPmdInfo->frmDesc[i].orgHeight;
        pObj->prms.scaleParams[i].width      = pImgPmdInfo->frmDesc[i].width;
        pObj->prms.scaleParams[i].height     = pImgPmdInfo->frmDesc[i].height;
        pObj->prms.scaleParams[i].x          = pImgPmdInfo->frmDesc[i].startX;
        pObj->prms.scaleParams[i].y          = pImgPmdInfo->frmDesc[i].startY;

    }

    pObj->handle = AlgIvision_create(
                &PD_FEATURE_PLANE_COMPUTATION_TI_VISION_FXNS,
                (IALG_Params*)&pObj->prms
                );
    UTILS_assert(pObj->handle!=NULL);

    pObj->inArgs.iVisionInArgs.size =
        sizeof(pObj->inArgs);

    pObj->inArgs.numScales = pImgPmdInfo->numScales;

    pObj->outArgs.iVisionOutArgs.size =
        sizeof(pObj->outArgs);

    pObj->inBufs.size = sizeof(pObj->inBufs);
    pObj->inBufs.numBufs = pObj->prms.maxNumScales;
    pObj->inBufs.bufDesc  = pObj->inBufDescList;

    pObj->outBufs.size = sizeof(pObj->outBufs);
    pObj->outBufs.numBufs = PD_FEATURE_PLANE_COMPUTATION_BUFDESC_OUT_TOTAL;
    pObj->outBufs.bufDesc = pObj->outBufDescList;

    for(i=0; i<pObj->prms.maxNumScales; i++)
    {
        pObj->inBufDescList[i] = &pObj->inBufDesc[i];

        pObj->inBufDesc[i].numPlanes                          = 2;
        pObj->inBufDesc[i].bufPlanes[0].width                 = pImgPmdInfo->frmDesc[i].pitch[0];
        pObj->inBufDesc[i].bufPlanes[0].height                = pImgPmdInfo->frmDesc[i].height;
        pObj->inBufDesc[i].bufPlanes[0].frameROI.width        = pImgPmdInfo->frmDesc[i].width;
        pObj->inBufDesc[i].bufPlanes[0].frameROI.height       = pImgPmdInfo->frmDesc[i].height;
        pObj->inBufDesc[i].bufPlanes[0].planeType             = 0; //Luma Y
        pObj->inBufDesc[i].bufPlanes[0].buf                   = NULL; //pImgPmdInfo->outBufAddr[i][0];

        pObj->inBufDesc[i].bufPlanes[1].width                 = pImgPmdInfo->frmDesc[i].pitch[1];
        pObj->inBufDesc[i].bufPlanes[1].height                = pImgPmdInfo->frmDesc[i].height/2;
        pObj->inBufDesc[i].bufPlanes[1].frameROI.width        = pImgPmdInfo->frmDesc[i].width;
        pObj->inBufDesc[i].bufPlanes[1].frameROI.height       = pImgPmdInfo->frmDesc[i].height/2;
        pObj->inBufDesc[i].bufPlanes[1].planeType             = 1; //C
        pObj->inBufDesc[i].bufPlanes[1].buf                   = NULL; //pImgPmdInfo->outBufAddr[i][1];

        if(i%pImgPmdInfo->scaleSteps)
        {
            pObj->inBufDesc[i].bufPlanes[0].frameROI.topLeft.x    = 0;
            pObj->inBufDesc[i].bufPlanes[0].frameROI.topLeft.y    = 0;
            pObj->inBufDesc[i].bufPlanes[1].frameROI.topLeft.x    = 0;
            pObj->inBufDesc[i].bufPlanes[1].frameROI.topLeft.y    = 0;
        }
        else
        {
            pObj->inBufDesc[i].bufPlanes[0].frameROI.topLeft.x    = pImgPmdInfo->frmDesc[i].startX;
            pObj->inBufDesc[i].bufPlanes[0].frameROI.topLeft.y    = pImgPmdInfo->frmDesc[i].startY;
            pObj->inBufDesc[i].bufPlanes[1].frameROI.topLeft.x    = pImgPmdInfo->frmDesc[i].startX;
            pObj->inBufDesc[i].bufPlanes[1].frameROI.topLeft.y    = pImgPmdInfo->frmDesc[i].startY/2;
        }
    }

    status = AlgIvision_control(pObj->handle,
                                TI_PD_CONTROL_GET_OUTPUT_BUF_SIZE,
                                (IALG_Params *)&pObj->prms,
                                (IALG_Params *)&pObj->prms);
    UTILS_assert(status==0);

    *outBufSize = pObj->prms.outPutBufSize;

    pObj->outBufDescList[PD_FEATURE_PLANE_COMPUTATION_BUFDESC_OUT_FEATURE_PLANES_BUFFER] = &pObj->outBufDesc;

    pObj->outBufDesc.numPlanes                          = 1;
    pObj->outBufDesc.bufPlanes[0].frameROI.topLeft.x    = 0;
    pObj->outBufDesc.bufPlanes[0].frameROI.topLeft.y    = 0;
    pObj->outBufDesc.bufPlanes[0].width                 = pObj->prms.outPutBufSize;
    pObj->outBufDesc.bufPlanes[0].height                = 1;
    pObj->outBufDesc.bufPlanes[0].frameROI.width        = pObj->prms.outPutBufSize;
    pObj->outBufDesc.bufPlanes[0].frameROI.height       = 1;
    pObj->outBufDesc.bufPlanes[0].planeType             = 0;
    pObj->outBufDesc.bufPlanes[0].buf                   = NULL;

    return 0;
}

Int32 Alg_fpComputeProcess(
            Alg_FPComputeObj *pObj,
            Void *inBufAddr,
            Void *outBufAddr
      )
{

    Int32 status = SYSTEM_LINK_STATUS_SOK;
    Int32 i;
    AlgorithmLink_ImgPyramidFrameDesc *pInFrmDesc = (AlgorithmLink_ImgPyramidFrameDesc *)inBufAddr;

    for(i = 0; i < pObj->prms.maxNumScales; i++)
    {
        pObj->inBufDesc[i].bufPlanes[0].buf = pInFrmDesc[i].bufAddr[0];
        pObj->inBufDesc[i].bufPlanes[1].buf = pInFrmDesc[i].bufAddr[1];
    }

    pObj->outBufDesc.bufPlanes[0].buf = outBufAddr;

    status = AlgIvision_process(
                pObj->handle,
                &pObj->inBufs,
                &pObj->outBufs,
                (IVISION_InArgs*)&pObj->inArgs,
                (IVISION_OutArgs*)&pObj->outArgs
            );
    UTILS_assert(status==0);

    return status;
}

Int32 Alg_fpComputeDelete(
            Alg_FPComputeObj *pObj
      )
{
    Int32 status = SYSTEM_LINK_STATUS_SOK;

    status = AlgIvision_delete(pObj->handle);
    UTILS_assert(status==0);

    return status;
}
