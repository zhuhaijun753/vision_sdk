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
 * \ingroup ALGORITHM_LINK_API
 * \defgroup ALGORITHM_LINK_IMPL Algorithm Link Implementation
 *
 * @{
 */

/**
 *******************************************************************************
 *
 * \file algorithmLink_cfg.h Algorithm Link private API/Data structures
 *
 * \brief  This link private header file has all the definitions which
 *         are specific to the processor type / processor core
 *         Functions APIs which need to be called by the use case are listed
 *         here
 *
 * \version 0.0 (March 2016) : [SN] First version
 *
 *******************************************************************************
 */

#ifndef ALGORITHM_LINK_CFG_H_
#define ALGORITHM_LINK_CFG_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *  Include files
 *******************************************************************************
 */
#include <include/link_api/algorithmLink.h>

/*******************************************************************************
 *  Defines
 *******************************************************************************
 */


/**
 *******************************************************************************
 *
 *   \brief Maximum number of algorithm links to be used and stack sizes
 *          to be used.
 *
 *          It is defined based on the processor core, thus giving
 *          flexibility of different number of links / stack sizes for different
 *          processor cores. However for different links on the same processor
 *          core, stack size is kept same.
 *
 *
 *   SUPPORTED in ALL platforms
 *
 *******************************************************************************
 */

// The below defines are for A15 linux

#define ALGORITHM_LINK_OBJ_MAX        (8U)
#define ALGORITHM_LINK_TSK_STACK_SIZE (SYSTEM_DEFAULT_TSK_STACK_SIZE)
#define ALGORITHM_LINK_ALG_MAXNUM     (ALGORITHM_LINK_A15_ALG_MAXNUM)

/*******************************************************************************
 *  Defines
 *******************************************************************************
 */
Int32 AlgorithmLink_initAlgPlugins(void);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/*@}*/

/* Nothing beyond this point */
