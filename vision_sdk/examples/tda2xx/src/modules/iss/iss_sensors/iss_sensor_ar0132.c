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
 * \file iss_sensor_ar0132.c
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
static Void ChainsIssSensor_IssGetSensorDefaultConfig_Ar0132(
    ChainsIssSensor_Params *pSensorPrms,
    IssM2mIspLink_OperatingMode ispOpMode,
    Bsp_VidSensorConfigParams *pSensorCfgPrms);
static Void ChainsIssSensor_IssGetDefaultIspConfig_Ar0132(
    ChainsIssSensor_Params *pSensorPrms,
    IssM2mIspLink_OperatingMode ispOpMode,
    IssIspConfigurationParameters *pIspConfig);
static Void ChainsIssSensor_IssGetAewbConfig_Ar0132(
    ChainsIssSensor_Params *pSensorPrms,
    IssM2mIspLink_OperatingMode ispOpMode,
    AlgorithmLink_IssAewbCreateParams *prms);


/*******************************************************************************
 *  Globals
 *******************************************************************************
 */

/* \brief Sensor Params structure used for registering this sensor to chains_iss
 */
static ChainsIssSensor_Params SensorParams_Ar0132 = {0};

/**
 * \brief Structure for ISP H3A AEWB engine parameters.
 */
static vpsissH3aAewbConfig_t aewbConfig_ar0132 =
{
    FALSE, /* enableALowComp */
    FALSE, /* enableMedFilt */
    0xFFF, /* midFiltThreshold */
    {   /* vpsissH3aPaxelConfig_t      winCfg */
        {32u, 16u}, /* Fvid2_PosConfig pos */
        64u,     /* width */
        24u,     /* height */
        16u,     /* horzCount */
        28u,     /* vertCount */
        4,      /* horzIncr */
        4,      /* vertIncr */
    },
    718U,       /* Black Line Vertical Start */
    2U,         /* Black Line Height */
    VPS_ISS_H3A_OUTPUT_MODE_SUM_ONLY, /* vpsissH3aOutputMode_t outMode; */
    0,  /* sumShift */
    1023u, /* satLimit */
    VPS_ISS_H3A_MODE_NORMAL /* vpsissH3aMode_t mode */

} ;

static vpsissIsifGainOfstConfig_t isifWbCfg_ar0132 =
{
    {512U, 512U, 512U},
    0U,
    {TRUE, TRUE, TRUE},
    {TRUE, TRUE, TRUE}
};

static vpsissIpipeInConfig_t ipipeInputCfg_ar132 =
{
    VPS_ISS_IPIPE_DATA_PATH_RAW_YUV422,
    {0U, 2U, 1280U, 720U - 2U}
};

static vpsissGlbceConfig_t glbceCfg_ar0132 =
{
    TRUE,           /* ENABLE */
    255,              /* IR Strength */
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

static vpsissGlbcePerceptConfig_t glbceFwbPerCfg_ar0132 =
{
    FALSE,
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

static vpsissGlbcePerceptConfig_t glbceRevPerCfg_ar0132 =
{
    FALSE,
    {
        0,228,455,683,910,1138,1369,1628,1912,2221,2556,2916,3304,3717,
        4158,4626,5122,5645,6197,6777,7386,8024,8691,9387,10113,10869,
        11654,12471,13317,14194,15103,16042,17012,18014,19048,20113,21210,
        22340,23501,24696,25922,27182,28475,29800,31159,32552,33977,
        35437,36930,38458,40019,41615,43245,44910,46609,48343,50112,
        51916,53755,55630,57539,59485,61466,63482,65535
    }
};

static vpsissGlbceWdrConfig_t glbceWdrCfg_ar0132 =
{
    TRUE,
    {
        0U   ,
        255U ,
        361U ,
        442U ,
        510U ,
        570U ,
        625U ,
        675U ,
        721U ,
        765U ,
        806U ,
        846U ,
        883U ,
        919U ,
        954U ,
        988U ,
        1020U,
        1051U,
        1082U,
        1112U,
        1140U,
        1169U,
        1196U,
        1223U,
        1249U,
        1275U,
        1300U,
        1325U,
        1349U,
        1373U,
        1397U,
        1420U,
        1442U,
        1465U,
        1487U,
        1509U,
        1530U,
        1551U,
        1572U,
        1592U,
        1613U,
        1633U,
        1653U,
        1672U,
        1691U,
        1711U,
        1729U,
        1748U,
        1767U,
        1785U,
        1803U,
        1821U,
        1839U,
        1856U,
        1874U,
        1891U,
        1908U,
        1925U,
        1942U,
        1959U,
        1975U,
        1992U,
        2008U,
        2024U,
        2040U,
        2056U,
        2072U,
        2087U,
        2103U,
        2118U,
        2133U,
        2149U,
        2164U,
        2179U,
        2194U,
        2208U,
        2223U,
        2238U,
        2252U,
        2266U,
        2281U,
        2295U,
        2309U,
        2323U,
        2337U,
        2351U,
        2365U,
        2378U,
        2392U,
        2406U,
        2419U,
        2433U,
        2446U,
        2459U,
        2472U,
        2485U,
        2498U,
        2511U,
        2524U,
        2537U,
        2550U,
        2563U,
        2575U,
        2588U,
        2600U,
        2613U,
        2625U,
        2638U,
        2650U,
        2662U,
        2674U,
        2687U,
        2699U,
        2711U,
        2723U,
        2735U,
        2746U,
        2758U,
        2770U,
        2782U,
        2793U,
        2805U,
        2817U,
        2828U,
        2840U,
        2851U,
        2862U,
        2874U,
        2885U,
        2896U,
        2907U,
        2919U,
        2930U,
        2941U,
        2952U,
        2963U,
        2974U,
        2985U,
        2996U,
        3006U,
        3017U,
        3028U,
        3039U,
        3049U,
        3060U,
        3071U,
        3081U,
        3092U,
        3102U,
        3113U,
        3123U,
        3133U,
        3144U,
        3154U,
        3164U,
        3175U,
        3185U,
        3195U,
        3205U,
        3215U,
        3226U,
        3236U,
        3246U,
        3256U,
        3266U,
        3276U,
        3285U,
        3295U,
        3305U,
        3315U,
        3325U,
        3335U,
        3344U,
        3354U,
        3364U,
        3373U,
        3383U,
        3393U,
        3402U,
        3412U,
        3421U,
        3431U,
        3440U,
        3450U,
        3459U,
        3468U,
        3478U,
        3487U,
        3496U,
        3506U,
        3515U,
        3524U,
        3533U,
        3543U,
        3552U,
        3561U,
        3570U,
        3579U,
        3588U,
        3597U,
        3606U,
        3615U,
        3624U,
        3633U,
        3642U,
        3651U,
        3660U,
        3669U,
        3678U,
        3686U,
        3695U,
        3704U,
        3713U,
        3722U,
        3730U,
        3739U,
        3748U,
        3756U,
        3765U,
        3774U,
        3782U,
        3791U,
        3799U,
        3808U,
        3816U,
        3825U,
        3833U,
        3842U,
        3850U,
        3859U,
        3867U,
        3876U,
        3884U,
        3892U,
        3901U,
        3909U,
        3917U,
        3926U,
        3934U,
        3942U,
        3950U,
        3959U,
        3967U,
        3975U,
        3983U,
        3991U,
        4000U,
        4008U,
        4016U,
        4024U,
        4032U,
        4040U,
        4048U,
        4056U,
        4064U,
        4072U,
        4080U
    }
};

/*******************************************************************************
 *  Function Definition
 *******************************************************************************
 */

Void ChainsIssSensor_Ar0132_Init()
{
    ChainsIssSensor_Params *prms = &SensorParams_Ar0132;

    ChainsIssSensor_Params_Init(prms);

    strncpy(prms->name, SENSOR_APTINA_AR0132, CHAINS_ISS_SENSOR_MAX_NAME);

    prms->dccId = 132;
    prms->qSpiOffset = ISS_SENSORS_DCC_QSPI_OFFSET_AR0132;
    prms->dataFormat = SYSTEM_DF_BAYER_GRBG;
    prms->videoIfWidth = SYSTEM_VIFW_12BIT;
    prms->videoIfMode =SYSTEM_VIFM_SCH_CPI;

    prms->aewbMode = ALGORITHMS_ISS_AEWB_MODE_AWB;
    prms->GetSensorDefaultConfig =
        ChainsIssSensor_IssGetSensorDefaultConfig_Ar0132;
    prms->GetAewbConfig = ChainsIssSensor_IssGetAewbConfig_Ar0132;
    prms->GetDefaultIspConfig = ChainsIssSensor_IssGetDefaultIspConfig_Ar0132;

    ChainsIssSensor_RegisterSensor(
        FVID2_VID_SENSOR_APT_AR0132_DRV,
        prms);
}


/*******************************************************************************
 *  Local Functions Definition
 *******************************************************************************
 */



static Void ChainsIssSensor_IssGetDefaultIspConfig_Ar0132(
    ChainsIssSensor_Params *pSensorPrms,
    IssM2mIspLink_OperatingMode ispOpMode,
    IssIspConfigurationParameters *pIspConfig)
{
    UTILS_assert(NULL != pIspConfig);

    /* Override common settings for specific sensor */
    pIspConfig->aewbCfg = &aewbConfig_ar0132;

    /* isifWbCfg */
    pIspConfig->isifWbCfg = &isifWbCfg_ar0132;

    pIspConfig->ipipeInputCfg = &ipipeInputCfg_ar132;

    pIspConfig->glbceCfg = &glbceCfg_ar0132;

    /* fwdPerCfg */
    pIspConfig->glbceFwdPerCfg = &glbceFwbPerCfg_ar0132;

    /* revPerCfg */
    pIspConfig->glbceRevPerCfg = &glbceRevPerCfg_ar0132;

    if (ISSM2MISP_LINK_OPMODE_12BIT_MONOCHROME == ispOpMode)
    {
        /* WDR Config */
        pIspConfig->glbceWdrCfg = &glbceWdrCfg_ar0132;
        glbceCfg_ar0132.irStrength = 0U;
    }
}

static Void ChainsIssSensor_IssGetAewbConfig_Ar0132(
    ChainsIssSensor_Params *pSensorPrms,
    IssM2mIspLink_OperatingMode ispOpMode,
    AlgorithmLink_IssAewbCreateParams *prm)
{
    AlgorithmLink_IssAewbAeDynamicParams *dynPrms = NULL;

    UTILS_assert(NULL != prm);

    /* Set H3A Params */
    prm->h3aParams.winCountH = aewbConfig_ar0132.winCfg.horzCount;
    prm->h3aParams.winCountV = aewbConfig_ar0132.winCfg.vertCount;
    prm->h3aParams.winSizeH  = aewbConfig_ar0132.winCfg.width;
    prm->h3aParams.winSizeV  = aewbConfig_ar0132.winCfg.height;
    prm->h3aParams.winSkipH  = aewbConfig_ar0132.winCfg.horzIncr;
    prm->h3aParams.winSkipV  = aewbConfig_ar0132.winCfg.vertIncr;
    prm->numH3aPlanes = 1u;

    prm->dataFormat = SensorParams_Ar0132.dataFormat;
    prm->mode = (AlgorithmLink_IssAewbMode)SensorParams_Ar0132.aewbMode;

    prm->isWdrEnable = IssM2mIspLink_IsWdrMode(ispOpMode);

    prm->dccCameraId = SensorParams_Ar0132.dccId;

    prm->calbData = NULL;

    dynPrms = &prm->aeDynParams;

    dynPrms->targetBrightnessRange.min = 0;
    dynPrms->targetBrightnessRange.max = 0;
    dynPrms->targetBrightness = 0;
    dynPrms->threshold = 0;

    dynPrms->exposureTimeRange[0].min = 0;
    dynPrms->exposureTimeRange[0].max = 0;
    dynPrms->apertureLevelRange[0].min = 1;
    dynPrms->apertureLevelRange[0].max = 1;
    dynPrms->sensorGainRange[0].min = 1000;
    dynPrms->sensorGainRange[0].max = 1000;
    dynPrms->ipipeGainRange[0].min = 512;
    dynPrms->ipipeGainRange[0].max = 512;

    dynPrms->numAeDynParams = 1;
    dynPrms->exposureTimeStepSize = 1;
    dynPrms->enableBlc = FALSE;
}

static Void ChainsIssSensor_IssGetSensorDefaultConfig_Ar0132(
    ChainsIssSensor_Params *pSensorPrms,
    IssM2mIspLink_OperatingMode ispOpMode,
    Bsp_VidSensorConfigParams *pSensorCfgPrms)
{
    UTILS_assert (NULL != pSensorPrms);

    pSensorCfgPrms->standard = SYSTEM_STD_720P_60;
    pSensorCfgPrms->dataformat = pSensorPrms->dataFormat;
    pSensorCfgPrms->videoIfWidth = pSensorPrms->videoIfWidth;
    pSensorCfgPrms->fps = FVID2_FPS_60;
    pSensorCfgPrms->bpp = FVID2_BPP_BITS12;
    pSensorCfgPrms->width = 1280U;
    pSensorCfgPrms->height = 720U;
}

