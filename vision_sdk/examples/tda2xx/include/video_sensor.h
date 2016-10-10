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
 * \defgroup EXAMPLES_VIDEO_SENSOR_API APIs for controlling external sensors
 *
 *         Sensor APIs can be used to control external sensors.
 *         Drivers for sensors can be part of BSP or any other package.
 *
 * @{
 *
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \file video_sensor.h
 *
 * \brief APIs for controlling external sensors.
 *
 * \version 0.0 (Jun 2013) : [CM] First version
 * \version 0.1 (Jul 2013) : [CM] Updates as per code review comments
 *
 *******************************************************************************
 */

#ifndef _VIDEO_SENSOR_H_
#define _VIDEO_SENSOR_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *  Include files
 *******************************************************************************
 */
#include <include/link_api/system.h>
#include <devices/bsp_videoSensor.h>
#include <boards/bsp_board.h>
#include <bsputils_lvds.h>
#include <bsputils_ub960.h>
#include <include/link_api/algorithmLink_issAewb.h>


/*******************************************************************************
 *  Defines
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \brief Number of LVDS cameras supported by SW.
 *
 *        The board itself supports upto 6 LVDS camera's.
 *        SW bringup of all camera is not complete hence
 *        this number could be < 6
 *
 *******************************************************************************
 */
#define VIDEO_SENSOR_NUM_LVDS_CAMERAS       (4)

/**
 *******************************************************************************
 *
 * \brief Maximum Number of LVDS cameras supported by the board
 *
 *******************************************************************************
 */
#define VIDEO_SENSOR_MAX_LVDS_CAMERAS       (6)

/**
 *******************************************************************************
 *
 * \brief Base command number
 *
 *******************************************************************************
*/
#define VID_SENSOR_CMD_BASE     (0x0)
/**
 *******************************************************************************
 *
 *   \brief CMD: To start the sensor
 *
 *           Sensor will start giving data to
 *           VIP or ISS
 *
 *   \param Fvid2_Handle  handle
 *
 *   \return SYSTEM_STATUS_SOK on success
 *
 *******************************************************************************
 */
 #define VID_SENSOR_CMD_START    (VID_SENSOR_CMD_BASE + 0x1)

/*******************************************************************************
 *
 *   \brief CMD: To stop the sensor
 *
 *           Sensor will stop giving data to
 *           VIP or ISS
 *
 *   \param Fvid2_Handle  handle
 *
 *   \return SYSTEM_STATUS_SOK on success
 *
 *******************************************************************************
 */
#define VID_SENSOR_CMD_STOP     (VID_SENSOR_CMD_BASE + 0x2)

/*******************************************************************************
 *
 *   \brief CMD: To reset the sensor
 *
 *           Sensor is put to reset
 *
 *   \param Fvid2_Handle  handle
 *   \param IOCTL_BSP_VID_SENSOR_RESET
 *
 *   \return SYSTEM_STATUS_SOK on success
 *
 *******************************************************************************
 */
#define VID_SENSOR_CMD_RESET    (VID_SENSOR_CMD_BASE + 0x3)

/*******************************************************************************
 *
 *   \brief CMD: Command max
 *
 *           There cannot be any sensor command after this.
 *
 *   \param None
 *
 *******************************************************************************
 */
#define VID_SENSOR_CMD_MAX      (VID_SENSOR_CMD_BASE + 0x4)

/**
 *******************************************************************************
 *
 *  \brief FVID2 driver handle returned by individual drivers
 *
 *******************************************************************************
*/
typedef Ptr VidSensor_Handle;

/**
 *******************************************************************************
 *
 *  \brief  Enum for sensor ID for image sensors.
 *
 *  These are set of sensors currently supported on the EVM
 *
 *******************************************************************************
*/
typedef enum {
    VID_SENSOR_OV10635,
    /**< Omnivision OV10635 sensor */
    VID_SENSOR_OV10630,
    /**< Omnivision OV10630 sensor*/
    VID_SENSOR_AR0132_RCCC,
    /**< Aptina AR0132 RCCC sensor*/
    VID_SENSOR_MT9M024,
    /**< Aptina MT9M024 sensor */
    VID_SENSOR_MULDES_OV1063X,
    /**< Omnivision OV10630 sensor */
    VID_SENSOR_MULDES_AR0132RCCC,
    /**< Aptina AR0132 RCCC sensor stereo module*/
    VID_SENSOR_OV10640,
    /** Omnivision OV10640 sensor with CSI2 interface */
    VID_SENSOR_AR0132_BAYER,
    /**< Aptina AR0132 Bayer sensor stereo module*/
    VID_SENSOR_AR0140_BAYER,
    /**< Aptina AR0140 Bayer sensor*/
    VID_SENSOR_IMX224_CSI2,
    /**< Sony IMX224 CSI sensor*/
    VID_SENSOR_AR0132_MONOCHROME,
    /**< Aptina AR0132 Monochrome sensor stereo module*/
    VID_SENSOR_MAX = 0xFFFFu,
    /**<Max */
    VID_SENSOR_FORCE32BITS = 0x7FFFFFFF
    /**< This should be the last value after the max enumeration value.
     *   This is to make sure enum size defaults to 32 bits always regardless
     *   of compiler.
     */
}VidSensor_Id;

/*******************************************************************************
 *  Data structure's
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 *  \brief  Sensor Create params.
 *
 *        This structure is used to set input parameters to create sensor.
 *        This structure is used as an argument to make certain BSP function
 *        calls like Bsp_boardGetVideoDeviceInstId to get the sensor device id.
 *        Bsp_boardGetVideoDeviceI2cInstId to get the i2c instance id.
 *        Bsp_boardGetVideoDeviceI2cAddr to get the i2c address.
 *
 *******************************************************************************
*/
typedef struct
{
    VidSensor_Id                sensorId;
    /**< ID of the sensor for which create is getting called. */
    UInt32                      vipInstId[6];
    /**< VIP port to which this sensor is connected */
    System_Standard             standard;
    /**< camer sensor standard.
     * _xx field indicated by System_Standard which indicates FPS should be
     * ignored while using this structure.
     * OV1063x supports following:
     * SYSTEM_STD_CIF -  352,  288,
     * SYSTEM_STD_720P_xx, 1280, 720},
     * SYSTEM_STD_VGA_xx,  640,  480},
     * SYSTEM_STD_WXGA_xx, 1280, 800},
     */
    System_VideoDataFormat      dataformat;
    /**< RGB or YUV data format. valid values are given below \n
     * OV1063x supports following:
     *   SYSTEM_DF_YUV422I_UYVY, \n
     *   SYSTEM_DF_YUV422I_YUYV,  \n
     *   SYSTEM_DF_YUV422I_YVYU,  \n
     *   SYSTEM_DF_YUV422I_VYUY,  \n
     *
     *   For valid values see System_DataFormat. */
    System_VideoIfWidth         videoIfWidth;
    /**< 8 or 12 or 14 camera interface mode.
     * OV1063x supports following:
     * SYSTEM_VIFW_8BIT
     * SYSTEM_VIFW_10BIT
     *
     *   For valid values see #System_VideoIfWidth. */

     System_VideoIfMode         videoIfMode;
     /**< Discrete Hsync/Vsync or CPI or CSI2 or LVDS */

     System_VideoFrameRate      fps;
    /**<     frame rate supported values are
     * 15,30,60 , actual configuration will depend on whether sensor supports
     * this or not
     * OV1063x supports following FPS
     * SYSTEM_FPS_60
     * SYSTEM_FPS_30
     * SYSTEM_FPS_25
     * SYSTEM_FPS_24
     * SYSTEM_FPS_15
     * For valid values see #System_VideoFrameRate. */
     Bool isLVDSCaptMode;
     /**<    Set to true if LVDS mode is used */
     Int32  numChan;
     /**< number of channel required */
     VidSensor_Handle              sensorHandle[VIDEO_SENSOR_MAX_LVDS_CAMERAS];
     /**< Sensor handle  */
    Bsp_VidSensorFeatures          sensorFeatures[VIDEO_SENSOR_MAX_LVDS_CAMERAS];
    /**< List of features supported by sensor */
} VidSensor_CreateParams;

/**
 *******************************************************************************
 *
 *  \brief  Sensor Create return parameters.
 *
 *        This structure is used to set output status of sensor create function.
 *
 *******************************************************************************
*/
typedef struct
{
    Int32 retVal;
    /**< Return value of CreateApi */
}VidSensor_CreateStatus;


/*******************************************************************************
 *  Functions
 *******************************************************************************
 */


/**
 *******************************************************************************
 *
 * \brief Set the default Create Params for OVI sensor params .
 *
 * \param  createParams   [IN] Create parameters for Sensor
 *
 *******************************************************************************
*/
 Void VidSensor_CreateParams_Init(VidSensor_CreateParams *createParams);

/**
 *******************************************************************************
 *
 * \brief Create function to create video sensor.
 *
 *        Creates the sensor handle using bsp function calls.
 *
 * \param  createParams     [IN] Create parameters for Sensor
 * \param  createStatus     [OUT] Status
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
*/
Int32 VidSensor_create(VidSensor_CreateParams *createParams,
                      VidSensor_CreateStatus *createStatus);

/*******************************************************************************
 *
 * \brief Delete function to delete video sensor.
 *
 *        Deletes the sensor handle using Fvid2_delete function calls.
 *
 * \param  handle         [IN] Handle to delete the sensor
 * \param  deleteArgs      Not used.
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
*/
Int32 VidSensor_delete(VidSensor_CreateParams *createParams,
                        Ptr deleteArgs);

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
                        UInt32 cmdStatusArgs);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif


/*@}*/
