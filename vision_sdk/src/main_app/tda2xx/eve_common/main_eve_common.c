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
 * \file main_eve_common.c
 *
 * \brief  This file implements the common code for main() and EVE_main()
 *         function for EVEs in TDA2x as per Vision SDK requirements
 *
 * \version 0.0 (Aug 2015) : [CSG] First version
 *
 *******************************************************************************
*/
/**
 *******************************************************************************
 *
 * \file main_eve1.c
 *
 * \brief  This file implements common the EVE_main()
 *
 *         This file has the EVE1 main(), the entry point to the core.
 *
 * \version 0.0 (Jul 2013) : [SS] First version
 *
 *******************************************************************************
*/

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */
#include <src/utils_common/include/utils.h>
#include <include/link_api/system_common.h>
#include <src/utils_common/include/utils_idle.h>
#include <src/main_app/tda2xx/eve_common/main_eve_common.h>
#include <ti/sysbios/family/shared/vayu/IntXbar.h>

/**
 *******************************************************************************
 *
 * \brief This function implements the wait loop of EVE.
 *
 *        This function would get called if the EVE doesn't run the demo
 *        or application. It is not used if EVE runs the app.  Do not
 *        remove this function, required when App is moved out of EVE1
 *
 * \param  arg0 [IN]
 * \param  arg1 [IN]
 *
 * \return  void
 *
 *******************************************************************************
 */
Void Eve_main_common(UArg arg0, UArg arg1)
{
    UInt32 state;
    UInt32 coreId = System_getSelfProcId();

    /* Doing a Timer Xbar configuration update as BIOS is hardcoding this
     * value this has to be reverted after fix in bios
     * 6th Irq Cross bar instance is tied to 339th instance for Timer 13.
     * 7th Irq Cross bar instance is tied to 340th instance for Timer 14.
     */
    IntXbar_connect(6U, 339U);
    IntXbar_connect(7U, 340U);

    while (1)
    {
        Task_sleep(10U);
        Utils_getAppInitState(coreId, &state);
        if (state == CORE_APP_INITSTATUS_DO_EXIT)
        {
            break;
        }
    }
}

/**
 *******************************************************************************
 *
 * \brief This is the main() implementation of EVE1.
 *
 *        This is the first function  and entry point to EVE1, does
 *         - Set the correct/required CPU frequency
 *
 * \return  SYSTEM_LINK_STATUS_SOK
 *
 *******************************************************************************
 */
int main_common(void)
{
    /* This should the first call for initializing the remote debug server
     */
        /* Doing a Timer Xbar configuration update as BIOS is hardcoding this
     * value
     * 6th Irq Cross bar instance is tied to 339th instance for Timer 13.
     * 7th Irq Cross bar instance is tied to 340th instance for Timer 14.
     */
    IntXbar_connect(6U, 339U);
    IntXbar_connect(7U, 340U);
    RemoteLog_init();
    Vps_printf(" ***** EVE Firmware build time %s %s \n",
               __TIME__,__DATE__);

    {
        UInt32 clkHz;

        clkHz = Utils_getClkHz(UTILS_CLK_ID_EVE)/2U;

        if(clkHz==0)
        {
            clkHz = SYSTEM_EVE_FREQ;
        }

        Utils_setCpuFrequency(clkHz);
    }
    /* Calling this init here to make sure EVE does not hang when
     * put to auto clock gate.
     * BIOS_Start API can reconfigure SCTM Timers if being used for BIOS ticks.
     * Currently we don't use SCTM in BIOS so this will not happen.
     * If we do use SCTM at some point later in time, we need to add additional
     * code in Eve1_main() to reconfigure the SCTM timers.
     */
    Utils_idlePrepare();

    return (SYSTEM_LINK_STATUS_SOK);
}

/**
 *******************************************************************************
 *
 * \brief This function enables the ARP32_CPU_0 Timer Ticks
 *
 * \param  arg [IN]
 *
 *******************************************************************************
 */
void mainARP32_common_TimerTick(UArg arg)
{
    Clock_tick();
}
