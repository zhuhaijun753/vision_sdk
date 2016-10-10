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
 * \file utils_ipu_ecc.c
 *
 * \brief  This file has the implementation for utility function for using
 *         IPU ECC on TDA3x
 *
 * \version 0.0 (Feb 2016) : [CSG] First version
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
#include "utils_common/include/utils_ipu_ecc.h"
#include "ipu_ecc.h"

/*******************************************************************************
 *  Defines
 *******************************************************************************
 */

#define IPU_L1TAG_SEC_ERR_INT             (80U)
#define IPU_L1TAG_DED_ERR_INT             (81U)
#define IPU_L1DATA_SEC_ERR_INT            (82U)
#define IPU_L1DATA_DED_ERR_INT            (83U)
#define IPU_L2RAM_SEC_ERR_INT             (84U)
#define IPU_L2RAM_DED_ERR_INT             (85U)

#define IPU_ECC_IRQ_NUM                   (6U)

static Void Utils_ipuEccISR(UArg ctx);

/*******************************************************************************
 *  Global variables
 *******************************************************************************
 */
/**
 * \brief Interrupt lines used on IPU1_C0/IPU1_C1
 */
const UInt32 gIpuEccErrIntrId[IPU_ECC_IRQ_NUM] =
{
    IPU_L1TAG_SEC_ERR_INT,
    IPU_L1TAG_DED_ERR_INT,
    IPU_L1DATA_SEC_ERR_INT,
    IPU_L1DATA_DED_ERR_INT,
    IPU_L2RAM_SEC_ERR_INT,
    IPU_L2RAM_DED_ERR_INT
};

static BspOsal_IntrHandle gIpuEccErrHwiHandle[IPU_ECC_IRQ_NUM] = {NULL};

/*******************************************************************************
 *  Functions
 *******************************************************************************
 */

static Void Utils_ipuEccISR(UArg ctx)
{
    UInt32 errId = (UInt32)ctx;
    UInt32 errStat, errType, errAddr = 0U;

    Vps_printf(" UTILS: IPU ECC: Error interrupt received !!!");

    switch (errId)
    {
        case IPU_L1TAG_SEC_ERR_INT:
            Vps_printf(" UTILS: IPU ECC: L1TAG SEC Interrupt received !!!");
            errType = ECC_IPU_MEM_TYPE_L1TAG;
            break;
        case IPU_L1TAG_DED_ERR_INT:
            Vps_printf(" UTILS: IPU ECC: L1TAG DED Interrupt received !!!");
            errType = ECC_IPU_MEM_TYPE_L1TAG;
            break;
        case IPU_L1DATA_SEC_ERR_INT:
            Vps_printf(" UTILS: IPU ECC: L1D SEC Interrupt received !!!");
            errType = ECC_IPU_MEM_TYPE_L1DATA;
            break;
        case IPU_L1DATA_DED_ERR_INT:
            Vps_printf(" UTILS: IPU ECC: L1D DED Interrupt received !!!");
            errType = ECC_IPU_MEM_TYPE_L1DATA;
            break;
        case IPU_L2RAM_SEC_ERR_INT:
            Vps_printf(" UTILS: IPU ECC: L2RAM SEC Interrupt received !!!");
            errType = ECC_IPU_MEM_TYPE_L2RAM;
            break;
        case IPU_L2RAM_DED_ERR_INT:
            Vps_printf(" UTILS: IPU ECC: L2RAM DED Interrupt received !!!");
            errType = ECC_IPU_MEM_TYPE_L2RAM;
            break;
        default:
            Vps_printf(" UTILS: IPU ECC: Invalid Interrupt received !!!");
            errType = ECC_IPU_MEM_TYPE_L2RAM;
            break;
    }

    errStat =
        ECCIPUMemGetErrStatus(SOC_IPU1_UNICACHE_MMU_CONF_REGS_BASE, errType);
    ECCIPUMemGetErrInfo(SOC_IPU1_UNICACHE_MMU_CONF_REGS_BASE,
                        &errAddr,
                        errType);
    Vps_printf(" UTILS: IPU ECC: Error location: 0x%08x !!!", errAddr);

    ECCIPUMemClrErrStatus(SOC_IPU1_UNICACHE_MMU_CONF_REGS_BASE,
                          errStat,
                          errType);
}

/*
 * \brief Setup IPU ECC error handlers
 *
 * \param  None
 *
 * \return None
 */
void Utils_ipuEccInit(Void)
{
    UInt32 i;
    for(i = 0U; i < IPU_ECC_IRQ_NUM; i++)
    {
        if(NULL == gIpuEccErrHwiHandle[i])
        {
            /* MISRA.CAST.Func.Ptr
            * MISRAC_2004_Rule_11.1
            * MISRAC_WAIVER:
            * Casting it to BspOsal_IntrFuncPtr as required by the OSAL API
            */
            /* MISRA.CAST.PTR_TO_INT
             * MISRAC_2004 Rule_11.3
             * MISRAC_WAIVER:
             * Passing pointer arguments to ISR and callback functions which 
             * takes arguments as UInt32 for legacy reasons and vice-versa.
             */
            gIpuEccErrHwiHandle[i] =
                BspOsal_registerIntr(gIpuEccErrIntrId[i],
                                    (BspOsal_IntrFuncPtr) &Utils_ipuEccISR,
                                    (Void *)gIpuEccErrIntrId[i]);
        }
    }
}

/*
 * \brief Unregister IPU ECC error handlers
 *
 * \param  None
 *
 * \return None
 */
void Utils_ipuEccDeInit(Void)
{
    UInt32 i;
    for(i = 0U; i < IPU_ECC_IRQ_NUM; i++)
    {
        if(NULL != gIpuEccErrHwiHandle)
        {
            BspOsal_unRegisterIntr(&gIpuEccErrHwiHandle[i]);
            gIpuEccErrHwiHandle[i] = NULL;
        }
    }
}
