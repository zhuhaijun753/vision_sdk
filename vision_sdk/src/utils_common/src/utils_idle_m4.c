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
 * \file utils_idle_M4.c
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
#include <src/utils_common/include/utils_idle.h>
#include <ti/sysbios/family/arm/ducati/TimestampProvider.h>
#include <src/utils_common/include/utils.h>
#include <src/utils_common/include/utils_pm.h>
#include <hw/hw_types.h>
#include <soc.h>
#include "hw_ipu_wugen_local_prcm.h"
#include "hw/hw_ipu_m4_nvic.h"
#include <armv7m/ipu_wugen.h>
#include <pm/pmlib/pmlib_cpuidle.h>
#include <pm/pmhal/pmhal_rm.h>
#include <pm/pmhal/pmhal_cm.h>

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
    Types_Timestamp64 startTimeStruct, endTimeStruct;
    UInt64 localStartTime64, localEndTime64, totalLocalTime64;
    UInt64 globalStartTime64, globalEndTime64, totalGlobalTime64;
    UInt64 tempDiff;
    uint64_t temp; /* Using uint64_t datatype as UInt64 datatype causes
                      MisraC issue while performing shift operation*/
    Utils_idleCpuObj *pObj;
    pObj = &gUtils_idleCpuObj;

    key = Hwi_disable();
    IPU_WUGEN_Interrupt_Lookup();
    Hwi_restore(key);

    if(0U != pObj->idleInitDone)
    {
        key = Hwi_disable();
        globalStartTime64 = Utils_getCurGlobalTimeInUsec();
        /* Get the time as measured by a a local (internal
         * timer will clock gate when the M4 goes to low power) timer.
         */
        TimestampProvider_get64(&startTimeStruct);
        Hwi_restore(key);

        /* Call to the function which places M4 in low power mode */
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
 *        In case of M4, this function sets the desired clock settings
 *
 * \return None
 *
 *******************************************************************************
 */
Void Utils_idlePrepare(Void)
{
    Utils_idleCpuObj *pObj;
#ifdef CPU_IDLE_ENABLED

    UInt32 procId;
    Int32 status = (Int32) PM_SUCCESS;
    pmhalPrcmModuleId_t moduleId;
    Types_FreqHz freq;
    uint64_t temp; /* Using uint64_t datatype as UInt64 datatype causes
                      MisraC issue while performing shift operation*/

    pObj = &gUtils_idleCpuObj;
    /* Enable IPU Clock domain to HW_AUTO/SW_WAKEUP and set next power state to
        ON as ipu is master is VISION SDK */

    procId = System_getSelfProcId();
    moduleId = (pmhalPrcmModuleId_t)Utils_pmGetModId(procId);

    status = PMLIBCpuModePrepare(moduleId,PMLIB_IDLE_CPU_MODE_AUTOCG);

    pObj->timeInSleep = (UInt64) 0;
    if(status == (Int32)PM_SUCCESS)
    {
        IPU_WUGEN_Init();
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

/**
 *******************************************************************************
 *
 * \brief This function configures the SEVONPEND field in the SYSTEM_CONTROL
 *        register.
 *        SEVONPEND == 1: Send Event on Pending. Wake up from WFE if a new
 *        interrupt is pended, regardless of whether the interrupt has priority
 *        higher than current level.
 *        This is done to handle IPU in SMP mode when the Core 1 is lightly
 *        loaded.
 *        The load module does not update the total elapsed time for the idle
 *        function on Core1 if no task switches happen. So, If there are no
 *        task switches during the sampling window, the idle task
 *        load on Core 1 will get artificially inflated.
 *        This is a Bug in Bios, will get fixed in 6.42.01.
 *        In the mean time, use the below workaround,
 *        Every clock tick send an SEV to wakeup IPU C1 and set the SEVONPEND to
 *        allow waking up.
 *
 * \param  enable [IN]
 *
 *******************************************************************************
 */
void Utils_idleSetupSEVONPEND(UInt32 enable)
{
    enable = enable & 0x1U;
    /* Set IPU in deep sleep to set CM standby */
    HW_WR_FIELD32(SOC_IPU1_C0_INTC_BASE + IPU_M4_NVIC_SYSTEM_CONTROL,
                      IPU_M4_NVIC_SYSTEM_CONTROL_SEVONPEND,
                      enable);
}

/**
 *******************************************************************************
 *
 * \brief This function executes the cortex M4 SEV instruction.
 *        This function is called only from IPU2 mainIPU2_TimerTick to wakeup
 *        Core 1 when the Core 0 recieves the BIOS tick. This makes sure the
 *        Core 1 task switch happens at regular intervals and the CPU load
 *        does not show up as an inflated number.
 *
 *******************************************************************************
 */
void Utils_idleSev(Void)
{
    asm("MY_SEV: SEV");
}


/* Nothing beyond this point */
