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
 * \file iss_sensor_ov10640.c
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
static Void ChainsIssSensor_IssGetSensorDefaultConfig_Ov10640(
    ChainsIssSensor_Params *pSensorPrms,
    IssM2mIspLink_OperatingMode ispOpMode,
    Bsp_VidSensorConfigParams *pSensorCfgPrms);
static Void ChainsIssSensor_IssGetDefaultIspConfig_Ov10640(
    ChainsIssSensor_Params *pSensorPrms,
    IssM2mIspLink_OperatingMode ispOpMode,
    IssIspConfigurationParameters *pIspConfig);
static Void ChainsIssSensor_IssGetAewbConfig_Ov10640(
    ChainsIssSensor_Params *pSensorPrms,
    IssM2mIspLink_OperatingMode ispOpMode,
    AlgorithmLink_IssAewbCreateParams *prms);
static Void ChainsIssSensor_IssGetAewbConfig_Ov10640_Ae(
    ChainsIssSensor_Params *pSensorPrms,
    IssM2mIspLink_OperatingMode ispOpMode,
    AlgorithmLink_IssAewbCreateParams *prm);
static Void ChainsIssSensors_Init_Imi(
    UInt32 sensorDrvId,
    ChainsIssSensor_Params *pSensorPrms,
    UInt32 captInstId[],
    Bool enBroadCast);
static Void ChainsIssSensors_DisBroadcast_Imi(
    ChainsIssSensor_Params *pSensorPrms,
    UInt32 captInstId[]);

/*******************************************************************************
 *  Globals
 *******************************************************************************
 */

/* \brief Sensor Params structure used for registering this sensor to chains_iss
 */
static ChainsIssSensor_Params SensorParams_Ov10640_Cpi = {0};
static ChainsIssSensor_Params SensorParams_Ov10640_Csi2 = {0};
static ChainsIssSensor_Params SensorParams_Ov10640_Imi = {0};

/**
 * \brief Structure for ISP H3A AF engine parameters.
 * Parameter values are chosen to divide the frame in a 3x3 grid of paxels
 * for scene obstruction detection purpose
 */

static vpsissH3aAfConfig_t afConfig_ov10640 =
{
    FALSE, /* enableALowCompr */
    TRUE, /*enableMedFilt */
    100,  /* midFiltThreshold*/
    VPS_ISS_H3A_AF_RGBPOS_GR_BG,  /* rgbPos */
    { /* vpsissH3aPaxelConfig_t*/
            {128u + 0, 16u + 1u}, /* Fvid2_PosConfig pos */
            (((1280u - 2u*128u)/3u) + 7u) & (~7u), /* width, rounded up to next multiple of 8*/
            (((720u - 2u*16u)/3u) + 7u) & (~7u), /* height, rounded up to next multiple of 8 */
            3u, /* horzCount */
            3u, /* vertCount*/
            8u, /* horzIncr */
            8u, /* vertIncr */
     },
     VPS_ISS_H3A_AF_FV_MODE_SUM, /* fvMode */
     VPS_ISS_H3A_AF_VF_VERT_HORZ, /* vfMode */
     { /* iirCfg1 */
     {8, -95, -49, 38, 76, 38, -111, -54, 17, -34, 17}, /* coeff */
        //{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        0 /* threshold */
     },
     { /* iirCfg2 */
    {11, -72, -50, 26, 51, 26, -92, -53, 19, -38, 19}, /* coeff */
        //{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        0
     },
     { /* firCfg1 */
     {3, -6, 0, 6, -3}, /* coeff */
       //{0, 0, 0, 0, 16},
        0 /* threshold */
     },
     { /* firCfg2 */
     {3, -6, 0, 6, -3}, /* coeff */
       //{0, 0, 0, 0, 16},
       0 /* threshold */
      },
      0, /* iirFiltStartPos */
      VPS_ISS_H3A_MODE_NORMAL /* AF mode */
};

/**
 * \brief Structure for ISP H3A AEWB engine parameters.
 */
static vpsissH3aAewbConfig_t aewbConfig_ov10640 =
{
    FALSE, /* enableALowComp */
    FALSE, /* enableMedFilt */
    0xFFF, /* midFiltThreshold */
    {   /* vpsissH3aPaxelConfig_t      winCfg */
        {32u, 106u}, /* Fvid2_PosConfig pos */
        32u,     /* width */
        16u,     /* height */
        32u,     /* horzCount */
        30u,     /* vertCount */
        4,      /* horzIncr */
        4,      /* vertIncr */
    },
    718u,       /* Black Line Vertical Start */
    2u,         /* Black Line Width */
    VPS_ISS_H3A_OUTPUT_MODE_SUM_ONLY, /* vpsissH3aOutputMode_t outMode; */
    2,  /* sumShift */
    1023u, /* satLimit */
    VPS_ISS_H3A_MODE_NORMAL /* vpsissH3aMode_t mode */
} ;

static vpsissIpipeInConfig_t ipipeInputCfg_ov10640 =
{
    VPS_ISS_IPIPE_DATA_PATH_RAW_YUV422,
    {0U, 0U, 1280U, 720U}
};


static vpsissGlbceConfig_t glbceCfg_ov10640 =
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
        38801,40921,42904,44764,46511,48156,49706,51171,52557,53870,
        55116,56299,57425,58498,59520,60497,61429,62322,63176,63995,
        64781,65535
    }
};

static vpsissGlbcePerceptConfig_t glbceFwbPerCfg_ov10640 =
{
    TRUE,
    {
        0,4622,8653,11684,14195,16380,18335,20118,21766,23304,24751,26119,
        27422,28665,29857,31003,32108,33176,34209,35211,36185,37132,38055,
        38955,39834,40693,41533,42355,43161,43951,44727,45488,46236,46971,
        47694,48405,49106,49795,50475,51145,51805,52456,53099,53733,54360,
        54978,55589,56193,56789,57379,57963,58539,59110,59675,60234,60787,
        61335,61877,62414,62946,63473,63996,64513,65026,65535
    }
};

static vpsissGlbcePerceptConfig_t glbceRevPerCfg_ov10640 =
{
    TRUE,
    {
        0,228,455,683,910,1138,1369,1628,1912,2221,2556,2916,3304,3717,4158,
        4626,5122,5645,6197,6777,7386,8024,8691,9387,10113,10869,11654,
        12471,13317,14194,15103,16042,17012,18014,19048,20113,21210,22340,
        23501,24696,25922,27182,28475,29800,31159,32552,33977,35437,36930,
        38458,40019,41615,43245,44910,46609,48343,50112,51916,53755,55630,
        57539,59485,61466,63482,65535
    }
};

static vpsissIsifGainOfstConfig_t isifWbCfg_ov10640 =
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

Void ChainsIssSensor_Ov10640_Csi2_Init()
{
    ChainsIssSensor_Params *prms = &SensorParams_Ov10640_Csi2;

    ChainsIssSensor_Params_Init(prms);

    strncpy(prms->name, SENSOR_OMNIVISION_OV10640_CSI2, CHAINS_ISS_SENSOR_MAX_NAME);

    prms->dccId = 10640;
    prms->qSpiOffset = ISS_SENSORS_DCC_QSPI_OFFSET_OV10640;
    prms->dataFormat = SYSTEM_DF_BAYER_BGGR;
    prms->videoIfWidth = SYSTEM_VIFW_4LANES;
    prms->videoIfMode = SYSTEM_VIFM_SCH_CSI2;
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
    prms->csi2Prms.csi2PhyClk = 400U;

    prms->aewbMode = ALGORITHMS_ISS_AEWB_MODE_AEWB;
    prms->GetSensorDefaultConfig =
        ChainsIssSensor_IssGetSensorDefaultConfig_Ov10640;
    prms->GetAewbConfig = ChainsIssSensor_IssGetAewbConfig_Ov10640_Ae;
    prms->GetDefaultIspConfig = ChainsIssSensor_IssGetDefaultIspConfig_Ov10640;

    ChainsIssSensor_RegisterSensor(
        FVID2_VID_SENSOR_OV10640_CSI2_DRV,
        prms);
}

Void ChainsIssSensor_Ov10640_Cpi_Init()
{
    ChainsIssSensor_Params *prms = &SensorParams_Ov10640_Cpi;

    ChainsIssSensor_Params_Init(prms);

    strncpy(prms->name, SENSOR_OMNIVISION_OV10640_CPI, CHAINS_ISS_SENSOR_MAX_NAME);

    prms->dccId = 10640;
    prms->qSpiOffset = ISS_SENSORS_DCC_QSPI_OFFSET_OV10640;
    prms->dataFormat = SYSTEM_DF_BAYER_BGGR;
    prms->videoIfWidth = SYSTEM_VIFW_12BIT;
    prms->videoIfMode =SYSTEM_VIFM_SCH_CPI;

    prms->aewbMode = ALGORITHMS_ISS_AEWB_MODE_AWB;
    prms->GetSensorDefaultConfig =
        ChainsIssSensor_IssGetSensorDefaultConfig_Ov10640;
    prms->GetAewbConfig = ChainsIssSensor_IssGetAewbConfig_Ov10640;
    prms->GetDefaultIspConfig = ChainsIssSensor_IssGetDefaultIspConfig_Ov10640;

    ChainsIssSensor_RegisterSensor(
        FVID2_VID_SENSOR_OV10640_CPI_DRV,
        prms);
}

Void ChainsIssSensor_Ov10640_Imi_Init()
{
    ChainsIssSensor_Params *prms = &SensorParams_Ov10640_Imi;

    ChainsIssSensor_Params_Init(prms);

    strncpy(prms->name, SENSOR_OMNIVISION_OV10640_IMI, CHAINS_ISS_SENSOR_MAX_NAME);

    prms->dccId = 10640;
    prms->maxChannels = 4U;
    prms->qSpiOffset = ISS_SENSORS_DCC_QSPI_OFFSET_OV10640;
    prms->dataFormat = SYSTEM_DF_BAYER_BGGR;
    prms->videoIfWidth = SYSTEM_VIFW_4LANES;
    prms->videoIfMode = SYSTEM_VIFM_SCH_CSI2;
    prms->inCsi2DataFormat = SYSTEM_CSI2_RAW12;
    prms->inCsi2VirtualChanNum = 0U;
    prms->csi2Prms.cmplxIoCfg.clockLane.pol       = FALSE;
    prms->csi2Prms.cmplxIoCfg.clockLane.position  = 1U;
    prms->csi2Prms.cmplxIoCfg.data1Lane.pol       = FALSE;
    prms->csi2Prms.cmplxIoCfg.data1Lane.position  = 2U;
    prms->csi2Prms.cmplxIoCfg.data2Lane.pol       = FALSE;
    prms->csi2Prms.cmplxIoCfg.data2Lane.position  = 3U;
    prms->csi2Prms.cmplxIoCfg.data3Lane.pol       = FALSE;
    prms->csi2Prms.cmplxIoCfg.data3Lane.position  = 4U;
    prms->csi2Prms.cmplxIoCfg.data4Lane.pol       = FALSE;
    prms->csi2Prms.cmplxIoCfg.data4Lane.position  = 5U;
    prms->csi2Prms.csi2PhyClk = 800U;

    prms->aewbMode = ALGORITHMS_ISS_AEWB_MODE_AEWB;
    prms->GetSensorDefaultConfig =
        ChainsIssSensor_IssGetSensorDefaultConfig_Ov10640;
    prms->GetAewbConfig = ChainsIssSensor_IssGetAewbConfig_Ov10640_Ae;
    prms->GetDefaultIspConfig = ChainsIssSensor_IssGetDefaultIspConfig_Ov10640;
    prms->SensorPreInit = ChainsIssSensors_Init_Imi;
    prms->SensorDisBroadcast = ChainsIssSensors_DisBroadcast_Imi;

    ChainsIssSensor_RegisterSensor(
        FVID2_VID_SENSOR_IMI_OV10640_DRV,
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
static Void ChainsIssSensor_IssGetDefaultIspConfig_Ov10640(
    ChainsIssSensor_Params *pSensorPrms,
    IssM2mIspLink_OperatingMode ispOpMode,
    IssIspConfigurationParameters *pIspConfig)
{
    UTILS_assert(NULL != pIspConfig);

    pIspConfig->aewbCfg = &aewbConfig_ov10640;

    /* Initialize AF config */
    pIspConfig->afCfg = &afConfig_ov10640;

    /* glbceCfg */
    pIspConfig->glbceCfg = &glbceCfg_ov10640;

    /* fwdPerCfg */
    pIspConfig->glbceFwdPerCfg = &glbceFwbPerCfg_ov10640;

    /* revPerCfg */
    pIspConfig->glbceRevPerCfg = &glbceRevPerCfg_ov10640;

    /* isifWbCfg */
    pIspConfig->isifWbCfg = &isifWbCfg_ov10640;

    pIspConfig->ipipeInputCfg = &ipipeInputCfg_ov10640;
}

static Void ChainsIssSensor_IssGetAewbConfig_Ov10640(
    ChainsIssSensor_Params *pSensorPrms,
    IssM2mIspLink_OperatingMode ispOpMode,
    AlgorithmLink_IssAewbCreateParams *prm)
{
    AlgorithmLink_IssAewbAeDynamicParams *dynPrms = NULL;

    UTILS_assert(NULL != prm);

    /* Set H3A Params */
    prm->h3aParams.winCountH = aewbConfig_ov10640.winCfg.horzCount;
    prm->h3aParams.winCountV = aewbConfig_ov10640.winCfg.vertCount;
    prm->h3aParams.winSizeH  = aewbConfig_ov10640.winCfg.width;
    prm->h3aParams.winSizeV  = aewbConfig_ov10640.winCfg.height;
    prm->h3aParams.winSkipH  = aewbConfig_ov10640.winCfg.horzIncr;
    prm->h3aParams.winSkipV  = aewbConfig_ov10640.winCfg.vertIncr;
    prm->numH3aPlanes = 1u;

    prm->dataFormat = SensorParams_Ov10640_Csi2.dataFormat;
    prm->mode = (AlgorithmLink_IssAewbMode)SensorParams_Ov10640_Csi2.aewbMode;

    prm->isWdrEnable = IssM2mIspLink_IsWdrMode(ispOpMode);

    prm->dccCameraId = SensorParams_Ov10640_Csi2.dccId;

    dynPrms = &prm->aeDynParams;

    dynPrms->targetBrightnessRange.min = 35;
    dynPrms->targetBrightnessRange.max = 45;
    dynPrms->targetBrightness = 40;
    dynPrms->threshold = 5;

    dynPrms->exposureTimeRange[0].min = 33333;
    dynPrms->exposureTimeRange[0].max = 33333;
    dynPrms->apertureLevelRange[0].min = 1;
    dynPrms->apertureLevelRange[0].max = 1;
    dynPrms->sensorGainRange[0].min = 1000;
    dynPrms->sensorGainRange[0].max = 1000;
    dynPrms->ipipeGainRange[0].min = 512;
    dynPrms->ipipeGainRange[0].max = 512;

    dynPrms->exposureTimeRange[1].min = 8333;
    dynPrms->exposureTimeRange[1].max = 16666;
    dynPrms->apertureLevelRange[1].min = 1;
    dynPrms->apertureLevelRange[1].max = 1;
    dynPrms->sensorGainRange[1].min = 2000;
    dynPrms->sensorGainRange[1].max = 2000;
    dynPrms->ipipeGainRange[1].min = 512;
    dynPrms->ipipeGainRange[1].max = 512;

    dynPrms->exposureTimeRange[2].min = 8333;
    dynPrms->exposureTimeRange[2].max = 16666;
    dynPrms->apertureLevelRange[2].min = 1;
    dynPrms->apertureLevelRange[2].max = 1;
    dynPrms->sensorGainRange[2].min = 4000;
    dynPrms->sensorGainRange[2].max = 4000;
    dynPrms->ipipeGainRange[2].min = 512;
    dynPrms->ipipeGainRange[2].max = 512;


    dynPrms->exposureTimeRange[3].min = 8333;
    dynPrms->exposureTimeRange[3].max = 16666;
    dynPrms->apertureLevelRange[3].min = 1;
    dynPrms->apertureLevelRange[3].max = 1;
    dynPrms->sensorGainRange[3].min = 8000;
    dynPrms->sensorGainRange[3].max = 8000;
    dynPrms->ipipeGainRange[3].min = 512;
    dynPrms->ipipeGainRange[3].max = 512;


    dynPrms->exposureTimeRange[4].min = 6666;
    dynPrms->exposureTimeRange[4].max = 16666;
    dynPrms->apertureLevelRange[4].min = 1;
    dynPrms->apertureLevelRange[4].max = 1;
    dynPrms->sensorGainRange[4].min = 20000;
    dynPrms->sensorGainRange[4].max = 20000;
    dynPrms->ipipeGainRange[4].min = 512;
    dynPrms->ipipeGainRange[4].max = 512;

    dynPrms->numAeDynParams = 1;

    dynPrms->exposureTimeStepSize = 1;

    dynPrms->enableBlc = TRUE;

    prm->calbData = NULL;
}

static Void ChainsIssSensor_IssGetAewbConfig_Ov10640_Ae(
    ChainsIssSensor_Params *pSensorPrms,
    IssM2mIspLink_OperatingMode ispOpMode,
    AlgorithmLink_IssAewbCreateParams *prm)
{
    AlgorithmLink_IssAewbAeDynamicParams *dynPrms = NULL;

    UTILS_assert(NULL != prm);

    /* Set H3A Params */
    prm->h3aParams.winCountH = aewbConfig_ov10640.winCfg.horzCount;
    prm->h3aParams.winCountV = aewbConfig_ov10640.winCfg.vertCount;
    prm->h3aParams.winSizeH  = aewbConfig_ov10640.winCfg.width;
    prm->h3aParams.winSizeV  = aewbConfig_ov10640.winCfg.height;
    prm->h3aParams.winSkipH  = aewbConfig_ov10640.winCfg.horzIncr;
    prm->h3aParams.winSkipV  = aewbConfig_ov10640.winCfg.vertIncr;
    prm->numH3aPlanes = 1u;

    prm->dataFormat = SensorParams_Ov10640_Imi.dataFormat;
    prm->mode = (AlgorithmLink_IssAewbMode)SensorParams_Ov10640_Imi.aewbMode;

    prm->isWdrEnable = IssM2mIspLink_IsWdrMode(ispOpMode);

    prm->dccCameraId = SensorParams_Ov10640_Imi.dccId;

    dynPrms = &prm->aeDynParams;

    if(prm->isWdrEnable)
    {
        dynPrms->targetBrightnessRange.min = 35;
        dynPrms->targetBrightnessRange.max = 45;
        dynPrms->targetBrightness = 40;
        dynPrms->threshold = 5;
        dynPrms->enableBlc = TRUE;
    }
    else
    {
        dynPrms->targetBrightnessRange.min = 15;
        dynPrms->targetBrightnessRange.max = 25;
        dynPrms->targetBrightness = 20;
        dynPrms->threshold = 5;
        dynPrms->enableBlc = FALSE;
    }

    dynPrms->exposureTimeRange[0].min = 100;
    dynPrms->exposureTimeRange[0].max = 16666;
    dynPrms->apertureLevelRange[0].min = 1;
    dynPrms->apertureLevelRange[0].max = 1;
    dynPrms->sensorGainRange[0].min = 1000;
    dynPrms->sensorGainRange[0].max = 1000;
    dynPrms->ipipeGainRange[0].min = 512;
    dynPrms->ipipeGainRange[0].max = 512;

    dynPrms->exposureTimeRange[1].min = 8333;
    dynPrms->exposureTimeRange[1].max = 16666;
    dynPrms->apertureLevelRange[1].min = 1;
    dynPrms->apertureLevelRange[1].max = 1;
    dynPrms->sensorGainRange[1].min = 2000;
    dynPrms->sensorGainRange[1].max = 2000;
    dynPrms->ipipeGainRange[1].min = 512;
    dynPrms->ipipeGainRange[1].max = 512;

    dynPrms->exposureTimeRange[2].min = 8333;
    dynPrms->exposureTimeRange[2].max = 16666;
    dynPrms->apertureLevelRange[2].min = 1;
    dynPrms->apertureLevelRange[2].max = 1;
    dynPrms->sensorGainRange[2].min = 4000;
    dynPrms->sensorGainRange[2].max = 4000;
    dynPrms->ipipeGainRange[2].min = 512;
    dynPrms->ipipeGainRange[2].max = 512;


    dynPrms->exposureTimeRange[3].min = 8333;
    dynPrms->exposureTimeRange[3].max = 16666;
    dynPrms->apertureLevelRange[3].min = 1;
    dynPrms->apertureLevelRange[3].max = 1;
    dynPrms->sensorGainRange[3].min = 8000;
    dynPrms->sensorGainRange[3].max = 8000;
    dynPrms->ipipeGainRange[3].min = 512;
    dynPrms->ipipeGainRange[3].max = 512;


    dynPrms->exposureTimeRange[4].min = 6666;
    dynPrms->exposureTimeRange[4].max = 16666;
    dynPrms->apertureLevelRange[4].min = 1;
    dynPrms->apertureLevelRange[4].max = 1;
    dynPrms->sensorGainRange[4].min = 20000;
    dynPrms->sensorGainRange[4].max = 20000;
    dynPrms->ipipeGainRange[4].min = 512;
    dynPrms->ipipeGainRange[4].max = 512;

    dynPrms->numAeDynParams = 5;

    dynPrms->exposureTimeStepSize = 1;

    prm->calbData = NULL;
}
static Void ChainsIssSensor_IssGetSensorDefaultConfig_Ov10640(
    ChainsIssSensor_Params *pSensorPrms,
    IssM2mIspLink_OperatingMode ispOpMode,
    Bsp_VidSensorConfigParams *pSensorCfgPrms)
{
    UTILS_assert (NULL != pSensorPrms);

    pSensorCfgPrms->standard = SYSTEM_STD_720P_60;
    pSensorCfgPrms->dataformat = pSensorPrms->dataFormat;
    pSensorCfgPrms->videoIfWidth = pSensorPrms->videoIfWidth;
    pSensorCfgPrms->fps = FVID2_FPS_30;
    pSensorCfgPrms->bpp = FVID2_BPP_BITS12;
    pSensorCfgPrms->width = 1280U;
    pSensorCfgPrms->height = 720U;
}

static Void ChainsIssSensors_Init_Imi(
    UInt32 sensorDrvId,
    ChainsIssSensor_Params *pSensorPrms,
    UInt32 captInstId[],
    Bool enBroadCast)
{
    Int32 status;
    UInt32 cnt;
    BspUtils_Ub960Status ub960Status;
    BspUtils_Ub960SourceI2cAddr ub960I2cAddr;

    ub960I2cAddr.slaveAddr = UB960_SLAVE_ADDR;
    ub960I2cAddr.numSource = SensorParams_Ov10640_Imi.maxChannels;
    for(cnt = 0 ; cnt < SensorParams_Ov10640_Imi.maxChannels; cnt ++)
    {
        ub960I2cAddr.rSlave1Addr[cnt] =
                        BspUtils_getSerAddrIMI(cnt);

        if (TRUE == enBroadCast)
        {
            /*
             *  If broadcast is enabled then provide the same I2C slave address
             *  of the sensor to all the instances
             */
            ub960I2cAddr.rSlave2Addr[cnt] =
                            Bsp_boardGetVideoDeviceI2cAddr(
                                FVID2_VID_SENSOR_IMI_OV10640_DRV,
                                FVID2_VPS_CAPT_VID_DRV,
                                captInstId[0U]);
        }
        else
        {
            ub960I2cAddr.rSlave2Addr[cnt] =
                            Bsp_boardGetVideoDeviceI2cAddr(
                                FVID2_VID_SENSOR_IMI_OV10640_DRV,
                                FVID2_VPS_CAPT_VID_DRV,
                                captInstId[cnt]);

        }
    }

    status = BspUtils_appInitUb960(
                            0U,
                            sensorDrvId,
                            &ub960I2cAddr,
                            &ub960Status);
    UTILS_assert (status == 0);
}

static Void ChainsIssSensors_DisBroadcast_Imi(
    ChainsIssSensor_Params *pSensorPrms,
    UInt32 captInstId[])
{
    Int32 status;
    UInt32 cnt;
    BspUtils_Ub960SourceI2cAddr ub960I2cAddr;

    ub960I2cAddr.slaveAddr = UB960_SLAVE_ADDR;
    ub960I2cAddr.numSource = SensorParams_Ov10640_Imi.maxChannels;
    for(cnt = 0 ; cnt < SensorParams_Ov10640_Imi.maxChannels; cnt ++)
    {
        /*
         *  For disabling the broadcast sensor programming:
         *      The slave addresses of UB960 should be unique
         *      The UB960 configuration is updated with the unique slave addr.
         */
        ub960I2cAddr.rSlave1Addr[cnt] =
                        BspUtils_getSerAddrIMI(cnt);

        ub960I2cAddr.rSlave2Addr[cnt] =
                        Bsp_boardGetVideoDeviceI2cAddr(
                            FVID2_VID_SENSOR_IMI_OV10640_DRV,
                            FVID2_VPS_CAPT_VID_DRV,
                            captInstId[cnt]);
    }

    status = BspUtils_updateSlaveAddrUb960(0U, &ub960I2cAddr);
    UTILS_assert (status == 0);
}
