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
 * \file utils_idle_c66x.c
 *
 * \brief  APIs from this file are used to interface the CPU Idle functions
 *
 *         The APIs allow a user to enable CPU idle on a given processor.
 *
 * \version 0.0 (Dec 2014) : [CM] First version
 *
 *******************************************************************************
*/

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */
#include <stdint.h>
#include <hw/hw_types.h>
#include <dsp_wugen.h>
#include <src/utils_common/include/utils_idle.h>
#include <ti/sysbios/family/c64p/TimestampProvider.h>
#include <src/utils_common/include/utils_pm.h>
#include <pm/pmlib/pmlib_cpuidle.h>
#include <pm/pmhal/pmhal_pdm.h>
#include <pm/pmhal/pmhal_cm.h>
#include <pm/pmhal/pmhal_sd.h>
#include <src/links_dsp/system/system_priv_c6xdsp.h>

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
    UInt32 key;
    Utils_idleCpuObj *pObj;
    Types_Timestamp64 startTimeStruct, endTimeStruct;
    UInt64 localStartTime64, localEndTime64, totalLocalTime64;
    UInt64 globalStartTime64, globalEndTime64, totalGlobalTime64;
    UInt64 tempDiff;
    uint64_t temp; /* Using uint64_t datatype as UInt64 datatype causes
                      MisraC issue while performing shift operation*/

    pObj = &gUtils_idleCpuObj;

    key = Hwi_disable();
    DSP_WUGEN_IRQ_Interrupt_Lookup();
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
        temp  = temp << 32U;
        localStartTime64  = (UInt64)startTimeStruct.lo | temp;

        temp = (uint64_t)endTimeStruct.hi & 0xFFFFFFFFU;
        temp  = temp << 32U;
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

        totalGlobalTime64 = (globalEndTime64 - globalStartTime64);
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
            Int64 timeInIdle = (Int64)temp;
            if (timeInIdle > 0)
            {
                pObj->timeInSleep = pObj->timeInSleep + (UInt64)(((UInt64) timeInIdle
                               * (UInt64)PMHALCMGetSysClockFreq())/(UInt64)1000);
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
 *
 * \return None
 *
 *******************************************************************************
 */
Void Utils_idlePrepare(Void)
{
    Utils_idleCpuObj *pObj;
#ifdef CPU_IDLE_ENABLED
    UInt32 key;
    UInt32 procId;
    Types_FreqHz freq;
    Int32 status = (Int32) PM_SUCCESS;
    pmhalPrcmModuleId_t moduleId;
    uint64_t temp; /* Using uint64_t datatype as UInt64 datatype causes
                      MisraC issue while performing shift operation*/

    pObj = &gUtils_idleCpuObj;

    /* Configure the DSP such that it goes to Clock gate state when
     * DSP Idle instruction is executed.
     */

    procId = System_getSelfProcId();
    moduleId = (pmhalPrcmModuleId_t)Utils_pmGetModId(procId);

    status = PMLIBCpuModePrepare(moduleId,PMLIB_IDLE_CPU_MODE_AUTOCG);

    pObj->timeInSleep = (UInt64) 0;
    if(status == (Int32)PM_SUCCESS)
    {
        key = Hwi_disable();
        DSP_WUGEN_IRQ_Init();
        DSP_WUGEN_IRQ_Interrupt_Lookup();
        Hwi_restore(key);
        PMLIBSetCorepacPowerDown((uint32_t) 1U);

        pObj->idleInitDone = 1U;
        TimestampProvider_getFreq(&freq);
        temp = (uint64_t)freq.hi & 0xFFFFFFFFU;
        temp  = temp << 32U;
        pObj->localTimerFrequency = ((UInt64)freq.lo | temp)/(UInt64)1000;
    }
#else
    pObj = &gUtils_idleCpuObj;
    pObj->timeInSleep = (UInt64) 0;

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
