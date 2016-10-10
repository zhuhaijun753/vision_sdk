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
 * \ingroup UTILS_API
 * \defgroup UTILS_BOOT_SLAVES_API Slave Boot APIs
 *
 * @{
 *
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \file utils_boot_slaves.h
 *
 * \brief  APIs to use SBL LIB to load and run slaves
 *
 * \version 0.0 (Jun 2015) : [YM] First version
 *
 *******************************************************************************
 */

#ifndef UTILS_BOOT_SLAVES_H
#define UTILS_BOOT_SLAVES_H

#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 *  Include files
 *******************************************************************************
 */
#include <include/link_api/system.h>

/*******************************************************************************
 *  Defines
 *******************************************************************************
 */
/**
 *******************************************************************************
 *
 * \brief Default qspi offset for UcLate AppImages
 *
 * SUPPORTED on TDA3x
 *
 *******************************************************************************
*/
#define SBLLIB_APP_IMAGE_LATE_OFFSET_QSPI (0xA80000U)

/*******************************************************************************
 *  Structures
 *******************************************************************************
 */

typedef struct
{
    UInt32 uclateOffset;
    /**< offset of UcLate multi-core appImage in the memory */

    UInt32 ddrAddress;
    /**< DDR address where RPRC image is copied before parsing. This is the
      *  done in order to remove the limitation of QSPI read and utilize the
      *  DDR bandwidth to do CRC. First App Image is copied from QSPI to DDR,
      *  then CRC is calculated using EDMA from DDR to CRC Signature register
      *  and finally App Image is parsed and executable sections are loaded
      *  into DDR using EDMA.
      */

    UInt32 maxDdrBuffSize;
    /**< Maximum size of DDR buffer */

    Bool useEdma;
    /**< if not true uses memcpy to copy sections to DDR */

    Bool loadCode;
    /**< Flag to state if the code needs to be loaded every time the slaves
     *   are booted or not.
     *   TRUE - Load Code, FALSE - Do not load code.
     */

    Bool enableCrc;
    /**< Flag to state if the CRC needs to be enabled or not (enabled by
     *   default). To disable CRC build SBL with CRC disabled and make the
     *   required changes in multicore image generation script (bat or sh).
     *   TRUE - Enable CRC, FALSE - Disable CRC.
     */
}Utils_BootSlaves_Params;

/*******************************************************************************
 *  Functions
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \brief Reads a Multi-core AppImage from given offset, parses it for entry
 *        points and sections to be loaded, loads and runs slave core
 *        using SBL LIB
 *
 * \return  SYSTEM_LINK_STATUS_SOK
 *
 *******************************************************************************
 */
Int32 Utils_bootSlaves(const Utils_BootSlaves_Params *params);

/**
 *******************************************************************************
 *
 * \brief Syncs up with all cores after boot up is complete and completes ipc
 *        set up
 *
 *
 * \return  SYSTEM_LINK_STATUS_SOK
 *
 *******************************************************************************
 */
Void Utils_syncSlaves(void);

/**
 *******************************************************************************
 *
 * \brief Initializes basic create time params
 *
 *
 * \return  SYSTEM_LINK_STATUS_SOK
 *
 *******************************************************************************
 */
static inline Void Utils_bootSlaves_paramsInit(Utils_BootSlaves_Params *params);
static inline Void Utils_bootSlaves_paramsInit(Utils_BootSlaves_Params *params)
{
    params->uclateOffset  = SBLLIB_APP_IMAGE_LATE_OFFSET_QSPI;
    params->ddrAddress  = 0U;
    params->maxDdrBuffSize  = 0U;
    params->useEdma = (Bool)TRUE;
    params->loadCode = (Bool)TRUE;
    params->enableCrc = (Bool)TRUE;
}


Void Utils_bootPowerDownSlaves(Void);

#ifdef __cplusplus
}
#endif

#endif

/* @} */
