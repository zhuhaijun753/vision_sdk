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
 * \file video_sensor.c
 *
 * \brief  This file has the implementataion of Sensor Control API
 *
 *         Sensor APIs can be used to control external sensors.
 *         Drivers for sensors can be part of BSP or any other package.
 *
 *
 * \version 0.0 (Jun 2013) : [CM] First version
 * \version 0.1 (Jul 2013) : [CM] Updates as per code review comments
 *
 *******************************************************************************
*/

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */
#if defined(BIOS_BUILD)
#include <xdc/std.h>
#endif

#include <fvid2/fvid2.h>
#include <vps/vps.h>
#include <examples/tda2xx/include/video_sensor.h>

#include <src/utils_common/include/utils_mem.h>



/**
 *******************************************************************************
 *
 * \brief Set the default Create Params for OVI sensor params .
 *
 * \param  createParams   [IN] Create parameters for Sensor
 *
 *******************************************************************************
*/
Void VidSensor_CreateParams_Init(VidSensor_CreateParams *createParams)
{
    UInt32 i;

    createParams->sensorId       = VID_SENSOR_OV10635;
    createParams->vipInstId[0]   = SYSTEM_CAPTURE_INST_VIP1_SLICE1_PORTA;
    createParams->standard       = SYSTEM_STD_720P_60;
    createParams->dataformat     = SYSTEM_DF_YUV422I_UYVY;
    createParams->videoIfWidth   = SYSTEM_VIFW_8BIT;
    createParams->fps            = SYSTEM_FPS_30;
    createParams->isLVDSCaptMode = FALSE;
    createParams->numChan        = 1;
    createParams->videoIfMode    = SYSTEM_VIFM_SCH_DS_AVID_VSYNC;

    for (i = 0u; i < VIDEO_SENSOR_MAX_LVDS_CAMERAS; i ++)
    {
        createParams->sensorHandle[i] = NULL;
    }

    if (BSP_BOARD_MONSTERCAM == Bsp_boardGetId())
    {
        createParams->sensorId       = VID_SENSOR_MULDES_AR0132RCCC;
        createParams->vipInstId[0]   = SYSTEM_CAPTURE_INST_VIP3_SLICE1_PORTA;
        createParams->vipInstId[1]   = SYSTEM_CAPTURE_INST_VIP3_SLICE2_PORTA;
        createParams->standard       = SYSTEM_STD_720P_60;
        createParams->dataformat     = SYSTEM_DF_YUV422I_UYVY;
        createParams->videoIfWidth   = SYSTEM_VIFW_16BIT;
        createParams->fps            = SYSTEM_FPS_60;
        createParams->isLVDSCaptMode = FALSE;
        createParams->numChan        = 2;
    }
}

/**
 *******************************************************************************
 *
 * \brief Create function to create video sensor.
 *
 *        Creates the sensor handle using bsp function calls.
 *
 * \param  createParams   [IN] Create parameters for Sensor
 * \param  createStatus   [OUT] Status
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
*/
Int32 VidSensor_create(VidSensor_CreateParams *createParams,
                        VidSensor_CreateStatus *createStatus)
{
    Int32  retVal = SYSTEM_LINK_STATUS_EFAIL,chanNum;
    UInt32 sensorInstId, sensorI2cInstId, sensorI2cAddr, sensorDrvId;
    Bsp_VidSensorChipIdParams sensorChipIdPrms;
    Bsp_VidSensorChipIdStatus sensorChipIdStatus;
    Bsp_VidSensorCreateStatus sensorCreateStatus;
    Bsp_VidSensorCreateParams sensorCreateParams;
    Bsp_BoardMode   boardMode;

    createStatus->retVal = SYSTEM_LINK_STATUS_EFAIL;
    sensorDrvId = VID_SENSOR_MAX;
    if(createParams->sensorId==VID_SENSOR_OV10635
        ||
        createParams->sensorId==VID_SENSOR_OV10630
        )
    {
        sensorDrvId = FVID2_VID_SENSOR_OV1063X_DRV;
    }
    else if(createParams->sensorId==VID_SENSOR_MT9M024)
    {
        sensorDrvId = BSP_VID_SENSOR_MT9M024;
    }
    else if(createParams->sensorId==VID_SENSOR_MULDES_OV1063X)
    {
        sensorDrvId = FVID2_VID_SENSOR_MULDES_OV1063X_DRV;
    }
    else if(createParams->sensorId==VID_SENSOR_MULDES_AR0132RCCC)
    {
        sensorDrvId = FVID2_VID_SENSOR_APT_AR0132RCCC_DRV;
    }
    else
    {
        /* unsupported sensor */
        UTILS_assert(0);
    }


    for(chanNum = 0 ; chanNum < createParams->numChan ;chanNum++)
    {
        sensorInstId = Bsp_boardGetVideoDeviceInstId(
                sensorDrvId,
                FVID2_VPS_CAPT_VID_DRV,
                createParams->vipInstId[chanNum]);
        sensorI2cInstId = Bsp_boardGetVideoDeviceI2cInstId(
                sensorDrvId,
                FVID2_VPS_CAPT_VID_DRV,
                createParams->vipInstId[chanNum]);
        sensorI2cAddr = Bsp_boardGetVideoDeviceI2cAddr(
                    sensorDrvId,
                    FVID2_VPS_CAPT_VID_DRV,
                    createParams->vipInstId[chanNum]);

        /* set capture port pinmux based on video interface bus width */
        if(createParams->videoIfWidth==SYSTEM_VIFW_8BIT)
        {
            boardMode = BSP_BOARD_MODE_VIDEO_8BIT;
        }
        else
        if(createParams->videoIfWidth==SYSTEM_VIFW_10BIT)
        {
            boardMode = BSP_BOARD_MODE_VIDEO_10BIT;
        }
        else
        if(createParams->videoIfWidth==SYSTEM_VIFW_12BIT)
        {
            boardMode = BSP_BOARD_MODE_VIDEO_12BIT;
        }
        else
        if(createParams->videoIfWidth==SYSTEM_VIFW_14BIT)
        {
            boardMode = BSP_BOARD_MODE_VIDEO_14BIT;
        }
        else
        if(createParams->videoIfWidth==SYSTEM_VIFW_16BIT)
        {
            boardMode = BSP_BOARD_MODE_VIDEO_16BIT;
        }
        else
        if(createParams->videoIfWidth==SYSTEM_VIFW_24BIT)
        {
            boardMode = BSP_BOARD_MODE_VIDEO_24BIT;
        }
        else if ((SYSTEM_VIFW_4LANES == createParams->videoIfWidth) ||
                 (SYSTEM_VIFW_3LANES == createParams->videoIfWidth)||
                 (SYSTEM_VIFW_2LANES == createParams->videoIfWidth)||
                 (SYSTEM_VIFW_1LANES == createParams->videoIfWidth))
        {
            boardMode = BSP_BOARD_MODE_NONE;
        }
        else
        {
            /* assume 8-bit mode if no match found */
            boardMode = BSP_BOARD_MODE_VIDEO_8BIT;
        }

        retVal = Bsp_boardSetPinMux(FVID2_VPS_CAPT_VID_DRV,
                                createParams->vipInstId[chanNum],
                                boardMode);
        UTILS_assert (retVal == 0);

         /* Power on video sensor at board level mux */
        retVal = Bsp_boardPowerOnDevice(sensorDrvId, sensorInstId, TRUE);
        UTILS_assert (retVal == 0);

        if (SYSTEM_LINK_STATUS_SOK == retVal)
        {
            /* select video sensor at board level mux */
            retVal = Bsp_boardSelectDevice(sensorDrvId, sensorInstId);
            if (SYSTEM_LINK_STATUS_SOK != retVal)
            {
                Vps_printf(" VIDEO_SENSOR: Device select failed !!!\n");
            }
        }

        retVal = Bsp_boardSelectMode(
                         sensorDrvId,
                         sensorInstId,
                         boardMode);
        if (SYSTEM_LINK_STATUS_SOK != retVal)
        {
            Vps_printf(" VIDEO_SENSOR: Board select failed !!!\n");
        }

        Vps_printf(" VIDEO_SENSOR: INST%d : I2C%d : I2C Addr = 0x%x\n",
                sensorInstId, sensorI2cInstId, sensorI2cAddr);

        if (SYSTEM_LINK_STATUS_SOK == retVal)
        {
            if (((BSP_BOARD_MULTIDES == Bsp_boardGetId()) || (BSP_BOARD_MONSTERCAM == Bsp_boardGetId())) &&
                (FVID2_VID_SENSOR_MULDES_OV1063X_DRV == sensorDrvId))
            {
                retVal = BspUtils_appConfSerDeSer(sensorDrvId, sensorInstId);
                if (retVal != SYSTEM_LINK_STATUS_SOK)
                {
                    Vps_printf(" VIDEO_SENSOR: MULTI_DES: Configuring instance %d failed !!!\n",sensorInstId);
                }
            }
        }
        sensorCreateParams.deviceI2cInstId    = sensorI2cInstId;
        sensorCreateParams.numDevicesAtPort   = 1u;
        sensorCreateParams.deviceI2cAddr[0]   = sensorI2cAddr;
        sensorCreateParams.deviceResetGpio[0] = BSP_VID_SENSOR_GPIO_NONE;
        sensorCreateParams.sensorCfg          = NULL;
        sensorCreateParams.numSensorCfg       = 0U;

        createParams->sensorHandle[chanNum] = Fvid2_create(
                                                        sensorDrvId,
                                                        sensorInstId,
                                                        &sensorCreateParams,
                                                        &sensorCreateStatus,
                                                        NULL);
        if (createParams->sensorHandle[chanNum] == NULL)
        {
            if (chanNum != 4)
            {
                createStatus->retVal = SYSTEM_LINK_STATUS_EFAIL;
            }
            else
            {
                /* Continue the use case if the front camera sensor is not connected. */
                createStatus->retVal = SYSTEM_LINK_STATUS_SOK;
                Vps_printf(
                " VIDEO_SENSOR: WARNING: Front Camera Sensor is NOT Connected !!! \n");
                /* Avoid sending any more command to front camera sensor. */
                createParams->numChan -= 1;
            }
        }
        else
        {
            /* Get the Features supported by Sensor */
            retVal = Fvid2_control(createParams->sensorHandle[chanNum],
                                   IOCTL_BSP_VID_SENSOR_GET_FEATURES,
                                   &createParams->sensorFeatures[chanNum],
                                   NULL);
            UTILS_assert (retVal == 0);

            if(Bsp_platformIsTda3xxFamilyBuild())
            {
                if(sensorDrvId==FVID2_VID_SENSOR_OV1063X_DRV)
                {
                    Bsp_VidSensorFlipParams flipParams;

                    flipParams.hFlip = TRUE;
                    flipParams.vFlip = TRUE;

                    retVal = Fvid2_control( createParams->sensorHandle[chanNum],
                                        IOCTL_BSP_VID_SENSOR_SET_FLIP_PARAMS,
                                        &flipParams,
                                        NULL);
                    UTILS_assert (retVal == 0);

                    Vps_printf(
                        " VIDEO_SENSOR: Flipping sensor output in H and V direction\n");
                }
            }
            sensorChipIdPrms.deviceNum = 0;
            retVal = Fvid2_control( createParams->sensorHandle[chanNum],
                                    IOCTL_BSP_VID_SENSOR_GET_CHIP_ID,
                                    &sensorChipIdPrms,
                                    &sensorChipIdStatus);
            UTILS_assert (retVal == 0);

            Vps_printf(
                " VIDEO_SENSOR: VIP %d: DRV ID %04x (I2C ADDR 0x%02x): %04x:%04x:%04x\n",
                    createParams->vipInstId[chanNum],
                    sensorDrvId,
                    sensorCreateParams.deviceI2cAddr[0],
                    sensorChipIdStatus.chipId,
                    sensorChipIdStatus.chipRevision,
                    sensorChipIdStatus.firmwareVersion);

            if(TRUE == createParams->sensorFeatures[chanNum].isSetCfgSupported)
            {
                Bsp_VidSensorConfigParams configParams;
                configParams.videoIfWidth = createParams->videoIfWidth;
                configParams.dataformat   = createParams->dataformat;
                configParams.standard     = createParams->standard;
                configParams.fps          = createParams->fps;
                retVal =
                    Fvid2_control(createParams->sensorHandle[chanNum], IOCTL_BSP_VID_SENSOR_SET_CONFIG,
                                &configParams,
                                NULL);
                UTILS_assert (retVal == 0);

            }

            createStatus->retVal = retVal;
        }
    }
    return (createStatus->retVal);
}
/*******************************************************************************
 *
 * \brief Delete function to delete video sensor.
 *
 *        Deletes the sensor handle using Fvid2_delete function calls.
 *
 * \param  handle         [IN] Handle to delete the sensor
 *
 * \param  deleteArgs      Not used.
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
*/
Int32 VidSensor_delete(VidSensor_CreateParams *createParams,
                       Ptr deleteArgs)
{
    Int32 status = SYSTEM_LINK_STATUS_SOK;
    Int32 chanNum, sensorInstId, sensorDrvId;

    sensorDrvId = VID_SENSOR_MAX;
    if(createParams->sensorId==VID_SENSOR_OV10635
        ||
        createParams->sensorId==VID_SENSOR_OV10630
        )
    {
        sensorDrvId = FVID2_VID_SENSOR_OV1063X_DRV;
    }
    else if(createParams->sensorId==VID_SENSOR_MT9M024)
    {
        sensorDrvId = BSP_VID_SENSOR_MT9M024;
    }
    else if(createParams->sensorId==VID_SENSOR_MULDES_OV1063X)
    {
        sensorDrvId = FVID2_VID_SENSOR_MULDES_OV1063X_DRV;
    }
    else if(createParams->sensorId==VID_SENSOR_MULDES_AR0132RCCC)
    {
        sensorDrvId = FVID2_VID_SENSOR_APT_AR0132RCCC_DRV;
    }
    else
    {
        /* For Misra C */
    }
    UTILS_assert(sensorDrvId != VID_SENSOR_MAX);

    for(chanNum = 0; chanNum < createParams->numChan; chanNum++)
    {
        sensorInstId = Bsp_boardGetVideoDeviceInstId(
                        sensorDrvId,
                        FVID2_VPS_CAPT_VID_DRV,
                        createParams->vipInstId[chanNum]);

        if(createParams->sensorHandle[chanNum] != NULL)
        {
            status = Fvid2_delete(
                        (Fvid2_Handle)createParams->sensorHandle[chanNum],
                        NULL);
        }

        if (((BSP_BOARD_MULTIDES == Bsp_boardGetId()) || (BSP_BOARD_MONSTERCAM == Bsp_boardGetId())) &&
            (FVID2_VID_SENSOR_MULDES_OV1063X_DRV == sensorDrvId))
        {
            status   =  BspUtils_appDeConfSerDeSer(sensorDrvId, sensorInstId);
        }
    }

    return status;
}

/*******************************************************************************
 *
 * \brief Control function to start stop and reset video sensor.
 *
 *        Control the sensor operation liske start and stop of the sensor using
 *        Fvid2 calls.
 *        Sensor reset is performed using IOCTL call IOCTL_BSP_VID_SENSOR_RESET
 *
 * \param  handle        [IN] Handle to control the sensor.
 *
 * \param  cmd           [IN] Control command for sensor
 *
 * \param  cmdArgs       [IN] Arguments for command if any.
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
*/
Int32 VidSensor_control(VidSensor_CreateParams *createParams,
                        UInt32 cmd,
                        Ptr    cmdArgs,
                        UInt32 cmdStatusArgs)
{
    Int32 status = SYSTEM_LINK_STATUS_SOK;
    Int32 chanNum;

    if(cmd==VID_SENSOR_CMD_START)
    {
        for(chanNum = 0; chanNum < createParams->numChan; chanNum++)
        {
            status = Fvid2_start(
                            (Fvid2_Handle)createParams->sensorHandle[chanNum],
                             NULL);
        }
    }
    else if(cmd==VID_SENSOR_CMD_STOP)
    {
        for(chanNum = 0; chanNum < createParams->numChan; chanNum++)
        {
            status = Fvid2_stop(
                        (Fvid2_Handle)createParams->sensorHandle[chanNum],
                         NULL);
        }
    }
    else if(cmd==VID_SENSOR_CMD_RESET)
    {
        for(chanNum = 0; chanNum < createParams->numChan; chanNum++)
        {
            status = Fvid2_control(
                        (Fvid2_Handle)createParams->sensorHandle[chanNum],
                        IOCTL_BSP_VID_SENSOR_RESET,
                        NULL, NULL);
        }
    }

    return status;
}




