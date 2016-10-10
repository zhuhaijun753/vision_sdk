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
 * \file chains_iss_sensor.h
 *
 * \brief APIs for controlling external sensors.
 *
 *
 *******************************************************************************
 */

#ifndef _CHAINS_ISS_H_
#define _CHAINS_ISS_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *  Include files
 *******************************************************************************
 */
#include <include/link_api/issM2mIspLink.h>
#include <include/link_api/issCaptureLink.h>
#include <include/link_api/algorithmLink_issAewb.h>


/*******************************************************************************
 *  Defines
 *******************************************************************************
 */

/* Maximum number of channels supported */
#define CHAINS_ISS_SENSOR_MAX_CHANNEL               (4U)

/* Max char in the sensor name */
#define CHAINS_ISS_SENSOR_MAX_NAME                  (50U)

/* Supports Sensor's name */
#define SENSOR_OMNIVISION_OV10640_CSI2  "SENSOR_OV10640_CSI2"
#define SENSOR_OMNIVISION_OV10640_CPI   "SENSOR_OV10640_PARALLEL"
#define SENSOR_APTINA_AR0140            "SENSOR_AR0140"
#define SENSOR_APTINA_AR0140_TIDA262    "SENSOR_AR0140_TIDA262"
#define SENSOR_APTINA_AR0132            "SENSOR_AR0132"
#define SENSOR_SONY_IMX224_CSI2         "SENSOR_IMX224_CSI2"
#define SENSOR_OMNIVISION_OV10640_IMI   "SENSOR_OV10640_IMI"


/* IOCTL Base number */
#define CHAINS_ISS_SENSOR_IOCTL_BASE            (0x8000U)

/**
 *******************************************************************************
 *
 *   \brief Command to get Default ISP configuration,
 *          Typically used to get the H3A and GLBCE configuration, the modules
 *          for which DCC is not supported.
 *          Also used for the sensor for which DCC is not available..
 *
 *   \param pIspCfg         Pointer to ISP configuation
 *                          #IssIspConfigurationParameters
 *
 *   \return SYSTEM_STATUS_SOK on success
 *           Error on failure
 *
 *******************************************************************************
 */
#define CHAINS_ISS_SENSOR_GET_DEFAULT_ISP_CONFIG    (                          \
    CHAINS_ISS_SENSOR_IOCTL_BASE)

/**
 *******************************************************************************
 *
 *   \brief Command to get exposure parameters
 *          When WDR is enabled in the sensor, this command is used to get
 *          the exposure ratio.
 *
 *   \param pAewbAlgOut         Pointer to AEWB Output configuation
 *                              #IssAewbAlgOutParams
 *
 *   \return SYSTEM_STATUS_SOK on success
 *           Error on failure
 *
 *******************************************************************************
 */
#define CHAINS_ISS_SENSOR_GET_EXPOSURE_PARAMS       (                          \
    CHAINS_ISS_SENSOR_GET_DEFAULT_ISP_CONFIG + 1U)

/**
 *******************************************************************************
 *
 *   \brief Command to set AEWB parameters
 *          When AEWB algorithm is running, it requires to change
 *          the AE and analog gain in the sensor runtime.
 *          This command is used for setting AE and analog gain
 *          in the sensor.
 *
 *   \param pAewbAlgOut         Pointer to AEWB Output configuation
 *                              #IssAewbAlgOutParams
 *
 *   \return SYSTEM_STATUS_SOK on success
 *           Error on failure
 *
 *******************************************************************************
 */
#define CHAINS_ISS_SENSOR_SET_AEWB_PARAMS           (                          \
    CHAINS_ISS_SENSOR_GET_EXPOSURE_PARAMS + 1U)

/**
 *******************************************************************************
 *
 *   \brief Command to Initialize AEWB Create parameters
 *          AEWB create parameters are sensor specific, this command is used
 *          for initializing create args ofr AEWB algorithm.
 *
 *   \param pAewbCreatePrms     Pointer to AEWB Create Configuration
 *                              #AlgorithmLink_IssAewbCreateParams
 *
 *   \return SYSTEM_STATUS_SOK on success
 *           Error on failure
 *
 *******************************************************************************
 */
#define CHAINS_ISS_SENSOR_INIT_AEWB_CREATE_PARAMS   (                          \
    CHAINS_ISS_SENSOR_SET_AEWB_PARAMS + 1U)

/**
 *******************************************************************************
 *
 *   \brief Command to set the new resolution in sensor
 *          The sensor will be opened with the default resolution
 *          at create time. This command can be used to change this
 *          default resolution.
 *
 *   \param bspConfig           Pointer to AEWB Create Configuration
 *                              #Bsp_VidSensorConfigParams
 *
 *   \return SYSTEM_STATUS_SOK on success
 *           Error on failure
 *
 *******************************************************************************
 */
#define CHAINS_ISS_SENSOR_SET_CONFIG                (                          \
    CHAINS_ISS_SENSOR_INIT_AEWB_CREATE_PARAMS + 1U)

/**
 *******************************************************************************
 *
 *   \brief Command to set the new sensor frame rate
 *          The sensor will be opened with the default resolution
 *          at create time. This command can be used to change this
 *          default resolution.
 *
 *   \param bspConfig           Pointer to AEWB Create Configuration
 *                              #Bsp_VidSensorConfigParams
 *
 *   \return SYSTEM_STATUS_SOK on success
 *           Error on failure
 *
 *******************************************************************************
 */
#define CHAINS_ISS_SENSOR_SET_FPS                (                          \
    CHAINS_ISS_SENSOR_SET_CONFIG + 1U)

/**
 *******************************************************************************
 *
 *   \brief Command to set the enable mirroring & flipping of captured images.
 *          \CAUTION Not All sensor support this.
 *
 *   \param UInt32 *  Value pointed by this pointer
 *                      if == 0, disable flipping and mirroring
 *                      if == 1, enable flipping
 *                      if == 2, enable mirroring
 *                      if == 3, enable flipping & enable mirroring
 *
 *   \return SYSTEM_STATUS_SOK on success Error on failure
 *
 *******************************************************************************
 */
#define CHAINS_ISS_SENSOR_SET_FLIP_MIRROR (CHAINS_ISS_SENSOR_SET_FPS + 1U)

/*******************************************************************************
 *  Data structure's
 *******************************************************************************
 */

/*
 *  \brief Create Time parameters, ISS sensor layer uses these
 *         parameters and configures board module and sensor.
 */
typedef struct
{
    char                            name[CHAINS_ISS_SENSOR_MAX_NAME];
    /**< Name of the sensor */
    UInt32                          numChan;
    /**< Number of channel in which sensor is to be opend */
    IssM2mIspLink_OperatingMode     ispOpMode;
    /**< ISP operation mode, used for configuring sensor in this mode */
    UInt32                          captInstId[CHAINS_ISS_SENSOR_MAX_CHANNEL];
    /**< Capture Instance Id, used for configuring board modules
         Fvid2 DRV ID */
    Bool sensorCfgBroadcast;
    /**< In case of identical multi camera system which are programmed
         identically then broadcast sensor programming is advntageous */
} ChainsIssSensor_CreateParams;

/*
 *  \brief Sensor Information structure,
 */
typedef struct
{
    UInt32                          numChan;
    /**< number of open sensor handle */
    Ptr                             sensorHandle[CHAINS_ISS_SENSOR_MAX_CHANNEL];
    /**< Pointer to the sensor handle */
    UInt32                          isDccSupported;
    /**< Flag to indicate if the DCC is supported or not,
         Following two variables are valid only if this is set to TRUE */
    UInt32                          qSpiOffset;
    /**< QSPI offset where DCC profile can be stored */
    Bsp_VidSensorDccParams          dccParams;
    /**< Sensor DCC Params */
    UInt32                          aewbMode;
    /**< AEWB mode */

    Bsp_VidSensorConfigParams       cfgPrms;

    Bsp_VidSensorOffsetParams       offsetPrms;
    /**< */

    System_VideoIfMode              videoIfMode;
    /**< Discrete Hsync/Vsync or CPI or CSI2 or LVDS */

    System_Csi2DataFormat           inCsi2DataFormat;
    /**< Input CSI2 Data Format, valid only if videoIfMode is CSI2 */
    UInt32                          inCsi2VirtualChanNum;
    /**< CSI2 Virtual Channel Num, must be between 0 to 3 */
    IssCaptureLink_Csi2Params       csi2Prms;
    /**< CSI2 Params when sensor is CSI2 based */
} ChainsIssSensor_Info;


/*******************************************************************************
 *  Functions Declarations
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \brief Creates the sensor, based on the create params, it
 *        searches for the given sensor in the registered sensors,
 *        configures the board module if sensor supports board
 *        module and creates sensor fvid2 driver.
 *        It also configures sensor in the given WDR mode and also sets
 *        the default output resolution in the sensor.
 *
 * \params pCreatePrms      Pointer to the create params
 *
 * \return handle to the created sensor
 *         NULL if there is any error
 *
 *******************************************************************************
*/
Ptr ChainsIssSensor_Create(ChainsIssSensor_CreateParams *pCreatePrms);

/**
 *******************************************************************************
 *
 * \brief Used for sending control commands, this layer
 *        supports all configuration using control commants.
 *        For the list of supported control command, see defines
 *        section of this file.
 *
 *        Must be called after Create function
 *
 * \params handle           Handle to the created sensor
 * \params cmd              Control command
 * \params cmdArgs          Pointer to the command specific arguments.
 * \params cmdRetArgs       Pointer to the command specific return arguments
 *
 * \return SOK in case of success
 *         error otherwise
 *
 *******************************************************************************
*/
Int32 ChainsIssSensor_Control(Ptr handle, UInt32 cmd, Ptr cmdArgs, Ptr cmdRetArgs);

/**
 *******************************************************************************
 *
 * \brief Used Starting the sensor, which is alraedy opened using
 *        Create function.
 *        Must be called after Create function.
 *
 * \return SOK in case of success
 *         error otherwise
 *
 *******************************************************************************
*/
Int32 ChainsIssSensor_Start(Ptr handle);

/**
 *******************************************************************************
 *
 * \brief Used stopping the sensor, which is alraedy opened using
 *        Create function.
 *        Must be called after Create function.
 *
 * \return SOK in case of success
 *         error otherwise
 *
 *******************************************************************************
*/
Int32 ChainsIssSensor_Stop(Ptr handle);

/**
 *******************************************************************************
 *
 * \brief Used deleting the sensor, which is already opened using
 *        Create function. Internally it deletes sensor's fvid2 driver.
 *        After this call, handle is not valid.
 *
 *        Must be called after Create function.
 *
 * \return SOK in case of success
 *         error otherwise
 *
 *******************************************************************************
*/
Int32 ChainsIssSensor_Delete(Ptr handle);

/**
 *******************************************************************************
 *
 * \brief This API returns sensor information, it returns how the
 *        sensor is connected to the capture module, It also
 *        returns sensor specific DCC information if dcc is
 *        supported by the sensor.
 *
 * \params name             Name of the sensor
 * \params pSensorInfo      Pointer to the sensor information
 *
 * \return SOK in case of success
 *         error otherwise
 *
 *******************************************************************************
*/
Int32 ChainsIssSensor_GetInfoFromName(
    char name[], ChainsIssSensor_Info *pSensorInfo);

/**
 *******************************************************************************
 *
 * \brief Used to get the sensor name from the given DCC camera id.
 *        API can be used at any point of time.
 *        For the sensors having same dcc id, it returns the first
 *        sensor name found in the list.
 *        This API is typically used for processing DCC commands, so
 *        if the sensors are having two dcc ids, it means both have same
 *        DCC information also.
 *
 * \params dccId            DCC ID of the sensor
 * \params sensorName       Sensor name output parameter
 *
 * \return SOK in case of success
 *         error otherwise
 *
 *******************************************************************************
*/
Int32 ChainsIssSensor_GetNameFromDccId(UInt32 dccId, char sensorName[]);

/**
 *******************************************************************************
 *
 * \brief Initialize sensor create Parameter
 *
 * \params prms             Pointer to the sensor create parameters
 *
 *******************************************************************************
*/
static inline Void ChainsIssSensor_CreateParams_init(
    ChainsIssSensor_CreateParams *prms);

/**
 *******************************************************************************
 *
 * \brief Initialize sensor info with the default parameters
 *
 * \params prms             Pointer to the sensor create parameters
 *
 *******************************************************************************
*/
static inline Void ChainsIssSensor_Info_Init(
    ChainsIssSensor_Info *prms);


/*******************************************************************************
 *  Functions definitions
 *******************************************************************************
 */

static inline Void ChainsIssSensor_CreateParams_Init(
    ChainsIssSensor_CreateParams *prms)
{
    UTILS_assert(NULL != prms);

    prms->numChan = 1U;
    prms->ispOpMode = ISSM2MISP_LINK_OPMODE_12BIT_LINEAR;
    prms->captInstId[0U] = VPS_CAPT_INST_ISS_CAL_A;
    strncpy(prms->name, SENSOR_APTINA_AR0140, CHAINS_ISS_SENSOR_MAX_NAME);
}

static inline Void ChainsIssSensor_Info_Init(
    ChainsIssSensor_Info *prms)
{
    UTILS_assert(NULL != prms);

    memset(prms, 0x0, sizeof(ChainsIssSensor_Info));

    prms->numChan = 1U;
    prms->sensorHandle[0U] = NULL;
    prms->isDccSupported = FALSE;
    prms->qSpiOffset = 24U*1024U*1024U;
    prms->aewbMode = ALGORITHMS_ISS_AEWB_MODE_AEWB;

    BspVidSensorConfigParams_Init(&prms->cfgPrms);
    BspVidSensorOffsetParams_Init(&prms->offsetPrms);

    prms->videoIfMode = SYSTEM_VIFM_SCH_CPI;
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
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* End of _CHAINS_ISS_H_*/


/*@}*/
