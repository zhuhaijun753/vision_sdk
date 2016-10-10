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
#include "chains_issMultCaptIsp2d3dSv_priv.h"
#include "chains_issMultCaptIsp2d3dSv.h"

#include <src/utils_common/include/utils_tsk.h>

#include <examples/tda2xx/src/alg_plugins/surroundview/include/svCommonDefs.h>
#include <examples/tda2xx/include/chains_common.h>
#include <examples/tda2xx/include/chains_common_iss.h>
#include <examples/tda2xx/include/chains_common_surround_view.h>
#include <src/links_common/system/system_priv_common.h>

/*******************************************************************************
 *  Defines / Globals
 *******************************************************************************
 */

/* Media Thread */
#define SRV_UC_3D_MEDIA_TASK_PRI            (4U)
#define SRV_UC_3D_MEDIA_TASK_STACK_SIZE     (32U * 1024U)
#pragma DATA_ALIGN(srvUc3dMediaTaskStack, 32)
#pragma DATA_SECTION(srvUc3dMediaTaskStack, ".bss:taskStackSection")
static UInt8 srvUc3dMediaTaskStack[SRV_UC_3D_MEDIA_TASK_STACK_SIZE];

/* View transitions Variables */
static UInt32 gNumViewPoints = 0U;
static UInt32 gSvAutoSwitchViews = TRUE;
static UInt32 gLastCount = 0U;
static UInt32 gViewId = 0U;
static UInt32 gDirViewPoint = 0U;

static chains_issMultCaptIsp2d3dSv_DisplayAppObj chainsObj;
/**< Instance object of this chain */

/*
 *******************************************************************************
 *  LDC Optimization parameters
 *******************************************************************************
 */
/*
 *  LDC read Max Tag count:
        0..15
 */
static UInt32 gLdcRdMaxTagCnt = 15U;

/*
 *  Route LDC read traffic on NRT1 port
 */
static ALG_PLUGIN_DEWARP_LDC_ROUTE gLdcTraffic = ALG_PLUGIN_DEWARP_LDC_ROUTE_NRT1;

/*
 *  Route ISP traffic on NRT2 port
 */
static IssM2mIspLink_Route gIspTraffic = ISSM2MISP_LINK_ROUTE_NRT2;

/*
 *  Place the LDC LUTs in OCMC RAM
 */
static Bool gUseOcmcLdcLut = TRUE;

/*
 *  Below setting is valid only if 'LOCAL_LDC_SLICE_PRM' is defied in
 *  chains_issMultCaptIsp2d3dSv_Media.c file:
 *      SRV_UC_3D_LDC_SLICE_PRM_1_16x16
 *      SRV_UC_3D_LDC_SLICE_PRM_1_16x8
 *      SRV_UC_3D_LDC_SLICE_PRM_2_16x16_16x8
 *      SRV_UC_3D_LDC_SLICE_PRM_2_16x16_16x4
 *      SRV_UC_3D_LDC_SLICE_PRM_1_16x4
 *      SRV_UC_3D_LDC_SLICE_PRM_2_16x8_16x4
 *      SRV_UC_3D_LDC_SLICE_PRM_3_16x16_16x8_16x4
 */
SRV_UC_3D_LDC_SLICE_PRM gLdcSlicePrmNo2d3d = SRV_UC_3D_LDC_SLICE_PRM_3_16x16_16x8_16x4;
static UInt32 gLdcPixelPad = 2U;

/*
 *  Luma Interpolation Type:
 *      VPS_ISS_LDC_LUMA_INTRP_BICUBIC
 *      VPS_ISS_LDC_LUMA_INTRP_BILINEAR
 */
static vpsissldcLumaIntrType_t gLdcLumaIntrType = VPS_ISS_LDC_LUMA_INTRP_BILINEAR;

/*
 *******************************************************************************
 *******************************************************************************
 */

static char gSrvUc3dRunTimeMenu[] = {
"\r\n "
"\r\n ===================="
"\r\n Chains Run-time Menu"
"\r\n ===================="
"\r\n "
"\r\n 0: Stop Chain"
"\r\n 1: Save a Captured RAW frame from channel 0 (Will be saved in DDR)"
"\r\n 2: Save a DeWarp Output Frame (Will be saved in DDR)"
"\r\n 3: Save ISP output frames (Will be saved in MMC/SD : All channels)"
"\r\n s: Stop / Start Transitions"
"\r\n n: Change to Next View Point, after transitions are stopped"
"\r\n r: Change to Previous View Point, after transitions are stopped"
"\r\n "
"\r\n "
"\r\n p: Print Performance Statistics "
"\r\n "
"\r\n Enter Choice: "
"\r\n "
};

/*******************************************************************************
 *  Local Function Implementation
 *******************************************************************************
 */
static Void setSyncParams(SyncLink_CreateParams *pSyncPrm,
                            UInt32 numCh,
                            UInt32 syncPeriod)
{
    pSyncPrm->syncDelta = syncPeriod;
    pSyncPrm->syncThreshold = pSyncPrm->syncDelta * 2U;
}

static Void updateIspWithAewbParams(IssAewbAlgOutParams *pAewbAlgOut,
                                    Void *appData)
{
    chains_issMultCaptIsp2d3dSv_DisplayAppObj *pObj =
        (chains_issMultCaptIsp2d3dSv_DisplayAppObj *)appData;

    UTILS_assert(NULL != pObj);
    UTILS_assert(NULL != pAewbAlgOut);

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

static Void updateIspDeWarpWithDccSuppliedParams(
                                    IssIspConfigurationParameters *ispCfg,
                                    IssM2mSimcopLink_ConfigParams *simcopCfg,
                                    Void                          *appData)
{
    Int32 status;
    UInt32 chId;
    chains_issMultCaptIsp2d3dSv_DisplayAppObj *pObj =
        (chains_issMultCaptIsp2d3dSv_DisplayAppObj *)appData;

    UTILS_assert(NULL != pObj);
    UTILS_assert(NULL != ispCfg);
    UTILS_assert(NULL != simcopCfg);

    for ( chId = 0U; chId < pObj->ucObj.IssCapturePrm.numCh; chId++)
    {
        ispCfg->channelId = chId;

        System_linkControl(pObj->ucObj.IssM2mIspLinkID,
                            ISSM2MISP_LINK_CMD_SET_ISPCONFIG,
                            ispCfg,
                            sizeof(IssIspConfigurationParameters),
                            TRUE);
    }

    if (NULL != simcopCfg->ldcConfig)
    {
        /* Copy LDC configuration and apply it,
           Will use same  */
        memcpy(&pObj->ldcCfg, simcopCfg->ldcConfig, sizeof(vpsissldcConfig_t));

        pObj->simcopConfig.ldcConfig = &pObj->ldcCfg;
        pObj->simcopConfig.vtnfConfig = &pObj->vtnfCfg;

        pObj->deWarpSimcopCfg.chNum         = pObj->simcopConfig.chNum;
        pObj->deWarpSimcopCfg.ldcConfig     = pObj->simcopConfig.ldcConfig;
        pObj->deWarpSimcopCfg.vtnfConfig    = pObj->simcopConfig.vtnfConfig;

        /* Frame and send the command to the plugin */
        pObj->deWarpSimcopCfg.baseClassControl.controlCmd
                                            = ALG_LINK_DEWARP_CMD_SET_LDCCONFIG;
        pObj->deWarpSimcopCfg.baseClassControl.size
                                            = sizeof(pObj->deWarpSimcopCfg);

        status = System_linkControl(pObj->ucObj.Alg_DeWarpLinkID,
                                    ALGORITHM_LINK_CMD_CONFIG,
                                    &pObj->deWarpSimcopCfg,
                                    sizeof(pObj->deWarpSimcopCfg),
                                    TRUE);
        UTILS_assert(SYSTEM_LINK_STATUS_SOK == status);
    }
}

static Void updateIspDeWarpWithDefaultParams(
            chains_issMultCaptIsp2d3dSv_DisplayAppObj *pObj)
{
    Int32 status;
    UInt32 numCh;
    AlgorithmLink_SrvCommonViewPointParams *pDefViewCfg;


    pDefViewCfg = &chainsObj.algViewParams[SRV_UC_3D_DEFAULT_VIEW_TO_USE];

    /* set default params */
    IssM2mSimcopLink_ConfigParams_Init(&pObj->simcopConfig);
    vpsissLdcCfg_init(&pObj->ldcCfg);
    vpsissVtnfCfg_init(&pObj->vtnfCfg);

    pObj->simcopConfig.ldcConfig = &pObj->ldcCfg;
    pObj->simcopConfig.vtnfConfig = &pObj->vtnfCfg;

    /* Set the Default SimCop configuration,
        This could get overwritten by DCC */
    ChainsCommon_SetIssSimcopConfig(&pObj->simcopConfig, pObj->bypassVtnf,
                                    pObj->bypassLdc, 0);

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
        pObj->ldcCfg.pixelPad = gLdcPixelPad;
    }
    pObj->ldcCfg.advCfg.outputBlockWidth = 16;
    pObj->ldcCfg.advCfg.outputBlockHeight = 16;
    pObj->ldcCfg.lumeIntrType = gLdcLumaIntrType;

    Vps_printf (" ********** LDC Optimization **************\n");
    Vps_printf (" gLdcRdMaxTagCnt    = %d\n",gLdcRdMaxTagCnt);
    Vps_printf (" gLdcTrafficOnNrt1  = %d\n",gLdcTraffic);
    Vps_printf (" gIspTrafficOnNrt2  = %d\n",gIspTraffic);
    Vps_printf (" gUseOcmcLdcLut     = %d\n",gUseOcmcLdcLut);
    Vps_printf (" gLdcSlicePrmNo2d3d = %d\n",gLdcSlicePrmNo2d3d);
    Vps_printf (" gLdcPixelPad       = %d\n",gLdcPixelPad);
    Vps_printf (" lumeIntrType(0:bicubic,1:bilinear) = %d\n",\
            pObj->ldcCfg.lumeIntrType);
    Vps_printf (" ******************************************\n");

    /* MUST be called after link create and before link start */
    /* Apply the same config to all channel,
        right now the sensor are the same so its fine. */
    for (numCh = 0U; numCh < pObj->ucObj.IssCapturePrm.numCh; numCh++)
    {
        pObj->ispConfig.channelId = numCh;
        pObj->simcopConfig.chNum = numCh;

        status = System_linkControl(pObj->ucObj.IssM2mIspLinkID,
                                    ISSM2MISP_LINK_CMD_SET_ISPCONFIG,
                                    &pObj->ispConfig,
                                    sizeof(pObj->ispConfig),
                                    TRUE);
        UTILS_assert(SYSTEM_LINK_STATUS_SOK == status);


        /* Configure the LDC Luts */
        ChainsCommon_SetIssLdcLutConfig(&pObj->ldcCfg.lutCfg, 3 + numCh);

        pObj->ldcCfg.lutCfg.downScaleFactor = \
                                            VPS_ISS_LDC_LUT_DOWN_SCALE_FACTOR_8;
        pObj->ldcCfg.lutCfg.lineOffset = \
                            SRV_UC_3D_MESH_TABLE_PITCH(pObj->captureOutWidth, \
                            (1U << pObj->ldcCfg.lutCfg.downScaleFactor));

        pObj->ldcCfg.advCfg.outputFrameWidth =
                                        pDefViewCfg->ldcOutFrameWidth[numCh];
        pObj->ldcCfg.advCfg.outputFrameHeight =
                                        pDefViewCfg->ldcOutFrameHeight[numCh];
        pObj->ldcCfg.advCfg.outputBlockWidth = SRV_UC_3D_LDC_BLOCK_WIDTH;
        pObj->ldcCfg.advCfg.outputBlockHeight = SRV_UC_3D_LDC_BLOCK_HEIGHT;

        pObj->deWarpSimcopCfg.chNum       = pObj->simcopConfig.chNum;
        pObj->deWarpSimcopCfg.ldcConfig   = pObj->simcopConfig.ldcConfig;
        pObj->deWarpSimcopCfg.vtnfConfig  = pObj->simcopConfig.vtnfConfig;
        /* Frame a plugin command and sent it out */
        pObj->deWarpSimcopCfg.baseClassControl.controlCmd
                                            = ALG_LINK_DEWARP_CMD_SET_LDCCONFIG;
        pObj->deWarpSimcopCfg.baseClassControl.size
                                            = sizeof(pObj->deWarpSimcopCfg);
        status = System_linkControl(pObj->ucObj.Alg_DeWarpLinkID,
                                    ALGORITHM_LINK_CMD_CONFIG,
                                    &pObj->deWarpSimcopCfg,
                                    sizeof(pObj->deWarpSimcopCfg),
                                    TRUE);
        UTILS_assert(SYSTEM_LINK_STATUS_SOK == status);
    }

}

static Void saveIspOutputIntoMmcsd(
                            chains_issMultCaptIsp2d3dSv_DisplayAppObj *pObj)
{
    UInt32 cnt, chanId;
    char fileName[128];
    Int32 fp;
    IssM2mIspLink_GetSaveFrameStatus saveFrameStatus;

    for (cnt = 0;cnt < pObj->ucObj.IssCapturePrm.numCh;cnt ++)
    {
        chanId = cnt;
        System_linkControl(pObj->ucObj.IssM2mIspLinkID,
                            ISSM2MISP_LINK_CMD_SAVE_FRAME,
                            &chanId,
                            sizeof(chanId),
                            TRUE);

        /* Wait for frame dump completion */
        saveFrameStatus.chId = chanId;
        do {
            System_linkControl(pObj->ucObj.IssM2mIspLinkID,
                                ISSM2MISP_LINK_CMD_GET_SAVE_FRAME_STATUS,
                                &saveFrameStatus,
                                sizeof(IssM2mIspLink_GetSaveFrameStatus),
                                TRUE);
            /* 30 FPS is the assumption */
            Task_sleep(35U);
        }while (saveFrameStatus.isSaveFrameComplete == FALSE);

        switch(cnt)
        {
            default:
            case 0:
                strcpy(fileName,"FRONT.YUV");
                break;
            case 1:
                strcpy(fileName,"RIGHT.YUV");
                break;
            case 2:
                strcpy(fileName,"BACK.YUV");
                break;
            case 3:
                strcpy(fileName,"LEFT.YUV");
                break;
        }

        fp = File_open(fileName,"wb");
        UTILS_assert(0 <= fp);

        Vps_printf("Writing YUV image %d to the file %s ...\n",cnt, fileName);

        /* Dump Y plane */
        ChainsCommon_SurroundView_fileWrite(
                            fp,
                            (UInt8*)saveFrameStatus.saveBufAddr[0],
                            saveFrameStatus.saveBufSize[0]);

        /* Dump UV plane */
        ChainsCommon_SurroundView_fileWrite(
                            fp,
                            (UInt8*)saveFrameStatus.saveBufAddr[1],
                            saveFrameStatus.saveBufSize[1]);

        File_close(fp);

        Vps_printf("Writing YUV image %d to the file %s DONE\n",cnt, fileName);
    }

    Vps_printf("Writing YUV image is completed\n");
}

/**
 *******************************************************************************
 *
 * \brief   LDC Advanced Configuration
 *
 *          Function sends a control command to capture and display link to
 *          to Start all the required links . Links are started in reverce
 *          order as information of next link is required to connect.
 *          System_linkStart is called with LinkId to start the links.
 *
 * \param   pObj  [IN] chains_issMultCaptIsp2d3dSv_DisplayAppObj
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
*/
static Void ldcOptimizationConfig(
                chains_issMultCaptIsp2d3dSv_DisplayAppObj *pObj,
                ALG_PLUGIN_DEWARP_LDC_ROUTE ldcTraffic,
                IssM2mIspLink_Route ispTraffic)
{
    Int32 status;
    AlgLink_DeWarpSetLdcRouteCfg ldcRouteCfg;
    IssM2mIspLink_RouteCfg ispRouteCfg;
    AlgLink_DeWarpTaskPriority dewarpTaskPri;

    ldcRouteCfg.route = ldcTraffic;
    ldcRouteCfg.baseClassControl.controlCmd = \
                                        ALG_LINK_DEWARP_CMD_LDC_ROUTECFG;
    ldcRouteCfg.baseClassControl.size = \
                                    sizeof (AlgLink_DeWarpSetLdcRouteCfg);

    status = System_linkControl(
                            pObj->ucObj.Alg_DeWarpLinkID,
                            ALGORITHM_LINK_CMD_CONFIG,
                            &ldcRouteCfg,
                            sizeof(AlgLink_DeWarpSetLdcRouteCfg),
                            TRUE);
    UTILS_assert(SYSTEM_LINK_STATUS_SOK == status);

    ispRouteCfg.route = ispTraffic;
    status = System_linkControl(
                            pObj->ucObj.IssM2mIspLinkID,
                            ISSM2MISP_LINK_CMD_SET_ISP_ROUTE_CFG,
                            &ispRouteCfg,
                            sizeof(IssM2mIspLink_RouteCfg),
                            TRUE);
    UTILS_assert(SYSTEM_LINK_STATUS_SOK == status);

    /*
     *  Set higher priority for the Dewarp alg plug-in
     */
    dewarpTaskPri.baseClassControl.controlCmd = ALG_LINK_DEWARP_CMD_GET_TASKPRI;
    dewarpTaskPri.baseClassControl.size = sizeof(AlgLink_DeWarpTaskPriority);

    status = System_linkControl(
                            pObj->ucObj.Alg_DeWarpLinkID,
                            ALGORITHM_LINK_CMD_CONFIG,
                            &dewarpTaskPri,
                            sizeof(AlgLink_DeWarpTaskPriority),
                            TRUE);
    UTILS_assert(SYSTEM_LINK_STATUS_SOK == status);

    Vps_printf(" Dewarp Task Priority = %d\n",dewarpTaskPri.taskPriority);

    if (ALGORITHM_LINK_TSK_PRI == dewarpTaskPri.taskPriority)
    {
        dewarpTaskPri.taskPriority += 2U;
    }
    dewarpTaskPri.baseClassControl.controlCmd = ALG_LINK_DEWARP_CMD_SET_TASKPRI;
    dewarpTaskPri.baseClassControl.size = sizeof(AlgLink_DeWarpTaskPriority);

    status = System_linkControl(
                            pObj->ucObj.Alg_DeWarpLinkID,
                            ALGORITHM_LINK_CMD_CONFIG,
                            &dewarpTaskPri,
                            sizeof(AlgLink_DeWarpTaskPriority),
                            TRUE);
    UTILS_assert(SYSTEM_LINK_STATUS_SOK == status);
}

static Int32 createMediaThread(
                        chains_issMultCaptIsp2d3dSv_DisplayAppObj *chainsObj)
{
    Int32 status;

    chainsObj->startSrvDemo = BspOsal_semCreate(0, (Bool) TRUE);
    UTILS_assert(NULL != chainsObj->startSrvDemo);

    chainsObj->mediaTaskHndl = BspOsal_taskCreate((BspOsal_TaskFuncPtr)
                                    chains_issMultCaptIsp2d3dSv_MediaTask,
                                    SRV_UC_3D_MEDIA_TASK_PRI,
                                    srvUc3dMediaTaskStack,
                                    sizeof(srvUc3dMediaTaskStack),
                                    chainsObj,
                                    UTILS_TSK_AFFINITY_CORE0);
    UTILS_assert(NULL != chainsObj->mediaTaskHndl);

    status = Utils_prfLoadRegister(chainsObj->mediaTaskHndl,
                                   "3D_SRV_MEDIA_TSK");
    UTILS_assert(SYSTEM_LINK_STATUS_SOK == status);

    return (status);
}

static Void deleteMediaThread(
                        chains_issMultCaptIsp2d3dSv_DisplayAppObj *chainsObj)
{
    Utils_prfLoadUnRegister(chainsObj->mediaTaskHndl);
    BspOsal_taskDelete(&chainsObj->mediaTaskHndl);
    BspOsal_semDelete(&chainsObj->startSrvDemo);
}

static AlgLink_DeWarpViewPointParams *provideViewParams (UInt32 processedCount)
{
    static UInt32 lastViewId = 0U;
    static Bool intiateViewPointCfg = TRUE;
    UInt32 switchFlag;

    /* In case of warparounds */
    if (gLastCount > processedCount)
    {
        gLastCount = processedCount;
    }

    if ((TRUE == intiateViewPointCfg) && (50U == processedCount))
    {
        intiateViewPointCfg = FALSE;
    }
    if (TRUE == chainsObj.freeToSwitch)
    {
        if (FALSE == gSvAutoSwitchViews)
        {
            /*
             *  If the Auto switch is OFF then keep using
             *  the last view point params.
             */
            lastViewId = gViewId;
        }
        else
        {
            if(((gViewId == 1U) && (gDirViewPoint == 0U)) ||
                ((gViewId == (gNumViewPoints - 2U)) && (gDirViewPoint == 1U)))
            {
                switchFlag = ((gLastCount + 50U) < processedCount);
            }
            else
            {
                switchFlag = (gLastCount < processedCount);
            }

            if (switchFlag)
            {
                lastViewId = gViewId;

                if(gDirViewPoint == 0)
                {
                    gViewId++;
                }
                else
                {
                    gViewId--;
                }

                if(gViewId == (gNumViewPoints - 1U))
                {
                    /*
                     *  Switch direction to DOWN
                     */
                    gDirViewPoint = 1U;
                }

                if(gViewId == 0U)
                {
                    /*
                     *  Switch direction to UP
                     */
                    gDirViewPoint = 0U;
                }

                gLastCount = processedCount;
            }
        } /* if (TRUE == gSvAutoSwitchViews) */

        return (&chainsObj.algViewParams[lastViewId]);
    }
    else
    {
        /*
         *  Start Surround view with first view point.
         */
        return (&chainsObj.algViewParams[0U]);
    }
}

/*******************************************************************************
 *  Functions that would be called to create, apply config, start and stop the
 *      chain.
 *******************************************************************************
 */

Void chains_issMultCaptIsp2d3dSv_SetAppPrms(
                    chains_issMultCaptIsp2d3dSvObj *pUcObj, Void *appObj)
{
    UInt32 numCh, chId, flipAndMirror;
    chains_issMultCaptIsp2d3dSv_DisplayAppObj *pObj
                        = (chains_issMultCaptIsp2d3dSv_DisplayAppObj*)appObj;
    IssM2mIspLink_OperatingMode ispOpMode;
    Int16 carBoxWidth;
    Int16 carBoxHeight;
    AlgorithmLink_SrvCommonViewPointParams *pDefViewCfg;
    UInt32 lineOffset;

    pDefViewCfg = &chainsObj.algViewParams[SRV_UC_3D_DEFAULT_VIEW_TO_USE];

    pObj->captureOutWidth  = SRV_UC_3D_CAPT_SENSOR_AR140_WIDTH;
    pObj->captureOutHeight = SRV_UC_3D_CAPT_SENSOR_AR140_HEIGHT;
    if (pObj->chainsCfg->captureSrc == CHAINS_CAPTURE_SRC_UB960_IMI)
    {
        pObj->captureOutWidth  = SRV_UC_3D_CAPT_SENSOR_OV10640_WIDTH;
        pObj->captureOutHeight = SRV_UC_3D_CAPT_SENSOR_OV10640_HEIGHT;
    }

    ChainsCommon_GetDisplayWidthHeight(pObj->chainsCfg->displayType,
                                       &pObj->displayWidth,
                                       &pObj->displayHeight);

    ispOpMode = pObj->chainsCfg->ispOpMode;

    pObj->bypassVtnf = TRUE;
    pObj->bypassLdc  = FALSE;

    pObj->simcopConfig.ldcConfig = &pObj->ldcCfg;
    pObj->simcopConfig.vtnfConfig = &pObj->vtnfCfg;

    ChainsCommon_SetIssCreatePrms(NULL,
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

    ChainsCommon_SetIssIspPrms(&pUcObj->IssM2mIspPrm,
                                pObj->captureOutWidth,
                                pObj->captureOutHeight,
                                0U,
                                0U,
                                ispOpMode,
                                NULL);

    /* Override for multiple channel capture */
    ChainsCommon_MultipleCam_UpdateIssCapturePrms(pObj->chainsCfg->sensorName,
                                                    &pUcObj->IssCapturePrm,
                                                    ispOpMode,
                                                    pObj->captureOutWidth,
                                                    pObj->captureOutHeight,
                                                    TRUE);

    /*
     *  ChainsCommon_SetIssAlgAewbPrms() should be called after sensor creation.
     *  The sensor creation is done in
     *  ChainsCommon_MultipleCam_UpdateIssCapturePrms().
     */
    ChainsCommon_SetIssAlgAewbPrms(&pObj->ucObj.Alg_IssAewbPrm);

    pObj->ucObj.Alg_IssAewbPrm.runAewbOnlyForOneCh = FALSE;
    pObj->ucObj.Alg_IssAewbPrm.chId = 0U;

    pUcObj->Alg_IssAewbPrm.appData = pObj;
    pUcObj->Alg_IssAewbPrm.cfgCbFxn = updateIspWithAewbParams;
    pUcObj->Alg_IssAewbPrm.mergeCbFxn = NULL;
    pUcObj->Alg_IssAewbPrm.dccIspCfgFxn = updateIspDeWarpWithDccSuppliedParams;

    /* Since we are operating in Linear mode, disable backlight compensation */
    if ((ispOpMode == ISSM2MISP_LINK_OPMODE_12BIT_LINEAR) ||
        (ispOpMode == ISSM2MISP_LINK_OPMODE_1PASS_WDR))
    {
        pUcObj->Alg_IssAewbPrm.aeDynParams.enableBlc = FALSE;
    }

    if (pObj->chainsCfg->captureSrc == CHAINS_CAPTURE_SRC_UB960_IMI)
    {
        /* Refer CHAINS_ISS_SENSOR_SET_FLIP_MIRROR for details */
        flipAndMirror = 2U; /* Flip disable and mirroring on */
        ChainsCommon_IssUpdateFlipParams(&flipAndMirror);
    }

    /* Enable frame dump in M2M ISP link */
    pUcObj->IssM2mIspPrm.allocBufferForDump = 1;

    /* deWarp create params */
    pUcObj->Alg_DeWarpPrm.allocBufferForDump = TRUE;
    for(chId = 0U; chId < ALG_LINK_DEWARP_MAX_CH; chId++)
    {
        pUcObj->Alg_DeWarpPrm.channelParams[chId].operatingMode
                                                    = ALGLINK_DEWARP_OPMODE_LDC;

        pUcObj->Alg_DeWarpPrm.channelParams[chId].numBuffersPerCh = 3U;
    }

    pUcObj->IssCapturePrm.allocBufferForRawDump = TRUE;

    /* Some of the links initialized by this function is not required, hence
        these are not passed. Additionally some links special initialization
        which is done furthure down */
    pUcObj->GrpxSrcPrm.tda3x3DSvSrcResolution = SYSTEM_STD_WXGA_30;
    if (CHAINS_CAPTURE_SRC_UB960_IMI == pObj->chainsCfg->captureSrc)
    {
        pUcObj->GrpxSrcPrm.tda3x3DSvSrcResolution = SYSTEM_STD_720P_60;
    }

    /*
     ***************************************************************************
     *                           3D SRV
     ***************************************************************************
     */

    ChainsCommon_SurroundView_SetParams(NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        &pUcObj->SyncPrm,
                                        NULL,
                                        NULL,
                                        &pUcObj->Alg_Synthesis_3dPrm,
                                        NULL,
                                        &pUcObj->Alg_PhotoAlign_3dPrm,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        &pUcObj->GrpxSrcPrm,
                                        &pUcObj->Display_3dPrm,
                                        NULL,
                                        NULL,
                                        &pUcObj->Display_GrpxPrm,
                                        pObj->chainsCfg->displayType,
                                        pUcObj->IssCapturePrm.numCh,
                                        ALGORITHM_LINK_SRV_OUTPUT_3D_LDC,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        FALSE /* Enable CAR overlay */);

    /* Set the Sync link parameters */
    setSyncParams(&pUcObj->SyncPrm, pUcObj->IssCapturePrm.numCh, 35U);

    /*
     *  The 3D display window is adjoining 2D display window
     */
    pUcObj->Display_3dPrm.rtParams.tarWidth  = SRV_UC_3D_DISPLAY_WIDTH;
    pUcObj->Display_3dPrm.rtParams.tarHeight = SRV_UC_3D_DISPLAY_HEIGHT;
    pUcObj->Display_3dPrm.rtParams.posX      = SRV_UC_3D_DISPLAY_STARTX;
    pUcObj->Display_3dPrm.rtParams.posY      = SRV_UC_3D_DISPLAY_STARTY;
    pUcObj->Display_3dPrm.displayId          = DISPLAY_LINK_INST_DSS_VID1;

    pUcObj->GrpxSrcPrm.enableJeepOverlay = FALSE;

    carBoxWidth = SRV_UC_3D_CARBOX_WIDTH;
    carBoxHeight = SRV_UC_3D_CARBOX_HEIGHT;

    ChainsCommon_SurroundView_SetSynthParams(&pUcObj->Alg_Synthesis_3dPrm,
                                            SRV_UC_3D_LDC_OUTPUT_WIDTH_MAX,
                                            SRV_UC_3D_LDC_OUTPUT_HEIGHT_MAX,
                                            SRV_UC_3D_SYNTH_OUTPUT_WIDTH_MAX,
                                            SRV_UC_3D_SYNTH_OUTPUT_HEIGHT_MAX,
                                            SRV_UC_3D_NUM_CAMERAS,
                                            carBoxWidth,
                                            carBoxHeight,
                                            ALGORITHM_LINK_SRV_OUTPUT_3D_LDC,
                                            FALSE); /* Enable CAR overlay */

    /* Set the LDC Input Frame size parameters in Synthesys */
    for (numCh = 0U; numCh < pUcObj->IssCapturePrm.numCh; numCh ++)
    {
            pUcObj->Alg_Synthesis_3dPrm.ldcFrmWidth[numCh] =
                                        pDefViewCfg->ldcOutFrameWidth[numCh];
            pUcObj->Alg_Synthesis_3dPrm.ldcFrmHeight[numCh] =
                                        pDefViewCfg->ldcOutFrameHeight[numCh];
    }

    pUcObj->Alg_Synthesis_3dPrm.carPosX = pDefViewCfg->carPosX;
    pUcObj->Alg_Synthesis_3dPrm.carPosY = pDefViewCfg->carPosY;

    gNumViewPoints = chainsObj.numViewPoints;

    ChainsCommon_SurroundView_SetPAlignParams(&pUcObj->Alg_PhotoAlign_3dPrm,
                                            SRV_UC_3D_LDC_OUTPUT_WIDTH_MAX,
                                            SRV_UC_3D_LDC_OUTPUT_HEIGHT_MAX,
                                            SRV_UC_3D_SYNTH_OUTPUT_WIDTH_MAX,
                                            SRV_UC_3D_SYNTH_OUTPUT_HEIGHT_MAX,
                                            SRV_UC_3D_NUM_CAMERAS,
                                            carBoxWidth,
                                            carBoxHeight,
                                            ALGORITHM_LINK_SRV_OUTPUT_3D_LDC);

    pUcObj->Alg_DeWarpPrm.provideViewPointParams = &provideViewParams;
    for (numCh = 0U; numCh < pUcObj->IssCapturePrm.numCh; numCh ++)
    {
        /*ISP */
        pUcObj->IssM2mIspPrm.channelParams[numCh].useBufSizePrmsFlag = TRUE;
        pUcObj->IssM2mIspPrm.channelParams[numCh].maxBufWidth =
                                            SRV_UC_3D_ISP_OUTPUT_WIDTH_W_PAD;
        pUcObj->IssM2mIspPrm.channelParams[numCh].maxBufHeight =
                                            SRV_UC_3D_ISP_OUTPUT_HEIGHT_W_PAD;

        pUcObj->IssM2mIspPrm.channelParams[numCh].numBuffersPerCh =
            SRV_UC_3D_ISP_NUM_OUTPUT_BUFS;

        /* De Warp */
        pUcObj->Alg_DeWarpPrm.channelParams[numCh].numBuffersPerCh =
            SRV_UC_3D_DEWARP_NUM_OUTPUT_BUFS;
        pUcObj->Alg_DeWarpPrm.channelParams[numCh].overrideInFrmSize =
            TRUE;
        pUcObj->Alg_DeWarpPrm.channelParams[numCh].inputFrameWidth =
                                            SRV_UC_3D_ISP_OUTPUT_WIDTH_W_PAD;
        pUcObj->Alg_DeWarpPrm.channelParams[numCh].inputFrameHeight =
                                            SRV_UC_3D_ISP_OUTPUT_HEIGHT_W_PAD;

        pUcObj->Alg_DeWarpPrm.channelParams[numCh].useMaxOutputFrameSize = TRUE;
        pUcObj->Alg_DeWarpPrm.channelParams[numCh].maxOutputFrameWidth =
                                                SRV_UC_3D_LDC_OUTPUT_WIDTH_MAX;
        pUcObj->Alg_DeWarpPrm.channelParams[numCh].maxOutputFrameHeight =
                                                SRV_UC_3D_LDC_OUTPUT_HEIGHT_MAX;

        pUcObj->Alg_DeWarpPrm.channelParams[numCh].useOutputFrameSize = TRUE;

        pUcObj->Alg_DeWarpPrm.channelParams[numCh].outputFrameWidth =
                                        pDefViewCfg->ldcOutFrameWidth[numCh];
        pUcObj->Alg_DeWarpPrm.channelParams[numCh].outputFrameHeight =
                                        pDefViewCfg->ldcOutFrameHeight[numCh];
    }

    /* Enable filling of output buffers for the dewarp algorithm */
    /* The filling can be enabled based on the camera */
    pUcObj->Alg_DeWarpPrm.fillOutBuf[0U] = ALGLINK_DEWARP_FILLTYPE_HORZ_DOWN;
    pUcObj->Alg_DeWarpPrm.fillOutBuf[1U] = ALGLINK_DEWARP_FILLTYPE_VERT_LEFT;
    pUcObj->Alg_DeWarpPrm.fillOutBuf[2U] = ALGLINK_DEWARP_FILLTYPE_HORZ_TOP;
    pUcObj->Alg_DeWarpPrm.fillOutBuf[3U] = ALGLINK_DEWARP_FILLTYPE_VERT_RIGHT;
    pUcObj->Alg_DeWarpPrm.fillValueY = 0x00;
    pUcObj->Alg_DeWarpPrm.fillValueUV = 0x80;

    /*
     *  A strip which is portion of the output buffer is filled.
     *  The strip is horizontal for Front and Back camera.
     *  The strip is vertical for Left and Right camera.
     *  1: Whole buffer,2: half of the buffer,4:quarter of the buffer
     */
    pUcObj->Alg_DeWarpPrm.fillRatio = 8;

    /* Enable OCMC RAM for LDC LUT */
    pUcObj->Alg_DeWarpPrm.useOcmcLdcLut = gUseOcmcLdcLut;

    if (TRUE == pUcObj->Alg_DeWarpPrm.useOcmcLdcLut)
    {
        lineOffset = (SRV_UC_3D_LDC_OUTPUT_WIDTH_MAX /
                        (1U << VPS_ISS_LDC_LUT_DOWN_SCALE_FACTOR_8)) + 1U;
        lineOffset += 15;
        lineOffset &= ~0xF;
        lineOffset *= 4;

        pUcObj->Alg_DeWarpPrm.maxLdcLutSize = lineOffset * \
                            ((SRV_UC_3D_LDC_OUTPUT_HEIGHT_MAX / \
                            (1U << VPS_ISS_LDC_LUT_DOWN_SCALE_FACTOR_8)) + 1U);
    }
    else
    {
        pUcObj->Alg_DeWarpPrm.maxLdcLutSize = 0U;
    }

    /* LDC Read Max Tag Count */
    pUcObj->Alg_DeWarpPrm.ldcRdMaxTagCnt = gLdcRdMaxTagCnt;

    /*
     ***************************************************************************
     *                           2D SRV
     ***************************************************************************
     */
    carBoxWidth = SRV_UC_2D_CARBOX_WIDTH;
    carBoxHeight = SRV_UC_2D_CARBOX_HEIGHT;

    /*
     *  The 2D display starts at (0, 0)
     */
    pUcObj->Display_2dPrm.rtParams.tarWidth  = SRV_UC_2D_DISPLAY_WIDTH;
    pUcObj->Display_2dPrm.rtParams.tarHeight = SRV_UC_2D_DISPLAY_HEIGHT;
    pUcObj->Display_2dPrm.rtParams.posX      = SRV_UC_2D_DISPLAY_STARTX;
    pUcObj->Display_2dPrm.rtParams.posY      = SRV_UC_2D_DISPLAY_STARTY;
    pUcObj->Display_2dPrm.displayId          = DISPLAY_LINK_INST_DSS_VID2;

    ChainsCommon_SurroundView_SetSynthParams(&pUcObj->Alg_Synthesis_2dPrm,
                                            pObj->captureOutWidth,
                                            pObj->captureOutHeight,
                                            SRV_UC_2D_SYNTH_OUTPUT_WIDTH,
                                            SRV_UC_2D_SYNTH_OUTPUT_HEIGHT,
                                            SRV_UC_3D_NUM_CAMERAS,
                                            carBoxWidth,
                                            carBoxHeight,
                                            ALGORITHM_LINK_SRV_OUTPUT_2D,
                                            FALSE);

    ChainsCommon_SurroundView_SetGAlignParams(&pUcObj->Alg_GeoAlign_2dPrm,
                                            pObj->captureOutWidth,
                                            pObj->captureOutHeight,
                                            SRV_UC_2D_SYNTH_OUTPUT_WIDTH,
                                            SRV_UC_2D_SYNTH_OUTPUT_HEIGHT,
                                            SRV_UC_3D_NUM_CAMERAS,
                                            carBoxWidth,
                                            carBoxHeight,
                                            ALGORITHM_LINK_SRV_OUTPUT_2D);

    ChainsCommon_SurroundView_SetPAlignParams(&pUcObj->Alg_PhotoAlign_2dPrm,
                                            pObj->captureOutWidth,
                                            pObj->captureOutHeight,
                                            SRV_UC_2D_SYNTH_OUTPUT_WIDTH,
                                            SRV_UC_2D_SYNTH_OUTPUT_HEIGHT,
                                            SRV_UC_3D_NUM_CAMERAS,
                                            carBoxWidth,
                                            carBoxHeight,
                                            ALGORITHM_LINK_SRV_OUTPUT_2D);

    /*
     *  For 2D the car image is ovelaid by the Garphics
     */
    pUcObj->GrpxSrcPrm.enableJeepOverlay = TRUE;

    /*
     *  Enable 2D + 3D graphics layout and disable the other layouts
     */
    pUcObj->GrpxSrcPrm.tda3xxSvFsRotLayoutEnable = FALSE;
    pUcObj->GrpxSrcPrm.tda3xx3DSvLayoutEnable = FALSE;
    pUcObj->GrpxSrcPrm.tda3xx2D3DSvLayoutEnable = TRUE;
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
 * \param   pObj  [IN] chains_issMultCaptIsp2d3dSv_DisplayAppObj
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
*/
static Void chains_issMultCaptIsp2d3dSv_StartApp(chains_issMultCaptIsp2d3dSv_DisplayAppObj *pObj)
{

    Chains_memPrintHeapStatus();

    updateIspDeWarpWithDefaultParams(pObj);

    /*
     *  LDC Optimization configuration for the best performance
     */
    ldcOptimizationConfig(
                pObj,
                gLdcTraffic,
                gIspTraffic);

    ChainsCommon_IssStartCaptureDevice();

    ChainsCommon_StartDisplayDevice(pObj->chainsCfg->displayType);

    chains_issMultCaptIsp2d3dSv_Start(&pObj->ucObj);

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
 * \param   pObj   [IN]   chains_issMultCaptIsp2d3dSv_DisplayAppObj
 *
 *******************************************************************************
*/
static Void chains_issMultCaptIsp2d3dSv_StopAndDeleteApp(
                            chains_issMultCaptIsp2d3dSv_DisplayAppObj *pObj)
{
    chains_issMultCaptIsp2d3dSv_Stop(&pObj->ucObj);
    chains_issMultCaptIsp2d3dSv_Delete(&pObj->ucObj);

    ChainsCommon_StopDisplayCtrl();

    ChainsCommon_StopCaptureDevice(pObj->chainsCfg->captureSrc);
    ChainsCommon_IssStopAndDeleteCaptureDevice();

    ChainsCommon_StopDisplayDevice(pObj->chainsCfg->displayType);

    /* Print the HWI, SWI and all tasks load */
    /* Reset the accumulated timer ticks */
    Chains_prfLoadCalcEnable(FALSE, TRUE, TRUE);
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
 * \param   pObj   [IN]   chains_issMultCaptIsp2d3dSv_DisplayAppObj
 *
 *******************************************************************************
*/
Void chains_issMultCaptIsp2d3dSv_get2DVideoPrm(
                                UInt32 *pStartX,
                                UInt32 *pStartY,
                                UInt32 *pWidth,
                                UInt32 *pHeight)
{
    *pStartX = SRV_UC_2D_DISPLAY_STARTX;
    *pStartY = SRV_UC_2D_DISPLAY_STARTY;
    *pWidth = SRV_UC_2D_DISPLAY_WIDTH;
    *pHeight = SRV_UC_2D_DISPLAY_HEIGHT;
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
 * \param   pObj   [IN]   chains_issMultCaptIsp2d3dSv_DisplayAppObj
 *
 *******************************************************************************
*/
Void chains_issMultCaptIsp2d3dSv_get3DVideoPrm(
                                UInt32 *pStartX,
                                UInt32 *pStartY,
                                UInt32 *pWidth,
                                UInt32 *pHeight)
{
    *pStartX = SRV_UC_3D_DISPLAY_STARTX;
    *pStartY = SRV_UC_3D_DISPLAY_STARTY;
    *pWidth = SRV_UC_3D_DISPLAY_WIDTH;
    *pHeight = SRV_UC_3D_DISPLAY_HEIGHT;
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
Void Chains_issMultCaptIsp2d3d3dSv_Display(Chains_Ctrl *chainsCfg)
{
    Int32 status;
    char ch;
    UInt32 done = FALSE, chId, validChId = 0;
    Chains_CaptureSrc oldCaptSrc;
    AlgLink_DeWarpSaveFrame *pDeWarpFrameCmd;
    Bool startWithCalibration;

    chainsObj.bypassVtnf = 0; /* KW error fix */
    chainsObj.bypassLdc  = 0; /* KW error fix */
    gLastCount = 0U;
    gViewId = 0U;
    gDirViewPoint = 0U;
    gSvAutoSwitchViews = TRUE;

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

    chainsObj.numViewPoints = 1U;
    chainsObj.freeToSwitch = FALSE;

    memset((void*) &chainsObj.algViewParams, 0,
                    (sizeof(AlgLink_DeWarpViewPointParams) *
                            SRV_UC_3D_MAX_NUM_VIEW_POINTS));
    Vps_printf(" CHAINS: Initiating read of LDC Look Up Tables from MMC/SD \n");

    /*
     *  The SV files are present in the TDA3X folder
     */
    status = File_chDir(SRV_DIR_3X);
    if (SYSTEM_LINK_STATUS_SOK != status)
    {
        Vps_printf(" CHAINS: -------------------------------------------------- \n");
        Vps_printf(" CHAINS: TDA3x Folder not present in MMC/SD \n");
        Vps_printf(" CHAINS: Create TDA3X folder and copy the following files from\n");
        Vps_printf(" CHAINS: '/vision_sdk/tools/surround_vision_tools/Srv_LUTs':\n");
        Vps_printf(" CHAINS:     1. LUT.BIN  \n");
        Vps_printf(" CHAINS:     2. LUT_IDX.BIN  \n");
        Vps_printf(" CHAINS: -------------------------------------------------- \n");
        Task_sleep (100U);

        return;
    }

    /* . Create a sem for media handshake
       . Wait for LDC LUTs read completion
       . Create the chain and proceed normally
       . DeWarp callback occurs at every frame, provide required LUTs
       */
    if (SYSTEM_LINK_STATUS_SOK == status)
    {
        createMediaThread(&chainsObj);

        /* Wait for LDC LUTs */
        BspOsal_semWait(chainsObj.startSrvDemo, BSP_OSAL_WAIT_FOREVER);
    }

    done = FALSE;

    startWithCalibration = TRUE;
    ChainsCommon_SurroundView_CalibInit(startWithCalibration);

    chains_issMultCaptIsp2d3dSv_Create(&chainsObj.ucObj, &chainsObj);

    chains_issMultCaptIsp2d3dSv_StartApp(&chainsObj);

    ch = 'a';
    do
    {
        Vps_printf(gSrvUc3dRunTimeMenu);

        ch = Chains_readChar();

        switch(ch)
        {
            case '0':
                done = TRUE;
                break;
            case '1':
                System_linkControl(chainsObj.ucObj.IssCaptureLinkID,
                                    ISSCAPTURE_LINK_CMD_SAVE_FRAME,
                                    NULL,
                                    0,
                                    TRUE);
                break;

            case '2':
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

                pDeWarpFrameCmd = &chainsObj.deWarpSaveFrame;

                pDeWarpFrameCmd->chId = chId;
                pDeWarpFrameCmd->baseClassControl.controlCmd
                                        = ALG_LINK_DEWARP_CMD_SAVE_FRAME;
                pDeWarpFrameCmd->baseClassControl.size
                                        = sizeof(AlgLink_DeWarpSaveFrame);

                status = System_linkControl(
                                        chainsObj.ucObj.Alg_DeWarpLinkID,
                                        ALGORITHM_LINK_CMD_CONFIG,
                                        pDeWarpFrameCmd,
                                        sizeof(AlgLink_DeWarpSaveFrame),
                                        TRUE);
                UTILS_assert(SYSTEM_LINK_STATUS_SOK == status);

                do
                {
                    /* Assuming 30 FPS */
                    Task_sleep(35U);
                    pDeWarpFrameCmd = &chainsObj.deWarpSaveFrame;

                    pDeWarpFrameCmd->chId = chId;
                    pDeWarpFrameCmd->baseClassControl.controlCmd
                                = ALG_LINK_DEWARP_CMD_GET_SAVE_FRAME_STATUS;
                    pDeWarpFrameCmd->baseClassControl.size
                                        = sizeof(AlgLink_DeWarpSaveFrame);

                    status = System_linkControl(
                                        chainsObj.ucObj.Alg_DeWarpLinkID,
                                        ALGORITHM_LINK_CMD_CONFIG,
                                        pDeWarpFrameCmd,
                                        sizeof(AlgLink_DeWarpSaveFrame),
                                        TRUE);
                    UTILS_assert(SYSTEM_LINK_STATUS_SOK == status);
                } while (FALSE == pDeWarpFrameCmd->isSaveFrameComplete);
                break;

            case '3':
                saveIspOutputIntoMmcsd(&chainsObj);
                break;

            case 's':
            {
                gSvAutoSwitchViews = (TRUE == gSvAutoSwitchViews)?FALSE:TRUE;
                Vps_printf ("SV Auto Switch is %d \n",gSvAutoSwitchViews);
                break;
            }

            case 'n':
            {
                if (FALSE == gSvAutoSwitchViews)
                {
                    gViewId ++;

                    if(gViewId == gNumViewPoints)
                    {
                        gViewId = 0U;
                    }

                    Vps_printf ("Switched to view point %d\n",gViewId);
                }
                break;
            }

            case 'r':
            {
                if (FALSE == gSvAutoSwitchViews)
                {
                    if(gViewId == 0U)
                    {
                        gViewId = gNumViewPoints;
                    }

                    gViewId --;

                    Vps_printf ("Switched to view point %d\n",gViewId);
                }
                break;
            }

            case 'p':
            case 'P':
                ChainsCommon_PrintStatistics();
                chains_issMultCaptIsp2d3dSv_printStatistics(
                                                        &chainsObj.ucObj);
                break;

            default:
                Vps_printf("\nUnsupported option '%c'. Please try again\n",
                            ch);
                break;
        }
    } while (FALSE == done);

    chains_issMultCaptIsp2d3dSv_StopAndDeleteApp(&chainsObj);

    /*
     *  Free the memory allocated for the LDC LUTs and Car image
     */
    chains_issMultCaptIsp2d3dSv_FreeBufs(&chainsObj);

    chainsCfg->captureSrc = oldCaptSrc;

    deleteMediaThread(&chainsObj);

    ChainsCommon_SurroundView_CalibDeInit();

    /*
     *  Change to the root dir
     */
    status = File_chDir("/\0");
    UTILS_assert(SYSTEM_LINK_STATUS_SOK == status);
}
