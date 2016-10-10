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
 * \file algorithmLink_cfg.c
 *
 * \brief  This file has some configuration of algorithm link
 *
 *         Functions in this file will be called by use case or application
 *
 * \version 0.0 (March 2016) : [SN] First version
 *
 *******************************************************************************
*/

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */
#include "algorithmLink_priv.h"
#include "algorithmLink_cfg.h"
#include <include/link_api/algorithmLink.h>

#include <include/link_api/algorithmLink_frameCopy.h>
#include <include/link_api/algorithmLink_opencvCanny.h>

/*******************************************************************************
 *  Declaring gAlgorithmLinkFuncTable for the current core
 *******************************************************************************
 */
AlgorithmLink_FuncTable gAlgorithmLinkFuncTable[ALGORITHM_LINK_ALG_MAXNUM];

/**
 *******************************************************************************
 * \brief Display Link object, stores all link related information
 *******************************************************************************
 */
AlgorithmLink_Obj gAlgorithmLink_obj[ALGORITHM_LINK_OBJ_MAX];

/**
 *******************************************************************************
 *
 * \brief Initializing alg plugins
 *
 *        This function needs to be called by the use case / application.
 *        This function will inturn call the algorithm specific init functions
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
 */
Int32 AlgorithmLink_initAlgPlugins(void)
{
    memset(gAlgorithmLinkFuncTable, 0, sizeof(gAlgorithmLinkFuncTable));

    /** For all alorithms on A15 */
    AlgorithmLink_FrameCopy_initPlugin();
#ifdef ENABLE_OPENCV
    AlgorithmLink_OpenCVCanny_initPlugin();
#endif

    return SYSTEM_LINK_STATUS_SOK;
}

/* Nothing beyond this point */
