/*
 *******************************************************************************
 *
 * Copyright (C) 2015 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */

/**
 *******************************************************************************
 * \file chains_lvdsVipMultiCam_sfm4ch.c
 *
 *
 * \version 0.0 (Dec 2015) : [MM] First version
 *
 *******************************************************************************
*/

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */
#include "chains_lvdsVipMultiCam_3d_perception_priv.h"
#include <linux/examples/tda2xx/include/chains.h>
#include <linux/examples/common/chains_common.h>
#include <linux/src/system/system_gbm_allocator.h>


//SHARED PARAMETERS
#define SFM_NUM_CAMS            (4) //cannot be changed freely!
#define MAX_NUM_POINTS_PER_CAM  (900)

//CAMERA/CAPTURE
#define CAPTURE_SENSOR_WIDTH    (1280)
#define CAPTURE_SENSOR_HEIGHT   (720)

//SPARSE OPTICAL FLOW
#define SOF_NUM_OUT_BUF         (16)
#define SOF_ALG_WIDTH           (CAPTURE_SENSOR_WIDTH)
#define SOF_ALG_HEIGHT          (CAPTURE_SENSOR_HEIGHT)

//DISPLAY
#define DISPLAY_WIDTH   (1920)
#define DISPLAY_HEIGHT  (1080)

//Sync
#define SYNC_TIGHT_DELTA_IN_MSEC            (16)
#define SYNC_TIGHT_DROP_THRESHOLD_IN_MSEC   (33)
#define SYNC_LOOSE_DELTA_IN_MSEC            (0x7FFFFFFF)
#define SYNC_LOOSE_DROP_THRESHOLD_IN_MSEC   (0x7FFFFFFF)

/**
 *******************************************************************************
 *
 *  \brief  SingleCameraViewObject
 *
 *        This structure contains all the LinksId's and create Params.
 *        The same is passed to all create, start, stop functions.
 *
 *******************************************************************************
*/
typedef struct {
    /**< Link Id's and device IDs to use for this use-case */
    chains_lvdsVipMultiCam_3d_perceptionObj ucObj;

    UInt32  appCtrlLinkId;

    Chains_Ctrl *chainsCfg;

    Chain_Common_SRV_CalibParams gaCalibPrm;

} Chains_lvdsVipMultiCam_3dPerceptionAppObj;

static struct control_srv_egl_ctx chainsEglParams = {
   .get_egl_native_display = gbm_allocator_get_native_display,
   .get_egl_native_buffer = gbm_allocator_get_native_buffer,
   .destroy_egl_native_buffer = gbm_allocator_destroy_native_buffer,
};

/**
 *******************************************************************************
 *
 * \brief   Set SGX3DSRV Link Parameters
 *
 *          It is called in Create function.

 *
 * \param   pPrm    [IN]    IpcLink_CreateParams
 *
 *******************************************************************************
*/
static Void chains_lvdsVipMultiCam_3d_perception_SetSgx3DsfmLinkPrms (
                                  Sgx3DsfmLink_CreateParams *prms)
{
    prms->inFramesHeight = CAPTURE_SENSOR_HEIGHT;
    prms->inFramesWidth = CAPTURE_SENSOR_WIDTH;
    prms->numCams = SFM_NUM_CAMS;

    prms->numInQue = SGX3DSFM_LINK_IPQID_MAXIPQ;
    prms->inBufType[SGX3DSFM_LINK_IPQID_MULTIVIEW] = SYSTEM_BUFFER_TYPE_VIDEO_FRAME_CONTAINER;
    prms->inBufType[SGX3DSFM_LINK_IPQID_EGOPOSE]   = SYSTEM_BUFFER_TYPE_METADATA;
    prms->inBufType[SGX3DSFM_LINK_IPQID_BOXES]     = SYSTEM_BUFFER_TYPE_METADATA;
    prms->bEglInfoInCreate = TRUE;
    prms->EglInfo = (void *)&chainsEglParams;
}

/**
 *******************************************************************************
 *
 * \brief   Set Algorithm related parameters
 *
 *******************************************************************************
*/
Void chains_lvdsVipMultiCam_3d_perception_SetSparseOpticalFlowPrm(
                    AlgorithmLink_SparseOpticalFlowCreateParams *pAlgPrm,
                    UInt32 startX,
                    UInt32 startY,
                    UInt32 width,
                    UInt32 height,
                    UInt32 syncChannel
                    )
{
    UInt8 i;

    pAlgPrm->numOutBuffers = SOF_NUM_OUT_BUF;
    pAlgPrm->enableRoi = FALSE;

    pAlgPrm->imgFrameStartX = startX;
    pAlgPrm->imgFrameStartY = startY;
    pAlgPrm->imgFrameWidth  = width;
    pAlgPrm->imgFrameHeight  = height;

    pAlgPrm->trackErrThr = 500;
    pAlgPrm->trackMinFlowQ4 = 0;
    pAlgPrm->trackNmsWinSize = 5;

    pAlgPrm->numLevels = 5;

    pAlgPrm->keyPointDetectInterval = 10;
    pAlgPrm->keyPointDetectMethod = 1;

    pAlgPrm->maxNumKeyPoints = MAX_NUM_POINTS_PER_CAM;
    pAlgPrm->maxPrevTrackPoints = MAX_NUM_POINTS_PER_CAM;

    pAlgPrm->fast9Threshold = 100;
    pAlgPrm->scoreMethod = 0;
    pAlgPrm->harrisScaling = 1310;
    pAlgPrm->nmsThreshold = 420000000;
    pAlgPrm->harrisScoreMethod = 0;
    pAlgPrm->harrisWindowSize = 0;
    pAlgPrm->suppressionMethod = 2;

    pAlgPrm->trackOffsetMethod = 0;

    pAlgPrm->maxItersLK[0]  = 20;
    pAlgPrm->maxItersLK[1]  = 20;
    pAlgPrm->maxItersLK[2]  = 10;
    pAlgPrm->maxItersLK[3]  = 10;
    pAlgPrm->maxItersLK[4]  = 10;
    pAlgPrm->maxItersLK[5]  = 10;
    pAlgPrm->maxItersLK[6]  = 10;
    pAlgPrm->maxItersLK[7]  = 10;

    for(i=0; i<PYRAMID_LK_TRACKER_TI_MAXLEVELS;i++)
    {
         pAlgPrm->minErrValue[i] = 64;
         pAlgPrm->searchRange[i] = 12;
    }

    pAlgPrm->compositeBufferInput = syncChannel;

}

/**
 *******************************************************************************
 * *
 * \brief   Set link Parameters
 *
 *          It is called in Create function of the auto generated use-case file.
 *
 * \param pUcObj    [IN] Auto-generated usecase object
 * \param appObj    [IN] Application specific object
 *
 *******************************************************************************
*/
Void chains_lvdsVipMultiCam_3d_perception_SetAppPrms(
            chains_lvdsVipMultiCam_3d_perceptionObj *pUcObj, Void *appObj)
{

    UInt32 displayWidth,displayHeight;
    Chains_lvdsVipMultiCam_3dPerceptionAppObj *pObj
            = (Chains_lvdsVipMultiCam_3dPerceptionAppObj*)appObj;

    ChainsCommon_MultiCam_SetCapturePrms(
                    &pUcObj->CapturePrm,
                    pObj->chainsCfg->numLvdsCh);

    {
        UInt32 i;
        CaptureLink_VipInstParams *pInstPrm;
        for (i=0; i<SYSTEM_CAPTURE_VIP_INST_MAX; i++)
        {
            pInstPrm = &pUcObj->CapturePrm.vipInst[i];
            pInstPrm->numBufs = 5;
        }
    }

    /*Graphics*/
    pUcObj->GrpxSrcPrm.grpxBufInfo.dataFormat  = SYSTEM_DF_BGR16_565;
    pUcObj->GrpxSrcPrm.grpxBufInfo.height   = DISPLAY_HEIGHT;
    pUcObj->GrpxSrcPrm.grpxBufInfo.width    = DISPLAY_WIDTH;

    pUcObj->GrpxSrcPrm.surroundViewStandaloneLayoutEnable = FALSE;
    pUcObj->GrpxSrcPrm.surroundViewEdgeDetectLayoutEnable = FALSE;
    pUcObj->GrpxSrcPrm.surroundViewDOFLayoutEnable = FALSE;
    pUcObj->GrpxSrcPrm.surroundViewPdTsrLayoutEnable = FALSE;
    pUcObj->GrpxSrcPrm.surroundViewLdLayoutEnable = FALSE;
    pUcObj->GrpxSrcPrm.sfmLayoutEnable  = TRUE;
    pUcObj->GrpxSrcPrm.statsDisplayEnable = TRUE;
    pUcObj->GrpxSrcPrm.enableJeepOverlay = FALSE;


    /*Sync before SOF*/
    pUcObj->Sync_1Prm.syncDelta = SYNC_TIGHT_DELTA_IN_MSEC;
    pUcObj->Sync_1Prm.syncThreshold = SYNC_TIGHT_DROP_THRESHOLD_IN_MSEC;

    /*Sync after SOF*/
    pUcObj->Sync_2Prm.syncDelta = SYNC_LOOSE_DELTA_IN_MSEC;
    pUcObj->Sync_2Prm.syncThreshold = SYNC_LOOSE_DROP_THRESHOLD_IN_MSEC;

    /*SGX link*/
    chains_lvdsVipMultiCam_3d_perception_SetSgx3DsfmLinkPrms(&pUcObj->Sgx3DsfmPrm);

    /*****************************
     * ALGORITHMS
     ****************************/
    //SOF 0
    chains_lvdsVipMultiCam_3d_perception_SetSparseOpticalFlowPrm(
        &pUcObj->Alg_SparseOpticalFlow_0Prm, 0, 0, SOF_ALG_WIDTH, SOF_ALG_HEIGHT, 0);

    //SOF 1
    chains_lvdsVipMultiCam_3d_perception_SetSparseOpticalFlowPrm(
        &pUcObj->Alg_SparseOpticalFlow_1Prm, 0, 0, SOF_ALG_WIDTH, SOF_ALG_HEIGHT, 1);

    //SOF 2
    chains_lvdsVipMultiCam_3d_perception_SetSparseOpticalFlowPrm(
        &pUcObj->Alg_SparseOpticalFlow_2Prm, 0, 0, SOF_ALG_WIDTH, SOF_ALG_HEIGHT, 2);

    //SOF 3
    chains_lvdsVipMultiCam_3d_perception_SetSparseOpticalFlowPrm(
        &pUcObj->Alg_SparseOpticalFlow_3Prm, 0, 0, SOF_ALG_WIDTH, SOF_ALG_HEIGHT, 3);


    //SFM
    ChainsCommon_StructureFromMotion_SetParams(
            &pUcObj->Alg_SfmMainPrm.algCreateParams,
            &pUcObj->Alg_SfmLinearTwoViewPrm.algCreateParams,
            &pUcObj->Alg_SfmMapPrm.algCreateParams,
            SFM_NUM_CAMS,
            MAX_NUM_POINTS_PER_CAM);

    //Sfm Calibration from File/QSPI
    pUcObj->Alg_SfmMainPrm.calmatAddr = (Int32 *)pObj->gaCalibPrm.autoPersMatAddr + GA_MAGIC_PATTERN_SIZE_IN_BYTES/4 ;

        ChainsCommon_GetDisplayWidthHeight(
        pObj->chainsCfg->displayType,
        &displayWidth,
        &displayHeight
        );

    pUcObj->Display_GrpxPrm.rtParams.tarWidth       = displayWidth;
    pUcObj->Display_GrpxPrm.rtParams.tarHeight      = displayHeight;
    pUcObj->Display_GrpxPrm.rtParams.posX           = 0;
    pUcObj->Display_GrpxPrm.rtParams.posY           = 0;
    pUcObj->Display_GrpxPrm.displayId               = DISPLAY_LINK_INST_DSS_GFX1;

    ChainsCommon_SetDisplayPrms(&pUcObj->Display_M4Prm,
                                NULL,
                                pObj->chainsCfg->displayType,
                                DISPLAY_WIDTH,
                                DISPLAY_HEIGHT
                               );

    ChainsCommon_StartDisplayCtrl(
        pObj->chainsCfg->displayType,
        DISPLAY_WIDTH,
        DISPLAY_HEIGHT
        );
}

/**
 *******************************************************************************
 *
 * \brief   Start the capture display Links
 *
 *          Function sends a control command to capture and display link to
 *          to Start all the required links . Links are started in reverce
 *          order as information of next link is required to connect.
 *          System_linkStart is called with LinkId to start the links.
 *
 * \param   pObj  [IN] Chains_lvdsVipMultiCam_3dPerceptionAppObj
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
*/
void chains_lvdsVipMultiCam_3d_perception_StartApp(Chains_lvdsVipMultiCam_3dPerceptionAppObj *pObj)
{
    ChainsCommon_statCollectorReset();
    ChainsCommon_memPrintHeapStatus();
    chains_lvdsVipMultiCam_3d_perception_Start(&pObj->ucObj);

}

/**
 *******************************************************************************
 *
 * \brief   Delete the capture display Links
 *
 *          Function sends a control command to capture and display link to
 *          to delete all the prior created links
 *          System_linkDelete is called with LinkId to delete the links.
 *
 * \param   pObj   [IN]   Chains_lvdsVipMultiCam_3dPerceptionAppObj
 *
 *******************************************************************************
*/
void chains_lvdsVipMultiCam_3d_perception_StopApp(Chains_lvdsVipMultiCam_3dPerceptionAppObj *pObj)
{
     chains_lvdsVipMultiCam_3d_perception_Stop(&pObj->ucObj);

     chains_lvdsVipMultiCam_3d_perception_Delete(&pObj->ucObj);

     ChainsCommon_StopDisplayCtrl();

     ChainsCommon_prfLoadCalcEnable(FALSE, FALSE, FALSE);

}

/**
 *******************************************************************************
 *
 * \brief   Single Channel Capture Display usecase function
 *
 *          This functions executes the create, start functions
 *
 *          Further in a while loop displays run time menu and waits
 *          for user inputs to print the statistics or to end the demo.
 *
 *          Once the user inputs end of demo stop and delete
 *          functions are executed.
 *
 * \param   chainsCfg       [IN]   Chains_Ctrl
 *
 *******************************************************************************
*/
Void chains_lvdsVipMultiCam_3d_perception(Chains_Ctrl *chainsCfg)
{
    char ch;
    UInt32 done = FALSE;
    Chains_lvdsVipMultiCam_3dPerceptionAppObj chainsObj;
    Chain_Common_SRV_CalibParams * gaCalibPrm = &chainsObj.gaCalibPrm;

    chainsObj.chainsCfg = chainsCfg;

    chainsObj.chainsCfg->numLvdsCh = 4;

    done = FALSE;

    ChainsCommon_SurroundView_InitCalibration(gaCalibPrm, TRUE, FALSE, chainsObj.chainsCfg->svOutputMode);

    chains_lvdsVipMultiCam_3d_perception_Create(&chainsObj.ucObj, &chainsObj);

    chains_lvdsVipMultiCam_3d_perception_StartApp(&chainsObj);

    ChainsCommon_prfLoadCalcEnable(TRUE, FALSE, FALSE);

    while(!done)
    {
        ch = Chains_menuRunTime();

        switch(ch)
        {
            case '0':
                done = TRUE;
                break;
            case 'p':
            case 'P':
                ChainsCommon_prfCpuLoadPrint();
                ChainsCommon_statCollectorPrint();
                chains_lvdsVipMultiCam_3d_perception_printStatistics(&chainsObj.ucObj);
                chains_lvdsVipMultiCam_3d_perception_printBufferStatistics(&chainsObj.ucObj);
                break;
            default:
                Vps_printf("\nUnsupported option '%c'. Please try again\n", ch);
                break;
        }
    }

    chains_lvdsVipMultiCam_3d_perception_StopApp(&chainsObj);

    ChainsCommon_SurroundView_StopCalibration(gaCalibPrm, chainsObj.chainsCfg->svOutputMode);

}


