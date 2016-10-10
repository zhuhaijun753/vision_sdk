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
 * \file chains_common_iss.c
 *
 *
 *
 *******************************************************************************
*/

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */
#include <examples/tda2xx/include/chains_common_iss.h>

#include "iss_vtnf_defaults.h"


/*******************************************************************************
 *  Defines
 *******************************************************************************
 */

#define CHAINS_COMMON_ISS_MESH_TABLE_PITCH(w, r) ((((((w)/(r))+1) + 15U) & ~15U) * (4U))

/*******************************************************************************
 *  Data Structures
 *******************************************************************************
 */

/*
 *  \brief Global Object storing sensor handle and other information
 */
typedef struct {
    Ptr                     sensorHandle;
    ChainsIssSensor_Info    sensorInfo;

    UInt16                  regAddr;
    UInt16                  regVal;
} ChainsCommon_IssObj;


/*******************************************************************************
 *  Globals
 *******************************************************************************
 */

ChainsCommon_IssObj gChainsCommon_IssObj = {NULL};

vpsissvtnfConfig_t gVtnfDefBypassed = ISS_VTNF_DEFAULT_BYPASS;
vpsissvtnfConfig_t gVtnfDefEnabled = ISS_VTNF_DEFAULT_ENABLED;

#pragma DATA_ALIGN(gChainsCommonIssLdcTable_1280x800, 32)

#ifdef ISS_INCLUDE /* LDC table included only if ISS is included */
/**< Default NULL LUT (1:1) mapping. size = (1280 * 800) / 2 */
UInt8 gChainsCommonIssLdcTable_1280x800[] =
{
    #include "iss_tables/iss_tables_ldc_lut.h"
};

#pragma DATA_ALIGN(gChainsCommonIssLdcTable_1920x1080, 32)
UInt8 gChainsCommonIssLdcTable_1920x1080[] =
{
    #include "iss_tables/iss_tables_ldc_lut_1920x1080.h"
};

#else
UInt8 gChainsCommonIssLdcTable_1280x800[1];
UInt8 gChainsCommonIssLdcTable_1920x1080[1];
#endif


/*******************************************************************************
 *  Local Functions Declarations
 *******************************************************************************
 */
static Void issCreateSensor(
    char sensorName[], IssM2mIspLink_OperatingMode ispOpMode,
    IssM2mIspLink_WdrOffsetParams_t *wdrOffsetPrms);
static Void getDefaultIspConfigFromSensor(
    IssIspConfigurationParameters *pIspConfig);
static UInt32 issGetQspiOffsetFromDccId(UInt32 sensorDccId);
static Void issCmdHandler(UInt32 cmd, Void *pPrm);
static System_VideoFrameStatus chainsCommonCsi2ErrorCb(Void *appObj,
                                System_Buffer *pBuffer,
                                System_VideoFrameStatus error);

/*******************************************************************************
 *  Function Definition
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \brief   ChainsCommon_Iss_Init
 *
 *          This function registers command handler in system
 *          link for iss comands.
 *
 * \param   NULL
 *
  *******************************************************************************
 */
Void ChainsCommon_Iss_Init()
{
    SystemLink_registerHandler(issCmdHandler);
}

/**
 *******************************************************************************
 *
 * \brief   ChainsCommon_Iss_DeInit
 *
 *          This function deregisters command handler in system
 *          link for iss comands.
 *
 * \param   NULL
 *
  *******************************************************************************
 */
Void ChainsCommon_Iss_DeInit()
{
    SystemLink_unregisterHandler(issCmdHandler);
}

/**
 *******************************************************************************
 *
 * \brief   ChainsCommon_SetIssCreatePrms
 *
 *          Function to initialize create params of the ISS links, based on the
 *          given parameters
 *
 * \param   sensorName      Name of the input sensor
 * \param   pCapturePrm     Pointer to Capture link create params
 * \param   pIspPrm         Pointer to ISP link create params
 * \param   pSimcopPrm      Pointer to SIMCOP link create params
 * \param   pAlgAewbPrm     Pointer to AEWB link create params
 * \param   pAlgSceneObstructionDetectPrm  Pointer to SOD link create params
 * \param   dispWidth       Display Width, ResizerB output size is based
 *                          on this parameter
 * \param   dispHeight      Display Width, ResizerB output size is based
 *                          on this parameter
 * \param   simcopOpMode    Simcop Operation mode
 * \param   ispOpMode       ISP Operation mmode
 * \param   wdrOffsePrms
 *
  *******************************************************************************
 */
Void ChainsCommon_SetIssCreatePrms(
    char sensorName[],
    IssCaptureLink_CreateParams *pCapturePrm,
    IssM2mIspLink_CreateParams *pIspPrm,
    IssM2mSimcopLink_CreateParams *pSimcopPrm,
    AlgorithmLink_IssAewbCreateParams *pAlgAewbPrm,
    AlgorithmLink_SceneObstructionDetectCreateParams *pAlgSceneObstructionDetectPrm,
    UInt32 dispWidth,
    UInt32 dispHeight,
    IssM2mSimcopLink_OperatingMode simcopOpMode,
    IssM2mIspLink_OperatingMode ispOpMode,
    IssM2mIspLink_WdrOffsetParams_t *wdrOffsetPrms)
{
    UInt32 width = 1280, height = 720;
    ChainsIssSensor_Info *pSensorInfo = &gChainsCommon_IssObj.sensorInfo;

    issCreateSensor(sensorName, ispOpMode, wdrOffsetPrms);

    width = pSensorInfo->cfgPrms.width;
    height = pSensorInfo->cfgPrms.height;

    /* First create the sensor and get the sensor parameters */
    if(pCapturePrm)
    {
        ChainsCommon_SetIssCapturePrms(pCapturePrm, pSensorInfo);

        width = pCapturePrm->outParams[0U].width;
        height = pCapturePrm->outParams[0U].height;
    }
    if(pIspPrm)
    {
        ChainsCommon_SetIssIspPrms(
            pIspPrm,
            width,
            height,
            ((dispWidth/3) & ~0x1),
            (dispHeight/3),
            ispOpMode,
            wdrOffsetPrms);
    }
    if(pSimcopPrm)
    {
        ChainsCommon_SetIssSimcopPrms(
            pSimcopPrm,
            simcopOpMode);
    }
    if(pAlgAewbPrm)
    {
        ChainsCommon_SetIssAlgAewbPrms(
            pAlgAewbPrm);
    }
    if(pAlgSceneObstructionDetectPrm)
    {
        ChainsCommon_SetSceneObstructionDetectPrm(
            pAlgSceneObstructionDetectPrm,
            ispOpMode,
            pIspPrm->channelParams[0].outParams.paxelNumHaf,
            pIspPrm->channelParams[0].outParams.paxelNumVaf
            );
    }
}

/**
 *******************************************************************************
 *
 * \brief   Function to initialize Capture Link Create Params from
 *          given sensor information
 *
 * \param   pCapturePrm     Pointer to Capture link create params
 * \param   pSensorInfo     Pointer to sensor information structure
 *
 *******************************************************************************
 */
Void ChainsCommon_SetIssCapturePrms(
    IssCaptureLink_CreateParams *pCapturePrm, ChainsIssSensor_Info *pSensorInfo)
{
    pCapturePrm->videoIfMode =
        (System_VideoIfMode)pSensorInfo->videoIfMode;
    pCapturePrm->videoIfWidth =
        (System_VideoIfWidth)pSensorInfo->cfgPrms.videoIfWidth;

    pCapturePrm->outParams[0U].dataFormat =
        (System_VideoDataFormat)pSensorInfo->cfgPrms.dataformat;
    pCapturePrm->outParams[0U].width = pSensorInfo->cfgPrms.width;
    pCapturePrm->outParams[0U].height = pSensorInfo->cfgPrms.height;

    pCapturePrm->outParams[0U].inCsi2DataFormat = pSensorInfo->inCsi2DataFormat;
    pCapturePrm->outParams[0U].inCsi2VirtualChanNum =
        pSensorInfo->inCsi2VirtualChanNum;

    pCapturePrm->csi2Params = pSensorInfo->csi2Prms;

    pCapturePrm->bufCaptMode = SYSTEM_CAPT_BCM_LAST_FRM_REPEAT;
    pCapturePrm->outParams[0U].maxWidth = pCapturePrm->outParams[0U].width;
    pCapturePrm->outParams[0U].maxHeight = pCapturePrm->outParams[0U].height;
    pCapturePrm->outParams[0U].numOutBuf = 3;
}

/**
 *******************************************************************************
 *
 * \brief   Function to initialize ISP Link Create Params
 *
 * \param   pCapturePrm     Pointer to ISP link create params
 * \param   outWidthRszA    Resizer A Output Width
 * \param   outHeightRszA   Resizer A Output Height
 * \param   outWidthRszB    Resizer B Output Width
 * \param   outHeightRszB   Resizer B Output Height
 * \param   ispOpMode       ISP Operation Mode
 * \param   wdrOffsetPrms   WDR Offset params used for Line interleaved wdr mode
 *
 *******************************************************************************
 */
Void ChainsCommon_SetIssIspPrms(
                        IssM2mIspLink_CreateParams *pPrm,
                        UInt16 outWidthRszA,
                        UInt16 outHeightRszA,
                        UInt16 outWidthRszB,
                        UInt16 outHeightRszB,
                        IssM2mIspLink_OperatingMode ispOpMode,
                        IssM2mIspLink_WdrOffsetParams_t *wdrOffsetPrms)
{
    UInt32 chId;

    for(chId = 0; chId < ISSM2MISP_LINK_MAX_CH; chId++)
    {
        pPrm->channelParams[chId].operatingMode   = ispOpMode;
        pPrm->channelParams[chId].inBpp           = SYSTEM_BPP_BITS12;

        if (NULL != wdrOffsetPrms)
        {
            pPrm->channelParams[chId].wdrOffsetPrms = *wdrOffsetPrms;
        }

        pPrm->channelParams[chId].numBuffersPerCh = 2;

        pPrm->channelParams[chId].outParams.widthRszA  = outWidthRszA;
        pPrm->channelParams[chId].outParams.heightRszA = outHeightRszA;
        pPrm->channelParams[chId].outParams.widthRszB  = outWidthRszB;
        pPrm->channelParams[chId].outParams.heightRszB = outHeightRszB;
        pPrm->channelParams[chId].outParams.winWidthH3a = 16;
        pPrm->channelParams[chId].outParams.winHeightH3a = 16;
        pPrm->channelParams[chId].outParams.dataFormat = SYSTEM_DF_YUV420SP_UV;

        pPrm->channelParams[chId].enableOut[ISSM2MISP_LINK_OUTPUTQUE_IMAGE_RSZ_A]  = 1;
        pPrm->channelParams[chId].enableOut[ISSM2MISP_LINK_OUTPUTQUE_IMAGE_RSZ_B]  = 0;
        pPrm->channelParams[chId].enableOut[ISSM2MISP_LINK_OUTPUTQUE_H3A]  = 1;
        pPrm->channelParams[chId].enableOut[ISSM2MISP_LINK_OUTPUTQUE_H3A_AF]  = 0;

        if(outWidthRszB && outHeightRszB)
        {
            pPrm->channelParams[chId].enableOut[ISSM2MISP_LINK_OUTPUTQUE_IMAGE_RSZ_B]  = 1;
        }

        if (ispOpMode == ISSM2MISP_LINK_OPMODE_2PASS_WDR_LINE_INTERLEAVED)
        {
            pPrm->channelParams[chId].outParams.widthRszA  = wdrOffsetPrms->width;
            pPrm->channelParams[chId].outParams.heightRszA = wdrOffsetPrms->height;
        }
    }
}

/**
 *******************************************************************************
 *
 * \brief   Function to initialize Simcop Link Create Params
 *
 * \param   pPrm            Pointer to Simcop link create params
 * \param   opMode          Simcop Operation mode
 *
 *******************************************************************************
 */
Void ChainsCommon_SetIssSimcopPrms(
                        IssM2mSimcopLink_CreateParams *pPrm,
                        IssM2mSimcopLink_OperatingMode opMode)
{
    int chId;

    if(System_isFastBootEnabled())
    {
        pPrm->allocBufferForDump = 0;
    }

    for(chId=0; chId<ISSM2MSIMCOP_LINK_MAX_CH; chId++)
    {
        pPrm->channelParams[chId].operatingMode
            = opMode;

        pPrm->channelParams[chId].numBuffersPerCh
            = ISSM2MSIMCOP_LINK_NUM_BUFS_PER_CH_DEFAULT;
    }
}

/**
 *******************************************************************************
 *
 * \brief   Function to initialize AEWB Link Create Params,
 *          This parameters are actually initialized by the sensor layer,
 *          so this API just calls to sensor layer.
 *
 * \param   pPrm            Pointer to AEWB link create params
 *
 *******************************************************************************
 */
Void ChainsCommon_SetIssAlgAewbPrms(
                        AlgorithmLink_IssAewbCreateParams *pPrm)
{
    Int32 status;

    status = ChainsIssSensor_Control(
            gChainsCommon_IssObj.sensorHandle,
            CHAINS_ISS_SENSOR_INIT_AEWB_CREATE_PARAMS,
            pPrm,
            NULL);
    UTILS_assert(SYSTEM_LINK_STATUS_SOK == status);

    pPrm->numSteps = 6;
}

/**
 *******************************************************************************
 *
 * \brief   Function to set the sensor fps
 *          This parameters are actually initialized by the sensor layer,
 *          so this API just calls to sensor layer.
 *
 * \param   pPrm            Pointer to AEWB link create params
 *
 *******************************************************************************
 */
Void ChainsCommon_SetIssSensorFps(UInt32 sesnorFps)
{
    Int32 status;

    status = ChainsIssSensor_Control(
            gChainsCommon_IssObj.sensorHandle,
            CHAINS_ISS_SENSOR_SET_FPS,
            &sesnorFps,
            NULL);
    UTILS_assert(SYSTEM_LINK_STATUS_SOK == status);
}

/**
 *******************************************************************************
 *
 * \brief   Function to initialize SOD Link Create Params,
 *
 * \param   pPrm            Pointer to AEWB link create params
 * \param   ispOpMode       ISP Operation Mode
 * \param   paxelNumH       Paxel horizontal size
 * \param   paxelNumV       Paxel vertical size
 *
 *******************************************************************************
 */
Void ChainsCommon_SetSceneObstructionDetectPrm(
    AlgorithmLink_SceneObstructionDetectCreateParams *pPrm,
    IssM2mIspLink_OperatingMode ispOpMode,
    UInt32 paxelNumH,
    UInt32 paxelNumV)
{

    if (TRUE == IssM2mIspLink_IsWdrMode(ispOpMode))
    {
        pPrm->wdrEnable= 1U;
    }
    else
    {
        pPrm->wdrEnable= 0U;
    }

    pPrm->paxNumH= paxelNumH;
    pPrm->paxNumV= paxelNumV;
    pPrm->vfEnable= 1U;
    pPrm->scaleFactor= 10.0;
    pPrm->classifierCoef[0]= 6.010115;
    pPrm->classifierCoef[1]= 1.562322;
    pPrm->classifierCoef[2]= -33.339242;
    pPrm->classifierCoef[3]= 1.963373;
    pPrm->classifierCoef[4]= -39.755250;
    pPrm->classifierCoef[5]= 0.0;
    pPrm->classifierCoef[6]= 0.0;
    pPrm->classifierCoef[7]= 0.0;
    pPrm->classifierCoef[8]= 0.0;
    pPrm->classifierCoef[9]= 0.0;
    pPrm->classifierCoef[10]= 0.0;
    pPrm->sensitivity= -2.0;
    pPrm->frameSkipInterval= 10;
    pPrm->numBlockedFramesThreshold= 3;
}

/**
 *******************************************************************************
 *
 * \brief   Function to enable/disable VTNF/LDC at run time,
 *          It copies default VTNF parameters based on the flag and also
 *          enables/disables back mapping based on the flag
 *
 * \param   ldcCfg          Pointer to LDC configuration
 * \param   vtnfCfg         Pointer to VTNF configuration
 * \param   bypassVtnf      Flag to indicate whether enable/bypass VTNF
 * \param   bypassLdc       Flag to indicate whether enable/bypass LDC
 *
 *******************************************************************************
 */
Void ChainsCommon_SetIssSimcopLdcVtnfRtConfig(
                        vpsissldcConfig_t *ldcCfg,
                        vpsissvtnfConfig_t *vtnfCfg,
                        Bool bypssVtnf,
                        Bool bypssLdc)
{
    /* LDC and VTNF Config pointers must not be null */
    UTILS_assert(NULL != ldcCfg);
    UTILS_assert(NULL != vtnfCfg);

    if(bypssLdc)
    {
        ldcCfg->enableBackMapping = FALSE;
    }
    else
    {
        ldcCfg->enableBackMapping = TRUE;
    }

    if (bypssVtnf)
    {
        memcpy(vtnfCfg, &gVtnfDefBypassed, sizeof(vpsissvtnfConfig_t));
    }
    else
    {
        memcpy(vtnfCfg, &gVtnfDefEnabled, sizeof(vpsissvtnfConfig_t));
    }
}

/**
 *******************************************************************************
 *
 * \brief   Function to set default configuration in Simcop
 *
 * \param   pPdm            Simcop Link Configuration
 * \param   bypassVtnf      Flag to indicate whether enable/bypass VTNF
 * \param   bypassLdc       Flag to indicate whether enable/bypass LDC
 * \param   ldcTableIdx     LDC Table Index
 *
 *******************************************************************************
 */
Void ChainsCommon_SetIssSimcopConfig(
                        IssM2mSimcopLink_ConfigParams *pPrm,
                        Bool bypassVtnf,
                        Bool bypassLdc,
                        UInt32 ldcTableIdx)
{
    vpsissldcConfig_t  *ldcCfg;
    vpsissvtnfConfig_t *vtnfCfg;

    /* LDC and VTNF Config pointers must not be null */
    UTILS_assert(NULL != pPrm->ldcConfig);
    UTILS_assert(NULL != pPrm->vtnfConfig);
    ldcCfg = pPrm->ldcConfig;
    vtnfCfg = pPrm->vtnfConfig;

    ldcCfg->isAdvCfgValid             = TRUE;
    ldcCfg->pixelPad                  = 8;
    ldcCfg->advCfg.outputBlockWidth   = 32;
    ldcCfg->advCfg.outputBlockHeight  = 16;
    ldcCfg->advCfg.outputStartX       = 0;
    ldcCfg->advCfg.outputStartY       = 0;
    ldcCfg->advCfg.enableCircAddrMode = FALSE;
    ldcCfg->advCfg.circBuffSize       = 0;
    ldcCfg->advCfg.enableConstOutAddr = TRUE;

    /* set LDC LUT config */
    ChainsCommon_SetIssLdcLutConfig(&ldcCfg->lutCfg, ldcTableIdx);

    if(bypassLdc)
    {
        ldcCfg->enableBackMapping = FALSE;
    }
    else
    {
        ldcCfg->enableBackMapping = TRUE;
    }

    if(bypassVtnf)
    {
        memcpy(vtnfCfg, &gVtnfDefBypassed, sizeof(vpsissvtnfConfig_t));
    }
    else
    {
        memcpy(vtnfCfg, &gVtnfDefEnabled, sizeof(vpsissvtnfConfig_t));
    }
}

/**
 *******************************************************************************
 *
 * \brief   Function to set start capture device
 *
 *******************************************************************************
 */
Void ChainsCommon_IssStartCaptureDevice()
{
    Int32 status;
    UInt32 start, end;

    start = Utils_getCurTimeInMsec();
    status = ChainsIssSensor_Start(gChainsCommon_IssObj.sensorHandle);
    UTILS_assert(0 == status);
    end = Utils_getCurTimeInMsec();
    Vps_printf(" Sensor Config time = %d msec\n",(end - start));
}

/**
 *******************************************************************************
 *
 * \brief   Function to set stop and delete sensor
 *
 *******************************************************************************
 */
Void ChainsCommon_IssStopAndDeleteCaptureDevice()
{
    Int32 status;

    status = ChainsIssSensor_Stop(gChainsCommon_IssObj.sensorHandle);
    UTILS_assert(0 == status);

    status = ChainsIssSensor_Delete(gChainsCommon_IssObj.sensorHandle);
    UTILS_assert(0 == status);

    gChainsCommon_IssObj.sensorHandle = NULL;
}

/**
 *******************************************************************************
 *
 * \brief   Function to set AEWB
 *          AEWB Algorithm layer updates gain and exposure based on
 *          the lighting condition. This needs to be set in the Sensor.
 *          This APi uses ISS sensor layer to update this parameters
 *          in the sensor.
 *
 * \param   pAewbAlgOut         Pointer to AEWB output parameters
 *
 *******************************************************************************
 */
Void ChainsCommon_IssUpdateAewbParams(IssAewbAlgOutParams *pAewbAlgOut)
{
    Int32 status;
    if (pAewbAlgOut->outPrms[0].useAeCfg)
    {
        status = ChainsIssSensor_Control(
            gChainsCommon_IssObj.sensorHandle,
            CHAINS_ISS_SENSOR_SET_AEWB_PARAMS,
            pAewbAlgOut,
            NULL);
        UTILS_assert(SYSTEM_LINK_STATUS_SOK == status);
    }
}

/**
 *******************************************************************************
 *
 * \brief   Function to get exposure parameters,
 *          It uses ISS sensor layer to get the exposure ratio from the sensor.
 *
 * \param   pAewbAlgOut         Pointer to AEWB output parameters
 *
 *******************************************************************************
 */
Void ChainsCommon_IssGetExposureParams(IssAewbAlgOutParams *pAewbAlgOut)
{
    Int32 status;

    status = ChainsIssSensor_Control(
        gChainsCommon_IssObj.sensorHandle,
        CHAINS_ISS_SENSOR_GET_EXPOSURE_PARAMS,
        pAewbAlgOut,
        NULL);
    UTILS_assert(SYSTEM_LINK_STATUS_SOK == status);
}

/**
 *******************************************************************************
 *
 * \brief   Function to get default ISP and Simcop Configuration,
 *          It uses DCC Profile either from driver or from qspi,
 *          parses it using AEWB plugin layer (uses linkid) and sets the
 *          default ISP/Simcop configuration
 *
 * \param   ispOpMode         ISP Operation Mode,
 *                            For monochrom mode, DCC profile is not used,
 *                            This is just used for informing whether it
 *                            is monochrome mode or not.
 * \param   pIspConfig        Pointer to ISP config, where default
 *                            values will be stored.
 * \param   pSimcopCfg        Pointer to Simcop config, where default
 *                            values will be stored.
 * \param   linkId            AEWB Link id, used for parsing DCC profile
 *
 *******************************************************************************
 */
Void ChainsCommon_IssGetDefaultIspSimcopConfig(
        IssM2mIspLink_OperatingMode ispOpMode,
        IssIspConfigurationParameters *pIspConfig,
        IssM2mSimcopLink_ConfigParams *pSimcopCfg,
        UInt32 linkId)
{
    Int32 status;
    UInt32 qSpiOffset, isDccProfileAvail = 0;;
    UInt32 *header = NULL;
    AlgorithmLink_IssAewbDccControlParams dccCtrlPrms;
    ChainsIssSensor_Info    *pSensorInfo = NULL;

    /* Initialize ISP configuration */
    IssM2mIspLink_ConfigParams_Init(pIspConfig);

    /* Get default configuration from sensor layer */
    getDefaultIspConfigFromSensor(pIspConfig);

    /* As of now, DCC is not supported for AR0132 monochrom sensor */
    if (ISSM2MISP_LINK_OPMODE_12BIT_MONOCHROME == ispOpMode)
    {
        Vps_printf(" CHAINS: For AR0132 Monochrome Sensor,");
        Vps_printf(" using ISP settings from Video Sensor layer !!!\n");
        Vps_printf(" CHAINS: No Support for DCC for AR0132 monochrome sensor !!!\n");
        return ;
    }

    pSensorInfo = &gChainsCommon_IssObj.sensorInfo;

    /* DCC is supported only if this flag is set in the driver,
       even dcc profile from qspi is supported only if this flag is set.
       Otherwise DCC parameters will not be used at all and configuration
       from the video sensor laye will be used */
    if (TRUE == pSensorInfo->isDccSupported)
    {
        memset(&dccCtrlPrms, 0x0, sizeof(dccCtrlPrms));

        /* Get the DCC Buffer */
        dccCtrlPrms.baseClassControl.controlCmd =
            ALGORITHM_AEWB_LINK_CMD_GET_DCC_BUF_PARAMS;
        dccCtrlPrms.baseClassControl.size = sizeof(dccCtrlPrms);

        dccCtrlPrms.dccBuf = NULL;

        status = System_linkControl(
            linkId,
            ALGORITHM_LINK_CMD_CONFIG,
            &dccCtrlPrms,
            sizeof(dccCtrlPrms),
            TRUE);
        UTILS_assert(0 == status);

        /* Get QSPI offst for this sensor */
        qSpiOffset = pSensorInfo->qSpiOffset;
        UTILS_assert(0 != qSpiOffset);

        if (NULL != dccCtrlPrms.dccBuf)
        {
            /* Check if the binary file in QSPI is valid, then use it instead
               of binary file form the driver */

             if(!System_isFastBootEnabled())
             {
                 /* Read the Header first */
                 System_qspiReadSector((UInt32)dccCtrlPrms.dccBuf,
                                       qSpiOffset,
                                       SystemUtils_align(
                                       CHAINS_COMMON_ISS_DCC_BIN_HEADER_SIZE,
                                       SYSTEM_QSPI_READ_WRITE_SIZE));
             }

            header = (UInt32 *)dccCtrlPrms.dccBuf;
            if(System_isFastBootEnabled())
            {
                 *header = 0x00000000;
            }
            if (CHAINS_COMMON_ISS_DCC_BIN_FILE_TAG_ID == *header)
            {
                /* Read bin file size */
                header ++;
                dccCtrlPrms.dccBufSize = *header;

                /* Read the binary file */
                System_qspiReadSector(
                    (UInt32)dccCtrlPrms.dccBuf,
                    qSpiOffset + CHAINS_COMMON_ISS_DCC_BIN_HEADER_SIZE,
                    SystemUtils_align(dccCtrlPrms.dccBufSize,
                                      SYSTEM_QSPI_READ_WRITE_SIZE));
                isDccProfileAvail = 1U;
                Vps_printf(" CHAINS: Using DCC Profile from QSPI \n");
            }
            else
            {
                Vps_printf(" CHAINS: DCC Tag ID check failed for QSPI \n");
                Vps_printf(" CHAINS: Using DCC Profile from Driver \n");

                memcpy(dccCtrlPrms.dccBuf,
                       pSensorInfo->dccParams.pDccCfg,
                       pSensorInfo->dccParams.dccCfgSize);
                    dccCtrlPrms.dccBufSize = pSensorInfo->dccParams.dccCfgSize;
                    isDccProfileAvail = 1U;
            }

            if (1U == isDccProfileAvail)
            {
                dccCtrlPrms.baseClassControl.controlCmd =
                    ALGORITHM_AEWB_LINK_CMD_PARSE_AND_SET_DCC_PARAMS;
                dccCtrlPrms.baseClassControl.size = sizeof(dccCtrlPrms);
                dccCtrlPrms.pIspCfg = pIspConfig;
                dccCtrlPrms.pSimcopCfg = pSimcopCfg;

                status = System_linkControl(
                    linkId,
                    ALGORITHM_LINK_CMD_CONFIG,
                    &dccCtrlPrms,
                    sizeof(dccCtrlPrms),
                    TRUE);
                UTILS_assert(0 == status);
            }
        }
        else
        {
            Vps_printf(" CHAINS: DCC buffer is NULL \n");
        }
    }
}

/**
 *******************************************************************************
 *
 * \brief   Update Capture Create Parameters for multiple channels
 *
 *******************************************************************************
*/
Void ChainsCommon_MultipleCam_UpdateIssCapturePrms(
                        char sensorName[],
                        IssCaptureLink_CreateParams *pPrm,
                        IssM2mIspLink_OperatingMode ispOpMode,
                        UInt32 width, UInt32 height,
                        Bool sensorCfgBroadcast)
{
    Int32 status;
    UInt32 cnt;
    ChainsIssSensor_CreateParams createPrms;
    ChainsIssSensor_Info *pSensorInfo = &gChainsCommon_IssObj.sensorInfo;

    pPrm->numCh = 4U;

    if (NULL != sensorName)
    {
        strncpy(createPrms.name, sensorName, CHAINS_ISS_SENSOR_MAX_NAME);
        createPrms.numChan = pPrm->numCh;

        for (cnt = 0U; cnt < pPrm->numCh; cnt ++)
        {
            createPrms.captInstId[cnt] = VPS_CAPT_INST_ISS_CAL_A + cnt;
        }
        createPrms.ispOpMode = ispOpMode;
        createPrms.sensorCfgBroadcast = sensorCfgBroadcast;

        gChainsCommon_IssObj.sensorHandle = ChainsIssSensor_Create(&createPrms);

        if (NULL == gChainsCommon_IssObj.sensorHandle)
        {
            Vps_printf(" Chains_Common: Sensor Open Failed !!\n\n");
            UTILS_assert(FALSE);
        }

        /* Get the configuration from the sensor to know default size and
           other parameters */
        status = ChainsIssSensor_GetInfoFromName(sensorName, pSensorInfo);
        UTILS_assert(SYSTEM_LINK_STATUS_SOK == status);
    }

    pPrm->videoIfMode =
        (System_VideoIfMode)pSensorInfo->videoIfMode;
    pPrm->videoIfWidth =
        (System_VideoIfWidth)pSensorInfo->cfgPrms.videoIfWidth;

    pPrm->csi2Params = pSensorInfo->csi2Prms;
    pPrm->csi2Params.csi2PhyClk = 800U;
    pPrm->bufCaptMode = SYSTEM_CAPT_BCM_LAST_FRM_REPEAT;

    for (cnt = 0U; cnt < pPrm->numCh; cnt ++)
    {
        pPrm->outParams[cnt].dataFormat =
            (System_VideoDataFormat)pSensorInfo->cfgPrms.dataformat;
        pPrm->outParams[cnt].width = pSensorInfo->cfgPrms.width;
        pPrm->outParams[cnt].height = pSensorInfo->cfgPrms.height;

        pPrm->outParams[cnt].inCsi2DataFormat = pSensorInfo->inCsi2DataFormat;
        pPrm->outParams[cnt].inCsi2VirtualChanNum = cnt;

        pPrm->outParams[cnt].maxWidth = pPrm->outParams[cnt].width;
        pPrm->outParams[cnt].maxHeight = pPrm->outParams[cnt].height;
        pPrm->outParams[cnt].numOutBuf = 3;
    }


}

/**
 *******************************************************************************
 *
 * \brief   Function to Set LDC Lut table in Simcop LDC Configuration
 *
 * \param   pLdcLutConfig     Pointer to LDC's LUT configuration
 *
 * \param   tableIdx          Index of the table, not used as of now.
 *
 *******************************************************************************
 */
Void ChainsCommon_SetIssLdcLutConfig(
    vpsissldcLutCfg_t *pLdcLutCfg, UInt32 tableIdx)
{
    if (0U == tableIdx)
    {
        pLdcLutCfg->address          = (UInt32)&gChainsCommonIssLdcTable_1280x800;
        pLdcLutCfg->downScaleFactor  = VPS_ISS_LDC_LUT_DOWN_SCALE_FACTOR_16;
        pLdcLutCfg->lineOffset       = CHAINS_COMMON_ISS_MESH_TABLE_PITCH(1280,
                                            (1U << pLdcLutCfg->downScaleFactor));
    }
    else
    {
        /* Using LDC Lut table of 1920x1080 size with 4x down scaling factor
           Using LDC table from the offset ((1920x1080) - (1280x720))/2 */
        pLdcLutCfg->address = (UInt32)&gChainsCommonIssLdcTable_1920x1080
        [
            (CHAINS_COMMON_ISS_MESH_TABLE_PITCH(1920,
                (1 <<VPS_ISS_LDC_LUT_DOWN_SCALE_FACTOR_4)) * 15) + 320
        ];
        pLdcLutCfg->downScaleFactor  = VPS_ISS_LDC_LUT_DOWN_SCALE_FACTOR_4;
        pLdcLutCfg->lineOffset       = CHAINS_COMMON_ISS_MESH_TABLE_PITCH(1920,
                                            (1U << pLdcLutCfg->downScaleFactor));
    }
}


/*  \brief Function to enables detection of CRC error on CSI2 interface
 *
 *  params captSrc      Source from which CAL is used to capture.
 *  params issCaptureLinkId  Capture Link identifier. Required to configure
 *                          the link to detect errors.
 *  returns none.
 */
Void ChainsCommon_SetIssCaptureErrorHandlingCb(UInt32 issCaptureLinkId)
{
    IssCaptureLink_SetErrorNotification calCsi2ErrCfg;

    calCsi2ErrCfg.errorCallback = &chainsCommonCsi2ErrorCb;
    calCsi2ErrCfg.errorsToDetect =
        ISSCAPTURE_LINK_ENABLE_ERROR_CRC_MISMATCH;
    calCsi2ErrCfg.appObj = NULL;

    System_linkControl(issCaptureLinkId,
                        ISSCAPTURE_LINK_CMD_SET_ERROR_NOTIFICATION,
                        &calCsi2ErrCfg,
                        0,
                        TRUE);
    return;
}

/*******************************************************************************
 *  Local Functions
 *******************************************************************************
 */

static Void issCreateSensor(
    char sensorName[], IssM2mIspLink_OperatingMode ispOpMode,
    IssM2mIspLink_WdrOffsetParams_t *wdrOffsetPrms)
{
    Int32 status;
    ChainsIssSensor_CreateParams createPrms;

    if (NULL != sensorName)
    {
        strncpy(createPrms.name, sensorName, CHAINS_ISS_SENSOR_MAX_NAME);
        createPrms.numChan = 1U;
        createPrms.captInstId[0U] = VPS_CAPT_INST_ISS_CAL_A;
        createPrms.ispOpMode = ispOpMode;

        gChainsCommon_IssObj.sensorHandle =
            ChainsIssSensor_Create(&createPrms);

        if (NULL == gChainsCommon_IssObj.sensorHandle)
        {
            Vps_printf(" Chains_Common: Sensor Open Failed !!\n\n");
            UTILS_assert(FALSE);
        }

        /* Get the configuration from the sensor to know default size and
           other parameters */
        status = ChainsIssSensor_GetInfoFromName(
            sensorName, &gChainsCommon_IssObj.sensorInfo);
        UTILS_assert(SYSTEM_LINK_STATUS_SOK == status);

        if (ISSM2MISP_LINK_OPMODE_2PASS_WDR_LINE_INTERLEAVED == ispOpMode)
        {
            wdrOffsetPrms->longLineOffset = gChainsCommon_IssObj.sensorInfo.
                offsetPrms.lineOffset[BSP_VID_SENSOR_EXP_LONG];
            wdrOffsetPrms->shortLineOffset = gChainsCommon_IssObj.sensorInfo.
                offsetPrms.lineOffset[BSP_VID_SENSOR_EXP_SHORT];

            wdrOffsetPrms->longPixelOffset = gChainsCommon_IssObj.sensorInfo.
                offsetPrms.pixelOffset[BSP_VID_SENSOR_EXP_LONG];
            wdrOffsetPrms->shortPixelOffset = gChainsCommon_IssObj.sensorInfo.
                offsetPrms.pixelOffset[BSP_VID_SENSOR_EXP_SHORT];

            wdrOffsetPrms->width = gChainsCommon_IssObj.sensorInfo.
                offsetPrms.width;
            wdrOffsetPrms->height = gChainsCommon_IssObj.sensorInfo.
                offsetPrms.height;
        }
    }
}

static Void getDefaultIspConfigFromSensor(
    IssIspConfigurationParameters *pIspConfig)
{
    Int32 status;

    status = ChainsIssSensor_Control(
        gChainsCommon_IssObj.sensorHandle,
        CHAINS_ISS_SENSOR_GET_DEFAULT_ISP_CONFIG,
        pIspConfig,
        NULL);

    UTILS_assert(0 == status);
}

/*  \brief Function to get the QSPI offset for give sensor DCC ID.
 *
 *  params sensorDccIf      DCC Id of the sensor
 *
 *  returns qspiOffset      Qspi offset for this give sensor
 *          0               If sensor is not supported. Offset 0 cannot be
 *                          used for writing, so using this for
 *                          unsupported sensor
 */
static UInt32 issGetQspiOffsetFromDccId(UInt32 sensorDccId)
{
    Int32 status;
    UInt32 qSpiOffset = 0U;
    char sensorName[CHAINS_ISS_SENSOR_MAX_NAME];
    ChainsIssSensor_Info sensorInfo;

    /* Get the Sensor QSPI offset for the given sensors DCC ID */
    status = ChainsIssSensor_GetNameFromDccId(sensorDccId, sensorName);

    if (SYSTEM_LINK_STATUS_SOK == status)
    {
        status = ChainsIssSensor_GetInfoFromName(sensorName, &sensorInfo);

        if (SYSTEM_LINK_STATUS_SOK == status)
        {
            qSpiOffset = sensorInfo.qSpiOffset;
        }
    }

    return (qSpiOffset);
}

/**
 *******************************************************************************
 *
 * \brief   callback to handle user defined commands reaching system link
 *
 *  \param  cmd [IN] Command that needs to be handled
 *               pPrm [IN/OUT] Parameters for this command
 *
 *******************************************************************************
 */
static Void issCmdHandler(UInt32 cmd, Void *pPrm)
{
    Int32 status;
    UInt32 qSpiOffset, dccCameraId;
    AlgorithmLink_IssAewbDccControlParams *dccCtrlPrms;

    if (NULL != pPrm)
    {
        if (SYSTEM_LINK_CMD_SAVE_DCC_FILE == cmd)
        {
            dccCtrlPrms = (AlgorithmLink_IssAewbDccControlParams *)pPrm;

            /* Get the Sensor DCC Id from the header */
            dccCameraId = *(((UInt32 *)dccCtrlPrms->dccBuf) +
                CHAINS_COMMON_ISS_DCC_BIN_DCC_ID_OFFSET);

            /* Get the Sensor QSPI offset for the given sensors DCC ID */
            qSpiOffset = issGetQspiOffsetFromDccId(dccCameraId);
            /* Offset 0 cannot be used, so it is used as
               error value here */
            if (0 != qSpiOffset)
            {
                /* Write complete bin file */
                System_qspiWriteSector(qSpiOffset,
                                       (UInt32)dccCtrlPrms->dccBuf,
                                       ALGORITHM_AEWB1_DCC_IN_BUF_SIZE);
            }
        }
        if (SYSTEM_LINK_CMD_CLEAR_DCC_QSPI_MEM == cmd)
        {
            dccCameraId = *(UInt32 *)pPrm;

            /* Get the Sensor QSPI offset for the given sensors DCC ID */
            qSpiOffset = issGetQspiOffsetFromDccId(dccCameraId);
            if (0 != qSpiOffset)
            {
                /* Write complete bin file */
                System_qspiEraseSector(qSpiOffset,
                                       ALGORITHM_AEWB1_DCC_IN_BUF_SIZE);
            }
        }
        if (SYSTEM_LINK_CMD_WRITE_SENSOR_REG == cmd)
        {
            UInt32 chanNum;
            UInt32 *prms = (UInt32 *)pPrm;
            Bsp_VidSensorRegRdWrParams regRdWrPrms;

            chanNum = *prms;
            prms ++;
            gChainsCommon_IssObj.regAddr = *prms;
            prms ++;
            gChainsCommon_IssObj.regVal = *prms;

            regRdWrPrms.deviceNum = 0;
            regRdWrPrms.numRegs = 1;
            regRdWrPrms.regAddr = &gChainsCommon_IssObj.regAddr;
            regRdWrPrms.regValue16 = &gChainsCommon_IssObj.regVal;

            if (NULL != gChainsCommon_IssObj.sensorInfo.sensorHandle[chanNum])
            {
                status =
                    Fvid2_control(
                        gChainsCommon_IssObj.sensorInfo.sensorHandle[chanNum],
                        IOCTL_BSP_VID_SENSOR_REG_WRITE,
                        &regRdWrPrms,
                        NULL);
                UTILS_assert (status == 0);
            }
        }
        if (SYSTEM_LINK_CMD_READ_SENSOR_REG == cmd)
        {
            UInt32 chanNum;
            UInt32 *prms = (UInt32 *)pPrm;
            Bsp_VidSensorRegRdWrParams regRdWrPrms;

            chanNum = *prms;
            prms ++;
            gChainsCommon_IssObj.regAddr = *prms;
            prms ++;

            regRdWrPrms.deviceNum = 0;
            regRdWrPrms.numRegs = 1;
            regRdWrPrms.regAddr = &gChainsCommon_IssObj.regAddr;
            regRdWrPrms.regValue16 = &gChainsCommon_IssObj.regVal;

            if (NULL != gChainsCommon_IssObj.sensorInfo.sensorHandle[chanNum])
            {
                status =
                    Fvid2_control(
                        gChainsCommon_IssObj.sensorInfo.sensorHandle[chanNum],
                        IOCTL_BSP_VID_SENSOR_REG_READ,
                        &regRdWrPrms,
                        NULL);
                UTILS_assert (status == 0);
            }
            *prms = gChainsCommon_IssObj.regVal;
        }
    }
}

/**
 *******************************************************************************
 *
 * \brief   Callback attached to the link. Will be called if an configured error
 *          is detected. If an frame has CRC error, the link would be instructed
 *          not to process that particular frame.
 *
 *  \param  cmd [IN] Command that needs to be handled
 *               pPrm [IN/OUT] Parameters for this command
 *
 *******************************************************************************
 */
static System_VideoFrameStatus chainsCommonCsi2ErrorCb(Void *appObj,
                                System_Buffer *pBuffer,
                                System_VideoFrameStatus error)
{
    System_VideoFrameStatus status = SYSTEM_FRAME_STATUS_COMPLETED;

    if (SYSTEM_FRAME_STATUS_CRC_ERROR == error)
    {
        /* Causes the capture link to not process this (pBuffer) frame */
        /* Change the status to SYSTEM_FRAME_STATUS_COMPLETED if you require to
            ignore the detected CRC errors */
        status = SYSTEM_FRAME_STATUS_ABORTED;
    }

    return status;
}

/**
 *******************************************************************************
 *
 * \brief   Enable / disable flipping and mirrioring
 *
 * \param   flip&Mirror
 *          if == 0, disable flipping and mirroring
 *          if == 1, enable flipping
 *          if == 2, enable mirroring
 *          if == 3, enable flipping & enable mirroring
 *
 *******************************************************************************
 */
Void ChainsCommon_IssUpdateFlipParams(UInt32 *flipAndMirror)
{
    Int32 status;

    status = ChainsIssSensor_Control(
        gChainsCommon_IssObj.sensorHandle,
        CHAINS_ISS_SENSOR_SET_FLIP_MIRROR,
        flipAndMirror,
        NULL);

    UTILS_assert(SYSTEM_LINK_STATUS_SOK == status);
}

/**
 *******************************************************************************
 *
 * \brief   Function to get the LDC Lut index based on the sensor name
 *
 * \param   sensorName  Sensor Name
 *******************************************************************************
 */
uint32_t ChainsCommon_GetLdcLutIdx(char sensorName[])
{
    uint32_t lutId;

    /* IMX224 sensor outputs 1280x960 frame size, so it requires bigger
       LDC Lut */
    if (0U ==
        strncmp(SENSOR_SONY_IMX224_CSI2, sensorName,
            CHAINS_ISS_SENSOR_MAX_NAME))
    {
        lutId = 1;
    }
    else /* For all other sensors, frame size is less or equal to 1280x800 */
    {
        lutId = 0;
    }

    return (lutId);
}
