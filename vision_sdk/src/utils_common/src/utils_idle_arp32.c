/*
 *******************************************************************************
 *
 * Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */

/**
 *******************************************************************************
 * \file utils_idle_arp32.c
 *
 * \brief  APIs from this file are used to interface the CPU Idle functions
 *
 *         The APIs allow a user to enable CPU idle on a given processor.
 *
 * \version 0.0 (Jan 2015) : [CM] First version
 *
 *******************************************************************************
*/

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */
#include <stdint.h>
#include <hw/hw_types.h>
#include <arp32_wugen.h>
#include "soc_defines.h"
#include "platform.h"
#include <src/utils_common/include/utils.h>
#include <ti/sysbios/family/arp32/TimestampProvider.h>
#include <src/utils_common/include/utils_pm.h>
#include <src/utils_common/include/utils_idle.h>
#include <src/utils_common/include/utils_prf.h>
#include <src/links_eve/system/system_priv_eve.h>
#include <include/link_api/system_trace.h>
#include <pmhal_prcm.h>
#include <pm/pmlib/pmlib_cpuidle.h>
#include <pm/pmhal/pmhal_pdm.h>
#include <pm/pmhal/pmhal_cm.h>
#include <ti/sysbios/knl/Swi.h>

/*******************************************************************************
 *  Defines
 *******************************************************************************
 */
static Utils_idleCpuObj gUtils_idleCpuObj = {0};

extern uint32_t gUtils_startLoadCalc;
/**
 *******************************************************************************
 *
 * \brief Idle function to be plugged into Idle task.
 *        This function executes idle instruction and puts CPU into low power
 *        state. The targeted low power state is Retention.
 *
 * \return None
 *
 *******************************************************************************
 */
Void Utils_idleFxn(Void)
{
#ifdef CPU_IDLE_ENABLED
    UInt32 key,sysClkFreq;
    Utils_idleCpuObj *pObj;
    Types_Timestamp64 startTimeStruct, endTimeStruct;
    UInt64 localStartTime64, localEndTime64, totalLocalTime64;
    UInt64 globalStartTime64, globalEndTime64, totalGlobalTime64;
    UInt64 tempDiff;
    Int64 timeInLowPower;
    uint64_t temp; /* Using uint64_t datatype as UInt64 datatype causes
                      MisraC issue while performing shift operation*/

    pObj = &gUtils_idleCpuObj;

    key = Hwi_disable();
    ARP32_WUGEN_IRQ_Interrupt_Lookup();
    Hwi_restore(key);

    if (0U != pObj->idleInitDone)
    {
        key = Hwi_disable();
        globalStartTime64 = Utils_getCurGlobalTimeInUsec();
        /* Get the time as measured by a a local (internal
         * timer will clock gate when the M4 goes to low power) timer.
         */
        TimestampProvider_get64(&startTimeStruct);
        Hwi_restore(key);

        /* Enter low power */
        PMLIBCpuIdle(PMHAL_PRCM_PD_STATE_RETENTION);

        /* After coming out of low power measure the time from local timer.
         */
        key = Hwi_disable();
        TimestampProvider_get64(&endTimeStruct);
        globalEndTime64 = Utils_getCurGlobalTimeInUsec();
        Hwi_restore(key);

        /* Calculate the time taken shown by the local timer */
        temp = (uint64_t)startTimeStruct.hi & 0xFFFFFFFFU;
        temp  = temp << 32;
        localStartTime64  = (UInt64)startTimeStruct.lo | temp;

        temp = (uint64_t)endTimeStruct.hi & 0xFFFFFFFFU;
        temp  = temp << 32;
        localEndTime64  = (UInt64)endTimeStruct.lo | temp;

        /* Convert to micro seconds */
        if (localEndTime64 < localStartTime64)
        {
            tempDiff = (localEndTime64 - localStartTime64) + (UInt64)0x00FFFFFF;
        }
        else
        {
            tempDiff = localEndTime64 - localStartTime64;
        }
        totalLocalTime64 = (tempDiff * (UInt64) 1000) /
                           pObj->localTimerFrequency;

        totalGlobalTime64 = globalEndTime64 - globalStartTime64;
        /* This is done as the 32K timer runs much slowly than the CPU timer
         * with a difference of 1 cycle count the time difference can be of
         * 1/32786 sec = 30.5 us. Hence adding 30 to the difference if it is
         * negative to get approximate value of the time difference.
         */
        if (totalGlobalTime64 < totalLocalTime64)
        {
            totalGlobalTime64 = totalGlobalTime64 + (UInt64) 30;
        }
        /* Calculate the number of clocks for which the M4 was in low power
         * at sysclock time stamp counts */
        key = Hwi_disable();
        if ((uint32_t)TRUE == gUtils_startLoadCalc)
        {
            temp = totalGlobalTime64 - totalLocalTime64;
            timeInLowPower = (Int64)temp;
            if (timeInLowPower > 0)
            {
                sysClkFreq = PMHALCMGetSysClockFreq();
                pObj->timeInSleep = pObj->timeInSleep + (((UInt64)timeInLowPower
                               * (UInt64)sysClkFreq)/(UInt64)1000);
            }
        }
        Hwi_restore(key);
    }
#endif
}

/**
 *******************************************************************************
 *
 * \brief Prepare the system configuration for CPU idle in IDLE Task.
 *        In case of A15, this function enables SR3-APG (HG / Fast Ramp-up).
 *        It also forces A15_1 to OFF.
 *
 * \return None
 *
 *******************************************************************************
 */
Void Utils_idlePrepare(Void)
{
    Utils_idleCpuObj *pObj;
#ifdef CPU_IDLE_ENABLED
    UInt32 regVal;
    Types_FreqHz freq;
    Int32 status = (Int32) PM_SUCCESS;
    UInt32 procId;
    pmhalPrcmModuleId_t moduleId;
    uint64_t temp; /* Using uint64_t datatype as UInt64 datatype causes
                      MisraC issue while performing shift operation*/

    pObj = &gUtils_idleCpuObj;
    procId = System_getSelfProcId();
    moduleId = (pmhalPrcmModuleId_t)Utils_pmGetModId(procId);

    status = PMLIBCpuModePrepare(moduleId,PMLIB_IDLE_CPU_MODE_AUTOCG);
    /* Update the state of the CPU Idle object */

    pObj->timeInSleep = (UInt64) 0;
    if(status == (Int32)PM_SUCCESS)
    {
        /* If the DMA is freed then disable the DMA by setting
        * Force standby and Force Idle.
        */
        regVal  = HW_RD_REG32(0x400A5640);
        if (0U == regVal)
        {
            HW_WR_REG32(0x40086000 + 0x10, 0);
            HW_WR_REG32(0x40087000 + 0x10, 0);
        }
        /* Configure the SCTM counters to idle when the
        * ARP32 is IDLE.
        */
        regVal  = HW_RD_REG32(0x40085000U);
        regVal &= (UInt32)0xFFFFFFF9U;
        regVal |= (UInt32)0x04U;
        HW_WR_REG32(0x40085000U, regVal);
        regVal  = HW_RD_REG32(0x40085100U);
        regVal &= ~(UInt32)0x20U;
        HW_WR_REG32(0x40085100U, regVal);
        ARP32_WUGEN_IRQ_Init();

        TimestampProvider_getFreq(&freq);
        temp = (uint64_t)freq.hi & 0xFFFFFFFFU;
        temp  = temp << 32;
        pObj->localTimerFrequency = ((UInt64)freq.lo | temp)/(UInt64)1000;
        pObj->idleInitDone = 1U;
    }
#else
    pObj = &gUtils_idleCpuObj;
    pObj->timeInSleep = (UInt64) 0;

#endif
}

/**
 *******************************************************************************
 *
 * \brief This Function Enables the TPTC by setting it to smart IDLE and smart
 *        standby. The EDMA is operational after this setting.
 *
 * \return None
 *
 *******************************************************************************
 */
Void Utils_idleEnableEveDMA(void)
{
#ifdef CPU_IDLE_ENABLED
    /* Enable DMA before ALG Process. Put the TPTC to
     * Smart Standby and Smart Idle
     */
    HW_WR_REG32(0x40086000 + 0x10, 0x28);
    HW_WR_REG32(0x40087000 + 0x10, 0x28);
#endif
}

/**
 *******************************************************************************
 *
 * \brief This Function Disables the TPTC by setting it to force IDLE and force
 *        standby. The EDMA is in low power mode after this setting.
 *
 * \return None
 *
 *******************************************************************************
 */
Void Utils_idleDisableEveDMA(void)
{
#ifdef CPU_IDLE_ENABLED
    /* If the DMA is freed then disable the DMA by setting
    * Force standby and Force Idle. Check the CCSTAT register to check if any
    * events are queued in the Transfer controllers.
    */
    UInt32 regVal  = HW_RD_REG32(0x400A0640) & 0xFFFF0000U;
    if (0U == regVal)
    {
        HW_WR_REG32(0x40086000 + 0x10, 0);
        HW_WR_REG32(0x40087000 + 0x10, 0);
    }
#endif
}

/**
 *******************************************************************************
 *
 * \brief  This function is called during the load update call back to populate
 *         the amount of time in 20 MHz ticks the CPU has been in low power.
 *
 * \return None
 *
 *******************************************************************************
 */
UInt64 Utils_idleGetLowPowerTime(Void)
{
    UInt32 key;
    Utils_idleCpuObj *pObj;
    UInt64 timeInLowPower;

    pObj = &gUtils_idleCpuObj;

    timeInLowPower = pObj->timeInSleep;

    /* Reset the total time in sleep when read */
    key = Hwi_disable();
    pObj->timeInSleep = (UInt64) 0;
    Hwi_restore(key);

    return timeInLowPower;
}

/* Nothing beyond this point */
