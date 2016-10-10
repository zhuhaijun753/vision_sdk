/*
 *******************************************************************************
 *
 * Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */
#include <stdint.h>
#include <src/utils_common/include/utils_eveloader.h>
#include "hw_types.h"
#include "soc.h"
#include "sbl_lib/sbl_lib.h"
#include "pmhal_prcm.h"
#include "pm/pmhal/pmhal_mm.h"
#include "pm/pmhal/pmhal_rm.h"
#include "pm/pmhal/pmhal_cm.h"

/****************************************************************
 *  GLOBAL VARIABLES
 ****************************************************************/
#ifdef IPU1_LOAD_EVES
/* This expected to be defined in IPU1 application */
extern unsigned char gTDA2XX_EVE_FIRMWARE;
#endif

/****************************************************************
 *  FUNCTION DEFINITIONS
 ****************************************************************/

/**
 * \brief   This function copies data from DDR
 *
 * \param   dstAddr       Destination Address
 * \param   srcOffsetAddr NOR Source Offset Address
 * \param   length        The length of data block to be copied.
 *
 * \return  status        Whether copy is done successfully.
 */
Int32 Utils_ddr3ReadLocal(void    *dstAddr,
                          UInt32 srcOffsetAddr,
                          UInt32 length)
{
    memcpy((UInt32 *)dstAddr, (UInt32 *)srcOffsetAddr, length);

    return 0;
}

/**
 * \brief   This function moves the read head by n bytes.
 *
 * \param   srcAddr     Read head pointer.
 * \param   numBytes    Number of bytes of data by which read head is moved.
 *
 * \return  None
 */
void Utils_ddr3Seek(UInt32 *srcAddr, UInt32 numBytes)
{
    *(srcAddr) = numBytes;
}

/**
 * \brief   This is a dummy DDR copy function. TDA2xx SOC family doesn't have
 *          CRC feature and hence data is directly read from boot media.
 *
 * \param   dstAddr   Destination Address
 * \param   srcAddr   DDR Source Address
 * \param   length    The length of data block to be copied.
 *
 * \return  status    Whether data is copied correctly
 */
Int32 Utils_dummyDDRRead(void   *dstAddr,
                         UInt32  srcAddr,
                         UInt32  length)
{
    Int32 retVal = 0;

    /* This is dummy function */
    return retVal;
}

Int32 Utils_loadAppImage(sbllibAppImageParseParams_t *imageParams)
{
    imageParams->appImageOffset = (UInt32) &gTDA2XX_EVE_FIRMWARE;

    SBLLibRegisterImageCopyCallback(&Utils_ddr3ReadLocal,
                                    &Utils_dummyDDRRead,
                                    &Utils_ddr3Seek);

    return (SBLLibMultiCoreImageParseV1(imageParams));
}

void Utils_resetAllEVECores(void)
{
    Int32 retVal = SYSTEM_LINK_STATUS_SOK;

    /* Enable EVE clock domains */
    retVal = PMHALCMSetCdClockMode(
        (pmhalPrcmCdId_t) PMHAL_PRCM_CD_EVE1,
        (pmhalPrcmCdClkTrnModes_t) PMHAL_PRCM_CD_CLKTRNMODES_SW_WAKEUP,
        PM_TIMEOUT_NOWAIT);
    retVal += PMHALCMSetCdClockMode(
        (pmhalPrcmCdId_t) PMHAL_PRCM_CD_EVE2,
        (pmhalPrcmCdClkTrnModes_t) PMHAL_PRCM_CD_CLKTRNMODES_SW_WAKEUP,
        PM_TIMEOUT_NOWAIT);
    retVal += PMHALCMSetCdClockMode(
        (pmhalPrcmCdId_t) PMHAL_PRCM_CD_EVE3,
        (pmhalPrcmCdClkTrnModes_t) PMHAL_PRCM_CD_CLKTRNMODES_SW_WAKEUP,
        PM_TIMEOUT_NOWAIT);
    retVal += PMHALCMSetCdClockMode(
        (pmhalPrcmCdId_t) PMHAL_PRCM_CD_EVE4,
        (pmhalPrcmCdClkTrnModes_t) PMHAL_PRCM_CD_CLKTRNMODES_SW_WAKEUP,
        PM_TIMEOUT_NOWAIT);

    /* Enable EVE modules */
    retVal += PMHALModuleModeSet(
        (pmhalPrcmModuleId_t) PMHAL_PRCM_MOD_EVE1,
        (pmhalPrcmModuleSModuleMode_t) PMHAL_PRCM_MODULE_MODE_AUTO,
        PM_TIMEOUT_INFINITE);
    retVal += PMHALModuleModeSet(
        (pmhalPrcmModuleId_t) PMHAL_PRCM_MOD_EVE2,
        (pmhalPrcmModuleSModuleMode_t) PMHAL_PRCM_MODULE_MODE_AUTO,
        PM_TIMEOUT_INFINITE);
    retVal += PMHALModuleModeSet(
        (pmhalPrcmModuleId_t) PMHAL_PRCM_MOD_EVE3,
        (pmhalPrcmModuleSModuleMode_t) PMHAL_PRCM_MODULE_MODE_AUTO,
        PM_TIMEOUT_INFINITE);
    retVal += PMHALModuleModeSet(
        (pmhalPrcmModuleId_t) PMHAL_PRCM_MOD_EVE4,
        (pmhalPrcmModuleSModuleMode_t) PMHAL_PRCM_MODULE_MODE_AUTO,
        PM_TIMEOUT_INFINITE);

    if (SYSTEM_LINK_STATUS_SOK != retVal)
    {
        Vps_printf("\n UTILS: EVELOADER: EVE PRCM Failed \n");
    }

    /* Reset EVE1 */
    SBLLibCPUReset(SBLLIB_CORE_ID_EVE1);

    /* Reset EVE2 */
    SBLLibCPUReset(SBLLIB_CORE_ID_EVE2);

    /* Reset EVE3 */
    SBLLibCPUReset(SBLLIB_CORE_ID_EVE3);

    /* Reset EVE4 */
    SBLLibCPUReset(SBLLIB_CORE_ID_EVE4);
}

