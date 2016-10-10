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
 * \file main_eve_common.h
 *
 * \brief  This file gives interface for common code for main() and EVE_main()
 *         function for EVEs in TDA2x as per Vision SDK requirements
 *
 * \version 0.0 (Aug 2015) : [CSG] First version
 *
 *******************************************************************************
*/

#ifndef MAIN_EVE_COMMON_H_
#define MAIN_EVE_COMMON_H_

/* =============================================================================
 * INCLUDE FILES (only if necessary)
 * =============================================================================
 */

#ifdef __cplusplus
extern "C" {
#endif

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
Void Eve_main_common(UArg arg0, UArg arg1);

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
Int32 main_common(void);

/**
 *******************************************************************************
 *
 * \brief This function enables the ARP32_CPU_0 Timer Ticks
 *
 * \param  arg [IN]
 *
 *******************************************************************************
 */
void mainARP32_common_TimerTick(UArg arg);



#ifdef __cplusplus
}
#endif

#endif  /* MAIN_EVE_COMMON_H_ */
/* EOF */

