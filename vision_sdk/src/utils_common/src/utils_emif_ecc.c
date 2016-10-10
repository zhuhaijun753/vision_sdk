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
 * \file utils_emif_ecc.c
 *
 * \brief  This file has the implementation for utility function for using
 *         EMIF ECC on TDA2x and TDA3x
 *
 * \version 0.0 (Dec 2015) : [CSG] First version
 *
 *******************************************************************************
*/

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */

#include <xdc/std.h>
#include <string.h>
#include "hw_types.h"
#include "soc.h"
#include "soc_defines.h"
#include <irq_xbar_interrupt_ids.h>
#include "emif.h"
#include "utils_common/include/utils_emif_ecc.h"
#include <ti/sysbios/family/shared/vayu/IntXbar.h>

/*******************************************************************************
 *  Defines
 *******************************************************************************
 */
#define EMIF_ECC_IPU_INTR_ID (40U)
#define EMIF_START_ADDR      (0x80000000U)

/*******************************************************************************
 *  Global variables
 *******************************************************************************
 */

static BspOsal_IntrHandle gEmifErrHwiHandle = NULL;

/*******************************************************************************
 *  Function prototypes
 *******************************************************************************
 */

static Void Utils_emifEccISR(UArg ctx);

/*******************************************************************************
 *  Functions
 *******************************************************************************
 */

static Void Utils_emifEccISR(UArg ctx)
{
    UInt32           errStat, i;
    emifEccErrInfo_t emifErrInfo;

    errStat = EMIFGetIntrStatus(SOC_EMIF1_CONF_REGS_BASE);
    Vps_printf(" UTILS: ECC: EMIF Error interrupt received !!!");

    if(errStat & EMIF_SYSTEM_OCP_INTERRUPT_STATUS_WR_ECC_ERR_SYS_MASK)
    {
        Vps_printf(" UTILS: ECC: Unaligned write detected !!!");
    }

    if(errStat & EMIF_SYSTEM_OCP_INTERRUPT_STATUS_TWOBIT_ECC_ERR_SYS_MASK)
    {
        Vps_printf(" UTILS: ECC: Two bit error detected !!!");
        EMIFConfigECCGetEccErrInfo(SOC_EMIF1_CONF_REGS_BASE,
                                   EMIF_ERR_ECC_TWOBIT,
                                   &emifErrInfo);
        Vps_printf(" UTILS: ECC: Two bit error location = 0x%x !!!",
                   emifErrInfo.errAddr[0] + EMIF_START_ADDR);
    }

    if(errStat & EMIF_SYSTEM_OCP_INTERRUPT_STATUS_ONEBIT_ECC_ERR_SYS_MASK)
    {
        Vps_printf(" UTILS: ECC: Single bit error detected and corrected !!!");
        EMIFConfigECCGetEccErrInfo(SOC_EMIF1_CONF_REGS_BASE,
                                   EMIF_ERR_ECC_ONEBIT,
                                   &emifErrInfo);
        Vps_printf(" UTILS: ECC: One bit error count = %d !!!",
                   emifErrInfo.errCnt);
        for(i = 0U; i < emifErrInfo.numValidErrAddr; i++)
        {
            Vps_printf(" UTILS: ECC: One bit error location = 0x%x !!!",
                       emifErrInfo.errAddr[i] + EMIF_START_ADDR);
        }
    }

    EMIFClearIntrStatus(SOC_EMIF1_CONF_REGS_BASE, errStat);
}

/*
 * \brief Setup EMIF ECC error handlers
 *
 * \param  None
 *
 * \return None
 */
void Utils_emifEccInit(void)
{
    if(NULL == gEmifErrHwiHandle)
    {
        /* MISRA.CAST.Func.Ptr
         * MISRAC_2004_Rule_11.1
         * MISRAC_WAIVER:
         * Casting it to BspOsal_IntrFuncPtr as required by the OSAL API
         */
        /* Enable interrupt in XBAR and register ISR */
        IntXbar_connectIRQ(EMIF_ECC_IPU_INTR_ID, EMIF1_IRQ);
        gEmifErrHwiHandle =
            BspOsal_registerIntr(EMIF_ECC_IPU_INTR_ID,
                                 (BspOsal_IntrFuncPtr) &Utils_emifEccISR,
                                 NULL);
        EMIFEnableIntr(SOC_EMIF1_CONF_REGS_BASE, EMIF_INTR_STATUS_ALL);
    }
}

/*
 * \brief Unregister EMIF ECC error handlers
 *
 * \param  None
 *
 * \return None
 */
void Utils_emifEccDeInit(void)
{
    if(NULL != gEmifErrHwiHandle)
    {
        IntXbar_disconnectIRQ(EMIF_ECC_IPU_INTR_ID);
        /* Disable interrupts and Un-register ISR */
        EMIFDisableIntr(SOC_EMIF1_CONF_REGS_BASE, EMIF_INTR_STATUS_ALL);
        BspOsal_unRegisterIntr(&gEmifErrHwiHandle);
        gEmifErrHwiHandle = NULL;
    }
}
