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
 *  \file utils_eveloader.c
 *
 *  \brief This file has implementation for eve loader. This is mainly used
 *         when linux is running on A15. It uses SBL from starterware to
 *         boot load eves
 *
 *
 *  \version 0.0 (Aug 2014) : [YM] First version
 *  \version 0.1 (May 2016) : [RG] Updates for new SBL
 *******************************************************************************
*/

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
*/
#include "stdint.h"
#include "sbl_lib/sbl_lib.h"
#include "sbl_lib/sbl_lib_tda2xx.h"
#include "pm/pmhal/pmhal_cm.h"
#include <src/utils_common/include/utils.h>
#include <src/utils_common/include/utils_eveloader.h>

/*******************************************************************************
 *  GLOBAL VARIABLES
 *******************************************************************************
*/

/*
 * Variable for version 1 of Meta Header structure
 */
sbllibMetaHeaderV1_t    gUtilsAppMetaHeaderV1;

/*
 * Variable for RPRC Header structure
 */
sbllibRPRCImageHeader_t gUtilsAppRPRCHeader;

/*
 * Variable for Entry points
 */
sbllibEntryPoints_t gUtilsEntryPoints =
{{0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U}};


/*******************************************************************************
 *  FUNCTION DEFINITIONS
 *******************************************************************************
*/

/**
 * \brief   This function acts as a wrapper for Print function.
 *
 * \param   message       Message to be printed.
 *
 * \return  None.
 */
void Utils_eveLoaderPrintFxn(const char *message)
{
    Vps_printf(message);
}

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
Int32 Utils_eveBoot(void)
{
    Int32 retVal = SYSTEM_LINK_STATUS_SOK;
    UInt32 sblBuildMode = SBLLIB_SBL_BUILD_MODE_PROD;
    UInt32 oppId = SBLLIB_PRCM_DPLL_OPP_NOM;
    sbllibAppImageParseParams_t appImgParams;
    pmhalPrcmDpllConfig_t      *dpllParams;
    pmhalPrcmSysClkVal_t sysClkFreq = PMHALCMGetSysClockFreqEnum();
    sbllibInitParams_t sblInitPrms;

    /* Default initialization of SBL Lib Params */
    SBLLibInitParamsInit(&sblInitPrms);

    /* Assign SBL Params */
    sblInitPrms.printFxn = &Utils_eveLoaderPrintFxn;
    SBLLibInit(&sblInitPrms);

    /* Configure DPLL EVE */
    retVal = SBLLibGetDpllStructure(PMHAL_PRCM_DPLL_EVE,
                                     sysClkFreq,
                                     oppId,
                                     &dpllParams);

    retVal += PMHALCMDpllConfigure(PMHAL_PRCM_DPLL_EVE,
                                   dpllParams,
                                   PM_TIMEOUT_INFINITE);
    if (SYSTEM_LINK_STATUS_SOK != retVal)
    {
        Vps_printf("\n UTILS: EVELOADER: DPLL EVE not configured Correctly \n");
        SBLLibAbortBoot();
    }

    /* Reset all EVEs */
    Utils_resetAllEVECores();

    /* Initialize App Image Params */
    SBLLibAppImageParamsInit(&appImgParams);
    appImgParams.appImgMetaHeaderV1 = &gUtilsAppMetaHeaderV1;
    appImgParams.appImgRPRCHeader   = &gUtilsAppRPRCHeader;
    appImgParams.entryPoints        = &gUtilsEntryPoints;
    appImgParams.skipDDRCopy        = 1U;

    Utils_loadAppImage(&appImgParams);

#ifdef PROC_EVE1_INCLUDE
    SBLLibEVE1BringUp(gUtilsEntryPoints.entryPoint[SBLLIB_CORE_ID_EVE1],
                      sblBuildMode);
#endif
#ifdef PROC_EVE2_INCLUDE
    SBLLibEVE2BringUp(gUtilsEntryPoints.entryPoint[SBLLIB_CORE_ID_EVE2],
                      sblBuildMode);
#endif
#ifdef PROC_EVE3_INCLUDE
    SBLLibEVE3BringUp(gUtilsEntryPoints.entryPoint[SBLLIB_CORE_ID_EVE3],
                      sblBuildMode);
#endif
#ifdef PROC_EVE4_INCLUDE
    SBLLibEVE4BringUp(gUtilsEntryPoints.entryPoint[SBLLIB_CORE_ID_EVE4],
                      sblBuildMode);
#endif

    return retVal;
}
