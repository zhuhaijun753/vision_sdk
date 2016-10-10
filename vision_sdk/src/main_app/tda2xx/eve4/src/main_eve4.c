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
 * \file main_eve4.c
 *
 * \brief  This file implements the EVE4 main()
 *
 *         This file has the EVE4 main(), the entry point to the core.
 *         Set the EVE4 clock and call System_start() & BIOS_start()
 *
 * \version 0.0 (Aug 2013) : [KRB] First version
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

/*******************************************************************************
 *  Function prototypes
 *******************************************************************************
 */
Int32 System_start(Task_FuncPtr chainsMainFunc);
Int32 StartupEmulatorWaitFxn (void);
Void Eve4_main(UArg arg0, UArg arg1);
void mainARP32_3_TimerTick(UArg arg);

/**
 *******************************************************************************
 *
 * \brief This function implements the wait loop of EVE4.
 *
 *        This function would get called if the EVE4 doesn't run the demo
 *        or application. It is not used if EVE4 runs the app.  Do not
 *        remove this function, required when App is moved out of EVE4
 *
 * \param  arg0 [IN]
 * \param  arg1 [IN]
 *
 * \return  void
 *
 *******************************************************************************
 */
Void Eve4_main(UArg arg0, UArg arg1)
{
    Eve_main_common(arg0, arg1);
}

/**
 *******************************************************************************
 *
 * \brief This is the main() implementation of EVE4.
 *
 *        This is the first function  and entry point to EVE4, does
 *         - Set the correct/required CPU frequency
 *         - Call the System_start with Eve4_main() and loops there
 *
 * \return  SYSTEM_LINK_STATUS_SOK
 *
 *******************************************************************************
 */
Int32 main(void)
{
    /* This is for debug purpose- see the description of function header */
    StartupEmulatorWaitFxn();

    main_common();

    System_start(Eve4_main);
    BIOS_start();

    return (SYSTEM_LINK_STATUS_SOK);
}

/**
 *******************************************************************************
 *
 * \brief This function enables the EVE4 debug option
 *
 *        This function enables the EVE4 debug option from main() onwards
 *         - Set the volatile variable enableDebug = 1 to enable debug
 *         - rebuild the code with enableDebug = 1
 *         - Once enableDebug is set to 1, the control waits in this
 *           function even after the free-run
 *         - Can connect to core EVE via CCS and J-Tag to debug
 *         - Once CCS is connected, reset enableDebug = 0 to come out of
 *           this function and proceeds with further debug
 *
 * \return  SYSTEM_LINK_STATUS_SOK
 *
 *******************************************************************************
 */
Int32 StartupEmulatorWaitFxn (void)
{
    volatile Int32 enableDebug = 0;
    do
    {
    }while (enableDebug);
    return SYSTEM_LINK_STATUS_SOK;
}

/**
 *******************************************************************************
 *
 * \brief This function enables the ARP32_CPU_3 Timer Ticks
 *
 * \param  arg [IN]
 *
 *******************************************************************************
 */
void mainARP32_3_TimerTick(UArg arg)
{
    Clock_tick();
}
