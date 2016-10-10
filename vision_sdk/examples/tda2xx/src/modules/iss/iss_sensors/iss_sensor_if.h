/*
 *******************************************************************************
 *
 * Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \ingroup EXAMPLES_API
 * \defgroup EXAMPLES_CHAINS_ISS_API APIs for controlling ISS sensors
 *
 *
 * @{
 *
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \file iss_sensors_if.h
 *
 * \brief APIs for controlling external sensors.
 *
 *
 *******************************************************************************
 */

#ifndef ISS_SENSORS_IF_H_
#define ISS_SENSORS_IF_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *  Include files
 *******************************************************************************
 */


/*******************************************************************************
 *  Defines
 *******************************************************************************
 */


/*******************************************************************************
 *  Data structure's
 *******************************************************************************
 */

/* Forward Declaration of Sensor Params */
typedef struct ChainsIssSensor_Params_t ChainsIssSensor_Params;

typedef Void (*IssSensorPreInitialize) (
                    UInt32 sensorDrvId,
                    ChainsIssSensor_Params *pSensorPrms,
                    UInt32 captInstId[],
                    Bool enBroadCast);
typedef Void (*IssSensorDisBroadcast) (
                    ChainsIssSensor_Params *pSensorPrms,
                    UInt32 captInstId[]);
typedef Void (*IssGetSensorParams) (ChainsIssSensor_Params *pSensorPrms,
                                    IssM2mIspLink_OperatingMode ispOpMode,
                                    Bsp_VidSensorConfigParams *pSensorCfgPrms);
typedef Void (*IssGetAewbConfig) (ChainsIssSensor_Params *pSensorPrms,
                                  IssM2mIspLink_OperatingMode ispOpMode,
                                  AlgorithmLink_IssAewbCreateParams *pPrm);
typedef Void (*IssGetDefaultIspConfig) (ChainsIssSensor_Params *pSensorPrms,
                                        IssM2mIspLink_OperatingMode ispOpMode,
                                        IssIspConfigurationParameters *ispCfg);

/**
 *  \brief Structure containins sensor information, used for
 *         registering it to the sensor framework.
 */
struct ChainsIssSensor_Params_t {
    char                            name[CHAINS_ISS_SENSOR_MAX_NAME];
    /**< Name of the sensor, using which it is registered to this framework */
    UInt32                          dccId;
    /**< DCC Id of the sensor,
         typically sensor driver provides dcc id, but if sensor driver
         is not opened and it is required to flash dcc profile in qspi,
         this id will be used.
         Note: it should be same as the id in the driver */
    UInt32                          qSpiOffset;
    /**< QSPI Offset at which dcc profile is saved in qspi,
         used only if dcc is supported by the sensor driver */

    System_VideoDataFormat          dataFormat;
    /**< Supported sensor dataFormat, it can be Bayer/YUV/RGB \n
     *   For valid values see System_DataFormat. */

    System_VideoIfWidth             videoIfWidth;
    /**< 8 or 12 or 14 camera interface mode.
     *   For valid values see #System_VideoIfWidth. */

     System_VideoIfMode             videoIfMode;
     /**< Discrete Hsync/Vsync or CPI or CSI2 or LVDS */

    System_Csi2DataFormat           inCsi2DataFormat;
    /**< Input CSI2 Data Format, valid only if videoIfMode is CSI2 */
    UInt32                          inCsi2VirtualChanNum;
    /**< CSI2 Virtual Channel Num, must be between 0 to 3 */
    IssCaptureLink_Csi2Params       csi2Prms;
    /**< CSI2 Params when sensor is CSI2 based */

    UInt32                          aewbMode;
    /**< Supported AEWB mode for this sensor */

    UInt32                          maxChannels;
    /**< Max number of channels supported by the Sensor,
         Must be less than #CHAINS_ISS_SENSOR_MAX_CHANNEL */

    Bool                            isBoardModuleSupported;
    /**< Flag to indicate if the sensor is supported in the board
         module of BSP drivers.
         Board module in BSP is used for configuring pinmux, IO expanders
         and other board specific configurations.
         Setting this flag to FALSE bypasses all the board specific
         configuration and directly opens up the sensor driver. */
    UInt32                          i2cInstId;
    /**< Sensor I2c Instance, used only if board module is not used
         for this sensor, ie #isBoardModuleSupported flag is set to FALSE */
    UInt32                          i2cAddr[CHAINS_ISS_SENSOR_MAX_CHANNEL];
    /**< I2c address of the sensors not supporting board modules,
         used only if board module is not used
         for this sensor, ie #isBoardModuleSupported flag is set to FALSE */

    /* Function pointers  */
    IssSensorPreInitialize          SensorPreInit;
    /**< Pointer to the function to pre-initialize sensor */
    IssSensorDisBroadcast           SensorDisBroadcast;
    /**< Pointer to the function to disable broadcast programming */
    IssGetSensorParams              GetSensorDefaultConfig;
    /**< Pointer to the function to get the sensor output parameters,
         like frame size, standard, fps, bpp etc.. */
    IssGetAewbConfig                GetAewbConfig;
    /**< Pointer to the function for getting AEWB Configuration
         for this sensor */
    IssGetDefaultIspConfig          GetDefaultIspConfig;
    /**< Pointer to the function for getting default ISP configuration,
         This ISP configuration is typically used when DCC profile is
         not available */
    Bool                            SensorCfgBroadcast;
    /**< In case of identical multi camera system which are programmed
         identically then broadcast sensor programming is advntageous */
};


/*******************************************************************************
 *  Functions Declarations
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \brief Initialize ISS Sensor framework,
 *        It initializes iss sensor frame work and also calls init functions of
 *        all the supported sensors, which in turn register itself to this
 *        framework.
 *
 *******************************************************************************
*/
Void ChainsIssSensor_Init();

/**
 *******************************************************************************
 *
 * \brief Used for registering sensor to the iss sensor framework,
 *
 * \params fvid2Id          FVID2 Driver id of the sensor
 * \params pSensorInfo      Pointer to the sensor information structure
 *
 * \return SOK if sensor is registered successfully
 *         error otherwise
 *
 *******************************************************************************
*/
Int32 ChainsIssSensor_RegisterSensor(
    UInt32 fvid2Id, ChainsIssSensor_Params *pSensorInfo);

/**
 *******************************************************************************
 *
 * \brief Initializes Sensor parmas, used for registering sensor to
 *        the iss sensor layer
 *
 * \params prms             Pointer to the sensor params
 *
 *******************************************************************************
*/
static inline Void ChainsIssSensor_Params_Init(ChainsIssSensor_Params *prms);


/*******************************************************************************
 *  Functions Declarations
 *******************************************************************************
 */

static inline Void ChainsIssSensor_Params_Init(ChainsIssSensor_Params *prms)
{
    UTILS_assert(NULL != prms);

    memset(prms, 0x0, sizeof(ChainsIssSensor_Params));

    strncpy(prms->name, SENSOR_APTINA_AR0140, CHAINS_ISS_SENSOR_MAX_NAME);

    prms->dccId = 0U;
    prms->maxChannels = 1U;
    prms->isBoardModuleSupported = TRUE;
    prms->qSpiOffset = 24U*1024U*1024U;
    prms->dataFormat = SYSTEM_DF_RAW12;
    prms->videoIfWidth = SYSTEM_VIFW_12BIT;
    prms->videoIfMode =SYSTEM_VIFM_SCH_CPI;
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

    prms->aewbMode = ALGORITHMS_ISS_AEWB_MODE_AEWB;
    prms->GetSensorDefaultConfig = NULL;
    prms->GetAewbConfig = NULL;
    prms->GetDefaultIspConfig = NULL;
}


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* End of ISS_SENSORS_IF_H_*/


/*@}*/
