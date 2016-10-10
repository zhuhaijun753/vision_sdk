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
 * \ingroup SAMPLE_MODULE_API
 * \defgroup RTI_LINK_API RTI (WDT) Link API
 *
 * \brief Valid for TDA3x ONLY
 * @{
 *
 *******************************************************************************
 */

/**
  ******************************************************************************
 * \file rtiLink.h
 *
 * \brief  RTI (WDT) Link API
 *
 * \version 0.1 (Aug 2015) : [CSG] First version
 *
 *******************************************************************************
 */


#ifndef RTI_LINK_H_
#define RTI_LINK_H_

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
 *  Include files
 *******************************************************************************
 */

/*******************************************************************************
 *  Function declarations
 *******************************************************************************
 */



/**
 *  \brief Set up the RTI link on all cores
 *         Will set up mapping between CPUs and RTI modules
 *         Create the RTI link task
 */
void System_rtiInit(void);

/**
 *  \brief Delete the RTI link task
 */
Int32 System_rtiDeinit(void);

/**
 *  \brief Enable RTI WWDT handling and monitoring on all cores
 *         Should be called only from IPU1_C0
 */
Int32 System_rtiEnableAll(void);

/**
 *  \brief Disable RTI WWDT handling and monitoring on all cores
 *         Should be called only from IPU1_C0
 */
Int32 System_rtiDisableAll(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif


/*@}*/
