/*
 *******************************************************************************
 *
 * Copyright (C) 2016 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */

/**
 *******************************************************************************
 * \file utils_ecc_c66x.c
 *
 * \brief  APIs from this file are used to enable the ECC for C66x.
 *
 * \version 0.0 (Feb 2016) : [RG] First version
 *
 *******************************************************************************
*/

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */
#include "stdint.h"
#include "hw_types.h"
#include <xdc/std.h>
#include "soc.h"
#include "dsp_icfg.h"
#include "osal/bsp_osal.h"
#include "stw_platformEcc.h"
#include <src/utils_common/include/utils_ecc_c66x.h>
#include <include/link_api/system_trace.h>
#include <ti/sysbios/family/c66/Cache.h>
#include <ti/sysbios/family/c64p/EventCombiner.h>

/*******************************************************************************
 *  Defines
 *******************************************************************************
 */
#define DSP_L1P_ED_INTR_ID (113U)
#define DSP_L2_ED1_INTR_ID (116U)
#define DSP_L2_ED2_INTR_ID (117U)

#define DSP_ECC_TIMEOUT    (1000U)

/*******************************************************************************
 *  Global variables
 *******************************************************************************
 */

/*******************************************************************************
 *  Function prototypes
 *******************************************************************************
 */
static Void Utils_dspL1PEccErrISR(UArg ctx);
static Void Utils_dspL2CorrEccErrISR(UArg ctx);
static Void Utils_dspL2UncorrEccErrISR(UArg ctx);

/**
 *******************************************************************************
 *
 * \brief This function divids DSPL2  into a cache and heap
 *
 *******************************************************************************
 */

static Void Utils_dspL1PEccErrISR(UArg ctx)
{
    eccDspErrStatus_t errStatus = {0U};
    eccDspErrInfo_t   eccDspInfo = {0U};

    ECCDspIntrGetErrStatus(&errStatus);
    ECCDspGetErrInfo(ECC_DSP_MEM_TYPE_L1P, &eccDspInfo);

    if (1U == errStatus.l1PProgramFetchErr)
    {
        if (0U == eccDspInfo.errRamOrCache)
        {
            Vps_printf(" UTILS: ECC: DSP L1P Cache Error interrupt received !!!");
            Vps_printf(" UTILS: ECC: DSP L1P Cache Error address = 0x%x",
                        eccDspInfo.errAddress);
            DSPICFGCacheInvalidateAll(SOC_DSP_ICFG_BASE, DSPICFG_MEM_L1P);
        }
    }

    if (1U == errStatus.l1PDmaAccessErr)
    {
        if (1U == eccDspInfo.errRamOrCache)
        {
            Vps_printf(" UTILS: ECC: DSP L1P RAM Error interrupt received !!!");
            Vps_printf(" UTILS: ECC: DSP L1P RAM Error address = 0x%x",
                        eccDspInfo.errAddress);
        }
    }

    ECCDspIntrClrErrStatus(&errStatus);
}

static Void Utils_dspL2CorrEccErrISR(UArg ctx)
{
    eccDspErrStatus_t errStatus = {0U};
    eccDspErrInfo_t   eccDspInfo = {0U};

    ECCDspIntrGetErrStatus(&errStatus);
    ECCDspGetErrInfo(ECC_DSP_MEM_TYPE_L2, &eccDspInfo);

    if (1U == errStatus.l2L1PAccessErr)
    {
        Vps_printf(" UTILS: ECC: DSP L2 L1P Access Error interrupt received !!!");
        DSPICFGCacheInvalidateAll(SOC_DSP_ICFG_BASE, DSPICFG_MEM_L1P);
    }

    if (1U == errStatus.l2L1DAccessErr)
    {
        Vps_printf(" UTILS: ECC: DSP L2 L1D Access Error interrupt received !!!");
    }

    if (1U == errStatus.l2DmaAccessErr)
    {
        Vps_printf(" UTILS: ECC: DSP L2 DMA Access Error interrupt received !!!");
    }

    if (1U == errStatus.l2VictimsErr)
    {
        Vps_printf(" UTILS: ECC: DSP L2 Victim Error interrupt received !!!");
    }

    Vps_printf(" UTILS: ECC: DSP L2 correctable error address = 0x%x",
                eccDspInfo.errAddress);
    Vps_printf(" UTILS: ECC: DSP L2 correctable error occured @ Ram(1)/Cache(0) = %d",
                eccDspInfo.errRamOrCache);
    Vps_printf(" UTILS: ECC: DSP L2 correctable error bit position = %d",
                eccDspInfo.errL2BitPosition);
    Vps_printf(" UTILS: ECC: DSP L2 correctable error type @ Single Bit(0)/Double Bit(1)/Parity(3) = %d",
                eccDspInfo.errL2Type);
    Vps_printf(" UTILS: ECC: DSP L2 correctable error way = %d",
                eccDspInfo.errL2way);
    Vps_printf(" UTILS: ECC: DSP L2 correctable error count = %d",
                eccDspInfo.errL2CorrectableCount);

    ECCDspIntrClrErrStatus(&errStatus);
}

static Void Utils_dspL2UncorrEccErrISR(UArg ctx)
{
    eccDspErrStatus_t errStatus = {0U};
    eccDspErrInfo_t   eccDspInfo = {0U};

    ECCDspIntrGetErrStatus(&errStatus);
    ECCDspGetErrInfo(ECC_DSP_MEM_TYPE_L2, &eccDspInfo);

    if (1U == errStatus.l2L1PAccessErr)
    {
        Vps_printf(" UTILS: ECC: DSP L2 L1P Access Error interrupt received !!!");
        DSPICFGCacheInvalidateAll(SOC_DSP_ICFG_BASE, DSPICFG_MEM_L1P);
    }

    if (1U == errStatus.l2L1DAccessErr)
    {
        Vps_printf(" UTILS: ECC: DSP L2 L1D Access Error interrupt received !!!");
    }

    if (1U == errStatus.l2DmaAccessErr)
    {
        Vps_printf(" UTILS: ECC: DSP L2 DMA Access Error interrupt received !!!");
    }

    if (1U == errStatus.l2VictimsErr)
    {
        Vps_printf(" UTILS: ECC: DSP L2 Victim Error interrupt received !!!");
    }

    Vps_printf(" UTILS: ECC: DSP L2 uncorrectable error address = 0x%x",
                eccDspInfo.errAddress);
    Vps_printf(" UTILS: ECC: DSP L2 uncorrectable error occured @ Ram(1)/Cache(0) = %d",
                eccDspInfo.errRamOrCache);
    Vps_printf(" UTILS: ECC: DSP L2 uncorrectable error bit position = %d",
                eccDspInfo.errL2BitPosition);
    Vps_printf(" UTILS: ECC: DSP L2 uncorrectable error type @ Single Bit(0)/Double Bit(1)/Parity(3) = %d",
                eccDspInfo.errL2Type);
    Vps_printf(" UTILS: ECC: DSP L2 uncorrectable error way = %d",
                eccDspInfo.errL2way);
    Vps_printf(" UTILS: ECC: DSP L2 uncorrectable error count = %d",
                eccDspInfo.errL2NonCorrectableCount);

    ECCDspIntrClrErrStatus(&errStatus);
}

Void Utils_dspEccEnable(Void)
{
    /* Enable ECC for L2 */
    ECCDspEnable(ECC_DSP_MEM_TYPE_L2,
                 ECC_ENABLE,
                 DSP_ECC_TIMEOUT);

    /* Disable Cache */
    Cache_disable(Cache_Type_L1P);

    /* Invalidate L1P Cache */
    Cache_invL1pAll();

    /* Enable ECC for L1P */
    ECCDspEnable(ECC_DSP_MEM_TYPE_L1P,
                 ECC_ENABLE,
                 DSP_ECC_TIMEOUT);

    /* Enable cache */
    Cache_enable(Cache_Type_L1P);
}

/*
 * \brief Setup DSP ECC error handlers
 *
 * \param  None
 *
 * \return None
 */
Void Utils_dspEccInit(Void)
{
    /* Register ISR */
    /* MISRA.CAST.Func.Ptr
     * MISRAC_2004_Rule_11.1
     * MISRAC_WAIVER:
     * Casting it to EventCombiner_FuncPtr as required by the OSAL API
     */
    EventCombiner_dispatchPlug(
                    DSP_L1P_ED_INTR_ID,
                    (EventCombiner_FuncPtr)(&Utils_dspL1PEccErrISR),
                    NULL,
                    (Bool)TRUE
                    );
    EventCombiner_enableEvent(DSP_L1P_ED_INTR_ID);

    /* MISRA.CAST.Func.Ptr
     * MISRAC_2004_Rule_11.1
     * MISRAC_WAIVER:
     * Casting it to EventCombiner_FuncPtr as required by the OSAL API
     */
    EventCombiner_dispatchPlug(
                    DSP_L2_ED1_INTR_ID,
                    (EventCombiner_FuncPtr)(&Utils_dspL2CorrEccErrISR),
                    NULL,
                    (Bool)TRUE
                    );
    EventCombiner_enableEvent(DSP_L2_ED1_INTR_ID);

    /* MISRA.CAST.Func.Ptr
     * MISRAC_2004_Rule_11.1
     * MISRAC_WAIVER:
     * Casting it to EventCombiner_FuncPtr as required by the OSAL API
     */
    EventCombiner_dispatchPlug(
                    DSP_L2_ED2_INTR_ID,
                    (EventCombiner_FuncPtr)(&Utils_dspL2UncorrEccErrISR),
                    NULL,
                    (Bool)TRUE
                    );
    EventCombiner_enableEvent(DSP_L2_ED2_INTR_ID);
}

/*
 * \brief Unregister DSP ECC error handlers
 *
 * \param  None
 *
 * \return None
 */
Void Utils_dspEccDeInit(Void)
{
    /* Un-register ISR */
    EventCombiner_disableEvent(DSP_L1P_ED_INTR_ID);
    EventCombiner_disableEvent(DSP_L2_ED1_INTR_ID);
    EventCombiner_disableEvent(DSP_L2_ED2_INTR_ID);
}

/* Nothing beyond this point */
