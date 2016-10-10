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
 * \ingroup UTILS_API
 * \defgroup UTILS_EVELOADER_API APIs to load EVE binary from M4 when Linux run's on A15
 *
 * \brief This module define APIs used to load EVEs when a15 is running linux
 *
 * @{
 *
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \file utils_eveloader.h
 *
 * \brief APIs to load EVE binary from M4 when Linux run's on A15
 *
 * \version 0.0 (Aug 2014) : [YM] First version
 * \version 0.1 (May 2016) : [RG] Updates for new SBL
 *
 *******************************************************************************
 */

#ifndef _UTILS_EVELOADER_H_
#define _UTILS_EVELOADER_H_
#include <src/utils_common/include/utils.h>
#include "tda2xx/soc_defines.h"
#include "sbl_lib/sbl_lib.h"

/**
 *******************************************************************************
 *
 * \brief Boots Eves with AppImage
 *
 *
 * \return SYSTEM_LINK_STATUS_SOK on success, else failure
 *
 *******************************************************************************
 */
Int32 Utils_eveBoot(void);

/**
 *******************************************************************************
 *
 * \brief Load multi-core AppImage for EVEs
 *
 * \param imageParams  Parameters for parsing App Image
 *
 * \return SYSTEM_LINK_STATUS_SOK on success, else failure
 *
 *******************************************************************************
 */
Int32 Utils_loadAppImage(sbllibAppImageParseParams_t *imageParams);

/**
 *******************************************************************************
 *
 * \brief Reset all EVE cores
 *
 *
 * \return None
 *
 *******************************************************************************
 */
void Utils_resetAllEVECores(void);

#endif

/* @} */

