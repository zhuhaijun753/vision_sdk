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
 * \file Chains_iss.c
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
#define CHAINS_ISS_SENSOR_MAX_SENSORS               (10U)


/*******************************************************************************
 *  Data Structures
 *******************************************************************************
 */
typedef struct
{
    UInt32                      isUsed;
    /**< Flag to indicate if given instance is free or not */
    UInt32                      sensorDrvId;
    /**< Sensor driver id with which it is registered */
    ChainsIssSensor_Params      sensorParams;
    /**< Sensor Parameters */
    Ptr                         sensorHandle[CHAINS_ISS_SENSOR_MAX_CHANNEL];
    /**< Pointer to the sensor handle */
    Bsp_VidSensorFeatures       sensorFeatures;
    /**< List of features supported by sensor */
    Bsp_VidSensorDccParams      dccParams;
    /**< Sensor DCC Params */
    ChainsIssSensor_CreateParams createPrms;
} ChainsIssSensor_Table;



/*******************************************************************************
 *  Globals
 *******************************************************************************
 */
ChainsIssSensor_Table   gChainsIssSensorTable[CHAINS_ISS_SENSOR_MAX_SENSORS];

/*******************************************************************************
 *  Local Functions Declarations
 *******************************************************************************
 */
static Int32 checkForHandle(Ptr handle);
static Void copySensorInfo(
    ChainsIssSensor_Table *pSensorTable, ChainsIssSensor_Info *pSensorInfo);
static Int32 setWdrParams(ChainsIssSensor_Table *pSensorTable);
static Int32 getDefaultIspConfig(
    ChainsIssSensor_Table *pSensorTable,
    IssIspConfigurationParameters *pIspConfig);
static Int32 getExposureParams(
    ChainsIssSensor_Table *pSensorTable, IssAewbAlgOutParams *pAewbAlgOut);
static Int32 setAewbParams(
    ChainsIssSensor_Table *pSensorTable, IssAewbAlgOutParams *pAewbAlgOut);
static Int32 getAewbCreateParams(
    ChainsIssSensor_Table *pSensorTable, AlgorithmLink_IssAewbCreateParams* pPrms);
static Int32 setConfig(
    ChainsIssSensor_Table *pSensorTable, Bsp_VidSensorConfigParams *cfgPrms);
static setFps(ChainsIssSensor_Table *pSensorTable, UInt32 *pFps);
static setFlipParams(ChainsIssSensor_Table *pSensorTable, UInt32 *pFps);

/*******************************************************************************
 *  Function Definition
 *******************************************************************************
 */

Void ChainsIssSensor_Init()
{
    UInt32 cnt;

    memset (&gChainsIssSensorTable, 0x0,
        sizeof(ChainsIssSensor_Table)*CHAINS_ISS_SENSOR_MAX_SENSORS);

    for (cnt = 0U; cnt < CHAINS_ISS_SENSOR_MAX_SENSORS; cnt ++)
    {
        gChainsIssSensorTable[cnt].isUsed = FALSE;
    }

    ChainsIssSensor_Ar0132_Init();
    ChainsIssSensor_Ar0140_Init();
    ChainsIssSensor_Imx224_Init();
    ChainsIssSensor_Ov10640_Csi2_Init();
    ChainsIssSensor_Ov10640_Cpi_Init();
    ChainsIssSensor_Ar0140_Tida262_Init();
    ChainsIssSensor_Ov10640_Imi_Init();
}

Int32 ChainsIssSensor_RegisterSensor(
    UInt32 fvid2Id, ChainsIssSensor_Params *pSensorInfo)
{
    Int32 status = SYSTEM_LINK_STATUS_SOK;
    UInt32 cnt = 0U;
    ChainsIssSensor_Table *pSensorTable;

    if ((NULL == pSensorInfo) || (0U == fvid2Id))
    {
        status = SYSTEM_LINK_STATUS_EFAIL;
    }
    else
    {
        /* Find a free entry in the sensor table */
        for (cnt = 0U; cnt < CHAINS_ISS_SENSOR_MAX_SENSORS; cnt ++)
        {
            pSensorTable = &gChainsIssSensorTable[cnt];
            if (FALSE == pSensorTable->isUsed)
            {
                /* Copy sensor params */
                pSensorTable->sensorDrvId = fvid2Id;
                memcpy(&pSensorTable->sensorParams, pSensorInfo,
                    sizeof(ChainsIssSensor_Params));
                pSensorTable->isUsed = TRUE;
                break;
            }
        }

        if (cnt == CHAINS_ISS_SENSOR_MAX_SENSORS)
        {
            Vps_printf(" ISS_SENSOR: Could not register sensor \n");
            status = SYSTEM_LINK_STATUS_EFAIL;
        }
    }

    return (status);
}

Ptr ChainsIssSensor_Create(ChainsIssSensor_CreateParams *pCreatePrms)
{
    Int32 status = SYSTEM_LINK_STATUS_SOK;
    UInt32 cnt, numChan;
    UInt32 found = FALSE;
    UInt32 sensorInstId, sensorI2cInstId, sensorI2cAddr, sensorDrvId;
    Bsp_VidSensorChipIdParams sensorChipIdPrms;
    Bsp_VidSensorChipIdStatus sensorChipIdStatus;
    Bsp_VidSensorCreateStatus sensorCreateStatus;
    Bsp_VidSensorCreateParams sensorCreateParams;
    Bsp_BoardMode   boardMode;
    Bsp_VidSensorFeatures *pSensorFeatures = NULL;
    Ptr sensorHandle = NULL;
    ChainsIssSensor_Table *pSensorTable = NULL;
    Bsp_VidSensorConfigParams cfgPrms;

    /* Check For Errors */
    if (NULL == pCreatePrms)
    {
        status = SYSTEM_LINK_STATUS_EINVALID_PARAMS;
    }
    else
    {
        if (pCreatePrms->numChan > CHAINS_ISS_SENSOR_MAX_CHANNEL)
        {
            status = SYSTEM_LINK_STATUS_EFAIL;
        }
    }

    if (SYSTEM_LINK_STATUS_SOK == status)
    {
        for (cnt = 0U; cnt < CHAINS_ISS_SENSOR_MAX_SENSORS; cnt ++)
        {
            pSensorTable = &gChainsIssSensorTable[cnt];

            if ((TRUE == pSensorTable->isUsed) &&
                (0 == strncmp(pSensorTable->sensorParams.name,
                        pCreatePrms->name,
                        CHAINS_ISS_SENSOR_MAX_NAME)))
            {
                found = TRUE;
                break;
            }
        }
    }

    if (found)
    {
        /* Copy Sensor parameters in the table */
        memcpy(&pSensorTable->createPrms, pCreatePrms,
            sizeof(ChainsIssSensor_CreateParams));

        sensorDrvId = pSensorTable->sensorDrvId;

        /* Copy Capture Driver Instance Id */
        numChan = pCreatePrms->numChan;

        pSensorTable->sensorParams.SensorCfgBroadcast = \
                                        pCreatePrms->sensorCfgBroadcast;

        if ((FVID2_VID_SENSOR_TIDA00262_APT_AR0140_DRV != sensorDrvId) &&
            (FVID2_VID_SENSOR_IMI_OV10640_DRV != sensorDrvId))
        {
            /* PreInitialize Sensor Specific */
            if (pSensorTable->sensorParams.SensorPreInit)
            {
                pSensorTable->sensorParams.SensorPreInit(
                    sensorDrvId,
                    &pSensorTable->sensorParams,
                    pCreatePrms->captInstId,
                    pSensorTable->sensorParams.SensorCfgBroadcast);
            }
        }

        for(cnt = 0U; cnt < numChan; cnt ++)
        {
            if (TRUE == pSensorTable->sensorParams.isBoardModuleSupported)
            {
                sensorInstId = Bsp_boardGetVideoDeviceInstId(
                        sensorDrvId,
                        FVID2_VPS_CAPT_VID_DRV,
                        pCreatePrms->captInstId[cnt]);
                sensorI2cInstId = Bsp_boardGetVideoDeviceI2cInstId(
                        sensorDrvId,
                        FVID2_VPS_CAPT_VID_DRV,
                        pCreatePrms->captInstId[cnt]);

                if ((FVID2_VID_SENSOR_TIDA00262_APT_AR0140_DRV == sensorDrvId) ||
                    (FVID2_VID_SENSOR_IMI_OV10640_DRV == sensorDrvId))
                {
                    sensorI2cAddr = Bsp_boardGetVideoDeviceI2cAddr(
                            sensorDrvId,
                            FVID2_VPS_CAPT_VID_DRV,
                            pCreatePrms->captInstId[cnt]);
                }
                else
                {
                    sensorI2cAddr = Bsp_boardGetVideoDeviceI2cAddr(
                            sensorDrvId,
                            FVID2_VPS_CAPT_VID_DRV,
                            pCreatePrms->captInstId[cnt]);
                }

                switch (pSensorTable->sensorParams.videoIfWidth)
                {
                    default:
                        boardMode = BSP_BOARD_MODE_NONE;
                        break;
                    case SYSTEM_VIFW_8BIT:
                        boardMode = BSP_BOARD_MODE_VIDEO_8BIT;
                        break;
                    case SYSTEM_VIFW_10BIT:
                        boardMode = BSP_BOARD_MODE_VIDEO_10BIT;
                        break;
                    case SYSTEM_VIFW_12BIT:
                        boardMode = BSP_BOARD_MODE_VIDEO_12BIT;
                        break;
                    case SYSTEM_VIFW_14BIT:
                        boardMode = BSP_BOARD_MODE_VIDEO_14BIT;
                        break;
                    case SYSTEM_VIFW_16BIT:
                        boardMode = BSP_BOARD_MODE_VIDEO_16BIT;
                        break;
                    case SYSTEM_VIFW_24BIT:
                        boardMode = BSP_BOARD_MODE_VIDEO_24BIT;
                        break;
                }

                status = Bsp_boardSetPinMux(
                                        FVID2_VPS_CAPT_VID_DRV,
                                        pCreatePrms->captInstId[cnt],
                                        boardMode);
                UTILS_assert (status == 0);

                 /* Power on video sensor at board level mux */
                status = Bsp_boardPowerOnDevice(sensorDrvId, sensorInstId, TRUE);
                UTILS_assert (status == 0);

                if (SYSTEM_LINK_STATUS_SOK == status)
                {
                    /* select video sensor at board level mux */
                    status = Bsp_boardSelectDevice(sensorDrvId, sensorInstId);
                    if (SYSTEM_LINK_STATUS_SOK != status)
                    {
                        Vps_printf(" ISS_SENSOR: Device select failed !!!\n");
                    }
                }

                status = Bsp_boardSelectMode(
                                 sensorDrvId,
                                 sensorInstId,
                                 boardMode);
                if (SYSTEM_LINK_STATUS_SOK != status)
                {
                    Vps_printf(" ISS_SENSOR: Board select failed !!!\n");
                }
            }
            else
            {
                sensorI2cInstId = pSensorTable->sensorParams.i2cInstId;
                sensorI2cAddr = pSensorTable->sensorParams.i2cAddr[cnt];
            }

            Vps_printf(" ISS_SENSOR: INST%d : I2C%d : I2C Addr = 0x%x\n",
                    sensorInstId, sensorI2cInstId, sensorI2cAddr);

            sensorCreateParams.deviceI2cInstId    = sensorI2cInstId;
            sensorCreateParams.numDevicesAtPort   = 1u;
            sensorCreateParams.deviceI2cAddr[0]   = sensorI2cAddr;
            sensorCreateParams.deviceResetGpio[0] = BSP_VID_SENSOR_GPIO_NONE;
            sensorCreateParams.sensorCfg          = NULL;
            sensorCreateParams.numSensorCfg       = 0U;

            sensorHandle = Fvid2_create(
                sensorDrvId,
                sensorInstId,
                &sensorCreateParams,
                &sensorCreateStatus,
                NULL);
            if (NULL == sensorHandle)
            {
                Vps_printf(
                    " ISS_SENSOR: WARNING: Camera Sensor %d is NOT Connected !!! \n",
                    cnt);
                status = sensorCreateStatus.retVal;
                break;
            }
            else
            {
                pSensorFeatures = &pSensorTable->sensorFeatures;
                pSensorTable->sensorHandle[cnt] = sensorHandle;

                /* Get the Features supported by Sensor */
                status = Fvid2_control( sensorHandle,
                                        IOCTL_BSP_VID_SENSOR_GET_FEATURES,
                                        pSensorFeatures,
                                        NULL);
                UTILS_assert (status == 0);

                if ((FVID2_VID_SENSOR_TIDA00262_APT_AR0140_DRV != sensorDrvId) &&
                    (FVID2_VID_SENSOR_IMI_OV10640_DRV != sensorDrvId))
                {
                    sensorChipIdPrms.deviceNum = 0;
                    status = Fvid2_control( sensorHandle,
                                            IOCTL_BSP_VID_SENSOR_GET_CHIP_ID,
                                            &sensorChipIdPrms,
                                            &sensorChipIdStatus);
                    UTILS_assert (status == 0);
                }

                Vps_printf(
                    " ISS_SENSOR: VIP %d: DRV ID %04x (I2C ADDR 0x%02x): %04x:%04x:%04x\n",
                        pCreatePrms->captInstId[cnt],
                        sensorDrvId,
                        sensorCreateParams.deviceI2cAddr[0],
                        sensorChipIdStatus.chipId,
                        sensorChipIdStatus.chipRevision,
                        sensorChipIdStatus.firmwareVersion);
            }
        }
    }
    else
    {
        status = SYSTEM_LINK_STATUS_EFAIL;
    }

    if (status != SYSTEM_LINK_STATUS_SOK)
    {
        pSensorTable = NULL;
    }
    else
    {
        /* Set WDR params */
        switch(pCreatePrms->ispOpMode)
        {
            case ISSM2MISP_LINK_OPMODE_1PASS_WDR:
            case ISSM2MISP_LINK_OPMODE_2PASS_WDR:
            case ISSM2MISP_LINK_OPMODE_2PASS_WDR_LINE_INTERLEAVED:
                status |= setWdrParams(pSensorTable);
                break;
            default:
                status |= SYSTEM_LINK_STATUS_SOK;

                if(TRUE == pSensorTable->sensorFeatures.isDccCfgSupported)
                {
                    /* Assuming all the channels are from same driver and
                       using same dcc profile */
                    status =
                        Fvid2_control(
                                pSensorTable->sensorHandle[0U],
                                IOCTL_BSP_VID_SENSOR_GET_DCC_PARAMS,
                                &pSensorTable->dccParams,
                                NULL);
                    UTILS_assert (status == 0);

                    /* Both the dcc ids, one from the driver and other from the
                       iss_sensor layer, must be same */
                    UTILS_assert(
                        pSensorTable->dccParams.dccCameraId == pSensorTable->
                            sensorParams.dccId);
                }

                break;
        }

        UTILS_assert(SYSTEM_LINK_STATUS_SOK == status);

        if ((FVID2_VID_SENSOR_TIDA00262_APT_AR0140_DRV != sensorDrvId) &&
            (FVID2_VID_SENSOR_IMI_OV10640_DRV != sensorDrvId))
        {
            /* Set the Default Configuration */
            if (NULL != pSensorTable->sensorParams.GetSensorDefaultConfig)
            {
                pSensorTable->sensorParams.GetSensorDefaultConfig(
                    &pSensorTable->sensorParams,
                    pSensorTable->createPrms.ispOpMode,
                    &cfgPrms);

                status = setConfig(
                    (Ptr)pSensorTable,
                    &cfgPrms);
                UTILS_assert(SYSTEM_LINK_STATUS_SOK == status);
            }
        }
    }

    return (Ptr)(pSensorTable);
}

Int32 ChainsIssSensor_Control(Ptr handle, UInt32 cmd,
    Ptr cmdArgs, Ptr cmdRetArgs)
{
    Int32 status = SYSTEM_LINK_STATUS_SOK;
    ChainsIssSensor_Table *pSensorTable;

    /* Check if the handle is valid or not */
    status = checkForHandle(handle);

    if (SYSTEM_LINK_STATUS_SOK == status)
    {
        pSensorTable = (ChainsIssSensor_Table *)handle;

        switch (cmd)
        {
            case CHAINS_ISS_SENSOR_GET_DEFAULT_ISP_CONFIG:
                if (NULL != cmdArgs)
                {
                    status = getDefaultIspConfig(pSensorTable, cmdArgs);
                }
                else
                {
                    status = SYSTEM_LINK_STATUS_EFAIL;
                }
                break;
            case CHAINS_ISS_SENSOR_GET_EXPOSURE_PARAMS:
                if (NULL != cmdArgs)
                {
                    status = getExposureParams(pSensorTable, cmdArgs);
                }
                else
                {
                    status = SYSTEM_LINK_STATUS_EFAIL;
                }
                break;
            case CHAINS_ISS_SENSOR_SET_AEWB_PARAMS:
                if (NULL != cmdArgs)
                {
                    status = setAewbParams(pSensorTable, cmdArgs);
                }
                else
                {
                    status = SYSTEM_LINK_STATUS_EFAIL;
                }
                break;
            case CHAINS_ISS_SENSOR_INIT_AEWB_CREATE_PARAMS:
                if (NULL != cmdArgs)
                {
                    status = getAewbCreateParams(pSensorTable, cmdArgs);
                }
                else
                {
                    status = SYSTEM_LINK_STATUS_EFAIL;
                }
                break;
            case CHAINS_ISS_SENSOR_SET_CONFIG:
                if (NULL != cmdArgs)
                {
                    status = setConfig(pSensorTable, cmdArgs);
                }
                else
                {
                    status = SYSTEM_LINK_STATUS_EFAIL;
                }
                break;
            case CHAINS_ISS_SENSOR_SET_FPS:
                if (NULL != cmdArgs)
                {
                    status = setFps(pSensorTable, cmdArgs);
                }
                else
                {
                    status = SYSTEM_LINK_STATUS_EFAIL;
                }
                break;
            case CHAINS_ISS_SENSOR_SET_FLIP_MIRROR:
                if (NULL != cmdArgs)
                {
                    status = setFlipParams(pSensorTable, cmdArgs);
                }
                else
                {
                    status = SYSTEM_LINK_STATUS_EFAIL;
                }
                break;
            default:
                status = SYSTEM_LINK_STATUS_EFAIL;
                break;
        }
    }

    return (status);
}

static Int32 setConfig(
    ChainsIssSensor_Table *pSensorTable, Bsp_VidSensorConfigParams *cfgPrms)
{
    Int32 status;
    UInt32 chanNum, cnt;

    chanNum = pSensorTable->createPrms.numChan;

    if(TRUE == pSensorTable->sensorFeatures.isSetCfgSupported)
    {
        for(cnt = 0; cnt < chanNum; cnt++)
        {
            status = Fvid2_control(
                pSensorTable->sensorHandle[cnt],
                IOCTL_BSP_VID_SENSOR_SET_CONFIG,
                cfgPrms,
                NULL);
            UTILS_assert (status == 0);
        }
    }

    return (status);
}

static setFps(ChainsIssSensor_Table *pSensorTable, UInt32 *pFps)

{
    Int32 status;
    UInt32 chanNum, cnt;

    chanNum = pSensorTable->createPrms.numChan;

    for (cnt = 0;cnt < chanNum;cnt ++)
    {
        status = Fvid2_control(
            pSensorTable->sensorHandle[cnt],
            IOCTL_BSP_VID_SENSOR_SET_FPS,
            pFps,
            NULL);
        UTILS_assert (status == 0);
    }

    return (status);
}

Int32 ChainsIssSensor_Start(Ptr handle)
{
    Int32 status;
    UInt32 cnt;
    ChainsIssSensor_Table *pSensorTable;
    Bsp_VidSensorConfigParams cfgPrms;

    /* Check if the handle is valid or not */
    status = checkForHandle(handle);

    if (SYSTEM_LINK_STATUS_SOK == status)
    {
        pSensorTable = (ChainsIssSensor_Table *)handle;

        if ((FVID2_VID_SENSOR_TIDA00262_APT_AR0140_DRV == pSensorTable->sensorDrvId) ||
            (FVID2_VID_SENSOR_IMI_OV10640_DRV == pSensorTable->sensorDrvId))
        {
            /* PreInitialize Sensor Specific */
            /*
             *  For TIDA00262 and IMI cameras the preInit is done after the
             *  capture driver is created.
             *  Because UB960 mandates this dependency.
             */
            if (pSensorTable->sensorParams.SensorPreInit)
            {
                pSensorTable->sensorParams.SensorPreInit(
                    pSensorTable->sensorDrvId,
                    &pSensorTable->sensorParams,
                    pSensorTable->createPrms.captInstId,
                    pSensorTable->sensorParams.SensorCfgBroadcast);
            }

            /* Set the Default Configuration */
            if (NULL != pSensorTable->sensorParams.GetSensorDefaultConfig)
            {
                pSensorTable->sensorParams.GetSensorDefaultConfig(
                                            &pSensorTable->sensorParams,
                                            pSensorTable->createPrms.ispOpMode,
                                            &cfgPrms);

                status = setConfig(
                            (Ptr)pSensorTable,
                            &cfgPrms);
                UTILS_assert(SYSTEM_LINK_STATUS_SOK == status);
            }

            if (TRUE == pSensorTable->sensorParams.SensorCfgBroadcast)
            {
                /*
                 *  If broadcast programming for the sensors is enabled then
                 *  we need to program only one instance of the sensor.
                 */
                status = Fvid2_start(
                    (Fvid2_Handle)pSensorTable->sensorHandle[0U],
                    NULL);
                UTILS_assert(FVID2_SOK == status);
            }
            else
            {
                for (cnt = 0; cnt < pSensorTable->createPrms.numChan; cnt++)
                {
                    status = Fvid2_start(
                        (Fvid2_Handle)pSensorTable->sensorHandle[cnt],
                        NULL);
                    UTILS_assert(FVID2_SOK == status);
                }
            }

            if (TRUE == pSensorTable->sensorParams.SensorCfgBroadcast)
            {
                /*
                 *  If broadcast programming of the sensors is done then
                 *  it may be required to disable broadcast programming of the
                 *  sensors for further programming them individually
                 *  So a disable broadcast function is provided.
                 */
                if (pSensorTable->sensorParams.SensorDisBroadcast)
                {
                    pSensorTable->sensorParams.SensorDisBroadcast(
                        &pSensorTable->sensorParams,
                        pSensorTable->createPrms.captInstId);
                }
            }
        }
        else
        {
            for (cnt = 0; cnt < pSensorTable->createPrms.numChan; cnt++)
            {
                status = Fvid2_start(
                    (Fvid2_Handle)pSensorTable->sensorHandle[cnt],
                    NULL);
                UTILS_assert(FVID2_SOK == status);
            }
        }
    }

    return (status);
}

Int32 ChainsIssSensor_Stop(Ptr handle)
{
    Int32 status;
    UInt32 cnt;
    ChainsIssSensor_Table *pSensorTable;

    /* Check if the handle is valid or not */
    status = checkForHandle(handle);

    if (SYSTEM_LINK_STATUS_SOK == status)
    {
        pSensorTable = (ChainsIssSensor_Table *)handle;

        for (cnt = 0; cnt < pSensorTable->createPrms.numChan; cnt++)
        {
            status = Fvid2_stop(
                (Fvid2_Handle)pSensorTable->sensorHandle[cnt],
                NULL);
            UTILS_assert(FVID2_SOK == status);
        }
    }

    return (status);
}

Int32 ChainsIssSensor_Delete(Ptr handle)
{
    Int32 status;
    UInt32 cnt;
    ChainsIssSensor_Table *pSensorTable;

    /* Check if the handle is valid or not */
    status = checkForHandle(handle);

    if (SYSTEM_LINK_STATUS_SOK == status)
    {
        pSensorTable = (ChainsIssSensor_Table *)handle;

        for (cnt = 0; cnt < pSensorTable->createPrms.numChan; cnt++)
        {
            status = Fvid2_delete(
                (Fvid2_Handle)pSensorTable->sensorHandle[cnt],
                NULL);
            UTILS_assert(FVID2_SOK == status);
        }
    }

    return (status);
}

Int32 ChainsIssSensor_GetInfoFromName(
    char name[], ChainsIssSensor_Info *pSensorInfo)
{
    Int32 status = SYSTEM_LINK_STATUS_EFAIL;
    UInt32 cnt;
    ChainsIssSensor_Table *pSensorTable;

    if (NULL != pSensorInfo && NULL != name)
    {
        /* Find a free entry in the sensor table */
        for (cnt = 0U; cnt < CHAINS_ISS_SENSOR_MAX_SENSORS; cnt ++)
        {
            pSensorTable = &gChainsIssSensorTable[cnt];
            if ((TRUE == pSensorTable->isUsed) &&
                (0 == strncmp(pSensorTable->sensorParams.name, name,
                        CHAINS_ISS_SENSOR_MAX_NAME)))
            {
                copySensorInfo(pSensorTable, pSensorInfo);

                status = SYSTEM_LINK_STATUS_SOK;
                break;
            }
        }
    }

    return (status);
}

Int32 ChainsIssSensor_GetNameFromDccId(UInt32 dccId, char sensorName[])
{
    Int32 status = SYSTEM_LINK_STATUS_EFAIL;
    UInt32 cnt;
    ChainsIssSensor_Table *pSensorTable;

    /* Find a free entry in the sensor table */
    for (cnt = 0U; cnt < CHAINS_ISS_SENSOR_MAX_SENSORS; cnt ++)
    {
        pSensorTable = &gChainsIssSensorTable[cnt];
        if ((TRUE == pSensorTable->isUsed) &&
            (dccId == pSensorTable->sensorParams.dccId))
        {
            strncpy(sensorName, pSensorTable->sensorParams.name,
                CHAINS_ISS_SENSOR_MAX_NAME);

            status = SYSTEM_LINK_STATUS_SOK;
            break;
        }
    }

    return (status);
}

/*******************************************************************************
 *  Local Functions Definition
 *******************************************************************************
 */

static Int32 checkForHandle(Ptr handle)
{
    Int32 found = SYSTEM_LINK_STATUS_EFAIL;
    UInt32 cnt;

    /* Find a free entry in the sensor table */
    for (cnt = 0U; cnt < CHAINS_ISS_SENSOR_MAX_SENSORS; cnt ++)
    {
        if (handle == (Ptr)&gChainsIssSensorTable[cnt])
        {
            found = SYSTEM_LINK_STATUS_SOK;
            break;
        }
    }

    return (found);
}

static Void copySensorInfo(
    ChainsIssSensor_Table *pSensorTable, ChainsIssSensor_Info *pSensorInfo)
{
    Int32 status;
    UInt32 cnt;

    memset(pSensorInfo, 0x0, sizeof(ChainsIssSensor_Info));

    pSensorInfo->numChan = pSensorTable->createPrms.numChan;
    pSensorInfo->isDccSupported = pSensorTable->sensorFeatures.isDccCfgSupported;
    pSensorInfo->qSpiOffset = pSensorTable->sensorParams.qSpiOffset;
    pSensorInfo->dccParams = pSensorTable->dccParams;
    pSensorInfo->aewbMode = pSensorTable->sensorParams.aewbMode;

    pSensorInfo->videoIfMode = pSensorTable->sensorParams.videoIfMode;
    pSensorInfo->inCsi2DataFormat = pSensorTable->sensorParams.inCsi2DataFormat;
    pSensorInfo->inCsi2VirtualChanNum = pSensorTable->sensorParams.inCsi2VirtualChanNum;
    pSensorInfo->csi2Prms = pSensorTable->sensorParams.csi2Prms;

    for (cnt = 0U; cnt < pSensorTable->createPrms.numChan; cnt ++)
    {
        pSensorInfo->sensorHandle[cnt] =
            pSensorTable->sensorHandle[cnt];
    }

    if (NULL != pSensorTable->sensorParams.GetSensorDefaultConfig)
    {
        pSensorTable->sensorParams.GetSensorDefaultConfig(
            &pSensorTable->sensorParams,
            pSensorTable->createPrms.ispOpMode,
            &pSensorInfo->cfgPrms);
    }

    /* For the Line Interleaved format, get the line offsets from
       the sensor driver */
    if (ISSM2MISP_LINK_OPMODE_2PASS_WDR_LINE_INTERLEAVED ==
            pSensorTable->createPrms.ispOpMode)
    {
        /* Assuming for all the channels and same sensors, line
           offsets remain same */
        status = Fvid2_control(
            pSensorTable->sensorHandle[0U],
            IOCTL_BSP_VID_SENSOR_GET_OFFSET_PARAMS,
            &pSensorInfo->offsetPrms,
            NULL);
        UTILS_assert (status == 0);
    }
}

static Int32 setWdrParams(ChainsIssSensor_Table *pSensorTable)
{
    Int32 status = SYSTEM_LINK_STATUS_SOK;
    UInt32 cnt;
    Bsp_VidSensorWdrParams wdrPrms;

    if (0U == pSensorTable->sensorFeatures.isWdrModeSupported)
    {
        if (TRUE == IssM2mIspLink_IsWdrMode(pSensorTable->createPrms.ispOpMode))
        {
            Vps_printf(" Chains_Iss: Sensor Does not support WDR Mode\n");
            UTILS_assert(FALSE);
        }
    }
    else
    {
        if ((ISSM2MISP_LINK_OPMODE_1PASS_WDR == pSensorTable->createPrms.
                ispOpMode) &&
            (FALSE == (pSensorTable->sensorFeatures.isWdrModeSupported &
                BSP_VID_SENSOR_WDR_MODE_1PASS)))
        {
            status = SYSTEM_LINK_STATUS_EFAIL;
        }
        if ((ISSM2MISP_LINK_OPMODE_2PASS_WDR == pSensorTable->createPrms.
                ispOpMode) &&
            (FALSE == (pSensorTable->sensorFeatures.isWdrModeSupported &
                BSP_VID_SENSOR_WDR_MODE_2PASS)))
        {
            status = SYSTEM_LINK_STATUS_EFAIL;
        }
        if ((ISSM2MISP_LINK_OPMODE_2PASS_WDR_LINE_INTERLEAVED ==
                pSensorTable->createPrms.ispOpMode) &&
            (FALSE == (pSensorTable->sensorFeatures.isWdrModeSupported &
                BSP_VID_SENSOR_WDR_MODE_2PASS_LINE_INTERLEAVED)))
        {
            status = SYSTEM_LINK_STATUS_EFAIL;
        }
    }
    if ((SYSTEM_LINK_STATUS_SOK == status) &&
        (0U != pSensorTable->sensorFeatures.isWdrModeSupported))
    {
        /* Set the Operation mode for the WDR sensors */
        if (ISSM2MISP_LINK_OPMODE_1PASS_WDR ==
            pSensorTable->createPrms.ispOpMode)
        {
            wdrPrms.wdrMode = BSP_VID_SENSOR_WDR_MODE_1PASS;
        }
        else if (ISSM2MISP_LINK_OPMODE_2PASS_WDR ==
            pSensorTable->createPrms.ispOpMode)
        {
            wdrPrms.wdrMode = BSP_VID_SENSOR_WDR_MODE_2PASS;
        }
        else if (ISSM2MISP_LINK_OPMODE_2PASS_WDR_LINE_INTERLEAVED ==
            pSensorTable->createPrms.ispOpMode)
        {
            wdrPrms.wdrMode = BSP_VID_SENSOR_WDR_MODE_2PASS_LINE_INTERLEAVED;
        }
        else
        {
            wdrPrms.wdrMode = BSP_VID_SENSOR_WDR_DISABLED;
        }

        for(cnt = 0; cnt < pSensorTable->createPrms.numChan; cnt++)
        {
            /* Call SET_WDR_PARAMS only if WDR mode is supported */
            status =
                Fvid2_control(
                    pSensorTable->sensorHandle[cnt],
                    IOCTL_BSP_VID_SENSOR_SET_WDR_PARAMS,
                    &wdrPrms,
                    NULL);
            UTILS_assert (status == 0);
        }
    }

    /* Whenever WDR Config change, get dcc since dcc profile also changes */
    if ((SYSTEM_LINK_STATUS_SOK == status) &&
        (TRUE == pSensorTable->sensorFeatures.isDccCfgSupported))
    {
        /* Assuming all the channels are from same driver and
           using same dcc profile */
        status =
            Fvid2_control(
                pSensorTable->sensorHandle[0U],
                IOCTL_BSP_VID_SENSOR_GET_DCC_PARAMS,
                &pSensorTable->dccParams,
                NULL);
        UTILS_assert (status == 0);

        /* Both the dcc ids, one from the driver and other from the
           iss_sensor layer, must be same */
        UTILS_assert(
            pSensorTable->dccParams.dccCameraId == pSensorTable->
                sensorParams.dccId);
    }

    return (status);
}

static Int32 getDefaultIspConfig(
    ChainsIssSensor_Table *pSensorTable,
    IssIspConfigurationParameters *pIspConfig)
{
    Int32 status = SYSTEM_LINK_STATUS_SOK;

    if (NULL != pSensorTable->sensorParams.GetDefaultIspConfig)
    {
        pSensorTable->sensorParams.GetDefaultIspConfig(
            &pSensorTable->sensorParams,
            pSensorTable->createPrms.ispOpMode,
            pIspConfig);
    }
    else
    {
        status = SYSTEM_LINK_STATUS_EFAIL;
    }

    return (status);
}

static Int32 getExposureParams(
    ChainsIssSensor_Table *pSensorTable, IssAewbAlgOutParams *pAewbAlgOut)
{
    Int32 status = SYSTEM_LINK_STATUS_EFAIL;
    UInt32 chanNum;
    Bsp_VidSensorExpRatioParams expRatioPrms;

    chanNum = pAewbAlgOut->channelId;

    if (chanNum < pSensorTable->createPrms.numChan)
    {
        expRatioPrms.exposureRatio = 0; /* Init for KW error */
        /* Get Exposure ration is used only for the WDR mode, so
           Call SET_WDR_PARAMS only if WDR mode is supported */
        if (pSensorTable->sensorFeatures.isWdrModeSupported)
        {
            status =
                Fvid2_control(
                    pSensorTable->sensorHandle[chanNum],
                    IOCTL_BSP_VID_SENSOR_GET_EXP_RATIO_PARAMS,
                    &expRatioPrms,
                    NULL);
            UTILS_assert (status == 0);

            /* Typically used only for one channel */
            pAewbAlgOut->exposureRatio = expRatioPrms.exposureRatio;
        }
    }
    else
    {
        status = SYSTEM_LINK_STATUS_EFAIL;
    }

    return (status);
}

static Int32 setAewbParams(
    ChainsIssSensor_Table *pSensorTable, IssAewbAlgOutParams *pAewbAlgOut)
{
    Int32 status = SYSTEM_LINK_STATUS_SOK;
    UInt32 chanNum;
    Bsp_VidSensorExposureParams expPrms;
    Bsp_VidSensorGainParams gainPrms;

    gainPrms.analogGain = pAewbAlgOut->outPrms[0].analogGain;
    expPrms.exposureTime = pAewbAlgOut->outPrms[0].exposureTime;
    chanNum = pAewbAlgOut->channelId;

    if (chanNum < pSensorTable->createPrms.numChan)
    {
        if (pSensorTable->sensorFeatures.isManualAnalogGainSupported)
        {
            status = Fvid2_control(
                pSensorTable->sensorHandle[chanNum],
                IOCTL_BSP_VID_SENSOR_SET_GAIN_PARAMS,
                &gainPrms,
                NULL);
            UTILS_assert (status == 0);
        }

        if (pSensorTable->sensorFeatures.isManualExposureSupported)
        {
            status = Fvid2_control(
                pSensorTable->sensorHandle[chanNum],
                IOCTL_BSP_VID_SENSOR_SET_EXP_PARAMS,
                &expPrms,
                NULL);
            UTILS_assert (status == 0);
        }
    }
    else
    {
        status = SYSTEM_LINK_STATUS_EFAIL;
    }

    return (status);
}

static Int32 getAewbCreateParams(
    ChainsIssSensor_Table *pSensorTable, AlgorithmLink_IssAewbCreateParams* pPrms)
{
    Int32 status = SYSTEM_LINK_STATUS_EFAIL;

    if (NULL != pSensorTable->sensorParams.GetAewbConfig)
    {
        pSensorTable->sensorParams.GetAewbConfig(
            &pSensorTable->sensorParams,
            pSensorTable->createPrms.ispOpMode,
            pPrms);

        status = SYSTEM_LINK_STATUS_SOK;
    }

    return (status);
}

static setFlipParams(ChainsIssSensor_Table *pSensorTable, UInt32 *pFps)

{
    Int32 status;
    UInt32 chanNum, cnt, temp;
    Bsp_VidSensorFlipParams flipPrms;

    chanNum = pSensorTable->createPrms.numChan;
    flipPrms.hFlip = 0U;
    flipPrms.vFlip = 0U;
    temp = *pFps;

    if (temp & 0x1)
    {
        flipPrms.vFlip = 1U;
    }
    if (temp & 0x2)
    {
        flipPrms.hFlip = 1U;
    }

    for (cnt = 0;cnt < chanNum;cnt ++)
    {
        status = Fvid2_control(
            pSensorTable->sensorHandle[cnt],
            IOCTL_BSP_VID_SENSOR_SET_FLIP_PARAMS,
            &flipPrms,
            NULL);
        UTILS_assert (status == 0);
    }

    return (status);
}
