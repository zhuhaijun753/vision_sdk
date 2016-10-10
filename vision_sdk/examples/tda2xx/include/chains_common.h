/*
 *******************************************************************************
 *
 * Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */

/*
 *******************************************************************************
 *
 * \file chains_common.h
 *
 * \brief This file contains common utility functions used by all use-cases
 *
 *******************************************************************************
 */

#ifndef _CHAINS_COMMON_H_
#define _CHAINS_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *  Includes
 *******************************************************************************
 */
#include <examples/tda2xx/include/chains.h>
#include <src/utils_common/include/utils_temperature.h>
#include "include/link_api/system_common.h"

/*******************************************************************************
 *  Defines
 *******************************************************************************
 */

#define CHAINS_COMMON_FC_ANALYTICS_LAYOUT_VERT_STRIP        (0)
#define CHAINS_COMMON_FC_ANALYTICS_LAYOUT_HORZ_STRIP        (1)
#define CHAINS_COMMON_FC_ANALYTICS_LAYOUT_HORZ_AND_VERT     (2)

/**
 *******************************************************************************
 * \brief Macro to identify HOT Temperature Configuration
 *******************************************************************************
 */
#define CHAINS_HOT_TEMPERATURE (0U)
/**
 *******************************************************************************
 * \brief Macro to identify COLD Temperature Configuration
 *******************************************************************************
 */
#define CHAINS_COLD_TEMPERATURE (1U)

/*******************************************************************************
 *  Function's
 *******************************************************************************
 */

Void ChainsCommon_Init();

Void ChainsCommon_DeInit();

Void ChainsCommon_Iss_Init();
Void ChainsCommon_Iss_DeInit();

Void ChainsCommon_SetAvbRxDecodePrm(
        AvbRxLink_CreateParams *pAvbPrm,
        DecLink_CreateParams *pDecPrm,
        UInt32 maxWidth,
        UInt32 maxHeight,
        UInt32 numCh);

Void ChainsCommon_SetDisplayPrms(
        DisplayLink_CreateParams *pPrm_Video,
        DisplayLink_CreateParams *pPrm_Grpx,
        Chains_DisplayType displayType,
        UInt32 displayWidth,
        UInt32 displayHeight
);

Int32 ChainsCommon_StartDisplayCtrl(UInt32 displayType,
        UInt32 displayWidth,
        UInt32 displayHeight
);

Int32 ChainsCommon_DualDisplay_StartDisplayCtrl (
        Chains_DisplayType lcdType,UInt32 displayLCDWidth, UInt32 displayLCDHeight);

Int32 ChainsCommon_StopDisplayCtrl();

Void ChainsCommon_InitCaptureDevice(Chains_CaptureSrc captureSrc);

Int32 ChainsCommon_StartCaptureDevice(Chains_CaptureSrc captureSrc,
        UInt32 captureOutWidth,
        UInt32 captureOutHeight);

Int32 ChainsCommon_MultiCam_StartCaptureDevice(Chains_CaptureSrc captureSrc,
        UInt32 portId[],
        UInt32 numLvdsCh
);

Int32 ChainsCommon_StopCaptureDevice(Chains_CaptureSrc captureSrc);

Int32 ChainsCommon_StartDisplayDevice(UInt32 displayType);
Int32 ChainsCommon_StopDisplayDevice(UInt32 displayType);

Void ChainsCommon_SetSystemL3DmmPri();

Void ChainsCommon_SingleCam_SetCapturePrms(
        CaptureLink_CreateParams *pPrm,
        UInt32 captureInWidth,
        UInt32 captureInHeight,
        UInt32 captureOutWidth,
        UInt32 captureOutHeight,
        Chains_CaptureSrc captureSrc
);

Void ChainsCommon_StereoCam_SetCapturePrms(
        CaptureLink_CreateParams *pPrm,
        UInt32 captureInWidth,
        UInt32 captureInHeight,
        UInt32 captureOutWidth,
        UInt32 captureOutHeight,
        Chains_CaptureSrc captureSrc
);

Void ChainsCommon_MultiCam_SetCapturePrms(
        CaptureLink_CreateParams *pPrm,
        UInt32 captureInWidth,
        UInt32 captureInHeight,
        UInt32 portId[],
        UInt32 numLvdsCh
);

Void ChainsCommon_PrintStatistics();

Void ChainsCommon_PrintPMStatistics();

Void ChainsCommon_SurroundView_CalibInit(Bool startWithCalibration);
char ChainsCommon_SurroundView_MenuCalibration();
Void ChainsCommon_SurroundView_CalibDeInit();

Void ChainsCommon_SurroundView_SetParams(
        CaptureLink_CreateParams *pVipCapture,
        AvbRxLink_CreateParams *pAvbRxPrm,
        DecLink_CreateParams *pDecPrm,
        SelectLink_CreateParams *pCaptureSelect,
        VpeLink_CreateParams *pSvOrgVpe,
        VpeLink_CreateParams *pFrontCamVpe,
        SyncLink_CreateParams *pSvSync,
        SyncLink_CreateParams *pSvOrgSync,
        SyncLink_CreateParams *pFrontCamSync,
        AlgorithmLink_SynthesisCreateParams *pSynthPrm,
        AlgorithmLink_GAlignCreateParams *pGAlignPrm,
        AlgorithmLink_PAlignCreateParams *pPAlignPrm,
        AlgorithmLink_EdgeDetectionCreateParams *pEdgeDetect,
        AlgorithmLink_DenseOptFlowCreateParams *pDof,
        AlgorithmLink_VectorToImageCreateParams *pDofVectorToImage,
        AlgorithmLink_DmaSwMsCreateParams *pSvOrgDmaSwMs,
        AlgorithmLink_DmaSwMsCreateParams *pFrontCamDmaSwMs,
        GrpxSrcLink_CreateParams *pGrpxSrc,
        DisplayLink_CreateParams *pSvDisplay,
        DisplayLink_CreateParams *pSvOrgDisplay,
        DisplayLink_CreateParams *pFrontCamDisplay,
        DisplayLink_CreateParams *pGrpxDisplay,
        Chains_DisplayType displayType,
        UInt32 numLvdsCh,
        AlgorithmLink_SrvOutputModes svOutputMode,
        VpeLink_CreateParams *VPE_algPdPrm,
        AlgorithmLink_FeaturePlaneComputationCreateParams *Alg_FeaturePlaneComputationPrm,
        AlgorithmLink_ObjectDetectionCreateParams *Alg_ObjectDetectionPrm,
        SyncLink_CreateParams *Sync_algPdPrm,
        AlgorithmLink_ObjectDrawCreateParams *Alg_ObjectDrawPrm,
        SyncLink_CreateParams *Sync_algLdPrm,
        AlgorithmLink_LaneDetectCreateParams *Alg_LaneDetectPrm,
        AlgorithmLink_ObjectDrawCreateParams *Alg_LaneDetectDrawPrm,
        Bool enableCarOverlayInAlg
);

Void ChainsCommon_GetDisplayWidthHeight(
        Chains_DisplayType displayType,
        UInt32 *displayWidth,
        UInt32 *displayHeight
);



Void ChainsCommon_SetGrpxSrcPrms(
        GrpxSrcLink_CreateParams *pPrm,
        UInt32 displayWidth,
        UInt32 displayHeight
);


Void ChainsCommon_Analytics_SetPrms(
        CaptureLink_CreateParams                *pCapturePrm,
        NullSrcLink_CreateParams                *pNullSrcPrm,
        DecLink_CreateParams                    *pDecPrm,
        AlgorithmLink_LaneDetectCreateParams    *pAlg_LaneDetectPrm,
        AlgorithmLink_ObjectDrawCreateParams *pAlg_LaneDetectDrawPrm,
        SyncLink_CreateParams                   *pSync_ldPrm,
        AlgorithmLink_FeaturePlaneComputationCreateParams *pAlg_FeaturePlaneComputationPrm,
        AlgorithmLink_ObjectDetectionCreateParams *pAlg_ObjectDetectionPrm,
        AlgorithmLink_ObjectDrawCreateParams    *pAlg_ObjectDrawPrm,
        SyncLink_CreateParams                   *pSync_pd_tsrPrm,
        AlgorithmLink_SparseOpticalFlowCreateParams *pSofAlgPrm,
        AlgorithmLink_ObjectDrawCreateParams *pSofDrawPrm,
        SyncLink_CreateParams                   *pSync_sofPrm,
        SyncLink_CreateParams                   *pSync_algPrm,
        AlgorithmLink_DmaSwMsCreateParams       *pAlg_DmaSwMsPrm,
        DisplayLink_CreateParams                *pDisplay_algPrm,
        GrpxSrcLink_CreateParams                *pGrpxSrcPrm,
        DisplayLink_CreateParams                *pDisplay_GrpxPrm,
        Chains_CaptureSrc captureSrc,
        Chains_DisplayType displayType,
        UInt32 Alg_FeaturePlaneComputationLinkID,
        UInt32 Alg_ObjectDetectionLinkID,
        UInt32 Alg_ObjectDrawLinkID,
        UInt32 dmaSwMsLayoutType
);

Void ChainsCommon_SetNetworkRxPrms(
        NullSrcLink_CreateParams *pPrm,
        DecLink_CreateParams *pDecPrm,
        UInt32 maxWidth,
        UInt32 maxHeight,
        UInt32 numCh,
        UInt32 fps /* in msecs */
);

Void ChainsCommon_SetFileRxPrms(
        NullSrcLink_CreateParams *pPrm,
        DecLink_CreateParams *pDecPrm,
        UInt32 maxWidth,
        UInt32 maxHeight,
        UInt32 numCh,
        UInt32 fps /* in msecs */
);


Void ChainsCommon_Analytics_Start(Bool useVipCapture);

Void ChainsCommon_Analytics_Stop(Bool useVipCapture);

/* Get the Sensor Create Parameters */
VidSensor_CreateParams *ChainsCommon_GetSensorCreateParams();

Void ChainsCommon_SurroundView_SetSynthParams(
        AlgorithmLink_SynthesisCreateParams *pPrm,
        UInt16 svInWidth,
        UInt16 svInHeight,
        UInt16 svOutWidth,
        UInt16 svOutHeight,
        UInt16 svNumViews,
        Int16  svCarBoxWidth,
        Int16  svCarBoxHeight,
        AlgorithmLink_SrvOutputModes svOutputMode,
        Bool enableCarOverlayInAlg);

Void ChainsCommon_SurroundView_SetGAlignParams(
        AlgorithmLink_GAlignCreateParams *pPrm,
        UInt16 svInWidth,
        UInt16 svInHeight,
        UInt16 svOutWidth,
        UInt16 svOutHeight,
        UInt16 svNumViews,
        Int16  svCarBoxWidth,
        Int16  svCarBoxHeight,
        AlgorithmLink_SrvOutputModes svOutputMode);

Void ChainsCommon_SurroundView_SetGAlign3DParams(
                            AlgorithmLink_GAlign3DCreateParams *pPrm,
                            UInt16 svOutWidth,
                            UInt16 svOutHeight,
                            AlgorithmLink_SrvOutputModes svOutputMode);

Void ChainsCommon_SurroundView_SetPAlignParams(
        AlgorithmLink_PAlignCreateParams *pPrm,
        UInt16 svInWidth,
        UInt16 svInHeight,
        UInt16 svOutWidth,
        UInt16 svOutHeight,
        UInt16 svNumViews,
        Int16  svCarBoxWidth,
        Int16  svCarBoxHeight,
        AlgorithmLink_SrvOutputModes svOutputMode
);

Void ChainsCommon_SurroundView_SetACDetectParams(AlgorithmLink_ACDetectCreateParams *pPrm);

Void ChainsCommon_SurroundView_SetPEstimateParams(AlgorithmLink_PEstimateCreateParams *pPrm);

Void ChainsCommon_Stereo_SetPrms(
        CaptureLink_CreateParams *pCapturePrm,
        VpeLink_CreateParams *pVPE_softispPrm,
        VpeLink_CreateParams *pVPE_orgdispPrm,
        VpeLink_CreateParams *pVPE_disparityPrm,
        AlgorithmLink_SoftIspCreateParams *pAlg_SoftIspPrm,
        AlgorithmLink_RemapMergeCreateParams *pAlg_RemapMergePrm,
        AlgorithmLink_CensusCreateParams *pAlg_CensusPrm,
        AlgorithmLink_DisparityHamDistCreateParams *pAlg_DisparityHamDistPrm,
        AlgorithmLink_StereoPostProcessCreateParams *pAlg_StereoPostProcessPrm,
        SyncLink_CreateParams *pSync_orgPrm,
        SyncLink_CreateParams *pSync_dispPrm,
        AlgorithmLink_DmaSwMsCreateParams *pAlg_DmaSwMsPrm,
        GrpxSrcLink_CreateParams *pGrpxSrcPrm,
        DisplayLink_CreateParams *pDisplay_orgPrm,
        DisplayLink_CreateParams *pDisplay_GrpxPrm,
        DisplayLink_CreateParams *pDisplay_disparityPrm,
        Chains_CaptureSrc captureSrc,
        Chains_DisplayType displayType,
        UInt32 captureSensorWidth,
        UInt32 captureSensorHeight,
        UInt32 remapWidth,
        UInt32 remapHeight,
        UInt32 stereoOutputWidth,
        UInt32 stereoOutputHeight,
        UInt32 disparityWinStartX,
        UInt32 disparityWinStartY,
        UInt32 disparityWinWidth,
        UInt32 disparityWinHeight,
        UInt32 orgVideoWinStartX,
        UInt32 orgVideoWinStartY,
        UInt32 orgVideoWinWidth,
        UInt32 orgVideoWinHeight,
        UInt32 orgVideoWinGap
);

Void ChainsCommon_Stereo_Init();

Void ChainsCommon_Stereo_Delete(
            AlgorithmLink_RemapMergeCreateParams *pAlg_RemapMergePrm);

char ChainsCommon_menuRunTime_SingleCamAnalytics();

Void ChainsCommon_thermalConfig();

Void ChainsCommon_modifyTemperatureThreshold(char ch_temp, UInt32 eventType);

Void ChainsCommon_readTemperatureThreshold(UInt32 eventType);

Void ChainsCommon_modifyTempThresholdStepSize(char ch_temp);

Void ChainsCommon_tempHotEventHandler(UInt32 captureLinkId, Void* pPrm);

Void ChainsCommon_tempColdEventHandler(UInt32 captureLinkId, Void* pPrm);

Ptr ChainsCommon_SurroundView_memAlloc(UInt32 size, UInt32 align);
Void ChainsCommon_SurroundView_memFree(Ptr addr, UInt32 size);

Void ChainsCommon_Analytics2_SetPrms(
            CaptureLink_CreateParams *pCapturePrm,
            AlgorithmLink_SparseOpticalFlowCreateParams *pSofPrm,
            AlgorithmLink_SfmCreateParams *pSfmPrm,
            AlgorithmLink_ImagePreProcessCreateParams  *pImgPreProcPrm,
            AlgorithmLink_ImgPyramidCreateParams *pImgPmd,
            AlgorithmLink_FPComputeCreateParams *pFpcPrm,
            AlgorithmLink_ObjectDetectionCreateParams *pOdPrm,
            AlgorithmLink_ObjectClassificationCreateParams *pOcPrm,
            SyncLink_CreateParams *pSyncFcwPrm,
            AlgorithmLink_FcwCreateParams *pFcwPrm,
            AlgorithmLink_LaneDetectCreateParams *pLdPrm,
            AlgorithmLink_ClrCreateParams *pClrPrm,
            SyncLink_CreateParams *pSyncAlgPrm,
            AlgorithmLink_ObjectDrawCreateParams *pOdDrawPrm,
            SyncLink_CreateParams *pSyncOdPrm,
            DisplayLink_CreateParams *pDisplayVideoPrm,
            GrpxSrcLink_CreateParams *pGrpxSrcPrm,
            DisplayLink_CreateParams *pDisplayGrpxPrm,
            Chains_CaptureSrc captureSrc,
            Chains_DisplayType displayType,
            UInt32 Alg_ImgPyramidLinkID,
            UInt32 Alg_FPComputeLinkID,
            UInt32 Alg_ObjectDrawinkID,
            UInt32 Alg_ObjectClassificationLinkID,
            UInt32 Alg_LaneDetectLinkID,
            UInt32 isInputModeFile
        );

Void ChainsCommon_Analytics2_Start(UInt32 isInputModeFile);
Void ChainsCommon_Analytics2_Stop(UInt32 isInputModeFile);

Void  ChainsCommon_SetSOFConfig(
                                 AlgorithmLink_SparseOpticalFlowCreateParams *pAlgSofPrm,
                                 UInt32 startX,
                                 UInt32 startY,
                                 UInt32 width,
                                 UInt32 height,
                                 UInt32 sfmTrackPoints);

UInt32 ChainsCommon_displayLHMStatus();

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

/* @} */

/**
 *******************************************************************************
 *
 *   \defgroup EXAMPLES_API Example code implementation
 *
 *******************************************************************************
 */

