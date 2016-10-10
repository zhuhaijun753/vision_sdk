/*
 *******************************************************************************
 *
 * Copyright (C) 2015 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \ingroup UTILS_API
 * \defgroup UTILS_PRCM_STATS_API PRCM Statistics APIs
 *
 * \brief  APIs to execute CPU PRCM Stats
 *
 * @{
 *
 *******************************************************************************
*/

/**
 *******************************************************************************
 *
 * \file utils_prcm_stats.h
 *
 * \brief Prcm Print Stats
 *
 * \version 0.0 First version : [CM] First version
 *
 *******************************************************************************
*/

#ifndef UTILS_PRCM_STATS_H_
#define UTILS_PRCM_STATS_H_


/*******************************************************************************
 *  Defines
 *******************************************************************************
 */

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */
#include <src/utils_common/include/utils.h>

/*******************************************************************************
 *  Functions
 *******************************************************************************
*/

/**
 *******************************************************************************
 *
 * \brief Print the temperature value for Available Temperature
 *        Sensor for given Voltage Id.
 *
 * \return None
 *
 *******************************************************************************
 */
Void Utils_prcmPrintTempValues(UInt32 voltId);

/**
 *******************************************************************************
 *
 * \brief Print the Voltage value for Available Voltage rails corrosponding to
 *         given voltage id
 *
 * \return None
 *
 *******************************************************************************
 */
Void Utils_prcmPrintVoltageValues(UInt32 voltId);

/**
 *******************************************************************************
 *
 * \brief Print all the Voltage value for available Voltage Rails.
 *
 * \return None
 *
 *******************************************************************************
 */
Void Utils_prcmPrintAllVoltageValues(void);

/**
 *******************************************************************************
 *
 * \brief Print all the temperature value for Available Temperature Sensor.
 *
 * \return None
 *
 *******************************************************************************
 */
Void Utils_prcmPrintAllVDTempValues(void);

/**
 *******************************************************************************
 *
 * \brief Print all PRCM Register data and the current state .
 *
 * \return None
 *
 *******************************************************************************
 */
Void Utils_prcmDumpRegisterData(void);

/**
 *******************************************************************************
 *
 * \brief Print all Dpll Register data and the current state .
 *
 * \return None
 *
 *******************************************************************************
 */
Void Utils_prcmPrintAllDpllValues(void);

/**
 *******************************************************************************
 *
 * \brief Init PMLIB ClockRate . Initialize during system init
 *
 * \return None
 *
 *******************************************************************************
 */
Int32 Utils_prcmClockRateInit(void);

/**
 *******************************************************************************
 *
 * \brief Print all CPU Frequency .
 *
 * \return None
 *
 *******************************************************************************
 */
Void Utils_prcmPrintAllCPUFrequency(void);

/**
 *******************************************************************************
 *
 * \brief Print all the Peripheral Frequency .
 *
 * \return None
 *
 *******************************************************************************
 */
Void Utils_prcmPrintAllPeripheralsFrequency(void);

/**
 *******************************************************************************
 *
 * \brief Print all Module current state .
 *        Print the Module SIDLE State,
 *                Clock Activity State,
 *                  Power Domain State.
 *
 * \return None
 *
 *******************************************************************************
 */
Void Utils_prcmPrintAllModuleState(void);

/**
 *******************************************************************************
 *
 * \brief Dummy I2C initialization function for PMIC Communication.
 *
 * \return None
 *
 *******************************************************************************
 */
Int32 Utils_prcmVoltageInit(Void);

/**
 *******************************************************************************
 *
 * \brief Initialize the INA226 to print the power consumption
 *
 * \return None
 *
 *******************************************************************************
 */
Void Utils_prcmPrintPowerConsumptionInit(Void);

/**
 *******************************************************************************
 *
 * \brief Print Power consumption
 *
 * \return None
 *
 *******************************************************************************
 */
Void Utils_prcmPrintPowerConsumption(Void);

#endif

/* @} */
