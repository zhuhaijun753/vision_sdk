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
 * \file iss_sensor_imx224.c
 *
 *
 *
 *******************************************************************************
*/

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */

#include "iss_sensors_priv.h"


/*******************************************************************************
 *  Defines
 *******************************************************************************
 */


/*******************************************************************************
 *  Data structure's
 *******************************************************************************
 */

/*******************************************************************************
 *  Local Functions Declarations
 *******************************************************************************
 */
static Void ChainsIssSensor_IssGetSensorDefaultConfig_Imx224(
    ChainsIssSensor_Params *pSensorPrms,
    IssM2mIspLink_OperatingMode ispOpMode,
    Bsp_VidSensorConfigParams *pSensorCfgPrms);
static Void ChainsIssSensor_IssGetDefaultIspConfig_Imx224(
    ChainsIssSensor_Params *pSensorPrms,
    IssM2mIspLink_OperatingMode ispOpMode,
    IssIspConfigurationParameters *pIspConfig);
static Void ChainsIssSensor_IssGetAewbConfig_Imx224(
    ChainsIssSensor_Params *pSensorPrms,
    IssM2mIspLink_OperatingMode ispOpMode,
    AlgorithmLink_IssAewbCreateParams *prms);



/*******************************************************************************
 *  Globals
 *******************************************************************************
 */

/* \brief Sensor Params structure used for registering this sensor to chains_iss
 */
static ChainsIssSensor_Params SensorParams_Imx224 = {0};

/**
 * \brief Structure for ISP H3A AEWB engine parameters.
 */
static vpsissH3aAewbConfig_t aewbConfig_imx224 =
{
    FALSE, /* enableALowComp */
    FALSE, /* enableMedFilt */
    0xFFF, /* midFiltThreshold */
    {   /* vpsissH3aPaxelConfig_t      winCfg */
        {16u, 48u}, /* Fvid2_PosConfig pos */
        36u,     /* width */
        54u,     /* height */
        32u,     /* horzCount */
        16u,     /* vertCount */
        6,      /* horzIncr */
        6,      /* vertIncr */
    },
    958u,       /* Black Line Vertical Start */
    2u,         /* Black Line Width */
    VPS_ISS_H3A_OUTPUT_MODE_SUM_ONLY, /* vpsissH3aOutputMode_t outMode; */
    0,  /* sumShift */
    1023u, /* satLimit */
    VPS_ISS_H3A_MODE_NORMAL /* vpsissH3aMode_t mode */

} ;

static vpsissGlbceConfig_t glbceCfg_imx224 =
{
    TRUE,           /* ENABLE */
    240u,           /* IR Strength */
    0,              /* blackLevel */
    65535,          /* White Level */
    12,             /* Intensity variance */
    7,              /* Spacial variance */
    6,              /* Bright Amplification Limit */
    6,              /* Dark Amplification Limit */
    VPS_ISS_GLBCE_DITHER_FOUR_BIT,
    64,             /* MAX Slope Limit */
    72,             /* MIN Slope Limit */
    {
        0,5377,10218,14600,18585,22224,25561,28631,31466,34092,36530,
        38801,40921,42904,44764,46511,48156,49706,51171,52557,53870,55116,
        56299,57425,58498,59520,60497,61429,62322,63176,63995,64781,
        65535
    }
};

static vpsissGlbcePerceptConfig_t glbceFwbPerCfg_imx224 =
{
    TRUE,
    {
        0,4622,8653,11684,14195,16380,18335,20118,21766,23304,24751,
        26119,27422,28665,29857,31003,32108,33176,34209,35211,36185,
        37132,38055,38955,39834,40693,41533,42355,43161,43951,44727,
        45488,46236,46971,47694,48405,49106,49795,50475,51145,51805,
        52456,53099,53733,54360,54978,55589,56193,56789,57379,57963,
        58539,59110,59675,60234,60787,61335,61877,62414,62946,63473,
        63996,64513,65026,65535
    }
};

static vpsissGlbcePerceptConfig_t glbceRevPerCfg_imx224 =
{
    TRUE,
    {
        0,228,455,683,910,1138,1369,1628,1912,2221,2556,2916,3304,3717,
        4158,4626,5122,5645,6197,6777,7386,8024,8691,9387,10113,10869,
        11654,12471,13317,14194,15103,16042,17012,18014,19048,20113,21210,
        22340,23501,24696,25922,27182,28475,29800,31159,32552,33977,35437,
        36930,38458,40019,41615,43245,44910,46609,48343,50112,51916,53755,
        55630,57539,59485,61466,63482,65535
    }
};


static vpsissIsifGainOfstConfig_t isifWbCfg_imx224 =
{
    {512U, 512U, 512U},
    0U,
    {TRUE, TRUE, TRUE},
    {TRUE, TRUE, TRUE}
};

/*******************************************************************************
 *  Function Definition
 *******************************************************************************
 */

Void ChainsIssSensor_Imx224_Init()
{
    ChainsIssSensor_Params *prms = &SensorParams_Imx224;

    ChainsIssSensor_Params_Init(prms);

    strncpy(prms->name, SENSOR_SONY_IMX224_CSI2, CHAINS_ISS_SENSOR_MAX_NAME);

    prms->dccId = 224;
    prms->qSpiOffset = ISS_SENSORS_DCC_QSPI_OFFSET_IMX224;
    prms->dataFormat = SYSTEM_DF_BAYER_RGGB;
    prms->videoIfWidth = SYSTEM_VIFW_4LANES;
    prms->videoIfMode =SYSTEM_VIFM_SCH_CSI2;

    prms->inCsi2DataFormat = SYSTEM_CSI2_RAW12;
    prms->inCsi2VirtualChanNum = 0U;
    prms->csi2Prms.cmplxIoCfg.clockLane.pol       = FALSE;
    prms->csi2Prms.cmplxIoCfg.clockLane.position  = 2U;
    prms->csi2Prms.cmplxIoCfg.data1Lane.pol       = FALSE;
    prms->csi2Prms.cmplxIoCfg.data1Lane.position  = 1U;
    prms->csi2Prms.cmplxIoCfg.data2Lane.pol       = FALSE;
    prms->csi2Prms.cmplxIoCfg.data2Lane.position  = 3U;
    prms->csi2Prms.cmplxIoCfg.data3Lane.pol       = FALSE;
    prms->csi2Prms.cmplxIoCfg.data3Lane.position  = 4U;
    prms->csi2Prms.cmplxIoCfg.data4Lane.pol       = FALSE;
    prms->csi2Prms.cmplxIoCfg.data4Lane.position  = 5U;
    prms->csi2Prms.csi2PhyClk = 150U;

    prms->aewbMode = ALGORITHMS_ISS_AEWB_MODE_AEWB;
    prms->GetSensorDefaultConfig =
        ChainsIssSensor_IssGetSensorDefaultConfig_Imx224;
    prms->GetAewbConfig = ChainsIssSensor_IssGetAewbConfig_Imx224;
    prms->GetDefaultIspConfig = ChainsIssSensor_IssGetDefaultIspConfig_Imx224;

    ChainsIssSensor_RegisterSensor(
        FVID2_VID_SENSOR_SONY_IMX224_CSI2_DRV,
        prms);
}

/*******************************************************************************
 *  Local Functions Definition
 *******************************************************************************
 */



/**
 *******************************************************************************
 *
 * \brief   Set iss isp m2m link configuration Parameters
 *
 * \param appObj    [IN] Application specific object
 *
 *******************************************************************************
*/
static Void ChainsIssSensor_IssGetDefaultIspConfig_Imx224(
    ChainsIssSensor_Params *pSensorPrms,
    IssM2mIspLink_OperatingMode ispOpMode,
    IssIspConfigurationParameters *pIspConfig)
{
    UTILS_assert(NULL != pIspConfig);

    pIspConfig->aewbCfg = &aewbConfig_imx224;

    /* glbceCfg */
    pIspConfig->glbceCfg = &glbceCfg_imx224;

    /* fwdPerCfg */
    pIspConfig->glbceFwdPerCfg = &glbceFwbPerCfg_imx224;

    /* revPerCfg */
    pIspConfig->glbceRevPerCfg = &glbceRevPerCfg_imx224;

    /* isifWbCfg */
    pIspConfig->isifWbCfg = &isifWbCfg_imx224;
}

static Void ChainsIssSensor_IssGetAewbConfig_Imx224(
    ChainsIssSensor_Params *pSensorPrms,
    IssM2mIspLink_OperatingMode ispOpMode,
    AlgorithmLink_IssAewbCreateParams *prm)
{
    AlgorithmLink_IssAewbAeDynamicParams *dynPrms = NULL;

    UTILS_assert(NULL != prm);

    /* Set H3A Params */
    prm->h3aParams.winCountH = aewbConfig_imx224.winCfg.horzCount;
    prm->h3aParams.winCountV = aewbConfig_imx224.winCfg.vertCount;
    prm->h3aParams.winSizeH  = aewbConfig_imx224.winCfg.width;
    prm->h3aParams.winSizeV  = aewbConfig_imx224.winCfg.height;
    prm->h3aParams.winSkipH  = aewbConfig_imx224.winCfg.horzIncr;
    prm->h3aParams.winSkipV  = aewbConfig_imx224.winCfg.vertIncr;
    prm->numH3aPlanes = 1u;

    prm->dataFormat = SensorParams_Imx224.dataFormat;
    prm->mode = (AlgorithmLink_IssAewbMode)SensorParams_Imx224.aewbMode;

    prm->isWdrEnable = IssM2mIspLink_IsWdrMode(ispOpMode);

    prm->dccCameraId = SensorParams_Imx224.dccId;

    dynPrms = &prm->aeDynParams;

    dynPrms->targetBrightnessRange.min = 30;
    dynPrms->targetBrightnessRange.max = 45;
    dynPrms->targetBrightness = 35;
    dynPrms->threshold = 1;

    dynPrms->exposureTimeRange[0].min = 100;
    dynPrms->exposureTimeRange[0].max = 8333;
    dynPrms->apertureLevelRange[0].min = 1;
    dynPrms->apertureLevelRange[0].max = 1;
    dynPrms->sensorGainRange[0].min = 1000;
    dynPrms->sensorGainRange[0].max = 1000;
    dynPrms->ipipeGainRange[0].min = 512;
    dynPrms->ipipeGainRange[0].max = 512;

    dynPrms->exposureTimeRange[1].min = 8333;
    dynPrms->exposureTimeRange[1].max = 8333;
    dynPrms->apertureLevelRange[1].min = 1;
    dynPrms->apertureLevelRange[1].max = 1;
    dynPrms->sensorGainRange[1].min = 1000;
    dynPrms->sensorGainRange[1].max = 2000;
    dynPrms->ipipeGainRange[1].min = 512;
    dynPrms->ipipeGainRange[1].max = 512;

    dynPrms->exposureTimeRange[2].min = 16666;
    dynPrms->exposureTimeRange[2].max = 30750;
    dynPrms->apertureLevelRange[2].min = 1;
    dynPrms->apertureLevelRange[2].max = 1;
    dynPrms->sensorGainRange[2].min = 1000;
    dynPrms->sensorGainRange[2].max = 1000;
    dynPrms->ipipeGainRange[2].min = 512;
    dynPrms->ipipeGainRange[2].max = 512;


    dynPrms->exposureTimeRange[3].min = 30750;
    dynPrms->exposureTimeRange[3].max = 30750;
    dynPrms->apertureLevelRange[3].min = 1;
    dynPrms->apertureLevelRange[3].max = 1;
    dynPrms->sensorGainRange[3].min = 1000;
    dynPrms->sensorGainRange[3].max = 160000;
    dynPrms->ipipeGainRange[3].min = 512;
    dynPrms->ipipeGainRange[3].max = 512;

    dynPrms->numAeDynParams = 4;

    dynPrms->exposureTimeStepSize = 1;

    dynPrms->enableBlc = TRUE;

    prm->calbData = NULL;

}

static Void ChainsIssSensor_IssGetSensorDefaultConfig_Imx224(
    ChainsIssSensor_Params *pSensorPrms,
    IssM2mIspLink_OperatingMode ispOpMode,
    Bsp_VidSensorConfigParams *pSensorCfgPrms)
{
    UTILS_assert (NULL != pSensorPrms);

    pSensorCfgPrms->standard = SYSTEM_STD_CUSTOM;
    pSensorCfgPrms->dataformat = pSensorPrms->dataFormat;
    pSensorCfgPrms->videoIfWidth = pSensorPrms->videoIfWidth;
    pSensorCfgPrms->fps = FVID2_FPS_30;
    pSensorCfgPrms->bpp = FVID2_BPP_BITS12;

    if (TRUE == IssM2mIspLink_IsWdrMergeMode(ispOpMode))
    {
        pSensorCfgPrms->width = 1312U;
        pSensorCfgPrms->height = 2164U;
    }
    else
    {
        pSensorCfgPrms->width = 1280U;
        pSensorCfgPrms->height = 960U;
    }
}

