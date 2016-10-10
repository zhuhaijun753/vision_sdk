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
 * \file iss_sensor_ar0140.c
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

/* #define _USE_DIGITAL_GAIN_ */


/*******************************************************************************
 *  Data structure's
 *******************************************************************************
 */

/*******************************************************************************
 *  Local Functions Declarations
 *******************************************************************************
 */
static Void ChainsIssSensor_IssGetSensorDefaultConfig_Ar0140(
    ChainsIssSensor_Params *pSensorPrms,
    IssM2mIspLink_OperatingMode ispOpMode,
    Bsp_VidSensorConfigParams *pSensorCfgPrms);
static Void ChainsIssSensor_IssGetDefaultIspConfig_Ar0140(
    ChainsIssSensor_Params *pSensorPrms,
    IssM2mIspLink_OperatingMode ispOpMode,
    IssIspConfigurationParameters *pIspConfig);
static Void ChainsIssSensor_IssGetAewbConfig_Ar0140(
    ChainsIssSensor_Params *pSensorPrms,
    IssM2mIspLink_OperatingMode ispOpMode,
    AlgorithmLink_IssAewbCreateParams *prms);
static Void ChainsIssSensors_Init_Tida262(
    UInt32 sensorDrvId,
    ChainsIssSensor_Params *pSensorPrms,
    UInt32 captInstId[],
    Bool enBroadCast);
static Void ChainsIssSensors_DisBroadcast_Tida262(
    ChainsIssSensor_Params *pSensorPrms,
    UInt32 captInstId[]);

/*******************************************************************************
 *  Globals
 *******************************************************************************
 */

/* \brief Sensor Params structure used for registering this sensor to chains_iss
 */
static ChainsIssSensor_Params SensorParams_Ar0140 = {0};

static ChainsIssSensor_Params SensorParams_Ar0140_Tida262 = {0};


/**
 * \brief Structure for ISP H3A AEWB engine parameters.
 */
static vpsissH3aAewbConfig_t aewbConfig_ar0140 =
{
    FALSE, /* enableALowComp */
    FALSE, /* enableMedFilt */
    0xFFF, /* midFiltThreshold */
    {   /* vpsissH3aPaxelConfig_t      winCfg */
        {64u, 16u}, /* Fvid2_PosConfig pos */
        36u,     /* width */
        48u,     /* height */
        32u,     /* horzCount */
        16u,     /* vertCount */
        6,      /* horzIncr */
        6,      /* vertIncr */
    },
    798U,       /* Black Line Vertical Start */
    2U,         /* Black Line Height */
    VPS_ISS_H3A_OUTPUT_MODE_SUM_ONLY, /* vpsissH3aOutputMode_t outMode; */
    0,  /* sumShift */
    1023u, /* satLimit */
    VPS_ISS_H3A_MODE_NORMAL /* vpsissH3aMode_t mode */
} ;

/**
 * \brief Structure for ISP ISIF Gain and Offset parameters.
 */
static vpsissIsifGainOfstConfig_t isifWbCfg_ar0140 =
{
    {512U, 512U, 512U},
    0U,
    {TRUE, TRUE, TRUE},
    {TRUE, TRUE, TRUE}
};

/**
 * \brief Structure for ISP IPIPE Input Parameters.
 */
static vpsissIpipeInConfig_t ipipeInputCfg_ar0140 =
{
    VPS_ISS_IPIPE_DATA_PATH_RAW_YUV422,
    {
        0U, 2U,
        1280U, 800U-2U
    }
};

/**
 * \brief Structure for ISP IPIPE White Balance Params.
 */
static vpsissIpipeWbConfig_t ipipeWbCfg =
{
    {0x0, 0x0, 0x0},
    {0x200, 0x200, 0x200, 0x200}
};

/**
 * \brief Structure for ISP GLBCE WDR Params.
 */
static vpsissGlbceWdrConfig_t glbceWdrCfg_ar0140 =
{
    TRUE,
    {
        0U,24U,64U,114U,172U,237U,307U,383U,464U,549U,638U,731U,828U,928U,
        1031U,1138U,1248U,1361U,1477U,1596U,1717U,1841U,1967U,2096U,2228U,
        2361U,2498U,2636U,2777U,2919U,3064U,3211U,3360U,3511U,3664U,3819U,
        3976U,4134U,4295U,4457U,4622U,4787U,4955U,5125U,5296U,5468U,5643U,
        5819U,5997U,6176U,6357U,6539U,6723U,6908U,7095U,7284U,7474U,7665U,
        7858U,8052U,8248U,8445U,8644U,8843U,9045U,9247U,9451U,9656U,9863U,
        10071U,10280U,10490U,10702U,10915U,11129U,11345U,11561U,11779U,11998U,
        12219U,12440U,12663U,12887U,13112U,13338U,13566U,13794U,14024U,14255U,
        14487U,14720U,14954U,15189U,15426U,15663U,15902U,16142U,16382U,16624U,
        16867U,17111U,17356U,17602U,17849U,18097U,18346U,18596U,18847U,19099U,
        19353U,19607U,19862U,20118U,20375U,20633U,20892U,21152U,21413U,21675U,
        21938U,22202U,22467U,22732U,22999U,23267U,23535U,23805U,24075U,24346U,
        24618U,24891U,25165U,25440U,25716U,25993U,26270U,26549U,26828U,27108U,
        27389U,27671U,27954U,28238U,28522U,28807U,29094U,29381U,29669U,29957U,
        30247U,30537U,30829U,31121U,31414U,31707U,32002U,32297U,32593U,32890U,
        33188U,33487U,33786U,34086U,34387U,34689U,34992U,35295U,35599U,35904U,
        36210U,36516U,36823U,37132U,37440U,37750U,38060U,38371U,38683U,38996U,
        39309U,39623U,39938U,40254U,40570U,40887U,41205U,41523U,41843U,42163U,
        42483U,42805U,43127U,43450U,43774U,44098U,44423U,44749U,45075U,45403U,
        45731U,46059U,46389U,46719U,47049U,47381U,47713U,48046U,48379U,48714U,
        49048U,49384U,49720U,50057U,50395U,50733U,51072U,51412U,51752U,52093U,
        52435U,52777U,53121U,53464U,53809U,54154U,54499U,54846U,55193U,55540U,
        55889U,56238U,56587U,56938U,57289U,57640U,57992U,58345U,58699U,59053U,
        59408U,59763U,60119U,60476U,60833U,61191U,61550U,61909U,62269U,62629U,
        62990U,63352U,63714U,64077U,64441U,64805U,65170U,65535U
    }
};

/*******************************************************************************
 *  Function Definition
 *******************************************************************************
 */

Void ChainsIssSensor_Ar0140_Init()
{
    ChainsIssSensor_Params *prms = &SensorParams_Ar0140;

    ChainsIssSensor_Params_Init(prms);

    strncpy(prms->name, SENSOR_APTINA_AR0140, CHAINS_ISS_SENSOR_MAX_NAME);

    prms->dccId = 140;
    prms->qSpiOffset = ISS_SENSORS_DCC_QSPI_OFFSET_AR0140;
    prms->dataFormat = SYSTEM_DF_BAYER_GRBG;
    prms->videoIfWidth = SYSTEM_VIFW_12BIT;
    prms->videoIfMode =SYSTEM_VIFM_SCH_CPI;

    prms->aewbMode = ALGORITHMS_ISS_AEWB_MODE_AEWB;
    prms->GetSensorDefaultConfig =
        ChainsIssSensor_IssGetSensorDefaultConfig_Ar0140;
    prms->GetAewbConfig = ChainsIssSensor_IssGetAewbConfig_Ar0140;
    prms->GetDefaultIspConfig = ChainsIssSensor_IssGetDefaultIspConfig_Ar0140;

    ChainsIssSensor_RegisterSensor(
        FVID2_VID_SENSOR_APT_AR0140_DRV,
        prms);
}

Void ChainsIssSensor_Ar0140_Tida262_Init()
{
    ChainsIssSensor_Params *prms = &SensorParams_Ar0140_Tida262;

    ChainsIssSensor_Params_Init(prms);

    strncpy(prms->name, SENSOR_APTINA_AR0140_TIDA262, CHAINS_ISS_SENSOR_MAX_NAME);

    prms->dccId = 140;
    prms->maxChannels = 4U;
    prms->qSpiOffset = ISS_SENSORS_DCC_QSPI_OFFSET_AR0140;
    prms->dataFormat = SYSTEM_DF_BAYER_GRBG;
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
        ChainsIssSensor_IssGetSensorDefaultConfig_Ar0140;
    prms->GetAewbConfig = ChainsIssSensor_IssGetAewbConfig_Ar0140;
    prms->GetDefaultIspConfig = ChainsIssSensor_IssGetDefaultIspConfig_Ar0140;
    prms->SensorPreInit = ChainsIssSensors_Init_Tida262;
    prms->SensorDisBroadcast = ChainsIssSensors_DisBroadcast_Tida262;

    ChainsIssSensor_RegisterSensor(
        FVID2_VID_SENSOR_TIDA00262_APT_AR0140_DRV,
        prms);
}

/*******************************************************************************
 *  Local Functions Definition
 *******************************************************************************
 */

static Void ChainsIssSensor_IssGetDefaultIspConfig_Ar0140(
    ChainsIssSensor_Params *pSensorPrms,
    IssM2mIspLink_OperatingMode ispOpMode,
    IssIspConfigurationParameters *pIspConfig)
{
    UTILS_assert(NULL != pIspConfig);

    /* Override common settings for specific sensor */
    pIspConfig->aewbCfg = &aewbConfig_ar0140;

    /* isifWbCfg */
    pIspConfig->isifWbCfg = &isifWbCfg_ar0140;

    pIspConfig->ipipeInputCfg = &ipipeInputCfg_ar0140;

    pIspConfig->wbCfg = &ipipeWbCfg;

    if (ISSM2MISP_LINK_OPMODE_1PASS_WDR == ispOpMode)
    {
        /* WDR Config */
        pIspConfig->glbceWdrCfg = &glbceWdrCfg_ar0140;
    }
}

static Void ChainsIssSensor_IssGetAewbConfig_Ar0140(
    ChainsIssSensor_Params *pSensorPrms,
    IssM2mIspLink_OperatingMode ispOpMode,
    AlgorithmLink_IssAewbCreateParams *prm)
{
    AlgorithmLink_IssAewbAeDynamicParams *dynPrms = NULL;

    UTILS_assert(NULL != prm);

    /* Set H3A Params */
    prm->h3aParams.winCountH = aewbConfig_ar0140.winCfg.horzCount;
    prm->h3aParams.winCountV = aewbConfig_ar0140.winCfg.vertCount;
    prm->h3aParams.winSizeH  = aewbConfig_ar0140.winCfg.width;
    prm->h3aParams.winSizeV  = aewbConfig_ar0140.winCfg.height;
    prm->h3aParams.winSkipH  = aewbConfig_ar0140.winCfg.horzIncr;
    prm->h3aParams.winSkipV  = aewbConfig_ar0140.winCfg.vertIncr;
    prm->numH3aPlanes = 1u;

    prm->dataFormat = SensorParams_Ar0140.dataFormat;
    prm->mode = (AlgorithmLink_IssAewbMode)SensorParams_Ar0140.aewbMode;

    prm->isWdrEnable = IssM2mIspLink_IsWdrMode(ispOpMode);

    prm->dccCameraId = SensorParams_Ar0140.dccId;

    dynPrms = &prm->aeDynParams;

    if (ISSM2MISP_LINK_OPMODE_1PASS_WDR == ispOpMode)
    {
        dynPrms->targetBrightnessRange.min = 18;
        dynPrms->targetBrightnessRange.max = 22;
        dynPrms->targetBrightness = 20;
    }
    else
    {
        dynPrms->targetBrightnessRange.min = 35;
        dynPrms->targetBrightnessRange.max = 45;
        dynPrms->targetBrightness = 40;
    }
    dynPrms->threshold = 5;

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


//Limiting exposure time to 16.6 ms for 16x WDR mode
    dynPrms->exposureTimeRange[2].min = 16666;
    dynPrms->exposureTimeRange[2].max = 16666;
    dynPrms->apertureLevelRange[2].min = 1;
    dynPrms->apertureLevelRange[2].max = 1;
    dynPrms->sensorGainRange[2].min = 1000;
    dynPrms->sensorGainRange[2].max = 1150;
    dynPrms->ipipeGainRange[2].min = 512;
    dynPrms->ipipeGainRange[2].max = 512;

    dynPrms->exposureTimeRange[3].min = 16666;
    dynPrms->exposureTimeRange[3].max = 16666;
    dynPrms->apertureLevelRange[3].min = 1;
    dynPrms->apertureLevelRange[3].max = 1;
    dynPrms->sensorGainRange[3].min = 1000;
    dynPrms->sensorGainRange[3].max = 12000;
    dynPrms->ipipeGainRange[3].min = 512;
    dynPrms->ipipeGainRange[3].max = 512;

#ifdef _USE_DIGITAL_GAIN_
    dynPrms->exposureTimeRange[4].min = 16666;
    dynPrms->exposureTimeRange[4].max = 16666;
    dynPrms->apertureLevelRange[4].min = 1;
    dynPrms->apertureLevelRange[4].max = 1;
    dynPrms->sensorGainRange[4].min = 12000;
    dynPrms->sensorGainRange[4].max = 12000;
    dynPrms->ipipeGainRange[4].min = 512;
    dynPrms->ipipeGainRange[4].max = 4095;

    dynPrms->numAeDynParams = 5;
#else
    dynPrms->numAeDynParams = 4;
#endif
    dynPrms->exposureTimeStepSize = 1;
    dynPrms->enableBlc = TRUE;

    prm->calbData = NULL;
}

static Void ChainsIssSensor_IssGetSensorDefaultConfig_Ar0140(
    ChainsIssSensor_Params *pSensorPrms,
    IssM2mIspLink_OperatingMode ispOpMode,
    Bsp_VidSensorConfigParams *pSensorCfgPrms)
{
    UTILS_assert (NULL != pSensorPrms);

    pSensorCfgPrms->standard = FVID2_STD_WXGA_30;
    pSensorCfgPrms->dataformat = pSensorPrms->dataFormat;
    pSensorCfgPrms->videoIfWidth = pSensorPrms->videoIfWidth;
    pSensorCfgPrms->fps = FVID2_FPS_30;
    pSensorCfgPrms->bpp = FVID2_BPP_BITS12;
    pSensorCfgPrms->width = 1280U;
    pSensorCfgPrms->height = 800U;
}

static Void ChainsIssSensors_Init_Tida262(
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
    ub960I2cAddr.numSource = SensorParams_Ar0140_Tida262.maxChannels;
    for(cnt = 0 ; cnt < SensorParams_Ar0140_Tida262.maxChannels; cnt ++)
    {
        ub960I2cAddr.rSlave1Addr[cnt] =
                        BspUtils_getSerAddrTida00262(cnt);

        if (TRUE == enBroadCast)
        {
            /*
             *  If broadcast is enabled then provide the same I2C slave address
             *  of the sensor to all the instances
             */
            ub960I2cAddr.rSlave2Addr[cnt] =
                            Bsp_boardGetVideoDeviceI2cAddr(
                                FVID2_VID_SENSOR_TIDA00262_APT_AR0140_DRV,
                                FVID2_VPS_CAPT_VID_DRV,
                                captInstId[0U]);
        }
        else
        {
            ub960I2cAddr.rSlave2Addr[cnt] =
                            Bsp_boardGetVideoDeviceI2cAddr(
                                FVID2_VID_SENSOR_TIDA00262_APT_AR0140_DRV,
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

static Void ChainsIssSensors_DisBroadcast_Tida262(
    ChainsIssSensor_Params *pSensorPrms,
    UInt32 captInstId[])
{
    Int32 status;
    UInt32 cnt;
    BspUtils_Ub960SourceI2cAddr ub960I2cAddr;

    ub960I2cAddr.slaveAddr = UB960_SLAVE_ADDR;
    ub960I2cAddr.numSource = SensorParams_Ar0140_Tida262.maxChannels;
    for(cnt = 0 ; cnt < SensorParams_Ar0140_Tida262.maxChannels; cnt ++)
    {
        /*
         *  For disabling the broadcast sensor programming:
         *      The slave addresses of UB960 should be unique
         *      The UB960 configuration is updated with the unique slave addr.
         */
        ub960I2cAddr.rSlave1Addr[cnt] =
                        BspUtils_getSerAddrTida00262(cnt);

        ub960I2cAddr.rSlave2Addr[cnt] =
                        Bsp_boardGetVideoDeviceI2cAddr(
                            FVID2_VID_SENSOR_TIDA00262_APT_AR0140_DRV,
                            FVID2_VPS_CAPT_VID_DRV,
                            captInstId[cnt]);
    }

    status = BspUtils_updateSlaveAddrUb960(0U, &ub960I2cAddr);
    UTILS_assert (status == 0);
}
