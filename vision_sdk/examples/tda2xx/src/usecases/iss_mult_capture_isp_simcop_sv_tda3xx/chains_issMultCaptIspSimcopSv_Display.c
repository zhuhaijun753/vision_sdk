/*
 *******************************************************************************
 *
 * Copyright (C) 2015 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */
#include "chains_issMultCaptIspSimcopSv_Display_priv.h"
#include <examples/tda2xx/include/chains_common.h>
#include <examples/tda2xx/include/chains_common_iss.h>

#define SV_ISP_LDC_MESH_TABLE_PITCH(w, r) \
                                        ((((((w)/(r))+1) + 15U) & ~15U) * (4U))

/* Sensor Output Frm Size */
#define SV_CAPT_SENSOR_AR140_WIDTH      (1280U)
#define SV_CAPT_SENSOR_AR140_HEIGHT     (800U)

/* ISP Output Frm Size */
#define SV_ISP_OUTPUT_WIDTH             (SV_CAPT_SENSOR_AR140_WIDTH)
#define SV_ISP_OUTPUT_HEIGHT            (800U)

/* LDC Frm size and other parameters */
#define SV_CH0_CH2_LDC_OUTPUT_WIDTH     (1280U)
#define SV_CH0_CH2_LDC_OUTPUT_HEIGHT    (280U)
#define SV_CH0_CH2_LDC_BLOCK_WIDTH      (16U)
#define SV_CH0_CH2_LDC_BLOCK_HEIGHT     (20U)

#define SV_CH1_CH3_LDC_OUTPUT_WIDTH     (560U)
#define SV_CH1_CH3_LDC_OUTPUT_HEIGHT    (720U)
#define SV_CH1_CH3_LDC_BLOCK_WIDTH      (16U)
#define SV_CH1_CH3_LDC_BLOCK_HEIGHT     (20U)

#define SV_CH1_CH3_LDC_INPUT_WIDTH      (SV_CAPT_SENSOR_AR140_WIDTH)
#define SV_CH1_CH3_LDC_INPUT_HEIGHT     (SV_CH1_CH3_LDC_OUTPUT_HEIGHT)

/* SRV Car Box Size */
#define SV_CARBOX_WIDTH                 (160U)
#define SV_CARBOX_HEIGHT                (160U)

/* SRV Num Views/Channels */
#define SV_NUM_VIEWS                    (4U)

/* SRV Output Frm Size */
#define SV_HOR_WIDTH                    (1280U)
#define SV_HOR_HEIGHT                   (720U)

/* SRV Output Frm Size for LDC Lut calculation */
#define SV_LDC_LUT_FRM_WIDTH            (1280U)
#define SV_LDC_LUT_FRM_HEIGHT           (800U)

#define SV_ALGO_ALIGN_IGNORE_FIRST_N_FRAMES (50U)
#define SV_ALGO_ALIGN_DEFAULT_FOCAL_LENGTH  (407U)

#define SIMCOP_OUTPUT_NUM_BUFFERS       (6U)
#define ISP_OUTPUT_NUM_BUFFERS          (4U)

#define ENABLE_WDR_MERGE_PARAMS_CFG

/**
 *******************************************************************************
 *
 *  \brief  chains_issMultCaptIspSimcopSv_DisplayAppObj
 *
 *        This structure contains all the LinksId's and create Params.
 *        The same is passed to all create, start, stop functions.
 *
 *******************************************************************************
*/
typedef struct {

    chains_issMultCaptIspSimcopSv_DisplayObj ucObj;

    IssIspConfigurationParameters ispConfig;

    IssM2mSimcopLink_ConfigParams simcopConfig;
    vpsissldcConfig_t             ldcCfg;
    vpsissvtnfConfig_t            vtnfCfg;

    UInt32  captureOutWidth;
    UInt32  captureOutHeight;
    UInt32  displayWidth;
    UInt32  displayHeight;

    Chains_Ctrl *chainsCfg;

    /* Not used for now */
    IssM2mSimcopLink_OperatingMode simcopMode;
    Bool bypassVtnf;
    Bool bypassLdc;

} chains_issMultCaptIspSimcopSv_DisplayAppObj;


char gChains_IssMultCaptIspSimcopSv_Display_runTimeMenu[] = {
"\r\n "
"\r\n ===================="
"\r\n Chains Run-time Menu"
"\r\n ===================="
"\r\n "
"\r\n 0: Stop Chain"
"\r\n 1: Camera Position Calibration"
"\r\n 2: Save a captured RAW frame from channel 0"
"\r\n 3: Save a Simcop Output Frame"
"\r\n "
"\r\n "
"\r\n p: Print Performance Statistics "
"\r\n "
"\r\n Enter Choice: "
"\r\n "
};

static Void chains_issMultCaptIspSimcopSv_Display_SetSyncPrm(
                    SyncLink_CreateParams *pGaPrm,
                    SyncLink_CreateParams *pSyncPrm,
                    UInt32 numCh,
                    UInt32 syncPeriod)
{
    pGaPrm->syncDelta =35U; /* round up 1 / 27.0 */
    pGaPrm->syncThreshold = pGaPrm->syncDelta * 2U;

    pSyncPrm->syncDelta = 35U; /* round up 1 / 27.0 */
    pSyncPrm->syncThreshold = pSyncPrm->syncDelta * 2U;;
}


static Void chains_issMultCaptIspSimcopSv_Display_SetSelectlinkPrms(SelectLink_CreateParams *pPrm)
{
    pPrm->numOutQue = 1U;

    pPrm->outQueChInfo[0].outQueId   = 0;
    pPrm->outQueChInfo[0].numOutCh   = 1;
    pPrm->outQueChInfo[0].inChNum[0] = 0;
}


static Void chains_issMultCaptIspSimcopSv_Display_Config_Aewb(
        IssAewbAlgOutParams *pAewbAlgOut,
        Void *appData)
{
    UInt32 chId;

    chains_issMultCaptIspSimcopSv_DisplayAppObj *pObj =
        (chains_issMultCaptIspSimcopSv_DisplayAppObj *)appData;

    UTILS_assert(NULL != pObj);
    UTILS_assert(NULL != pAewbAlgOut);

    for ( chId = 0U; chId < pObj->ucObj.IssCapturePrm.numCh; chId++)
    {
        pAewbAlgOut->channelId = chId;
        /* AEWB Output parameters are already converted and stored in
           ispCfg parameter of alg out, so set it in the ISP using ISP
           Link */
        System_linkControl(
            pObj->ucObj.IssM2mIspLinkID,
            ISSM2MISP_LINK_CMD_SET_AEWB_PARAMS,
            pAewbAlgOut,
            sizeof(IssAewbAlgOutParams),
            TRUE);

        ChainsCommon_IssUpdateAewbParams(pAewbAlgOut);
    }
}

static Void chains_issMultCaptIspSimcopSv_Display_Config_DCC_Params(
        IssIspConfigurationParameters *ispCfg,
        IssM2mSimcopLink_ConfigParams *simcopCfg,
        Void                          *appData)
{
    UInt32 chId;
    chains_issMultCaptIspSimcopSv_DisplayAppObj *pObj =
        (chains_issMultCaptIspSimcopSv_DisplayAppObj *)appData;

    UTILS_assert(NULL != pObj);
    UTILS_assert(NULL != ispCfg);
    UTILS_assert(NULL != simcopCfg);

    for ( chId = 0U; chId < pObj->ucObj.IssCapturePrm.numCh; chId++)
    {
        ispCfg->channelId = chId;

        System_linkControl(
            pObj->ucObj.IssM2mIspLinkID,
            ISSM2MISP_LINK_CMD_SET_ISPCONFIG,
            ispCfg,
            sizeof(IssIspConfigurationParameters),
            TRUE);
    }

    if (NULL != simcopCfg->ldcConfig)
    {
        /* Copy LDC configuration and apply it,
           Will use same  */
        memcpy(
            &pObj->ldcCfg,
            simcopCfg->ldcConfig,
            sizeof(vpsissldcConfig_t));

        pObj->simcopConfig.ldcConfig = &pObj->ldcCfg;
        pObj->simcopConfig.vtnfConfig = &pObj->vtnfCfg;

        /* MUST be called after link create and before link start */
        System_linkControl(
                pObj->ucObj.IssM2mSimcopLinkID,
                ISSM2MSIMCOP_LINK_CMD_SET_SIMCOPCONFIG,
                &pObj->simcopConfig,
                sizeof(pObj->simcopConfig),
                TRUE);
    }
}

#ifdef ENABLE_WDR_MERGE_PARAMS_CFG
/* Function to get the exposure parameters from the sensor and update
   the merge parameters in the ISP. Called only when WDR merge is enabled. */
static Void chains_issMultCaptIspSimcopSv_Display_Config_Merge(
        IssAewbAlgOutParams *pAewbAlgOut,
        Void *appData)
{
    chains_issMultCaptIspSimcopSv_DisplayAppObj *pObj =
        (chains_issMultCaptIspSimcopSv_DisplayAppObj *)appData;
    IssM2mIspLink_CreateParams              *pIssM2mIspPrm;
    AlgorithmLink_IssAewbCreateParams       *pAlg_IssAewbPrm;

    UTILS_assert(NULL != pObj);
    UTILS_assert(NULL != pAewbAlgOut);

    pIssM2mIspPrm     = &pObj->ucObj.IssM2mIspPrm;
    pAlg_IssAewbPrm   = &pObj->ucObj.Alg_IssAewbPrm;

    /* Get Exposure ratio parameters from the sensor */
    ChainsCommon_IssGetExposureParams(pAewbAlgOut);

    /* When dgain is applied only to long after split, there is a
        different ratio for split and merge */
    /* setting up merge parameters even for 2 pass wdr mode when AE is
       not enabled because merge parameter calculation uses digital gain
       for the calculation */
    if ((TRUE == pAewbAlgOut->outPrms[0].useAeCfg) ||
        ((TRUE == IssM2mIspLink_IsWdrMergeMode(
            pIssM2mIspPrm->channelParams[0].operatingMode)) &&
         (ALGORITHMS_ISS_AEWB_MODE_AWB == pAlg_IssAewbPrm->mode)))
    {
        System_linkControl(
            pObj->ucObj.IssM2mIspLinkID,
            ISSM2MISP_LINK_CMD_SET_WDR_MERGE_PARAMS,
            pAewbAlgOut,
            sizeof(IssAewbAlgOutParams),
            TRUE);
    }
}
#endif


static Void chains_issMultCaptIspSimcopSv_Display_SetIspConfig(
            chains_issMultCaptIspSimcopSv_DisplayAppObj *pObj)
{
    Int32 status;
    UInt32 numCh;

    /* set default params */
    IssM2mSimcopLink_ConfigParams_Init(&pObj->simcopConfig);
    vpsissLdcCfg_init(&pObj->ldcCfg);
    vpsissVtnfCfg_init(&pObj->vtnfCfg);

    pObj->simcopConfig.ldcConfig = &pObj->ldcCfg;
    pObj->simcopConfig.vtnfConfig = &pObj->vtnfCfg;

    /* Set the Default SimCop configuration,
        This could get overwritten by DCC */
    ChainsCommon_SetIssSimcopConfig(
        &pObj->simcopConfig,
        pObj->bypassVtnf,
        pObj->bypassLdc,
        0);

    /* Assuming all channels will be using same isp operating mode */
    ChainsCommon_IssGetDefaultIspSimcopConfig(
        pObj->ucObj.IssM2mIspPrm.channelParams[0].operatingMode,
        &pObj->ispConfig,
        &pObj->simcopConfig,
        pObj->ucObj.Alg_IssAewbLinkID);

    /* Changing Simcop Configuration as required for this usecase */
    if (pObj->bypassLdc == TRUE)
    {
        pObj->ldcCfg.pixelPad = 0;
    }
    else
    {
        pObj->ldcCfg.pixelPad = 4U;
    }
    pObj->ldcCfg.advCfg.outputBlockWidth = 64;
    pObj->ldcCfg.advCfg.outputBlockHeight = 32;
    //pObj->ldcCfg.lumeIntrType = VPS_ISS_LDC_LUMA_INTRP_BILINEAR;
    pObj->ldcCfg.lumeIntrType = VPS_ISS_LDC_LUMA_INTRP_BICUBIC;

    /* MUST be called after link create and before link start */
    /* Apply the same config to all channel,
        right now the sensor are the same so its fine. */
    for (numCh = 0U; numCh < pObj->ucObj.IssCapturePrm.numCh; numCh++)
    {
        pObj->ispConfig.channelId = numCh;
        pObj->simcopConfig.chNum = numCh;

        status = System_linkControl(
                pObj->ucObj.IssM2mIspLinkID,
                ISSM2MISP_LINK_CMD_SET_ISPCONFIG,
                &pObj->ispConfig,
                sizeof(pObj->ispConfig),
                TRUE);
        UTILS_assert(0 == status);


        /* Configure the LDC Luts */
        ChainsCommon_SetIssLdcLutConfig( &pObj->ldcCfg.lutCfg, 3 + numCh);

        pObj->ldcCfg.lutCfg.downScaleFactor =
                                            VPS_ISS_LDC_LUT_DOWN_SCALE_FACTOR_8;
        pObj->ldcCfg.lutCfg.lineOffset =
                    SV_ISP_LDC_MESH_TABLE_PITCH(SV_ISP_OUTPUT_WIDTH, \
                                (1U << pObj->ldcCfg.lutCfg.downScaleFactor));

        if (numCh == 0 || numCh == 2)
        {
            pObj->ldcCfg.advCfg.outputFrameWidth = SV_CH0_CH2_LDC_OUTPUT_WIDTH;
            pObj->ldcCfg.advCfg.outputFrameHeight = SV_CH0_CH2_LDC_OUTPUT_HEIGHT;
            pObj->ldcCfg.advCfg.outputBlockWidth = SV_CH0_CH2_LDC_BLOCK_WIDTH;
            pObj->ldcCfg.advCfg.outputBlockHeight = SV_CH0_CH2_LDC_BLOCK_HEIGHT;

        }
        else
        {
            pObj->ldcCfg.advCfg.outputFrameWidth = SV_CH1_CH3_LDC_OUTPUT_WIDTH;
            pObj->ldcCfg.advCfg.outputFrameHeight = SV_CH1_CH3_LDC_OUTPUT_HEIGHT;
            pObj->ldcCfg.advCfg.outputBlockWidth = SV_CH1_CH3_LDC_BLOCK_WIDTH;
            pObj->ldcCfg.advCfg.outputBlockHeight = SV_CH1_CH3_LDC_BLOCK_HEIGHT;

        }

        /* MUST be called after link create and before link start */
        status = System_linkControl(
                pObj->ucObj.IssM2mSimcopLinkID,
                ISSM2MSIMCOP_LINK_CMD_SET_SIMCOPCONFIG,
                &pObj->simcopConfig,
                sizeof(pObj->simcopConfig),
                TRUE);
        UTILS_assert(0 == status);
    }

}

Void chains_issMultCaptIspSimcopSv_Display_SetAppPrms(chains_issMultCaptIspSimcopSv_DisplayObj *pUcObj, Void *appObj)
{
    UInt32 numCh;
    chains_issMultCaptIspSimcopSv_DisplayAppObj *pObj
        = (chains_issMultCaptIspSimcopSv_DisplayAppObj*)appObj;
    IssM2mIspLink_OperatingMode ispOpMode;
    Int16 carBoxWidth;
    Int16 carBoxHeight;

    pObj->captureOutWidth  = SV_CAPT_SENSOR_AR140_WIDTH;
    pObj->captureOutHeight = SV_CAPT_SENSOR_AR140_HEIGHT;

    ChainsCommon_GetDisplayWidthHeight(
        pObj->chainsCfg->displayType,
        &pObj->displayWidth,
        &pObj->displayHeight
        );

    pObj->simcopMode = ISSM2MSIMCOP_LINK_OPMODE_LDC;
    pObj->bypassVtnf = TRUE;
    pObj->bypassLdc  = FALSE;

    /* Initialize Simcop Pointers */
    pObj->simcopConfig.ldcConfig = &pObj->ldcCfg;
    pObj->simcopConfig.vtnfConfig = &pObj->vtnfCfg;

    ispOpMode = pObj->chainsCfg->ispOpMode;

    ChainsCommon_SetIssCreatePrms(
        NULL,
        &pUcObj->IssCapturePrm,
        &pUcObj->IssM2mIspPrm,
        &pUcObj->IssM2mSimcopPrm,
        NULL,
        NULL,
        pObj->displayWidth,
        pObj->displayHeight,
        pObj->simcopMode,
        ispOpMode,
        NULL);
    ChainsCommon_SetIssIspPrms(
        &pUcObj->IssM2mIspPrm,
        SV_CAPT_SENSOR_AR140_WIDTH,
        SV_CAPT_SENSOR_AR140_HEIGHT,
        0U, 0U, ispOpMode, NULL);

    /* Override for multiple channel capture */
    ChainsCommon_MultipleCam_UpdateIssCapturePrms(
        pObj->chainsCfg->sensorName,
        &pUcObj->IssCapturePrm,
        ispOpMode, pObj->captureOutWidth, pObj->captureOutHeight,
        FALSE);

    /*
     *  ChainsCommon_SetIssAlgAewbPrms() should be called after sensor creation.
     *  The sensor creation is done in ChainsCommon_MultipleCam_UpdateIssCapturePrms().
     */
    ChainsCommon_SetIssAlgAewbPrms(&pUcObj->Alg_IssAewbPrm);

    /* Since we are operating in Linear mode, disable backlight compensation */
    if ((ispOpMode == ISSM2MISP_LINK_OPMODE_12BIT_LINEAR) ||
        (ispOpMode == ISSM2MISP_LINK_OPMODE_1PASS_WDR))
    {
        pUcObj->Alg_IssAewbPrm.aeDynParams.enableBlc = FALSE;
    }

    pUcObj->Alg_IssAewbPrm.appData = pObj;
    pUcObj->Alg_IssAewbPrm.cfgCbFxn =
        chains_issMultCaptIspSimcopSv_Display_Config_Aewb;

    #ifdef ENABLE_WDR_MERGE_PARAMS_CFG
    /* Only support for two pass wdr flow */
    if (TRUE == IssM2mIspLink_IsWdrMergeMode(pObj->chainsCfg->ispOpMode))
    {
        pUcObj->Alg_IssAewbPrm.mergeCbFxn =
            chains_issMultCaptIspSimcopSv_Display_Config_Merge;
    }
    else
    {
        pUcObj->Alg_IssAewbPrm.mergeCbFxn = NULL;
    }
    #endif

    pUcObj->Alg_IssAewbPrm.dccIspCfgFxn =
        chains_issMultCaptIspSimcopSv_Display_Config_DCC_Params;

    pUcObj->IssCapturePrm.allocBufferForRawDump = TRUE;

    chains_issMultCaptIspSimcopSv_Display_SetSelectlinkPrms(&pUcObj->SelectPrm);


    ChainsCommon_SurroundView_SetParams(
        NULL, // Capture Params
        NULL,
        NULL,
        NULL, //&pUcObj->SelectPrm,
        NULL, //&pUcObj->VPE_sv_orgPrm1,
        NULL, //&pUcObj->VPE_sv_orgPrm2,
        &pUcObj->Sync_gaPrm,
        NULL, //&pUcObj->Sync_sv_orgPrm1,
        NULL, //&pUcObj->Sync_sv_orgPrm2,
        &pUcObj->Alg_SynthesisPrm,
        &pUcObj->Alg_GeoAlignPrm,
        &pUcObj->Alg_PhotoAlignPrm,
        NULL,
        NULL,
        NULL,
        NULL, //&pUcObj->Alg_DmaSwMs_sv_orgPrm1,
        NULL, //&pUcObj->Alg_DmaSwMs_sv_orgPrm2,
        &pUcObj->GrpxSrcPrm,
        &pUcObj->Display_VideoPrm,
        NULL, //&pUcObj->Display_sv_orgPrm1,
        NULL, //&pUcObj->Display_sv_orgPrm12,
        &pUcObj->Display_GrpxPrm,
        pObj->chainsCfg->displayType,
        pUcObj->IssCapturePrm.numCh,
        ALGORITHM_LINK_SRV_OUTPUT_2D_LDC, //pObj->chainsCfg->svOutputMode,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        TRUE /* Enable CAR overlay */
        );

    carBoxWidth = SV_CARBOX_WIDTH;
    carBoxHeight = SV_CARBOX_HEIGHT;

    chains_issMultCaptIspSimcopSv_Display_SetSyncPrm(
                    &pUcObj->Sync_gaPrm,
                    &pUcObj->Sync_synPrm,
                    pUcObj->IssCapturePrm.numCh, 0x0); /* Sync period not used
                                                            in this func now */

    ChainsCommon_SurroundView_SetSynthParams(&pUcObj->Alg_SynthesisPrm,
                                            SV_ISP_OUTPUT_WIDTH,
                                            SV_ISP_OUTPUT_HEIGHT,
                                            SV_HOR_WIDTH,
                                            SV_HOR_HEIGHT,
                                            SV_NUM_VIEWS,
                                            carBoxWidth,
                                            carBoxHeight,
                                            ALGORITHM_LINK_SRV_OUTPUT_2D_LDC,
                                            TRUE); /* Enable CAR overlay */
    /* Set the LDC Input Frame size parameters in Synthesys */
    for (numCh = 0U; numCh < pUcObj->IssCapturePrm.numCh; numCh ++)
    {
        if (numCh == 0 || numCh == 2)
        {
            pUcObj->Alg_SynthesisPrm.ldcFrmWidth[numCh] =
                SV_CH0_CH2_LDC_OUTPUT_WIDTH;
            pUcObj->Alg_SynthesisPrm.ldcFrmHeight[numCh] =
                SV_CH0_CH2_LDC_OUTPUT_HEIGHT;
        }
        else
        {
            pUcObj->Alg_SynthesisPrm.ldcFrmWidth[numCh] =
                SV_CH1_CH3_LDC_OUTPUT_WIDTH;
            pUcObj->Alg_SynthesisPrm.ldcFrmHeight[numCh] =
                SV_CH1_CH3_LDC_OUTPUT_HEIGHT;
        }
    }

    ChainsCommon_SurroundView_SetGAlignParams(&pUcObj->Alg_GeoAlignPrm,
                                            SV_ISP_OUTPUT_WIDTH,
                                            SV_ISP_OUTPUT_HEIGHT,
                                            SV_HOR_WIDTH,
                                            SV_HOR_HEIGHT,
                                            SV_NUM_VIEWS,
                                            carBoxWidth,
                                            carBoxHeight,
                                            ALGORITHM_LINK_SRV_OUTPUT_2D_LDC);
    pUcObj->Alg_GeoAlignPrm.ldcLutOnlyMode = TRUE;
    pUcObj->Alg_GeoAlignPrm.ldcOutFrmWidth = SV_LDC_LUT_FRM_WIDTH;
    pUcObj->Alg_GeoAlignPrm.ldcOutFrmHeight = SV_LDC_LUT_FRM_HEIGHT;
    pUcObj->Alg_GeoAlignPrm.ldcDownScaleFactor =
        VPS_ISS_LDC_LUT_DOWN_SCALE_FACTOR_8;
    pUcObj->Alg_GeoAlignPrm.numOutputTables = 2U;

    /* Override parameters specific to this use case */
    pUcObj->Alg_GeoAlignPrm.ignoreFirstNFrames =
                                    SV_ALGO_ALIGN_IGNORE_FIRST_N_FRAMES;
    pUcObj->Alg_GeoAlignPrm.defaultFocalLength =
                                    SV_ALGO_ALIGN_DEFAULT_FOCAL_LENGTH;

    ChainsCommon_SurroundView_SetPAlignParams(&pUcObj->Alg_PhotoAlignPrm,
                                            /*SV_CAPT_SENSOR_AR140_WIDTH,
                                            SV_ISP_OUTPUT_HEIGHT,*/
                                            SV_HOR_WIDTH,
                                            SV_HOR_HEIGHT,
                                            SV_HOR_WIDTH,
                                            SV_HOR_HEIGHT,
                                            SV_NUM_VIEWS,
                                            carBoxWidth,
                                            carBoxHeight,
                                            ALGORITHM_LINK_SRV_OUTPUT_2D_LDC);


    for (numCh = 0U; numCh < pUcObj->IssCapturePrm.numCh; numCh ++)
    {
        pUcObj->IssM2mSimcopPrm.channelParams[numCh].useOutputFrameSize = TRUE;

        if (numCh == 0 || numCh == 2)
        {
            pUcObj->IssM2mSimcopPrm.channelParams[numCh].outputFrameWidth =
                SV_CH0_CH2_LDC_OUTPUT_WIDTH;
            pUcObj->IssM2mSimcopPrm.channelParams[numCh].outputFrameHeight =
                SV_CH0_CH2_LDC_OUTPUT_HEIGHT;
        }
        else
        {
            pUcObj->IssM2mSimcopPrm.channelParams[numCh].outputFrameWidth =
                SV_CH1_CH3_LDC_OUTPUT_WIDTH;
            pUcObj->IssM2mSimcopPrm.channelParams[numCh].outputFrameHeight =
                SV_CH1_CH3_LDC_OUTPUT_HEIGHT;

            /* Change input frame size also for channel 1 and 3 */
            pUcObj->IssM2mSimcopPrm.channelParams[numCh].overrideInFrmSize =
                FALSE;

            pUcObj->IssM2mSimcopPrm.channelParams[numCh].inputFrameWidth =
                SV_CH1_CH3_LDC_INPUT_WIDTH;
            pUcObj->IssM2mSimcopPrm.channelParams[numCh].inputFrameHeight =
                SV_CH1_CH3_LDC_INPUT_HEIGHT;

            /* Change maximum output buffer size in the ISP link as well */
            pUcObj->IssM2mIspPrm.channelParams[numCh].useBufSizePrmsFlag = FALSE;
            pUcObj->IssM2mIspPrm.channelParams[numCh].maxBufWidth = SV_CH1_CH3_LDC_INPUT_WIDTH;
            pUcObj->IssM2mIspPrm.channelParams[numCh].maxBufHeight = SV_CH1_CH3_LDC_INPUT_HEIGHT;
        }

        pUcObj->IssM2mSimcopPrm.channelParams[numCh].numBuffersPerCh =
            SIMCOP_OUTPUT_NUM_BUFFERS;
        pUcObj->IssM2mIspPrm.channelParams[numCh].numBuffersPerCh =
            ISP_OUTPUT_NUM_BUFFERS;
    }
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
 * \param   pObj  [IN] chains_issMultCaptIspSimcopSv_DisplayAppObj
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
*/
static Void chains_issMultCaptIspSimcopSv_Display_StartApp(chains_issMultCaptIspSimcopSv_DisplayAppObj *pObj)
{
    Chains_memPrintHeapStatus();

    /* Sets the Simcop Config also */
    chains_issMultCaptIspSimcopSv_Display_SetIspConfig(pObj);

    ChainsCommon_IssStartCaptureDevice();

    ChainsCommon_StartDisplayDevice(pObj->chainsCfg->displayType);

    chains_issMultCaptIspSimcopSv_Display_Start(&pObj->ucObj);

    Chains_prfLoadCalcEnable(TRUE, FALSE, FALSE);
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
 * \param   pObj   [IN]   chains_issMultCaptIspSimcopSv_DisplayAppObj
 *
 *******************************************************************************
*/
static Void chains_issMultCaptIspSimcopSv_Display_StopAndDeleteApp(chains_issMultCaptIspSimcopSv_DisplayAppObj *pObj)
{
    chains_issMultCaptIspSimcopSv_Display_Stop(&pObj->ucObj);
    chains_issMultCaptIspSimcopSv_Display_Delete(&pObj->ucObj);

    ChainsCommon_StopDisplayCtrl();

    ChainsCommon_IssStopAndDeleteCaptureDevice();

    ChainsCommon_StopDisplayDevice(pObj->chainsCfg->displayType);

    /* Print the HWI, SWI and all tasks load */
    /* Reset the accumulated timer ticks */
    Chains_prfLoadCalcEnable(FALSE, TRUE, TRUE);
}

/**
 *******************************************************************************
 *
 * \brief   4 Channel surround view usecase entry function
 *
 *          This function configure, creates, link various links to establish
 *          usecase.
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
Void Chains_issMultCaptIspSimcopSv_Display(Chains_Ctrl *chainsCfg)
{
    char ch, chPrev;
    UInt32 done = FALSE, chId, validChId = 0;
    Bool startWithCalibration;
    chains_issMultCaptIspSimcopSv_DisplayAppObj chainsObj;
    Chains_CaptureSrc oldCaptSrc;
    chainsObj.bypassVtnf = 0; /* KW error fix */
    chainsObj.bypassLdc  = 0; /* KW error fix */

    if ((ISSM2MISP_LINK_OPMODE_1PASS_WDR != chainsCfg->ispOpMode) &&
        (ISSM2MISP_LINK_OPMODE_12BIT_LINEAR != chainsCfg->ispOpMode))
    {
        Vps_printf(" CHAINS: WDR should either be disabled or "
                    "in One Pass mode!!!\n");
        return;
    }

    oldCaptSrc = chainsCfg->captureSrc;
    chainsCfg->displayType = CHAINS_DISPLAY_TYPE_HDMI_720P;
    chainsCfg->captureSrc = CHAINS_CAPTURE_SRC_UB960_TIDA00262;
    chainsObj.chainsCfg = chainsCfg;

    do
    {
        done = FALSE;
        /* Set startWithCalibration = TRUE to start the demo with calibration.
           Else it will use the previously calibrated LUTs */
        startWithCalibration = TRUE;
        ChainsCommon_SurroundView_CalibInit(startWithCalibration);
        chains_issMultCaptIspSimcopSv_Display_Create(&chainsObj.ucObj, &chainsObj);
        chains_issMultCaptIspSimcopSv_Display_StartApp(&chainsObj);

        ch = 'a';
        chPrev = '1';
        do
        {
            Vps_printf(gChains_IssMultCaptIspSimcopSv_Display_runTimeMenu);

            ch = Chains_readChar();

            switch(ch)
            {
                case '0':
                    done = TRUE;
                    chPrev = '3';
                    break;
                case '1':
                    chPrev = ChainsCommon_SurroundView_MenuCalibration();
                    done = TRUE;
                    break;
                case '2':
                    System_linkControl(
                            chainsObj.ucObj.IssCaptureLinkID,
                            ISSCAPTURE_LINK_CMD_SAVE_FRAME,
                            NULL,
                            0,
                            TRUE);
                    break;

                case '3':
                    validChId = 0U;
                    do
                    {
                        Vps_printf("Enter Channel Id [0:%d]:",
                            chainsObj.ucObj.IssCapturePrm.numCh - 1);
                        ch = Chains_readChar();

                        chId = ch - '0';

                        if (chId < chainsObj.ucObj.IssCapturePrm.numCh)
                        {
                            validChId = 1U;
                        }
                    } while (!validChId);

                    /* Send command to Capture Link to save a frame */
                    System_linkControl(
                            chainsObj.ucObj.IssM2mSimcopLinkID,
                            ISSM2MSIMCOP_LINK_CMD_SAVE_FRAME,
                            &chId,
                            0,
                            TRUE);
                    break;

                case 'p':
                case 'P':
                    ChainsCommon_PrintStatistics();
                    chains_issMultCaptIspSimcopSv_Display_printStatistics(
                                    &chainsObj.ucObj);
                    break;
                default:
                    Vps_printf("\nUnsupported option '%c'. Please try again\n", ch);
                    break;
            }
        } while ((chPrev!='3') && (FALSE == done));

        chains_issMultCaptIspSimcopSv_Display_StopAndDeleteApp(&chainsObj);
        ChainsCommon_SurroundView_CalibDeInit();
        Task_sleep(100);
        Vps_printf("\n Restarting...\n");

    } while (chPrev!='3');
    chainsCfg->captureSrc = oldCaptSrc;
}

