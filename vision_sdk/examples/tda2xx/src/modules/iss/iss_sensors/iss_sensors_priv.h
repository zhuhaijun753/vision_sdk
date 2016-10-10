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
 * \file iss_sensors_priv.h
 *
 * \brief APIs for controlling external sensors.
 *
 *
 *******************************************************************************
 */

#ifndef ISS_SENSORS_PRIV_H_
#define ISS_SENSORS_PRIV_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *  Include files
 *******************************************************************************
 */

#include <examples/tda2xx/include/chains.h>
#include <examples/tda2xx/include/chains_iss_sensors.h>
#include <examples/tda2xx/src/modules/iss/iss_sensors/iss_sensor_if.h>


/*******************************************************************************
 *  Defines
 *******************************************************************************
 */

/* Based QSPI offset where DCC binary files are stored */
#define ISS_SENSORS_DCC_QSPI_OFFSET       (24U*1024U*1024U)

/* QSPI Offset for the AR0140 sensor */
#define ISS_SENSORS_DCC_QSPI_OFFSET_AR0140 (ISS_SENSORS_DCC_QSPI_OFFSET)

/* QSPI Offset for the OV10640 sensor */
#define ISS_SENSORS_DCC_QSPI_OFFSET_OV10640                              \
                                        (ISS_SENSORS_DCC_QSPI_OFFSET +   \
                                         ALGORITHM_AEWB1_DCC_IN_BUF_SIZE)

/* QSPI Offset for the IMX224 sensor */
#define ISS_SENSORS_DCC_QSPI_OFFSET_IMX224                               \
                                        (ISS_SENSORS_DCC_QSPI_OFFSET +   \
                                         ALGORITHM_AEWB1_DCC_IN_BUF_SIZE * 2U)

/* QSPI Offset for the AR0132 sensor */
#define ISS_SENSORS_DCC_QSPI_OFFSET_AR0132                               \
                                        (ISS_SENSORS_DCC_QSPI_OFFSET +   \
                                         ALGORITHM_AEWB1_DCC_IN_BUF_SIZE * 3U)


/*******************************************************************************
 *  Data structure's
 *******************************************************************************
 */


/*******************************************************************************
 *  Functions
 *******************************************************************************
 */

Void ChainsIssSensor_Ar0140_Init();
Void ChainsIssSensor_Ov10640_Csi2_Init();
Void ChainsIssSensor_Ov10640_Cpi_Init();
Void ChainsIssSensor_Imx224_Init();
Void ChainsIssSensor_Ar0132_Init();
Void ChainsIssSensor_Ar0140_Tida262_Init();
Void ChainsIssSensor_Ov10640_Imi_Init();

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* End of ISS_SENSORS_PRIV_H_*/


/*@}*/
