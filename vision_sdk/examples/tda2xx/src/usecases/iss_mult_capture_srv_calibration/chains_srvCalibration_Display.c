/*
 *******************************************************************************
 *
 * Copyright (C) 2016 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */
#include "chains_srvCalibration_Display_tda2x_priv.h"
#include "chains_srvCalibration_Display_tda3x_priv.h"
#include <examples/tda2xx/include/chains_common.h>
#include <examples/tda2xx/include/chains_common_iss.h>
#include <examples/tda2xx/include/chains_common_surround_view.h>
#include <src/utils_common/include/utils_dma.h>
#include <examples/tda2xx/include/chains_main_srv_calibration.h>
#include <src/utils_common/include/utils_tsk.h>

#define ISP_OUTPUT_NUM_BUFFERS                  (4U)
#define SRV_ALGO_ALIGN_IGNORE_FIRST_N_FRAMES    (10U)
#define SRV_ALGO_ALIGN_DEFAULT_FOCAL_LENGTH     (407U)

#define SRV_CAPT_SENSOR_AR140_WIDTH             (1280U)
#define SRV_CAPT_SENSOR_AR140_HEIGHT            (800U)
#define SRV_CAPT_SENSOR_OV10640_WIDTH           (1280U)
#define SRV_CAPT_SENSOR_OV10640_HEIGHT          (720U)

#define SRV_NUM_CAMERAS                         (4U)
#define SRV_HOR_WIDTH                           (752U)
#define SRV_HOR_HEIGHT                          (1008U)
#define SRV_LDC_OUTPUT_WIDTH_MAX                (1280U)
#define SRV_LDC_OUTPUT_HEIGHT_MAX               (1008U)
#define SRV_LDC_LUT_FRM_WIDTH                   (SRV_LDC_OUTPUT_WIDTH_MAX)
#define SRV_LDC_LUT_FRM_HEIGHT                  (SRV_LDC_OUTPUT_HEIGHT_MAX)

/*
 *  TDA2XX Calibration
 */
#define SRV_TDA2X_CAPTURE_WIDTH                 (1280U)
#define SRV_TDA2X_CAPTURE_HEIGHT                (720U)
#define SRV_TDA2X_OUT_WIDTH                     (1080U)
#define SRV_TDA2X_OUT_HEIGHT                    (1080U)

/*
 *  2D calibration parameters
 */
#define SRV_HOR_WIDTH_2D                        (720U)
#define SRV_HOR_HEIGHT_2D                       (960U)

#define SRV_HOR_WIDTH_2D_TDA2X                  (880U)
#define SRV_HOR_HEIGHT_2D_TDA2X                 (1080U)

static char calibUseCaseRunTimeMenu[] = {
"\n "
"\n ===================="
"\n Chains Run-time Menu"
"\n ===================="
"\n "
"\n 0: Stop Chain"
"\n 1: Auto Calibration"
"\n 2: Manual Calibration"
"\n 3: Save display frame to MMC/SD"
"\n 4: Unmount File System before removing MMC/SD card"
"\n 5: Mount File System after inserting MMC/SD card"
"\n "
"\n "
"\n p: Print Performance Statistics "
"\n "
"\n Enter Choice: "
"\n "
};

static char manualCalibMenu[] = {
"\n "
"\n ===================="
"\n Manual Calibration"
"\n ===================="
"\n "
"\n 0: Exit"
"\n 1: Save ISP output frames (Will be saved in MMC/SD : All channels)"
"\n 2: Read the Calibration Matrix (CAL MAT) from file"
"\n 3: Compute LDC LUTs for 3D SRV (All view points)"
"\n 4: Unmount File System before removing MMC/SD card"
"\n 5: Mount File System after inserting MMC/SD card"
"\n "
"\n Enter Choice: "
"\n "
};

/*******************************************************************************
 *  FUNCTION DEFINITIONS
 *******************************************************************************
 */
Int32 ChainsCommon_setSensorFps(UInt32 sensorFps);
static Void disWbFrameCb(System_LinkChInfo *pChInfo, Void *pBuf, Void *arg);
static Void capFrameCb(System_LinkChInfo *pChInfo, Void *pBuf, Void *arg);

/**
 *******************************************************************************
 *
 *  \brief  chains_srvCalibration_DisplayAppObj
 *
 *        This structure contains all the LinksId's and create Params.
 *        The same is passed to all create, start, stop functions.
 *
 *******************************************************************************
*/
typedef struct {

    chains_srvCalibration_Display_tda2xObj ucTda2xObj;
    chains_srvCalibration_Display_tda3xObj ucTda3xObj;

    IssIspConfigurationParameters ispConfig;
    IssM2mSimcopLink_ConfigParams simcopConfig;

    UInt32  numCh;
    UInt32  captureOutWidth;
    UInt32  captureOutHeight;
    UInt32  displayWidth;
    UInt32  displayHeight;

    Chains_Ctrl *chainsCfg;

    AlgorithmLink_SrvCommonViewPointParams algViewParams;
    System_VideoFrameCompositeBuffer ldcMeshTableContainer;
    AlgorithmLink_GAlignControlParams geoAlignCtrlPrmsViewPrms;
    AlgorithmLink_lutGenPrm_3DSRV gAlignGenLutPrm3DSrv;
    AlgorithmLink_calMatGenPrm_3DSRV calMatGenPrm3DSrv;
    Bool isCalMatGenerated;
    UInt32 yuvDumpCount;
    UInt32 imgSize;
    Int8 *inChartPosBuf;
    Utils_DmaChObj dumpFramesDmaObj;

    /* ISP frame save */
    BspOsal_SemHandle saveFrameSync;
    Bool saveFrame;
    UInt32 saveFrameChId;

    /* Display write back */
    BspOsal_SemHandle disWbSync;
    Bool disWbFrame;
    UInt32 disWbFrameChId;

    UInt16 *gpuLut3DBuf;
    UInt32 gpuLutSize;
    Chain_Common_SRV_3DAutoCalibCreateParams autoCalibCreatePrm;
    Bool printStatistics;

} chains_srvCalibration_DisplayAppObj;

typedef enum
{
    SRV_CALIB_TYPE_AUTO,
    SRV_CALIB_TYPE_MANUAL

} SRV_CALIB_TYPE;

Int32 gOutputCorners[SRV_CORNERPOINT_SIZE/4];
UInt32 gCapWidth = 0U;
UInt32 gCapHeight = 0U;
Bool gDrawCornerPts = FALSE;
Bool gClearCornerPts = FALSE;

/**
 *******************************************************************************
 *
 * \brief   Do the ISP Set Config
 *
 *          Function sends a control command to capture and display link to
 *          to Start all the required links . Links are started in reverce
 *          order as information of next link is required to connect.
 *          System_linkStart is called with LinkId to start the links.
 *
 * \param   pObj  [IN] chains_issMultCaptIspSv_DisplayAppObj
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
*/
static Void setIspConfig(
            chains_srvCalibration_DisplayAppObj *pObj,
            chains_srvCalibration_Display_tda3xObj *pUcObj)
{
    Int32 status;
    UInt32 numCh;

    /* Assuming all channels will be using same isp operating mode */
    ChainsCommon_IssGetDefaultIspSimcopConfig(
        pUcObj->IssM2mIspPrm.channelParams[0].operatingMode,
        &pObj->ispConfig,
        &pObj->simcopConfig,
        pUcObj->Alg_IssAewbLinkID);

    /* MUST be called after link create and before link start */
    /* Apply the same config to all channel,
        right now the sensor are the same so its fine. */
    for (numCh = 0U; numCh < pObj->numCh; numCh++)
    {
        pObj->ispConfig.channelId = numCh;
        status = System_linkControl(
                pUcObj->IssM2mIspLinkID,
                ISSM2MISP_LINK_CMD_SET_ISPCONFIG,
                &pObj->ispConfig,
                sizeof(pObj->ispConfig),
                TRUE);
        UTILS_assert(0 == status);
    }
}

/**
 *******************************************************************************
 *
 * \brief   Start the Calibration Use case
 *
 *          Function sends a control command to capture and display link to
 *          to Start all the required links . Links are started in reverce
 *          order as information of next link is required to connect.
 *          System_linkStart is called with LinkId to start the links.
 *
 * \param   pObj  [IN] chains_issMultCaptIspSv_DisplayAppObj
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
*/
static Void startCalibUseCase(
                            chains_srvCalibration_DisplayAppObj *pObj)
{
    Chains_memPrintHeapStatus();

    if (Bsp_platformIsTda3xxFamilyBuild())
    {
        ChainsCommon_IssStartCaptureDevice();

        /* Sets the Simcop Config also */
        setIspConfig(pObj, &pObj->ucTda3xObj);

        ChainsCommon_StartDisplayDevice(pObj->chainsCfg->displayType);

        chains_srvCalibration_Display_tda3x_Start(&pObj->ucTda3xObj);
    }
    else
    {
        chains_srvCalibration_Display_tda2x_Start(&pObj->ucTda2xObj);
    }

    Chains_prfLoadCalcEnable(TRUE, FALSE, FALSE);
}

/**
 *******************************************************************************
 *
 * \brief   Stop and Delete the Calibration Use case
 *
 *          Function sends a control command to capture and display link to
 *          to delete all the prior created links
 *          System_linkDelete is called with LinkId to delete the links.
 *
 * \param   pObj   [IN]   chains_issMultCaptIspSv_DisplayAppObj
 *
 *******************************************************************************
*/
static Void stopAndDeleteCalibUseCase(
                            chains_srvCalibration_DisplayAppObj *pObj)
{
    if (Bsp_platformIsTda3xxFamilyBuild())
    {
        chains_srvCalibration_Display_tda3x_Stop(&pObj->ucTda3xObj);
        chains_srvCalibration_Display_tda3x_Delete(&pObj->ucTda3xObj);
        ChainsCommon_IssStopAndDeleteCaptureDevice();
    }
    else
    {
        chains_srvCalibration_Display_tda2x_Stop(&pObj->ucTda2xObj);
        chains_srvCalibration_Display_tda2x_Delete(&pObj->ucTda2xObj);
    }

    ChainsCommon_StopDisplayCtrl();

    ChainsCommon_StopDisplayDevice(pObj->chainsCfg->displayType);

#ifndef A15_TARGET_OS_LINUX
    /* Print the HWI, SWI and all tasks load */
    /* Reset the accumulated timer ticks */
    Chains_prfLoadCalcEnable(FALSE, TRUE, TRUE);
#endif
}

/**
 *******************************************************************************
 *
 * \brief   Set the Sync Link Paremeters
 *
 *          Function sends a control command to capture and display link to
 *          to delete all the prior created links
 *          System_linkDelete is called with LinkId to delete the links.
 *
 * \param   pObj   [IN]   chains_issMultCaptIspSv_DisplayAppObj
 *
 *******************************************************************************
*/
static Void setSyncLinkParams(
                    SyncLink_CreateParams *pPrm,
                    UInt32 numCh,
                    UInt32 syncPeriod)
{
    pPrm->syncDelta = 0x7FFFFFFF;
    pPrm->syncThreshold = 0x7FFFFFFF;
}

/**
 *******************************************************************************
 *
 * \brief   Set DMA SW Mosaic Create Parameters
 *
 *          It is called in Create function.
 *          In this function SwMs alg link params are set
 *          The algorithm which is to run on core is set to
 *          baseClassCreate.algId. The input whdth and height to alg are set.
 *          Number of input buffers required by alg are also set here.
 *
 *******************************************************************************
*/
static Void setDmaSwMsLinkParams(
                    AlgorithmLink_DmaSwMsCreateParams *pPrm,
                    UInt32 numCh,
                    UInt32 winWidth,
                    UInt32 winHeight,
                    UInt32 outWidth,
                    UInt32 outHeight
                   )
{
    UInt32 algId, winId;
    UInt32 useLocalEdma;
    AlgorithmLink_DmaSwMsLayoutWinInfo *pWinInfo;

    useLocalEdma = FALSE;
    algId = ALGORITHM_LINK_IPU_ALG_DMA_SWMS;

    pPrm->baseClassCreate.algId   = algId;
    pPrm->numOutBuf               = 4;
    pPrm->useLocalEdma            = useLocalEdma;
    pPrm->initLayoutParams.numWin = numCh;

    pPrm->maxOutBufWidth     = outWidth;
    pPrm->maxOutBufHeight    = outHeight;

    for(winId=0; winId<pPrm->initLayoutParams.numWin; winId++)
    {
        pWinInfo = &pPrm->initLayoutParams.winInfo[winId];
        pWinInfo->chId = winId;
        pWinInfo->inStartX = 0;
        pWinInfo->inStartY = 0;
        pWinInfo->width    = winWidth;
        pWinInfo->height   = winHeight;

        /*
         *  For 4x4 layout
         */
        switch(winId)
        {
            default:
            case 0:
                pWinInfo->outStartX = 0;
                pWinInfo->outStartY = 0;
                break;
            case 1:
                pWinInfo->outStartX = winWidth;
                pWinInfo->outStartY = 0;
                break;
            case 2:
                pWinInfo->outStartX = 0;
                pWinInfo->outStartY = winHeight;
                break;
            case 3:
                pWinInfo->outStartX = winWidth;
                pWinInfo->outStartY = winHeight;
                break;
        }
    }

    pPrm->initLayoutParams.outBufWidth  = pPrm->maxOutBufWidth;
    pPrm->initLayoutParams.outBufHeight = pPrm->maxOutBufHeight;
}

/**
 *******************************************************************************
 *
 * \brief   Set AEWB Link Create Parameters
 *
 *          It is called in Create function.
 *          In this function SwMs alg link params are set
 *          The algorithm which is to run on core is set to
 *          baseClassCreate.algId. The input whdth and height to alg are set.
 *          Number of input buffers required by alg are also set here.
 *
 *******************************************************************************
*/
static Void setAewbLinkParams(
        IssAewbAlgOutParams *pAewbAlgOut,
        Void *appData)
{
    chains_srvCalibration_DisplayAppObj *pObj =
        (chains_srvCalibration_DisplayAppObj *)appData;
    chains_srvCalibration_Display_tda3xObj *pUcObj = &pObj->ucTda3xObj;

    UTILS_assert(NULL != pObj);
    UTILS_assert(NULL != pAewbAlgOut);

    /* AEWB Output parameters are already converted and stored in
       ispCfg parameter of alg out, so set it in the ISP using ISP
       Link */
    System_linkControl(
        pUcObj->IssM2mIspLinkID,
        ISSM2MISP_LINK_CMD_SET_AEWB_PARAMS,
        pAewbAlgOut,
        sizeof(IssAewbAlgOutParams),
        TRUE);

    ChainsCommon_IssUpdateAewbParams(pAewbAlgOut);
}

/**
 *******************************************************************************
 *
 * \brief   Set M2M ISP Link Create Parameters
 *
 *          It is called in Create function.
 *          In this function SwMs alg link params are set
 *          The algorithm which is to run on core is set to
 *          baseClassCreate.algId. The input whdth and height to alg are set.
 *          Number of input buffers required by alg are also set here.
 *
 *******************************************************************************
*/
static Void setM2MIspLinkParams(
        IssIspConfigurationParameters *ispCfg,
        IssM2mSimcopLink_ConfigParams *simcopCfg,
        Void                          *appData)
{
    UInt32 chId;
    chains_srvCalibration_DisplayAppObj *pObj =
        (chains_srvCalibration_DisplayAppObj *)appData;
    chains_srvCalibration_Display_tda3xObj *pUcObj = &pObj->ucTda3xObj;

    UTILS_assert(NULL != pObj);
    UTILS_assert(NULL != ispCfg);
    UTILS_assert(NULL != simcopCfg);

    for ( chId = 0U; chId < pObj->numCh; chId++)
    {
        ispCfg->channelId = chId;

        System_linkControl(
            pUcObj->IssM2mIspLinkID,
            ISSM2MISP_LINK_CMD_SET_ISPCONFIG,
            ispCfg,
            sizeof(IssIspConfigurationParameters),
            TRUE);
    }
}

/**
 *******************************************************************************
 *
 * \brief   Set VPE Link Create Parameters
 *
 *          It is called in Create function.
 *          In this function SwMs alg link params are set
 *          The algorithm which is to run on core is set to
 *          baseClassCreate.algId. The input whdth and height to alg are set.
 *          Number of input buffers required by alg are also set here.
 *
 *******************************************************************************
*/
static Void setVpeLinkParams(
                        VpeLink_CreateParams *pPrm,
                        UInt32 numCh,
                        UInt32 OutWidth,
                        UInt32 OutHeight,
                        UInt32 inCropWidth,
                        UInt32 inCropHeight
                    )
{
    UInt32 chId;
    VpeLink_ChannelParams *chPrms;
    UInt32 outId = 0;

    pPrm->enableOut[0] = TRUE;

    for (chId = 0; chId < numCh; chId++)
    {
        chPrms = &pPrm->chParams[chId];
        chPrms->outParams[outId].numBufsPerCh =
                                 VPE_LINK_NUM_BUFS_PER_CH_DEFAULT;

        chPrms->outParams[outId].width = OutWidth;
        chPrms->outParams[outId].height = OutHeight;
        chPrms->outParams[outId].dataFormat = SYSTEM_DF_YUV420SP_UV;

        chPrms->scCfg.bypass       = FALSE;
        chPrms->scCfg.nonLinear    = FALSE;
        chPrms->scCfg.stripSize    = 0;

        chPrms->scCropCfg.cropStartX = 0;
        chPrms->scCropCfg.cropStartY = 0;
        chPrms->scCropCfg.cropWidth  = inCropWidth;
        chPrms->scCropCfg.cropHeight = inCropHeight;
    }
}

/**
 *******************************************************************************
 *
 * \brief   Set link Parameters of the DSS WB capture
 *
 *******************************************************************************
*/
static void setCaptureDssWbPrms(
                CaptureLink_CreateParams *pPrm,
                UInt32 displayWidth,
                UInt32 displayHeight)
{
    pPrm->numVipInst = 0;
    pPrm->numDssWbInst = 1;

    pPrm->dssWbInst[0].dssWbInstId = VPS_CAPT_INST_DSS_WB1;
    System_VideoScanFormat scanFormat = SYSTEM_SF_PROGRESSIVE;

    if (Bsp_platformIsTda3xxFamilyBuild())
    {
        pPrm->dssWbInst[0].dssWbInputPrms.inNode = SYSTEM_WB_IN_NODE_LCD1;
    }
    else
    {
        pPrm->dssWbInst[0].dssWbInputPrms.inNode = SYSTEM_WB_IN_NODE_TV;
    }
    pPrm->dssWbInst[0].dssWbInputPrms.wbInSourceWidth = displayWidth;
    pPrm->dssWbInst[0].dssWbInputPrms.wbInSourceHeight = displayHeight;
    pPrm->dssWbInst[0].dssWbInputPrms.wbInWidth = displayWidth;
    pPrm->dssWbInst[0].dssWbInputPrms.wbInHeight = displayHeight;
    pPrm->dssWbInst[0].dssWbInputPrms.wbPosx = 0;
    pPrm->dssWbInst[0].dssWbInputPrms.wbPosy = 0;
    pPrm->dssWbInst[0].dssWbInputPrms.wbInSourceDataFmt = SYSTEM_DF_BGR24_888;
    pPrm->dssWbInst[0].dssWbInputPrms.wbScanFormat = scanFormat;

    pPrm->dssWbInst[0].dssWbOutputPrms.wbWidth = displayWidth;
    pPrm->dssWbInst[0].dssWbOutputPrms.wbHeight = displayHeight;
    pPrm->dssWbInst[0].dssWbOutputPrms.wbDataFmt = SYSTEM_DF_YUV420SP_UV;
    pPrm->dssWbInst[0].dssWbOutputPrms.wbScanFormat = scanFormat;

    pPrm->dssWbInst[0].numBufs = 4U;
}

/**
 *******************************************************************************
 *
 * \brief   Set Calibration Use case Link Parameters for TDA3X
 *
 *          It is called in Create function.
 *          In this function SwMs alg link params are set
 *          The algorithm which is to run on core is set to
 *          baseClassCreate.algId. The input whdth and height to alg are set.
 *          Number of input buffers required by alg are also set here.
 *
 *******************************************************************************
*/
Void chains_srvCalibration_Display_tda3x_SetAppPrms(
                chains_srvCalibration_Display_tda3xObj *pUcObj, Void *appObj)
{
    chains_srvCalibration_DisplayAppObj *pObj
        = (chains_srvCalibration_DisplayAppObj*)appObj;
    IssM2mIspLink_OperatingMode ispOpMode;
    UInt32 numCh;
    UInt32 swMsWinWidth,swMsWinHeight;
    UInt32 flipAndMirror;

    ChainsCommon_GetDisplayWidthHeight(
                        pObj->chainsCfg->displayType,
                        &pObj->displayWidth,
                        &pObj->displayHeight);

    /*
     *  ISS Capture create on TDA3XX
     */
    pObj->captureOutWidth  = SRV_CAPT_SENSOR_AR140_WIDTH;
    pObj->captureOutHeight = SRV_CAPT_SENSOR_AR140_HEIGHT;

    if (pObj->chainsCfg->captureSrc == CHAINS_CAPTURE_SRC_UB960_IMI)
    {
        pObj->captureOutWidth  = SRV_CAPT_SENSOR_OV10640_WIDTH;
        pObj->captureOutHeight = SRV_CAPT_SENSOR_OV10640_HEIGHT;
    }

    ispOpMode = pObj->chainsCfg->ispOpMode;

    ChainsCommon_SetIssCreatePrms(
                        NULL,
                        &pUcObj->IssCapturePrm,
                        &pUcObj->IssM2mIspPrm,
                        NULL,
                        NULL,
                        NULL,
                        pObj->displayWidth,
                        pObj->displayHeight,
                        ISSM2MSIMCOP_LINK_OPMODE_MAXNUM,
                        ispOpMode,
                        NULL);

    ChainsCommon_SetIssIspPrms(
                        &pUcObj->IssM2mIspPrm,
                        pObj->captureOutWidth,
                        pObj->captureOutHeight,
                        0U,
                        0U,
                        ispOpMode,
                        NULL);

    /* Override for multiple channel capture */
    ChainsCommon_MultipleCam_UpdateIssCapturePrms(
        pObj->chainsCfg->sensorName,
        &pUcObj->IssCapturePrm,
        ispOpMode,
        pObj->captureOutWidth,
        pObj->captureOutHeight,
        TRUE);

    pUcObj->IssCapturePrm.allocBufferForRawDump = TRUE;

    pObj->numCh = pUcObj->IssCapturePrm.numCh;

    /* Enable frame dump in M2M ISP link */
    pUcObj->IssM2mIspPrm.allocBufferForDump = TRUE;

    for (numCh = 0U; numCh < pObj->numCh; numCh ++)
    {
        pUcObj->IssM2mIspPrm.channelParams[numCh].numBuffersPerCh =
            ISP_OUTPUT_NUM_BUFFERS;
    }

    /* Flip parameters for IMI */
    if (pObj->chainsCfg->captureSrc == CHAINS_CAPTURE_SRC_UB960_IMI)
    {
        /* Refer CHAINS_ISS_SENSOR_SET_FLIP_MIRROR for details */
        flipAndMirror = 2U; /* Flip disable and mirroring on */
        ChainsCommon_IssUpdateFlipParams(&flipAndMirror);
    }

    /*
     *  ChainsCommon_SetIssAlgAewbPrms() should be called after sensor creation.
     *  The sensor creation is done in ChainsCommon_MultipleCam_UpdateIssCapturePrms().
     */
    ChainsCommon_SetIssAlgAewbPrms(&pUcObj->Alg_IssAewbPrm);

    pUcObj->Alg_IssAewbPrm.runAewbOnlyForOneCh = FALSE;
    pUcObj->Alg_IssAewbPrm.chId = 0U;

    pUcObj->Alg_IssAewbPrm.appData = pObj;
    pUcObj->Alg_IssAewbPrm.cfgCbFxn = setAewbLinkParams;
    pUcObj->Alg_IssAewbPrm.mergeCbFxn = NULL;
    pUcObj->Alg_IssAewbPrm.dccIspCfgFxn = setM2MIspLinkParams;

    /* Since we are operating in Linear mode, disable back-light compensation */
    if ((ispOpMode == ISSM2MISP_LINK_OPMODE_12BIT_LINEAR) ||
        (ispOpMode == ISSM2MISP_LINK_OPMODE_1PASS_WDR))
    {
        pUcObj->Alg_IssAewbPrm.aeDynParams.enableBlc = FALSE;
    }

    /* These parameters are used by the Graphics */
    gCapWidth = pObj->captureOutWidth;
    gCapHeight = pObj->captureOutHeight;

    /* Null link call back for M2M ISP frame buffer */
    pUcObj->Null_ispPrm.appCb = capFrameCb;
    pUcObj->Null_ispPrm.appCbArg = pObj;

    /* SW MS link params */
    swMsWinWidth = pObj->displayWidth/2;
    swMsWinHeight = pObj->displayHeight/2;

    /*
     *  Resize to the SW Mosaic 4x4 layout window size
     */
    setVpeLinkParams(
                    &pUcObj->VPEPrm,
                    pObj->numCh,                    // numCh
                    swMsWinWidth,                   // OutWidth
                    swMsWinHeight,                  // OutHeight
                    pObj->captureOutWidth,          // inCropWidth
                    pObj->captureOutHeight          // inCropHeight
                    );

    setSyncLinkParams(
                    &pUcObj->SyncPrm,
                    pObj->numCh, 0x0);              /* Sync period not used
                                                            in this func now */

    setDmaSwMsLinkParams(
                    &pUcObj->Alg_DmaSwMsPrm,
                    pObj->numCh,                    // numCh
                    swMsWinWidth,                   // winWidth
                    swMsWinHeight,                  // winHeight
                    pObj->displayWidth,             // outWidth
                    pObj->displayHeight             // outHeight
                    );

    pUcObj->DisplayPrm.rtParams.tarWidth = pObj->displayWidth;
    pUcObj->DisplayPrm.rtParams.tarHeight = pObj->displayHeight;
    pUcObj->DisplayPrm.displayId = DISPLAY_LINK_INST_DSS_VID1;

    ChainsCommon_StartDisplayCtrl(
        pObj->chainsCfg->displayType,
        pObj->displayWidth,
        pObj->displayHeight
        );

    /* Graphics */
    pUcObj->GrpxSrcPrm.grpxBufInfo.dataFormat = SYSTEM_DF_BGR16_565;
    pUcObj->GrpxSrcPrm.grpxBufInfo.width = pObj->displayWidth;
    pUcObj->GrpxSrcPrm.grpxBufInfo.height = pObj->displayHeight;
    pUcObj->GrpxSrcPrm.statsDisplayEnable = TRUE;
    pUcObj->GrpxSrcPrm.srvCalibLayout = TRUE;

    /* Garphics Display */
    pUcObj->Display_GrpxPrm.rtParams.tarWidth = pObj->displayWidth;
    pUcObj->Display_GrpxPrm.rtParams.tarHeight = pObj->displayHeight;
    pUcObj->Display_GrpxPrm.rtParams.posX = 0;
    pUcObj->Display_GrpxPrm.rtParams.posY = 0;
    pUcObj->Display_GrpxPrm.displayId = DISPLAY_LINK_INST_DSS_GFX1;

    /* DSS WB capture */
    setCaptureDssWbPrms(
            &pUcObj->Capture_dsswbPrm,
            pObj->displayWidth,
            pObj->displayHeight);

    /* Null link call back for display write back buffer */
    pUcObj->Null_disWbPrm.appCb = disWbFrameCb;
    pUcObj->Null_disWbPrm.appCbArg = pObj;
}

/**
 *******************************************************************************
 *
 * \brief   Set Calibration Use case Link Parameters for TDA2X
 *
 *          It is called in Create function.
 *          In this function SwMs alg link params are set
 *          The algorithm which is to run on core is set to
 *          baseClassCreate.algId. The input whdth and height to alg are set.
 *          Number of input buffers required by alg are also set here.
 *
 *******************************************************************************
*/
Void chains_srvCalibration_Display_tda2x_SetAppPrms(
                chains_srvCalibration_Display_tda2xObj *pUcObj, Void *appObj)
{
#ifndef A15_TARGET_OS_LINUX
    UInt32 cnt;
    UInt32 portId[VIDEO_SENSOR_MAX_LVDS_CAMERAS];
    CaptureLink_VipInstParams *pInstPrm;
#endif
    chains_srvCalibration_DisplayAppObj *pObj
        = (chains_srvCalibration_DisplayAppObj*)appObj;
    UInt32 swMsWinWidth,swMsWinHeight;

    ChainsCommon_GetDisplayWidthHeight(
                        pObj->chainsCfg->displayType,
                        &pObj->displayWidth,
                        &pObj->displayHeight);

    /*
     *  VIP Capture create on TDA2XX
     */
    pObj->captureOutWidth  = SRV_TDA2X_CAPTURE_WIDTH;
    pObj->captureOutHeight = SRV_TDA2X_CAPTURE_HEIGHT;
    pObj->numCh = 4U;
#ifndef A15_TARGET_OS_LINUX
    ChainsCommon_MultiCam_StartCaptureDevice(
                                CHAINS_CAPTURE_SRC_OV10635,
                                portId,
                                pObj->numCh);

    ChainsCommon_MultiCam_SetCapturePrms(
                                &pUcObj->CapturePrm,
                                pObj->captureOutWidth,
                                pObj->captureOutHeight,
                                portId,
                                pObj->numCh);

    for (cnt = 0; cnt < SYSTEM_CAPTURE_VIP_INST_MAX; cnt++)
    {
        pInstPrm = &pUcObj->CapturePrm.vipInst[cnt];
        pInstPrm->numBufs = 6U;
    }
#else
    /*
     *  The sensor init is already done in the Linux boot sequence.
     *  Copy the capture create parameters
     */
    memcpy (&pUcObj->CapturePrm,
            &pObj->autoCalibCreatePrm.capturePrm,
            sizeof (CaptureLink_CreateParams));
#endif

    /* These parameters are used by the Graphics */
    gCapWidth = pObj->captureOutWidth;
    gCapHeight = pObj->captureOutHeight;

    /* Null link call back for M2M ISP frame buffer */
    pUcObj->Null_capturePrm.appCb = capFrameCb;
    pUcObj->Null_capturePrm.appCbArg = pObj;

    /* SW MS link params */
    swMsWinWidth = pObj->displayWidth/2;
    swMsWinHeight = pObj->displayHeight/2;

    /*
     *  Resize to the SW Mosaic 4x4 layout window size
     */
    setVpeLinkParams(
                    &pUcObj->VPEPrm,
                    pObj->numCh,                    // numCh
                    swMsWinWidth,                   // OutWidth
                    swMsWinHeight,                  // OutHeight
                    pObj->captureOutWidth,          // inCropWidth
                    pObj->captureOutHeight          // inCropHeight
                    );

    setSyncLinkParams(
                    &pUcObj->SyncPrm,
                    pObj->numCh, 0x0);              /* Sync period not used
                                                            in this func now */

    setDmaSwMsLinkParams(
                    &pUcObj->Alg_DmaSwMsPrm,
                    pObj->numCh,                    // numCh
                    swMsWinWidth,                   // winWidth
                    swMsWinHeight,                  // winHeight
                    pObj->displayWidth,             // outWidth
                    pObj->displayHeight             // outHeight
                    );

    pUcObj->DisplayPrm.rtParams.tarWidth = pObj->displayWidth;
    pUcObj->DisplayPrm.rtParams.tarHeight = pObj->displayHeight;
    pUcObj->DisplayPrm.displayId = DISPLAY_LINK_INST_DSS_VID1;

    ChainsCommon_StartDisplayCtrl(
        pObj->chainsCfg->displayType,
        pObj->displayWidth,
        pObj->displayHeight
        );

    /* Graphics */
    pUcObj->GrpxSrcPrm.grpxBufInfo.dataFormat = SYSTEM_DF_BGR16_565;
    pUcObj->GrpxSrcPrm.grpxBufInfo.width = pObj->displayWidth;
    pUcObj->GrpxSrcPrm.grpxBufInfo.height = pObj->displayHeight;
    pUcObj->GrpxSrcPrm.statsDisplayEnable = TRUE;
    pUcObj->GrpxSrcPrm.srvCalibLayout = TRUE;

    /* Garphics Display */
    pUcObj->Display_GrpxPrm.rtParams.tarWidth = pObj->displayWidth;
    pUcObj->Display_GrpxPrm.rtParams.tarHeight = pObj->displayHeight;
    pUcObj->Display_GrpxPrm.rtParams.posX = 0;
    pUcObj->Display_GrpxPrm.rtParams.posY = 0;
    pUcObj->Display_GrpxPrm.displayId = DISPLAY_LINK_INST_DSS_GFX1;

    /* DSS WB capture */
    setCaptureDssWbPrms(
            &pUcObj->Capture_dsswbPrm,
            pObj->displayWidth,
            pObj->displayHeight);

    /* Null link call back for display write back buffer */
    pUcObj->Null_disWbPrm.appCb = disWbFrameCb;
    pUcObj->Null_disWbPrm.appCbArg = pObj;
}

/**
 *******************************************************************************
 *
 * \brief   Call back function from Null_isp link for saving the ISP output
 *          frames
 *
 *          It is called in Create function.
 *          In this function SwMs alg link params are set
 *          The algorithm which is to run on core is set to
 *          baseClassCreate.algId. The input whdth and height to alg are set.
 *          Number of input buffers required by alg are also set here.
 *
 *******************************************************************************
*/
static Void capFrameCb(System_LinkChInfo *pChInfo, Void *pBuf, Void *arg)
{
    Int32 status;
    UInt32 chId, dstPitch;
    Utils_DmaCopyFill2D dmaPrm;
    System_Buffer *pFrameBuf = (System_Buffer*)pBuf;
    System_VideoFrameBuffer *pVidFrame = \
                                (System_VideoFrameBuffer*)pFrameBuf->payload;
    chains_srvCalibration_DisplayAppObj *pObj = \
                        (chains_srvCalibration_DisplayAppObj*)arg;

    if (TRUE == pObj->saveFrame)
    {
        chId = pObj->saveFrameChId;

        if (chId == pFrameBuf->chNum)
        {
            pObj->saveFrame = FALSE;

            dmaPrm.dataFormat = \
                    System_Link_Ch_Info_Get_Flag_Data_Format(pChInfo->flags);

            /*
             *  Making sure to copy the frame with pitch equal to the capture
             *  width
             */
            if ((SYSTEM_DF_YUV422I_YUYV == dmaPrm.dataFormat)   |
                (SYSTEM_DF_YUV422I_UYVY == dmaPrm.dataFormat))
            {
                dstPitch = (pObj->captureOutWidth << 1);
            }
            else
            {
                dstPitch = pObj->captureOutWidth;
            }

            dmaPrm.destAddr[0] = \
                    (Ptr)pObj->calMatGenPrm3DSrv.inImgPtr[chId];
            dmaPrm.destAddr[1]= \
                (Ptr)((UInt32)pObj->calMatGenPrm3DSrv.inImgPtr[chId] + \
                        (dstPitch * pObj->captureOutHeight));
            dmaPrm.destPitch[0] = dstPitch;
            dmaPrm.destPitch[1] = dstPitch;
            dmaPrm.destStartX   = 0;
            dmaPrm.destStartY   = 0;
            dmaPrm.width        = pChInfo->width;
            dmaPrm.height       = pChInfo->height;
            dmaPrm.srcAddr[0]   = pVidFrame->bufAddr[0];
            dmaPrm.srcAddr[1]   = pVidFrame->bufAddr[1];
            dmaPrm.srcPitch[0]  = pChInfo->pitch[0];
            dmaPrm.srcPitch[1]  = pChInfo->pitch[1];
            dmaPrm.srcStartX    = pChInfo->startX;
            dmaPrm.srcStartY    = pChInfo->startY;

            status = Utils_dmaCopy2D(
                            &pObj->dumpFramesDmaObj,
                            &dmaPrm,
                            1U);
            UTILS_assert (SYSTEM_LINK_STATUS_SOK == status);

            BspOsal_semPost(pObj->saveFrameSync);
        }
    }

    if (TRUE == pObj->printStatistics)
    {
        chains_srvCalibration_Display_tda2x_printStatistics(&pObj->ucTda2xObj);
        pObj->printStatistics = FALSE;
    }
}

/**
 *******************************************************************************
 *
 * \brief   Dump YUV frames from the ISP output to DDR and save to MMCSD
 *
 *          It is called in Create function.
 *          In this function SwMs alg link params are set
 *          The algorithm which is to run on core is set to
 *          baseClassCreate.algId. The input whdth and height to alg are set.
 *          Number of input buffers required by alg are also set here.
 *
 *******************************************************************************
*/
static Void saveIspOutFrames(
                chains_srvCalibration_DisplayAppObj *pObj)
{
    /*
     *  Define this to dump multiple sets of YUV frames
     */
#define DUMP_MULTI_FRAMES

    UInt32 cnt;
    char fileName[128U];
    Int32 fp;

    for(cnt = 0U;cnt < pObj->numCh;cnt ++)
    {
        /*
         *  Initiate the frame save and wait for completion
         */
        pObj->saveFrameChId = cnt;
        pObj->saveFrame = TRUE;
        BspOsal_semWait(pObj->saveFrameSync, BSP_OSAL_WAIT_FOREVER);

        switch(cnt)
        {
            default:
            case 0:
                sprintf(fileName,"%s_%d.YUV",
                        SRV_IMG_FILE_FRONT,pObj->yuvDumpCount);
                break;
            case 1:
                sprintf(fileName,"%s_%d.YUV",
                        SRV_IMG_FILE_RIGHT,pObj->yuvDumpCount);
                break;
            case 2:
                sprintf(fileName,"%s_%d.YUV",
                        SRV_IMG_FILE_BACK,pObj->yuvDumpCount);
                break;
            case 3:
                sprintf(fileName,"%s_%d.YUV",
                        SRV_IMG_FILE_LEFT,pObj->yuvDumpCount);
                break;
        }

        fp = File_open(fileName,"wb");
        UTILS_assert(0 <= fp);

        Vps_printf("SRV_CALIB_UC: Writing YUV image %d to the file %s ...\n",\
                    cnt, fileName);

        ChainsCommon_SurroundView_fileWrite(
                        fp,
                        (UInt8*)pObj->calMatGenPrm3DSrv.inImgPtr[cnt],
                        (pObj->captureOutWidth * pObj->captureOutHeight * 3)/2);

        File_close(fp);

        Vps_printf("SRV_CALIB_UC: Writing YUV image %d to the file %s DONE\n",\
                    cnt, fileName);
    }

#ifdef DUMP_MULTI_FRAMES
    pObj->yuvDumpCount ++;
#endif

    Vps_printf("SRV_CALIB_UC: Writing YUV image is completed\n");
}

/**
 *******************************************************************************
 *
 * \brief   Campute the LDC LUT on DSP_1
 *
 *          It is called in Create function.
 *          In this function SwMs alg link params are set
 *          The algorithm which is to run on core is set to
 *          baseClassCreate.algId. The input whdth and height to alg are set.
 *          Number of input buffers required by alg are also set here.
 *
 *******************************************************************************
*/
static Int32 computeLdcLut(
                chains_srvCalibration_DisplayAppObj *pObj,
                AlgorithmLink_SrvCommonViewPointParams *pSrvViewPointParams,
                Int32 *pCalMat,
                UInt32 calMatSize,
                UInt32 v2wMeshSize,
                UInt32 ldcLutSize)
{
    UInt32 camId;

    pObj->gAlignGenLutPrm3DSrv.numCameras = SRV_NUM_CAMERAS;
    pObj->gAlignGenLutPrm3DSrv.subsampleratio = SRV_3D_LDC_SUB_SAMPLE_RATIO;
    pObj->gAlignGenLutPrm3DSrv.SVOutDisplayHeight = SRV_HOR_HEIGHT;
    pObj->gAlignGenLutPrm3DSrv.SVOutDisplayWidth = SRV_HOR_WIDTH;
    pObj->gAlignGenLutPrm3DSrv.SVInCamFrmHeight = pObj->captureOutHeight;
    pObj->gAlignGenLutPrm3DSrv.SVInCamFrmWidth = pObj->captureOutWidth;
    pObj->gAlignGenLutPrm3DSrv.calMatSize = calMatSize;
    pObj->gAlignGenLutPrm3DSrv.v2wMeshSize = v2wMeshSize;
    pObj->gAlignGenLutPrm3DSrv.ldcLutSize = ldcLutSize;
    pObj->gAlignGenLutPrm3DSrv.carPosx = pSrvViewPointParams->carPosX;
    pObj->gAlignGenLutPrm3DSrv.carPosy = pSrvViewPointParams->carPosY;
    pObj->gAlignGenLutPrm3DSrv.view2worldmesh =                          \
                    pSrvViewPointParams->viewToWorldMeshTable;
    pObj->gAlignGenLutPrm3DSrv.calmat = pCalMat;

    for (camId = 0U;camId < pObj->gAlignGenLutPrm3DSrv.numCameras;camId++)
    {
        pObj->gAlignGenLutPrm3DSrv.LDCLUT3D[camId] =                     \
                        pSrvViewPointParams->pCompBuf->metaBufAddr[camId];
        pObj->gAlignGenLutPrm3DSrv.LDC3DWidth[camId] =                  \
                        pSrvViewPointParams->ldcOutFrameWidth[camId];
        pObj->gAlignGenLutPrm3DSrv.LDC3DHeight[camId] =                 \
                        pSrvViewPointParams->ldcOutFrameHeight[camId];
    }

    System_linkControl(
            SYSTEM_LINK_ID_DSP1,
            TDA3X_SV_3D_GEN_LDCLUT,
            &pObj->gAlignGenLutPrm3DSrv,
            sizeof(AlgorithmLink_lutGenPrm_3DSRV),
            TRUE);

    return 0;
}

/**
 *******************************************************************************
 *
 * \brief   Read V2W Mesh and write LDC LUT to file
 *
 *          It is called in Create function.
 *          In this function SwMs alg link params are set
 *          The algorithm which is to run on core is set to
 *          baseClassCreate.algId. The input whdth and height to alg are set.
 *          Number of input buffers required by alg are also set here.
 *
 *******************************************************************************
*/
static Int32 mediaReadv2wWriteLdcLutViewPointParams(
                chains_srvCalibration_DisplayAppObj *pChainsObj,
                ChainsCommon_SurroundView_mediaObj *pMediaObj,
                AlgorithmLink_SrvCommonViewPointParams *pSrvViewPointsParam,
                System_VideoFrameCompositeBuffer *pVidCompBuf,
                UInt32 numViewPoints,
                UInt32 numCameras,
                UInt32 v2wMeshSize,
                UInt32 ldcLutSize,
                UInt32 carImgSize,
                Int32 *pCalMat,
                UInt32 calMatSize)
{
    Int32 retVal;
    UInt32 cnt, idx;
    UInt32 alignedV2WMeshSize, alignedCarImgSize;
    UInt32 alignedLdcLutSize, alignedHeaderSize;
    UInt32 rawMemSize, rawMemSizeLdclut;
    UInt32 start, end;

    /* DSP will use this CAR Image, hence aliging on 128 boundary */

    alignedLdcLutSize = SystemUtils_align(ldcLutSize, 128U);
    alignedV2WMeshSize = SystemUtils_align(v2wMeshSize, 128U);
    alignedCarImgSize = SystemUtils_align(carImgSize, 128U);
    alignedHeaderSize = SystemUtils_align(SRV_MEDIA_SIZE_HEADER, 128U);

    rawMemSizeLdclut = alignedHeaderSize + \
                       alignedCarImgSize + \
                       (alignedLdcLutSize * numCameras);

    rawMemSize = rawMemSizeLdclut + \
                 alignedV2WMeshSize;

    pSrvViewPointsParam->pCompBuf = pVidCompBuf;

    /*
     *  Allocate single super buffer
     */
    pSrvViewPointsParam->baseBufAddr = ChainsCommon_SurroundView_memAlloc(
                                                    rawMemSize,
                                                    128U);
    UTILS_assert(NULL != pSrvViewPointsParam->baseBufAddr);

    /* Write the num view points into the LUT index file */
    ChainsCommon_SurroundView_fileWriteNumViewPoints(
                        pMediaObj,
                        numViewPoints,
                        SRV_MEDIA_FILE_LUT);

    start = Utils_getCurTimeInMsec();

    for(cnt = 0U;cnt < numViewPoints;cnt ++)
    {
        Vps_printf("SRV_CALIB_UC: Reading V2W Mesh for view point %d ...\n",cnt);

        /*
         *  Read the V2W Mesh from the file
         */
        retVal = ChainsCommon_SurroundView_mediaReadBlock(
                                pMediaObj,
                                pSrvViewPointsParam,
                                cnt,
                                SRV_MEDIA_FILE_V2W);
        if(SYSTEM_LINK_STATUS_SOK == retVal)
        {
            pSrvViewPointsParam->viewToWorldMeshTable = (float*)            \
                                ((UInt32)pSrvViewPointsParam->baseBufAddr + \
                                alignedHeaderSize);

            pSrvViewPointsParam->carImgPtr = (UInt32 *)                     \
                                ((UInt32)pSrvViewPointsParam->baseBufAddr + \
                                alignedHeaderSize +                         \
                                alignedV2WMeshSize);

            for(idx = 0U; idx < numCameras; idx++)
            {
                pSrvViewPointsParam->pCompBuf->metaBufAddr[idx] = (Void*)   \
                                ((UInt32)pSrvViewPointsParam->baseBufAddr + \
                                alignedHeaderSize +                         \
                                alignedCarImgSize +                         \
                                alignedV2WMeshSize +                        \
                                (alignedLdcLutSize * idx));
            }

            pSrvViewPointsParam->pCompBuf->numFrames = numCameras;
        }

        Vps_printf("SRV_CALIB_UC: Generating LDC LUT for view point %d ...\n",cnt);

        retVal = computeLdcLut(
                            pChainsObj,
                            pSrvViewPointsParam,
                            pCalMat,
                            calMatSize,
                            alignedV2WMeshSize,
                            alignedLdcLutSize);
        UTILS_assert(SYSTEM_LINK_STATUS_SOK == retVal);

        Vps_printf("SRV_CALIB_UC: Writing LDC LUT for view point %d ...\n",cnt);

        retVal = ChainsCommon_SurroundView_mediaWriteBlock(
                                pMediaObj,
                                pSrvViewPointsParam,
                                rawMemSizeLdclut,
                                alignedV2WMeshSize,
                                SRV_MEDIA_FILE_LUT);
        UTILS_assert(SYSTEM_LINK_STATUS_SOK == retVal);
    }

    ChainsCommon_SurroundView_memFree(
                    pSrvViewPointsParam->baseBufAddr,
                    rawMemSize);

    end = Utils_getCurTimeInMsec();

    Vps_printf("SRV_CALIB_UC: Time taken to write %d view point data = %d msec\n",
               numViewPoints, (end - start));

    return retVal;
}

/**
 *******************************************************************************
 *
 * \brief   Read V2W Mesh and write LDC LUT to file for all the view points
 *
 *          It is called in Create function.
 *          In this function SwMs alg link params are set
 *          The algorithm which is to run on core is set to
 *          baseClassCreate.algId. The input whdth and height to alg are set.
 *          Number of input buffers required by alg are also set here.
 *
 *******************************************************************************
*/
static Void generateLdcLut4ViewPoints(
                    chains_srvCalibration_DisplayAppObj *pObj)
{
    Int32 status;
    UInt32 lutIdxFilePresent, v2wIdxFilePresent, numViewPoints;
    UInt32 lineOffset, ldcLutSize;
    ChainsCommon_SurroundView_mediaObj mediaObj;

    UTILS_COMPILETIME_ASSERT (SRV_MEDIA_SIZE_HEADER >= \
                                sizeof(AlgorithmLink_SrvCommonViewPointParams));

    if (TRUE == pObj->isCalMatGenerated)
    {
        /* Media Init */
        status = ChainsCommon_SurroundView_mediaInit(
                        &mediaObj,
                        &lutIdxFilePresent,
                        &v2wIdxFilePresent,
                        &numViewPoints,
                        SRV_MEDIA_FILE_V2W);
        UTILS_assert(SYSTEM_LINK_STATUS_SOK == status);

        if (v2wIdxFilePresent == TRUE)
        {
            lineOffset = (SRV_LDC_LUT_FRM_WIDTH /
                            (1U << VPS_ISS_LDC_LUT_DOWN_SCALE_FACTOR_8)) + 1U;
            lineOffset += 15;
            lineOffset &= ~0xF;
            lineOffset *= 4;

            ldcLutSize = lineOffset * ((SRV_LDC_LUT_FRM_HEIGHT /
                                        (1U << VPS_ISS_LDC_LUT_DOWN_SCALE_FACTOR_8))
                                        + 1U);

            /*
             *  Read the View to world mesh table from the file and generate the
             *  LDC LUTs for all the view points
             */
            status = mediaReadv2wWriteLdcLutViewPointParams(
                            pObj,
                            &mediaObj,
                            &pObj->algViewParams,
                            &pObj->ldcMeshTableContainer,
                            numViewPoints,
                            SRV_NUM_CAMERAS,
                            SRV_LDC_VIEW2WORLDMESH_MAX_SIZE,
                            ldcLutSize,
                            SRV_LDC_CARIMAGE_MAX_SIZE,
                            pObj->calMatGenPrm3DSrv.calMatBuf,
                            SRV_CALMAT_SIZE);
            UTILS_assert(SYSTEM_LINK_STATUS_SOK == status);
        }
        else
        {
            Vps_printf("SRV_CALIB_UC: The V2W files are not present ...\n");
            Vps_printf("SRV_CALIB_UC: Exiting ...\n");
        }

        /* Media De init */
        ChainsCommon_SurroundView_mediaDeinit(&mediaObj);
    }
    else
    {
        Vps_printf("SRV_CALIB_UC: The CAL MAT is not generated\n");
        Vps_printf("SRV_CALIB_UC: Use option # 2 to generate CAL MAT\n");
    }
}

/**
 *******************************************************************************
 *
 * \brief   Generate the GPU LUT from the Cal mat
 *
 *          It is called in Create function.
 *          In this function SwMs alg link params are set
 *          The algorithm which is to run on core is set to
 *          baseClassCreate.algId. The input whdth and height to alg are set.
 *          Number of input buffers required by alg are also set here.
 *
 *******************************************************************************
*/
static Int32 generateGpuLut(
                    chains_srvCalibration_DisplayAppObj *pObj)
{
#ifndef A15_TARGET_OS_LINUX
    Int32 fp;
#endif

    pObj->gAlignGenLutPrm3DSrv.numCameras = SRV_NUM_CAMERAS;
    pObj->gAlignGenLutPrm3DSrv.subsampleratio = SRV_3D_GPU_SUB_SAMPLE_RATIO;
    pObj->gAlignGenLutPrm3DSrv.SVOutDisplayHeight = SRV_TDA2X_OUT_HEIGHT;
    pObj->gAlignGenLutPrm3DSrv.SVOutDisplayWidth = SRV_TDA2X_OUT_WIDTH;
    pObj->gAlignGenLutPrm3DSrv.SVInCamFrmHeight = pObj->captureOutHeight;
    pObj->gAlignGenLutPrm3DSrv.SVInCamFrmWidth = pObj->captureOutWidth;
    pObj->gAlignGenLutPrm3DSrv.calmat = pObj->calMatGenPrm3DSrv.calMatBuf;
    pObj->gAlignGenLutPrm3DSrv.calMatSize = SRV_CALMAT_SIZE;
    pObj->gAlignGenLutPrm3DSrv.GPULUT3D = (UInt16*)pObj->gpuLut3DBuf;
    pObj->gAlignGenLutPrm3DSrv.gpuLutSize = pObj->gpuLutSize;

    System_linkControl(
            SYSTEM_LINK_ID_DSP1,
            TDA2X_SV_3D_GEN_GPULUT,
            &pObj->gAlignGenLutPrm3DSrv,
            sizeof(AlgorithmLink_lutGenPrm_3DSRV),
            TRUE);

    Cache_inv(
            pObj->gpuLut3DBuf,
            pObj->gpuLutSize,
            Cache_Type_ALLD,
            TRUE);

#ifndef A15_TARGET_OS_LINUX
    Vps_printf("SRV_CALIB_UC: Writing GPU LUT to file ...\n");

    fp = File_open(SRV_FILENAME_GPULUT, "wb");
    UTILS_assert (0 <= fp);

    /* Write the GPU LUT on MMC/SD card */
    ChainsCommon_SurroundView_fileWrite(
                    fp,
                    (UInt8*)pObj->gpuLut3DBuf,
                    pObj->gpuLutSize);

    File_close(fp);

    Vps_printf("SRV_CALIB_UC: Writing GPU LUT to file DONE\n");
#endif

    return 0;
}

/**
 *******************************************************************************
 *
 * \brief   Write the Cal Mat into the file
 *
 *          It is called in Create function.
 *          In this function SwMs alg link params are set
 *          The algorithm which is to run on core is set to
 *          baseClassCreate.algId. The input whdth and height to alg are set.
 *          Number of input buffers required by alg are also set here.
 *
 *******************************************************************************
*/
static Void writeCalMat(
                chains_srvCalibration_DisplayAppObj *pObj,
                Int32 *calMatBuf,
                Int32 *persMatBuf)
{
    UInt32 cnt, numCameras, calMatSize[SRV_NUM_CAMERAS];
    Int32 fp;
    UInt8 *writeAddr;
    Int32 calmatin[] = {1048026, -16410, -29713, 13825, -631940, 836643, -31000, -836597, -631393, -407016514, 611492292, -588147015, -268, -567677, 881621, -1047489, -39986, -26066, 47731, -880713, -567078, 512084324, 473822479, -279362567, -1047793, 16801, 36880, -19731, 622004, -843939, -35399, -844002, -621223, 396087790, -11589476, 385966811, 28626, 612723, -850449, 1042830, -102540, -38776, -105823, -844729, -612164, -558003622, 80362877, 365531026};
    Int32 persmatin[] = {1526160, 42266, -107953821, -50364, 1784019, 20033216, -2578, -34, 1048576, 1914092, 27589, -62553600, -46057, 1497130, 99178447, 134, 2170, 1048576, 1579176, 26982, 21179348, -40733, 1828445, -12097588, 2717, 134, 1048576, 1957089, 60164, -84390275, -6674, 1034789, 113570362, 194, -3257, 1048576};

    Vps_printf("SRV_CALIB_UC: Writing Cal Mat to the file ...\n");

    /*
     *  Write the default cal mat to the file
     */
    fp = File_open(
            SRV_CALMAT_FILE, "wb");
    UTILS_assert (0 <= fp);

    /* Write the no of cameras */
    numCameras = SRV_NUM_CAMERAS;
    ChainsCommon_SurroundView_fileWrite(
                    fp,
                    (UInt8*)&numCameras,
                    4U);

    /* Write the size of the calMat for each camera in no of bytes */
    for (cnt = 0U;cnt < numCameras;cnt++)
    {
        calMatSize[cnt] = SRV_CALMAT_SIZE;
        ChainsCommon_SurroundView_fileWrite(
                        fp,
                        (UInt8*)&calMatSize[cnt],
                        4U);
    }

    /* Leave hole for the meta data */
    File_seek(
        fp,
        SRV_MEDIA_SIZE_METADATA);       // 128 bytes including the numCameras
                                        // and (calMatSize * numCameras)

    /* Write the calMat for each camera */
    if (calMatBuf == NULL)
    {
        writeAddr = (UInt8*)calmatin;
    }
    else
    {
        writeAddr = (UInt8*)calMatBuf;
    }

    for (cnt = 0U;cnt < numCameras;cnt ++)
    {
        ChainsCommon_SurroundView_fileWrite(
                        fp,
                        writeAddr,
                        calMatSize[cnt]);
        writeAddr += calMatSize[cnt];
    }

    File_close(
            fp);

    Vps_printf("SRV_CALIB_UC: Writing Cal Mat to the file DONE\n");

    /*
     *  Write the Pers mat to the file
     */

    Vps_printf("SRV_CALIB_UC: Writing Pers Mat to the file ...\n");

    fp = File_open(
            SRV_PERSMAT_FILE, "wb");
    UTILS_assert (0 <= fp);

    /* Write the calMat for each camera */
    if (persMatBuf == NULL)
    {
        writeAddr = (UInt8*)persmatin;
    }
    else
    {
        writeAddr = (UInt8*)persMatBuf;
    }

    /* Write Pers mat to the file */
    numCameras = SRV_NUM_CAMERAS;
    ChainsCommon_SurroundView_fileWrite(
                    fp,
                    writeAddr,
                    (SRV_PERSMAT_SIZE * SRV_NUM_CAMERAS));

    File_close(
            fp);

    Vps_printf("SRV_CALIB_UC: Writing Pers Mat to the file DONE\n");
}

/**
 *******************************************************************************
 *
 * \brief   Read Cal Mat from file
 *
 *          It is called in Create function.
 *          In this function SwMs alg link params are set
 *          The algorithm which is to run on core is set to
 *          baseClassCreate.algId. The input whdth and height to alg are set.
 *          Number of input buffers required by alg are also set here.
 *
 *******************************************************************************
*/
static Void readCalMat(
                    chains_srvCalibration_DisplayAppObj *pObj)
{
    Int32 fp;
    UInt32 cnt, readSize, numCameras;
    UInt32 calMatSize[SRV_NUM_CAMERAS];
    UInt8 *readAddr;

    Vps_printf("SRV_CALIB_UC: => Reading Cal Mat from %s file ...\n",\
                SRV_CALMAT_FILE);

    fp = File_open(
            SRV_CALMAT_FILE,
            "rb");

    if (0 <= fp)
    {
        ChainsCommon_SurroundView_fileRead(
                            fp,
                            (UInt8*)&numCameras,
                            4U,
                            &readSize);

        for (cnt = 0U;cnt < numCameras;cnt ++)
        {
            ChainsCommon_SurroundView_fileRead(
                                fp,
                                (UInt8*)&calMatSize[cnt],
                                4U,
                                &readSize);
        }

        File_seek(
            fp,
            SRV_MEDIA_SIZE_METADATA);

        readAddr = (UInt8*)pObj->calMatGenPrm3DSrv.calMatBuf;
        for (cnt = 0U;cnt < numCameras;cnt ++)
        {
            ChainsCommon_SurroundView_fileRead(
                                fp,
                                readAddr,
                                calMatSize[cnt],
                                &readSize);

            readAddr += calMatSize[cnt];
        }

        File_close(
                fp);

        pObj->isCalMatGenerated = TRUE;
    }
    else
    {
        Vps_printf("SRV_CALIB_UC: => CALMAT.BIN file is NOT present ...\n");
    }
}

/**
 *******************************************************************************
 *
 * \brief   Generate the Calibration Matrix on DSP_1
 *
 *          It is called in Create function.
 *          In this function SwMs alg link params are set
 *          The algorithm which is to run on core is set to
 *          baseClassCreate.algId. The input whdth and height to alg are set.
 *          Number of input buffers required by alg are also set here.
 *
 *******************************************************************************
*/
static Void generateCalMat(
                    chains_srvCalibration_DisplayAppObj *pObj,
                    SRV_CALIB_TYPE calibType)
{
 #ifndef A15_TARGET_OS_LINUX
    Int32 fp;
    UInt32 readSize;
#endif

    UInt32 cnt;

    pObj->isCalMatGenerated = FALSE;

    if (calibType == SRV_CALIB_TYPE_AUTO)
    {
        Vps_printf("SRV_CALIB_UC: => Dumping YUV frames ...\n");

        for(cnt = 0U;cnt < pObj->numCh;cnt ++)
        {
            /*
             *  Initiate the frame save and wait for completion
             */
            pObj->saveFrameChId = cnt;
            pObj->saveFrame = TRUE;
            BspOsal_semWait(pObj->saveFrameSync, BSP_OSAL_WAIT_FOREVER);
        }

#ifndef A15_TARGET_OS_LINUX
        Vps_printf("SRV_CALIB_UC: => Reading file %s ...\n",SRV_CHARTPOS_FILE);

        fp = File_open(
                    SRV_CHARTPOS_FILE,
                    "rb");
        if (0 <= fp)
        {
            ChainsCommon_SurroundView_fileRead(
                                fp,
                                (UInt8*)pObj->inChartPosBuf,
                                SRV_CHARTPOS_SIZE,
                                &readSize);

            File_close(
                    fp);

            Vps_printf("SRV_CALIB_UC: => Cal Mat Generation on DSP ...\n");
#else
        if(1U)
        {
#endif
            pObj->calMatGenPrm3DSrv.numCameras = SRV_NUM_CAMERAS;
            pObj->calMatGenPrm3DSrv.imgPitch[0U] = pObj->captureOutWidth;
            pObj->calMatGenPrm3DSrv.imgPitch[1U] = pObj->captureOutWidth;
            pObj->calMatGenPrm3DSrv.imgWidth = pObj->captureOutWidth;
            pObj->calMatGenPrm3DSrv.imgHeight = pObj->captureOutHeight;
            pObj->calMatGenPrm3DSrv.inChartPosBuf = pObj->inChartPosBuf;
            pObj->calMatGenPrm3DSrv.inChartPosSize = SRV_CHARTPOS_SIZE;

            /*
             *  2D pers mat generation parameters
             */
            if (Bsp_platformIsTda3xxFamilyBuild())
            {
                pObj->calMatGenPrm3DSrv.displayWidth = SRV_HOR_WIDTH_2D;
                pObj->calMatGenPrm3DSrv.displayHeight = SRV_HOR_HEIGHT_2D;
            }
            else
            {
                pObj->calMatGenPrm3DSrv.displayWidth = SRV_HOR_WIDTH_2D_TDA2X;
                pObj->calMatGenPrm3DSrv.displayHeight = SRV_HOR_HEIGHT_2D_TDA2X;
            }

            gClearCornerPts = TRUE;

            System_linkControl(
                    SYSTEM_LINK_ID_DSP1,
                    TDA3X_SV_3D_GEN_CALMAT,
                    &pObj->calMatGenPrm3DSrv,
                    sizeof(AlgorithmLink_calMatGenPrm_3DSRV),
                    TRUE);

            if (pObj->calMatGenPrm3DSrv.retStatus == SYSTEM_LINK_STATUS_SOK)
            {
                /* Invalidate the cal Mat generated on the DSP */
                Cache_inv(
                        pObj->calMatGenPrm3DSrv.calMatBuf,
                        (SRV_CALMAT_SIZE * SRV_NUM_CAMERAS),
                        Cache_Type_ALLD,
                        TRUE);

                Cache_inv(
                        pObj->calMatGenPrm3DSrv.persMatBuf,
                        (SRV_PERSMAT_SIZE * SRV_NUM_CAMERAS),
                        Cache_Type_ALLD,
                        TRUE);

                Cache_inv(
                        pObj->calMatGenPrm3DSrv.cornerPointsBuf,
                        SRV_CORNERPOINT_SIZE,
                        Cache_Type_ALLD,
                        TRUE);

#ifndef A15_TARGET_OS_LINUX
                /* Write the generated Cal Mat to the file */
                writeCalMat(
                    pObj,
                    pObj->calMatGenPrm3DSrv.calMatBuf,
                    pObj->calMatGenPrm3DSrv.persMatBuf);
#endif

                memcpy(gOutputCorners,
                       pObj->calMatGenPrm3DSrv.cornerPointsBuf,
                       SRV_CORNERPOINT_SIZE);

                gDrawCornerPts = TRUE;
                pObj->isCalMatGenerated = TRUE;
            }
            else
            {
                /*
                 *  The Auto Calibration has failed so if CALMAT.BIN file is
                 *  present then copy from it.
                 */
                Vps_printf("SRV_CALIB_UC: => Auto Calibration FAILED ...\n");
                readCalMat(pObj);
            }
        }
        else
        {
            Vps_printf("SRV_CALIB_UC: => CHARTPOS.BIN file is NOT present\n");
        }
    }
    else if (calibType == SRV_CALIB_TYPE_MANUAL)
    {
        readCalMat(pObj);
    }
}

/**
 *******************************************************************************
 *
 * \brief   Allocate the buffers used in the Calibration Use case
 *
 *          It is called in Create function.
 *          In this function SwMs alg link params are set
 *          The algorithm which is to run on core is set to
 *          baseClassCreate.algId. The input whdth and height to alg are set.
 *          Number of input buffers required by alg are also set here.
 *
 *******************************************************************************
*/
static Void allocCalibUseCaseBufs(
                            chains_srvCalibration_DisplayAppObj *pObj)
{
    UInt32 cnt;
    Int32 retVal;
    Utils_DmaChCreateParams dmaParams;
    UInt32 quadrantSizeInBytes;

    pObj->calMatGenPrm3DSrv.calMatBuf = ChainsCommon_SurroundView_memAlloc(
                            (SRV_CALMAT_SIZE * SRV_NUM_CAMERAS),
                            128U);
    UTILS_assert(NULL != pObj->calMatGenPrm3DSrv.calMatBuf);

    pObj->calMatGenPrm3DSrv.persMatBuf = ChainsCommon_SurroundView_memAlloc(
                            (SRV_PERSMAT_SIZE * SRV_NUM_CAMERAS),
                            128U);
    UTILS_assert(NULL != pObj->calMatGenPrm3DSrv.persMatBuf);

    pObj->imgSize = (pObj->captureOutWidth * pObj->captureOutHeight * 3)/2;
    for (cnt = 0;cnt < pObj->numCh;cnt++)
    {
        pObj->calMatGenPrm3DSrv.inImgPtr[cnt] = \
                            ChainsCommon_SurroundView_memAlloc(
                                            pObj->imgSize,
                                            128U);
    }

    pObj->inChartPosBuf = ChainsCommon_SurroundView_memAlloc(
                                            SRV_CHARTPOS_SIZE,
                                            128U);

    pObj->calMatGenPrm3DSrv.cornerPointsBuf = ChainsCommon_SurroundView_memAlloc(
                                                SRV_CORNERPOINT_SIZE,
                                                128U);

    pObj->calMatGenPrm3DSrv.persistentBaseAddr =
                            ChainsCommon_SurroundView_memAlloc(
                                SRV_PERSISTENT_BUF_SIZE,
                                128U);
    pObj->calMatGenPrm3DSrv.persistentMemSize = SRV_PERSISTENT_BUF_SIZE;

    pObj->calMatGenPrm3DSrv.scratchBaseAddr =
                            ChainsCommon_SurroundView_memAlloc(
                                SRV_SRCATCH_BUF_SIZE,
                                128U);
    pObj->calMatGenPrm3DSrv.srcatchMemSize = SRV_SRCATCH_BUF_SIZE;

    if (Bsp_platformIsTda2xxFamilyBuild())
    {
        /* GPU LUT buffer */
        quadrantSizeInBytes = \
        (9U * (1U + (SRV_TDA2X_OUT_WIDTH/(2U * SRV_3D_GPU_SUB_SAMPLE_RATIO))) *\
        (1U + (SRV_TDA2X_OUT_HEIGHT/(2U * SRV_3D_GPU_SUB_SAMPLE_RATIO)))) * 2U;

        pObj->gpuLutSize = quadrantSizeInBytes * 4U;

        pObj->gpuLut3DBuf = ChainsCommon_SurroundView_memAlloc(
                            pObj->gpuLutSize,
                            128U);
    }

    /* EDMA Init */
    Utils_DmaChCreateParams_Init(&dmaParams);
    retVal = Utils_dmaCreateCh(
                        &pObj->dumpFramesDmaObj,
                        &dmaParams);
    UTILS_assert(SYSTEM_LINK_STATUS_SOK == retVal);

    pObj->isCalMatGenerated = FALSE;

    /* Semaphore for frame save sync */
    pObj->saveFrameSync = BspOsal_semCreate(0, (Bool) TRUE);
    UTILS_assert(NULL != pObj->saveFrameSync);

    /* Semaphore for display write back frame save sync */
    pObj->disWbSync = BspOsal_semCreate(0, (Bool) TRUE);
    UTILS_assert(NULL != pObj->disWbSync);
}

/**
 *******************************************************************************
 *
 * \brief   Free the buffers used in the Calibration Use case
 *
 *          It is called in Create function.
 *          In this function SwMs alg link params are set
 *          The algorithm which is to run on core is set to
 *          baseClassCreate.algId. The input whdth and height to alg are set.
 *          Number of input buffers required by alg are also set here.
 *
 *******************************************************************************
*/
static Void freeCalibUseCaseBufs(
                            chains_srvCalibration_DisplayAppObj *pObj)
{
    UInt32 cnt;

    if(pObj->calMatGenPrm3DSrv.calMatBuf != NULL)
    {
        ChainsCommon_SurroundView_memFree(
                        pObj->calMatGenPrm3DSrv.calMatBuf,
                        (SRV_CALMAT_SIZE * SRV_NUM_CAMERAS));
    }

    if(pObj->calMatGenPrm3DSrv.persMatBuf != NULL)
    {
        ChainsCommon_SurroundView_memFree(
                        pObj->calMatGenPrm3DSrv.persMatBuf,
                        (SRV_PERSMAT_SIZE * SRV_NUM_CAMERAS));
    }

    for (cnt = 0;cnt < pObj->numCh;cnt++)
    {
        if (pObj->calMatGenPrm3DSrv.inImgPtr[cnt] != NULL)
        {
            ChainsCommon_SurroundView_memFree(
                        pObj->calMatGenPrm3DSrv.inImgPtr[cnt],
                        pObj->imgSize);
        }
    }

    if (pObj->inChartPosBuf != NULL)
    {
        ChainsCommon_SurroundView_memFree(
                        pObj->inChartPosBuf,
                        SRV_CHARTPOS_SIZE);
    }

    if (pObj->calMatGenPrm3DSrv.cornerPointsBuf != NULL)
    {
        ChainsCommon_SurroundView_memFree(
                        pObj->calMatGenPrm3DSrv.cornerPointsBuf,
                        SRV_CORNERPOINT_SIZE);
    }

    if (pObj->calMatGenPrm3DSrv.persistentBaseAddr != NULL)
    {
        ChainsCommon_SurroundView_memFree(
                        pObj->calMatGenPrm3DSrv.persistentBaseAddr,
                        SRV_PERSISTENT_BUF_SIZE);
    }

    if (pObj->calMatGenPrm3DSrv.scratchBaseAddr != NULL)
    {
        ChainsCommon_SurroundView_memFree(
                        pObj->calMatGenPrm3DSrv.scratchBaseAddr,
                        SRV_SRCATCH_BUF_SIZE);
    }

    if (Bsp_platformIsTda2xxFamilyBuild())
    {
        if (pObj->gpuLut3DBuf != NULL)
        {
            ChainsCommon_SurroundView_memFree(
                            pObj->gpuLut3DBuf,
                            pObj->gpuLutSize);
        }
    }

    /* EDMA Deinit */
    Utils_dmaDeleteCh(&pObj->dumpFramesDmaObj);

    /* Frame save sync semaphore */
    BspOsal_semDelete(&pObj->saveFrameSync);

    /* Display write back Frame save sync semaphore */
    BspOsal_semDelete(&pObj->disWbSync);
}

/**
 *******************************************************************************
 *
 * \brief   Unmount FAT File System
 *
 *          It is called in Create function.
 *          In this function SwMs alg link params are set
 *          The algorithm which is to run on core is set to
 *          baseClassCreate.algId. The input whdth and height to alg are set.
 *          Number of input buffers required by alg are also set here.
 *
 *******************************************************************************
*/
static Void unmountFatFs()
{
    Int32 retVal;

    retVal = File_chDir("/\0");
    UTILS_assert(0 == retVal);

    File_unmountFs();
    Vps_printf("SRV_CALIB_UC: File system Unmounted\n");
}

/**
 *******************************************************************************
 *
 * \brief   Mount the FAT File System
 *
 *          It is called in Create function.
 *          In this function SwMs alg link params are set
 *          The algorithm which is to run on core is set to
 *          baseClassCreate.algId. The input whdth and height to alg are set.
 *          Number of input buffers required by alg are also set here.
 *
 *******************************************************************************
*/
static Void mountFatFs()
{
    Int32 retVal;

    File_mountFs();
    Vps_printf("SRV_CALIB_UC: File System Mounted\n");

    /*
     *  The SV files are present in the TDA3X_3DSV folder
     */
    if (Bsp_platformIsTda3xxFamilyBuild())
    {
        retVal = File_chDir(SRV_DIR_3X);
        UTILS_assert(0 == retVal);
    }
    else
    {
        retVal = File_chDir(SRV_DIR_2X);
        UTILS_assert(0 == retVal);
    }
}

/**
 *******************************************************************************
 *
 * \brief   Auto Calibration
 *
 *          It is called in Create function.
 *          In this function SwMs alg link params are set
 *          The algorithm which is to run on core is set to
 *          baseClassCreate.algId. The input whdth and height to alg are set.
 *          Number of input buffers required by alg are also set here.
 *
 *******************************************************************************
*/
static Void autoCalibration(
                chains_srvCalibration_DisplayAppObj *pObj)
{
    /* 1.Generate the Calibration Matrix */
    Vps_printf("SRV_CALIB_UC: 1.Generating the calibration Matrix ...\n");

    generateCalMat(
            pObj,
            SRV_CALIB_TYPE_AUTO);

    if (Bsp_platformIsTda3xxFamilyBuild())
    {
        /* 2. Generate the LDC LUTs for all the views */
        Vps_printf("SRV_CALIB_UC: 2.Generating the LDC LUTs ...\n");
        generateLdcLut4ViewPoints(pObj);
    }
    else
    {
        /* 2. Generate the GPU LUTs for all the views */
        Vps_printf("SRV_CALIB_UC: 2.Generating the GPU LUT ...\n");
        generateGpuLut(pObj);
    }

    Vps_printf("SRV_CALIB_UC: *************************************\n");
    Vps_printf("SRV_CALIB_UC: 3.Auto Calibration is completed   ...\n");
    Vps_printf("SRV_CALIB_UC: *************************************\n");
}

/**
 *******************************************************************************
 *
 * \brief   Manual Calibration
 *
 *          It is called in Create function.
 *          In this function SwMs alg link params are set
 *          The algorithm which is to run on core is set to
 *          baseClassCreate.algId. The input whdth and height to alg are set.
 *          Number of input buffers required by alg are also set here.
 *
 *******************************************************************************
*/
static Void manualCalibration(
                chains_srvCalibration_DisplayAppObj *pObj)
{
    char ch;
    Bool done = FALSE;

    do{
        Vps_printf(manualCalibMenu);
        ch = Chains_readChar();

        switch(ch)
        {
            case '0':
                done = TRUE;
                break;
            case '1':
                saveIspOutFrames(pObj);
                break;
            case '2':
                generateCalMat(pObj, SRV_CALIB_TYPE_MANUAL);
                break;
            case '3':
                if (Bsp_platformIsTda3xxFamilyBuild())
                {
                    generateLdcLut4ViewPoints(pObj);
                }
                else
                {
                    generateGpuLut(pObj);
                }
                break;
            case '4':
                unmountFatFs();
                break;
            case '5':
                mountFatFs();
                break;
            case '6':
                writeCalMat(pObj, NULL, NULL);
                break;

            default:
                Vps_printf("\nUnsupported option '%c'. Please try again\n",
                            ch);
                break;
        }
    }while (done == FALSE);

    Vps_printf("SRV_CALIB_UC: *************************************\n");
    Vps_printf("SRV_CALIB_UC: Manual Calibration is completed   ...\n");
    Vps_printf("SRV_CALIB_UC: *************************************\n");
}

/**
 *******************************************************************************
 *
 * \brief   Display write back frame call back
 *
 *          It is called in Create function.
 *          In this function SwMs alg link params are set
 *          The algorithm which is to run on core is set to
 *          baseClassCreate.algId. The input whdth and height to alg are set.
 *          Number of input buffers required by alg are also set here.
 *
 *******************************************************************************
*/
static Void disWbFrameCb(System_LinkChInfo *pChInfo, Void *pBuf, Void *arg)
{
    Int32 fp;
    System_Buffer *pFrameBuf = (System_Buffer*)pBuf;
    System_VideoFrameBuffer *pVidFrame = \
                                (System_VideoFrameBuffer*)pFrameBuf->payload;
    chains_srvCalibration_DisplayAppObj *pObj = \
                        (chains_srvCalibration_DisplayAppObj*)arg;

    if (TRUE == pObj->disWbFrame)
    {
        pObj->disWbFrame = FALSE;

        Vps_printf("SRV_CALIB_UC: Writing Display frame to %s file ...\n",\
                    SRV_DISPLAYFRAME_FILE);

        fp = File_open(SRV_DISPLAYFRAME_FILE,"wb");
        UTILS_assert(0 <= fp);

        /* Dump Y plane */
        ChainsCommon_SurroundView_fileWrite(
                        fp,
                        (UInt8*)pVidFrame->bufAddr[0],
                        pObj->displayWidth * pObj->displayHeight);

        /* Dump UV plane */
        ChainsCommon_SurroundView_fileWrite(
                        fp,
                        (UInt8*)pVidFrame->bufAddr[1],
                        (pObj->displayWidth * pObj->displayHeight) >> 1);

        File_close(fp);

        Vps_printf("SRV_CALIB_UC: Writing Display frame to %s file DONE\n",\
                    SRV_DISPLAYFRAME_FILE);

        BspOsal_semPost(pObj->disWbSync);
    }
}

/**
 *******************************************************************************
 *
 * \brief   Save Display frame
 *
 *          It is called in Create function.
 *          In this function SwMs alg link params are set
 *          The algorithm which is to run on core is set to
 *          baseClassCreate.algId. The input whdth and height to alg are set.
 *          Number of input buffers required by alg are also set here.
 *
 *******************************************************************************
*/
static Void saveDisplayFrame(
                chains_srvCalibration_DisplayAppObj *pObj)
{
    /*
     *  Initiate frame copy and Wait for completion.
     */
    pObj->disWbFrame = TRUE;
    BspOsal_semWait(pObj->disWbSync, BSP_OSAL_WAIT_FOREVER);
}

Int32 *chains_srvCalibration_Display_getCornerPoints(Void)
{
    return (gOutputCorners);
}

Void chains_srvCalibration_Display_getCaptureSize(
                                            UInt32 *pWidth, UInt32 *pHeight)
{
    *pWidth = gCapWidth;
    *pHeight = gCapHeight;
}

Bool chains_srvCalibration_Display_getDrawCornerPts()
{
    return (gDrawCornerPts);
}

Void chains_srvCalibration_Display_setDrawCornerPts(Bool drawCornerPts)
{
    gDrawCornerPts = drawCornerPts;
}

Bool chains_srvCalibration_Display_getClearCornerPts()
{
    return (gClearCornerPts);
}

Void chains_srvCalibration_Display_setClearCornerPts(Bool clearCornerPts)
{
    gClearCornerPts = clearCornerPts;
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
Void chains_srvCalibration_Display(Chains_Ctrl *chainsCfg)
{
    Int32 retVal;
    char ch, chPrev = 0U;
    UInt32 done = FALSE;
    chains_srvCalibration_DisplayAppObj chainsObj;
    Chains_CaptureSrc oldCaptSrc;

    if ((ISSM2MISP_LINK_OPMODE_1PASS_WDR != chainsCfg->ispOpMode) &&
        (ISSM2MISP_LINK_OPMODE_12BIT_LINEAR != chainsCfg->ispOpMode))
    {
        Vps_printf(" CHAINS: WDR should either be disabled or "
                    "in One Pass mode!!!\n");
        return;
    }

    oldCaptSrc = chainsCfg->captureSrc;
    chainsCfg->displayType = CHAINS_DISPLAY_TYPE_HDMI_1080P;
    if ((oldCaptSrc != CHAINS_CAPTURE_SRC_UB960_TIDA00262) &&
        (oldCaptSrc != CHAINS_CAPTURE_SRC_UB960_IMI))
    {
        chainsCfg->captureSrc = CHAINS_CAPTURE_SRC_UB960_TIDA00262;
    }

    chainsObj.chainsCfg = chainsCfg;
    chainsObj.yuvDumpCount = 0U;
    chainsObj.saveFrame = FALSE;
    chainsObj.disWbFrame = FALSE;
    chainsObj.printStatistics = FALSE;

    /*
     *  The SV files are present in the TDA3X_3DSV folder
     */
    if (Bsp_platformIsTda3xxFamilyBuild())
    {
        retVal = File_chDir(SRV_DIR_3X);
        if (SYSTEM_LINK_STATUS_SOK != retVal)
        {
            Vps_printf(" CHAINS: -------------------------------------------------- \n");
            Vps_printf(" CHAINS: TDA3x Folder not present in MMC/SD \n");
            Vps_printf(" CHAINS: Create TDA3X folder and copy the following files from\n");
            Vps_printf(" CHAINS: '/vision_sdk/tools/surround_vision_tools/Srv_LUTs':\n");
            Vps_printf(" CHAINS:     1. V2W.BIN  \n");
            Vps_printf(" CHAINS:     2. V2W_IDX.BIN  \n");
            Vps_printf(" CHAINS:     3. CHARTPOS.BIN  \n");
            Vps_printf(" CHAINS: -------------------------------------------------- \n");
            Task_sleep (100U);

            return;
        }
    }
    else
    {
        retVal = File_chDir(SRV_DIR_2X);
        if (SYSTEM_LINK_STATUS_SOK != retVal)
        {
            Vps_printf(" CHAINS: -------------------------------------------------- \n");
            Vps_printf(" CHAINS: TDA2X Folder not present in MMC/SD \n");
            Vps_printf(" CHAINS: Create TDA2X folder and copy the following files from\n");
            Vps_printf(" CHAINS: '/vision_sdk/tools/surround_vision_tools/Srv_LUTs':\n");
            Vps_printf(" CHAINS:     1. CHARTPOS.BIN  \n");
            Vps_printf(" CHAINS: -------------------------------------------------- \n");
            Task_sleep (100U);

            return;
        }
    }

    gDrawCornerPts = FALSE;
    gClearCornerPts = FALSE;
    memset(
        gOutputCorners,
        0x00,
        SRV_CORNERPOINT_SIZE);

    do
    {
        done = FALSE;

        if (Bsp_platformIsTda3xxFamilyBuild())
        {
            chains_srvCalibration_Display_tda3x_Create(
                                                    &chainsObj.ucTda3xObj,
                                                    &chainsObj);
        }
        else
        {
            chains_srvCalibration_Display_tda2x_Create(
                                                    &chainsObj.ucTda2xObj,
                                                    &chainsObj);
        }

        allocCalibUseCaseBufs(&chainsObj);
        startCalibUseCase(&chainsObj);

        if (Bsp_platformIsTda3xxFamilyBuild())
        {
            ChainsCommon_SetIssSensorFps(FVID2_FPS_15);
        }

        ch = 'a';
        chPrev = '1';

        do
        {
            Vps_printf(calibUseCaseRunTimeMenu);

            ch = Chains_readChar();

            switch(ch)
            {
                case '0':
                    done = TRUE;
                    chPrev = '3';
                    break;
                case '1':
                    autoCalibration(&chainsObj);
                    break;
                case '2':
                    manualCalibration(&chainsObj);
                    break;
                case '3':
                    saveDisplayFrame(&chainsObj);
                    break;
                case '4':
                    unmountFatFs();
                    break;
                case '5':
                    mountFatFs();
                    break;

                case 'p':
                case 'P':
                    ChainsCommon_PrintStatistics();
                    if (Bsp_platformIsTda3xxFamilyBuild())
                    {
                        chains_srvCalibration_Display_tda3x_printStatistics(
                                        &chainsObj.ucTda3xObj);
                    }
                    else
                    {
                        chains_srvCalibration_Display_tda2x_printStatistics(
                                        &chainsObj.ucTda2xObj);
                    }
                    break;
                default:
                    Vps_printf("\nUnsupported option '%c'. Please try again\n",
                                ch);
                    break;
            }
        } while ((chPrev!='3') && (FALSE == done));

        stopAndDeleteCalibUseCase(&chainsObj);
        freeCalibUseCaseBufs(&chainsObj);
        Task_sleep(100);
        Vps_printf("\n Restarting...\n");

    } while (chPrev!='3');
    chainsCfg->captureSrc = oldCaptSrc;

    /*
     *  Change to the root dir
     */
    retVal = File_chDir("/\0");
    UTILS_assert(0 == retVal);
}

/*
 *  TDA2X 3D SRV Auto Calibration Functions
 */
/*******************************************************************************
 *  DEFINES
 *******************************************************************************
 */
#define SRV_TDA2X_AUTOCALIB_TASK_PRI            (4U)
#define SRV_TDA2X_AUTOCALIB_TASK_STACK_SIZE     (32U * 1024U)
#pragma DATA_ALIGN(tda2xAutoCalibTaskStack, 32)
#pragma DATA_SECTION(tda2xAutoCalibTaskStack, ".bss:taskStackSection")

 /*******************************************************************************
 *  GLOBALS
 *******************************************************************************
 */
Chains_Ctrl gChainsCfg;
chains_srvCalibration_DisplayAppObj gChainsObj;
BspOsal_TaskHandle gAutoCalibTaskHndl = NULL;
static UInt8 tda2xAutoCalibTaskStack[SRV_TDA2X_AUTOCALIB_TASK_STACK_SIZE];

/**
 *******************************************************************************
 *
 * \brief   TDA2X 3D SRV Auto Calibration Create Task
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
Void tda2x3DSrvAutoCalibCreateTask(UArg arg1, UArg arg2)
{
    Chain_Common_SRV_3DAutoCalibCreateParams *pAutoCalibCreatePrm = \
        (Chain_Common_SRV_3DAutoCalibCreateParams*)arg1;

    /*
     *  Delay provided to make sure the IPU1_0 command handler has
     *  acked the 'TDA2X_SV_3D_AUTO_CALIB_CREATE' before Graphics Source
     *  starts sending commands to all the cores to reset and start the
     *  statistics.
     */
    Task_sleep (1000U);

    gChainsObj.chainsCfg = &gChainsCfg;
    gChainsObj.chainsCfg->displayType = CHAINS_DISPLAY_TYPE_HDMI_1080P;
    gChainsObj.yuvDumpCount = 0U;
    gChainsObj.saveFrame = FALSE;
    gChainsObj.disWbFrame = FALSE;
    gChainsObj.printStatistics = FALSE;

    Cache_inv(
        pAutoCalibCreatePrm,
        sizeof(Chain_Common_SRV_3DAutoCalibCreateParams),
        Cache_Type_ALLD,
        TRUE);

    /* Save the Auto calibration create parameters */
    memcpy (&gChainsObj.autoCalibCreatePrm,
            pAutoCalibCreatePrm,
            sizeof (Chain_Common_SRV_3DAutoCalibCreateParams));

    /*
     *  Create and start the Calibration use case
     */
    chains_srvCalibration_Display_tda2x_Create(
                                            &gChainsObj.ucTda2xObj,
                                            &gChainsObj);
    allocCalibUseCaseBufs(&gChainsObj);
    startCalibUseCase(&gChainsObj);

    Vps_printf (" CHAINS: ------------------------------------------\n");
    Vps_printf (" CHAINS: Surround View Calibration Use case created\n");
    Vps_printf (" CHAINS: ------------------------------------------\n");
}
/**
 *******************************************************************************
 *
 * \brief   TDA2X 3D SRV Auto Calibration Create Function
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
static Void tda2x3DSrvAutoCalibCreate(Void *pPrm)
{
    if (NULL != gAutoCalibTaskHndl)
    {
        BspOsal_taskDelete(&gAutoCalibTaskHndl);
        gAutoCalibTaskHndl = NULL;
    }

    gAutoCalibTaskHndl = BspOsal_taskCreate(
                            (BspOsal_TaskFuncPtr)tda2x3DSrvAutoCalibCreateTask,
                            SRV_TDA2X_AUTOCALIB_TASK_PRI,
                            tda2xAutoCalibTaskStack,
                            sizeof(tda2xAutoCalibTaskStack),
                            pPrm,
                            UTILS_TSK_AFFINITY_CORE0);
}

/**
 *******************************************************************************
 *
 * \brief   TDA2X 3D SRV Auto Calibration Get Chart Buffer Function
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
static Void tda2x3DSrvAutoCalibGetBuf(Void *pPrm)
{
    Chain_Common_SRV_3DAutoCalibGetBufParams *pGetChartBufPrm = \
        (Chain_Common_SRV_3DAutoCalibGetBufParams*)pPrm;

    Cache_inv(
        pGetChartBufPrm,
        sizeof(Chain_Common_SRV_3DAutoCalibGetBufParams),
        Cache_Type_ALLD,
        TRUE);

    pGetChartBufPrm->inChartPosBuf = gChainsObj.inChartPosBuf;
    pGetChartBufPrm->inChartPosBufSize = SRV_CHARTPOS_SIZE;
    pGetChartBufPrm->calMatBuf = gChainsObj.calMatGenPrm3DSrv.calMatBuf;
    pGetChartBufPrm->calMatSize = SRV_CALMAT_SIZE;
}

/**
 *******************************************************************************
 *
 * \brief   TDA2X 3D SRV Auto Calibration Process Function
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
static Void tda2x3DSrvAutoCalibProcess(Void *pPrm)
{
    Chain_Common_SRV_3DCalibProcessParams *pAutoCalibPrm = \
                            (Chain_Common_SRV_3DCalibProcessParams*)pPrm;

    Cache_inv(
        pAutoCalibPrm,
        sizeof(Chain_Common_SRV_3DCalibProcessParams),
        Cache_Type_ALLD,
        TRUE);

    /*
     *  Invalidate the chart position buffer which is filled by A15
     */
    Cache_inv(
            gChainsObj.inChartPosBuf,
            SRV_CHARTPOS_SIZE,
            Cache_Type_ALLD,
            TRUE);

    /*
     *  Perform the auto calibration
     */
    autoCalibration(&gChainsObj);

    /*
     *  Return the GPU LUT address and size
     */
    pAutoCalibPrm->gpuLutAddr = gChainsObj.gpuLut3DBuf;
    pAutoCalibPrm->gpuLutSize = gChainsObj.gpuLutSize;
    pAutoCalibPrm->calMatBufAddr = gChainsObj.calMatGenPrm3DSrv.calMatBuf;
    pAutoCalibPrm->calMatSize = SRV_CALMAT_SIZE;
}

/**
 *******************************************************************************
 *
 * \brief   TDA2X 3D SRV Cal Mat Calibration Process Function
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
static Void tda2x3DSrvCalMatCalibProcess(Void *pPrm)
{
    Chain_Common_SRV_3DCalibProcessParams *pCalMatCalibPrm = \
                            (Chain_Common_SRV_3DCalibProcessParams*)pPrm;

    Cache_inv(
        pCalMatCalibPrm,
        sizeof(Chain_Common_SRV_3DCalibProcessParams),
        Cache_Type_ALLD,
        TRUE);

    /*
     *  The Cal Mat buffer is filled by A15
     */
    Cache_inv(
            gChainsObj.calMatGenPrm3DSrv.calMatBuf,
            SRV_CALMAT_SIZE * SRV_NUM_CAMERAS,
            Cache_Type_ALLD,
            TRUE);

    generateGpuLut(&gChainsObj);

    /*
     *  Return the GPU LUT address and size
     */
    pCalMatCalibPrm->gpuLutAddr = gChainsObj.gpuLut3DBuf;
    pCalMatCalibPrm->gpuLutSize = gChainsObj.gpuLutSize;
    pCalMatCalibPrm->calMatBufAddr = gChainsObj.calMatGenPrm3DSrv.calMatBuf;
    pCalMatCalibPrm->calMatSize = SRV_CALMAT_SIZE;
}

/**
 *******************************************************************************
 *
 * \brief   TDA2X 3D SRV Auto Calibration Dump Frames Function
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
static Void tda2x3DSrvAutoCalibDumpframes(Void *pPrm)
{
    UInt32 cnt;
    Chain_Common_SRV_3DAutoCalibDumpframesParams *pDumpFramesPrm = \
        (Chain_Common_SRV_3DAutoCalibDumpframesParams*)pPrm;

    Cache_inv(
        pDumpFramesPrm,
        sizeof(Chain_Common_SRV_3DAutoCalibDumpframesParams),
        Cache_Type_ALLD,
        TRUE);

    pDumpFramesPrm->frameSize = \
            ((gChainsObj.captureOutWidth * gChainsObj.captureOutHeight * 3)/2);

    for(cnt = 0U;cnt < gChainsObj.numCh;cnt ++)
    {
        /*
         *  Initiate the frame save and wait for completion
         */
        gChainsObj.saveFrameChId = cnt;
        gChainsObj.saveFrame = TRUE;
        BspOsal_semWait(gChainsObj.saveFrameSync, BSP_OSAL_WAIT_FOREVER);

        pDumpFramesPrm->frameAddr[cnt] = \
            (UInt32)gChainsObj.calMatGenPrm3DSrv.inImgPtr[cnt];

        Cache_wb(
            (Ptr)pDumpFramesPrm->frameAddr[cnt],
            pDumpFramesPrm->frameSize,
            Cache_Type_ALLD,
            TRUE);
    }
}

/**
 *******************************************************************************
 *
 * \brief   TDA2X 3D SRV Auto Calibration Print Statistics Function
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
static Void tda2x3DSrvAutoCalibPrintStats(Void *pPrm)
{
    /*
     *  Here only the flag is enabled actual statistics get printed from
     *  within the capture frame call back function.
     */
    gChainsObj.printStatistics = TRUE;
}

/**
 *******************************************************************************
 *
 * \brief   TDA2X 3D SRV Auto Calibration Delete Task
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
Void tda2x3DSrvAutoCalibDeleteTask(UArg arg1, UArg arg2)
{
    Task_sleep (1000U);

    /*
     *  Stop and delete the calibration use case
     */
    stopAndDeleteCalibUseCase(&gChainsObj);
    freeCalibUseCaseBufs(&gChainsObj);

    Vps_printf (" CHAINS: ------------------------------------------\n");
    Vps_printf (" CHAINS: Surround View Calibration Use case deleted\n");
    Vps_printf (" CHAINS: ------------------------------------------\n");
}

/**
 *******************************************************************************
 *
 * \brief   TDA2X 3D SRV Auto Calibration Delete Function
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
static Void tda2x3DSrvAutoCalibDelete(Void *pPrm)
{
    if (NULL != gAutoCalibTaskHndl)
    {
        BspOsal_taskDelete(&gAutoCalibTaskHndl);
        gAutoCalibTaskHndl = NULL;
    }

    gAutoCalibTaskHndl = BspOsal_taskCreate(
                            (BspOsal_TaskFuncPtr)tda2x3DSrvAutoCalibDeleteTask,
                            SRV_TDA2X_AUTOCALIB_TASK_PRI,
                            tda2xAutoCalibTaskStack,
                            sizeof(tda2xAutoCalibTaskStack),
                            pPrm,
                            UTILS_TSK_AFFINITY_CORE0);
}

/**
 *******************************************************************************
 *
 * \brief   TDA2X 3D SRV Auto Calibration Message Handler
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
static Void tda2x3dSrvAcHandler(UInt32 cmd, Void *pPrm)
{
    if (TDA2X_SV_3D_AUTO_CALIB_CREATE == cmd)
    {
        tda2x3DSrvAutoCalibCreate(pPrm);
    }
    if (TDA2X_SV_3D_AUTO_CALIB_GET_BUF == cmd)
    {
        tda2x3DSrvAutoCalibGetBuf(pPrm);
    }
    if (TDA2X_SV_3D_AUTO_CALIB_PROCESS == cmd)
    {
        tda2x3DSrvAutoCalibProcess(pPrm);
    }
    if (TDA2X_SV_3D_CALMAT_CALIB_PROCESS == cmd)
    {
        tda2x3DSrvCalMatCalibProcess(pPrm);
    }
    if (TDA2X_SV_3D_AUTO_CALIB_DUMPFRAMES == cmd)
    {
        tda2x3DSrvAutoCalibDumpframes(pPrm);
    }
    if (TDA2X_SV_3D_AUTO_CALIB_PRINTSTAT == cmd)
    {
        tda2x3DSrvAutoCalibPrintStats(pPrm);
    }
    if (TDA2X_SV_3D_AUTO_CALIB_DELETE == cmd)
    {
        tda2x3DSrvAutoCalibDelete(pPrm);
    }
}

/**
 *******************************************************************************
 *
 * \brief   TDA2X 3D SRV Auto Calibration Init
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
void Utils_tda2x3dSrvAcInit()
{
    SystemLink_registerHandler(tda2x3dSrvAcHandler);
}

/**
 *******************************************************************************
 *
 * \brief   TDA2X 3D SRV Auto Calibration DeInit
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
void Utils_tda2x3dSrvAcDeInit()
{
    SystemLink_unregisterHandler(tda2x3dSrvAcHandler);
}
