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
 * \file utils_clk.c
 *
 * \brief Utility functions implementation for gettin DPLL clock settings
 *
 * \version 0.0 (July 2013) : [KC] First version
 *
 *******************************************************************************
 */

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */

#include <src/utils_common/include/utils.h>
#include <pm/pmlib/pmlib_clkrate.h>
#include <pm/pmlib/pmlib_boardconfig.h>

#define UTILS_SYS_CLK1           (20U*1000U*1000U)

/* Various register address definitions */
#define IPU_CM_CORE_AON              ((UInt32)0x4a005500U)
#define CKGEN_CM_CORE_AON            ((UInt32)0x4a005100U)

#define CM_CLKMODE_DPLL_CORE         ((UInt32)CKGEN_CM_CORE_AON + (UInt32)0x020U)
#define CM_CLKMODE_DPLL_MPU          ((UInt32)CKGEN_CM_CORE_AON + (UInt32)0x060U)
#define CM_CLKMODE_DPLL_ABE          ((UInt32)CKGEN_CM_CORE_AON + (UInt32)0x0E0U)
#define CM_CLKMODE_DPLL_DSP          ((UInt32)CKGEN_CM_CORE_AON + (UInt32)0x134U)
#define CM_CLKMODE_DPLL_EVE          ((UInt32)CKGEN_CM_CORE_AON + (UInt32)0x184U)

#define CM_IPU1_IPU1_CLKCTRL         ((UInt32)IPU_CM_CORE_AON   + (UInt32)0x020U)

#define CM_CLKSEL_DPLL_OFFSET        (0x0CU)
#define CM_DIV_M2_DPLL_OFFSET        (0x10U)
#define CM_DIV_H22_DPLL_OFFSET       (0x34U)


/**
 *******************************************************************************
 * \brief DPLL clkout info
 *******************************************************************************
 */
typedef struct {

    UInt32 f_dpll;
    /**< DPLL clock output value */

    UInt32 clkout_M2;
    /**< M2 clock output value */

    UInt32 clkoutX2_M2;
    /**< X2_M2 clock output value */

    UInt32 isEnableClkout_M2;
    /**< TRUE, M2 clk out is enabled, else disabled */

} Utils_DpllClkOutInfo;

/**
 *******************************************************************************
 * \brief Function protoypes
 *******************************************************************************
 */

Void Utils_getDpllClkOutInfo(UInt32 base_address, Utils_DpllClkOutInfo *pPrm);
Void Utils_getDpllClkOutInfoA15(UInt32 base_address, Utils_DpllClkOutInfo *pPrm);
Void Utils_getDpllClkOutInfoAbe(UInt32 base_address, Utils_DpllClkOutInfo *pPrm);
UInt32 Utils_getDpllClkOutInfoIpu(void);
UInt32 Utils_getClkHz(Utils_ClkId clkId);

/**
 *******************************************************************************
 * \brief Get DPLL Clock out info
 *******************************************************************************
 */
Void Utils_getDpllClkOutInfo(UInt32 base_address, Utils_DpllClkOutInfo *pPrm)
{
    UInt32 temp = 0x0U, dpll_div_n, dpll_div_m;
    UInt32 dpll_div_m2;

    temp = base_address + CM_CLKSEL_DPLL_OFFSET;
    /* MISRA.CAST.PTR_TO_INT
     * MISRAC_2004_Rule_11.3:Cast between a pointer and an integral type
     * State: Defer -> Waiver -> Case by case
     * Memory mapped registers and other hardware features access requires typecasting of pointer to integer.
     * The address space is 32 bit and type casting to integer will not result into any loss because of TI's compiler
     * treatment to integer
     */
    temp = *(volatile UInt32*)temp;
    dpll_div_n = temp & (UInt32)0x7FU;
    dpll_div_m = (temp >> (UInt32)8U) & (UInt32)0x7FFU;
    pPrm->f_dpll = (UInt32)(((UInt32)UTILS_SYS_CLK1/((UInt32)(dpll_div_n+(UInt32)1U)))*dpll_div_m);

    temp = base_address + CM_DIV_M2_DPLL_OFFSET;
    /* MISRA.CAST.PTR_TO_INT
     * MISRAC_2004_Rule_11.3:Cast between a pointer and an integral type
     * State: Defer -> Waiver -> Case by case
     * Memory mapped registers and other hardware features access requires typecasting of pointer to integer.
     * The address space is 32 bit and type casting to integer will not result into any loss because of TI's compiler
     * treatment to integer
     */
    temp = *(volatile UInt32*)temp;
    dpll_div_m2 = temp & (UInt32)0xF;
    pPrm->isEnableClkout_M2 = (UInt32)((temp >> (UInt32)9U) & (UInt32)0x1U);

    pPrm->f_dpll = (UInt32)((((UInt32)2U * UTILS_SYS_CLK1) / (dpll_div_n + (UInt32)1U))*dpll_div_m);
    pPrm->clkout_M2 = (UInt32)((pPrm->f_dpll) / (dpll_div_m2 * (UInt32)2U));
    pPrm->clkoutX2_M2 = (UInt32)((pPrm->f_dpll)/(dpll_div_m2));
}

/**
 *******************************************************************************
 * \brief Get DPLL Clock out info for A15 DPLL
 *******************************************************************************
 */
Void Utils_getDpllClkOutInfoA15(UInt32 base_address,
                Utils_DpllClkOutInfo *pPrm)
{
    UInt32 temp = 0x0U,dpll_div_n,dpll_div_m,dpll_div_m2,dcc_en;

    temp = base_address + CM_CLKSEL_DPLL_OFFSET;
    /* MISRA.CAST.PTR_TO_INT
     * MISRAC_2004_Rule_11.3:Cast between a pointer and an integral type
     * State: Defer -> Waiver -> Case by case
     * Memory mapped registers and other hardware features access requires typecasting of pointer to integer.
     * The address space is 32 bit and type casting to integer will not result into any loss because of TI's compiler
     * treatment to integer
     */
    temp = *(volatile UInt32*)temp;
    dpll_div_n = temp & (UInt32)0x7FU;
    dpll_div_m = (UInt32)((temp >> (UInt32)8U) & (UInt32)0x7FFU);
    dcc_en = (UInt32)((temp >> (UInt32)22U ) & (UInt32)0x1U);

    temp = base_address + CM_DIV_M2_DPLL_OFFSET;
    /* MISRA.CAST.PTR_TO_INT
     * MISRAC_2004_Rule_11.3:Cast between a pointer and an integral type
     * State: Defer -> Waiver -> Case by case
     * Memory mapped registers and other hardware features access requires typecasting of pointer to integer.
     * The address space is 32 bit and type casting to integer will not result into any loss because of TI's compiler
     * treatment to integer
     */
    temp = *(volatile UInt32*)temp;
    dpll_div_m2 = temp & (UInt32)0xFU;
    pPrm->isEnableClkout_M2 = (UInt32)((temp >> (UInt32)9U) & (UInt32)0x1U);

    if(dcc_en == (UInt32)0x1U)
    {
        pPrm->f_dpll = (UInt32)((UTILS_SYS_CLK1 / (dpll_div_n + (UInt32)1U))*dpll_div_m);
        pPrm->clkout_M2 = (UInt32)(pPrm->f_dpll / dpll_div_m2);
    }
    else
    {
        pPrm->f_dpll = (UInt32)((((UInt32)2U * UTILS_SYS_CLK1) / (dpll_div_n + (UInt32)1U))*dpll_div_m);
        pPrm->clkout_M2 = (UInt32)(pPrm->f_dpll / (dpll_div_m2 * (UInt32)2U));
    }
}

/**
 *******************************************************************************
 * \brief Get DPLL Clock out info for ABE DPLL
 *******************************************************************************
 */
Void Utils_getDpllClkOutInfoAbe(UInt32 base_address,
                Utils_DpllClkOutInfo *pPrm)
{
    UInt32 temp = 0x0U,dpll_div_n,dpll_div_m;
    UInt32 dpll_div_m2;

    temp = base_address + CM_CLKSEL_DPLL_OFFSET;
    /* MISRA.CAST.PTR_TO_INT
     * MISRAC_2004_Rule_11.3:Cast between a pointer and an integral type
     * State: Defer -> Waiver -> Case by case
     * Memory mapped registers and other hardware features access requires typecasting of pointer to integer.
     * The address space is 32 bit and type casting to integer will not result into any loss because of TI's compiler
     * treatment to integer
     */
    temp = *(volatile UInt32*)temp;
    dpll_div_n = temp & (UInt32)0x7FU;
    dpll_div_m = (temp >> (UInt32)8U) & (UInt32)0x7FFU;
    pPrm->f_dpll = (UInt32)((UTILS_SYS_CLK1 / (dpll_div_n + (UInt32)1U))*dpll_div_m);

    temp = base_address + CM_DIV_M2_DPLL_OFFSET;
    /* MISRA.CAST.PTR_TO_INT
     * MISRAC_2004_Rule_11.3:Cast between a pointer and an integral type
     * State: Defer -> Waiver -> Case by case
     * Memory mapped registers and other hardware features access requires typecasting of pointer to integer.
     * The address space is 32 bit and type casting to integer will not result into any loss because of TI's compiler
     * treatment to integer
     */
    temp = *(volatile UInt32*)temp;
    dpll_div_m2 = temp & (UInt32)0xFU;
    pPrm->isEnableClkout_M2 = (UInt32)((temp >> (UInt32)9U) & (UInt32)0x1U);

    pPrm->f_dpll = (UInt32)((((UInt32)2U * UTILS_SYS_CLK1) / (dpll_div_n + (UInt32)1U)) * dpll_div_m);
    pPrm->clkout_M2 = (UInt32)(pPrm->f_dpll / (dpll_div_m2 * (UInt32)2U));
    pPrm->clkoutX2_M2 = (UInt32)(pPrm->f_dpll / dpll_div_m2);
}

/**
 *******************************************************************************
 * \brief Get IPU Clk Hz
 *******************************************************************************
 */
UInt32 Utils_getDpllClkOutInfoIpu(void)
{
    Utils_DpllClkOutInfo coreDpll;
    Utils_DpllClkOutInfo abeDpll;

    UInt32 temp,ipu_clksel,divhs, clkHz;
    UInt32 isEnableClkout_M2;

    clkHz = 0U;

    Utils_getDpllClkOutInfo(CM_CLKMODE_DPLL_CORE, &coreDpll);
    Utils_getDpllClkOutInfoAbe(CM_CLKMODE_DPLL_ABE, &abeDpll);

    /* MISRA.CAST.PTR_TO_INT
     * MISRAC_2004_Rule_11.3:Cast between a pointer and an integral type
     * State: Defer -> Waiver -> Case by case
     * Memory mapped registers and other hardware features access requires typecasting of pointer to integer.
     * The address space is 32 bit and type casting to integer will not result into any loss because of TI's compiler
     * treatment to integer
     */
    temp = (UInt32)*(volatile UInt32*)CM_IPU1_IPU1_CLKCTRL;
    ipu_clksel = (UInt32)((temp >> (UInt32)24U) & (UInt32)0x1U);

    if(ipu_clksel == 0)
    {
        if(abeDpll.isEnableClkout_M2)
        {
            clkHz = abeDpll.clkoutX2_M2;
        }
        else
        {
            /* Misra C fix */
        }
    }
    else
    {
        temp = CM_CLKMODE_DPLL_CORE + CM_DIV_H22_DPLL_OFFSET;
        /* MISRA.CAST.PTR_TO_INT
         * MISRAC_2004_Rule_11.3:Cast between a pointer and an integral type
         * State: Defer -> Waiver -> Case by case
         * Memory mapped registers and other hardware features access requires typecasting of pointer to integer.
         * The address space is 32 bit and type casting to integer will not result into any loss because of TI's compiler
         * treatment to integer
         */
        temp = *(volatile UInt32*)temp;
        divhs = temp & (UInt32)0x3F;
        isEnableClkout_M2 = (UInt32)((temp >> (UInt32)9U) & (UInt32)0x1U);
        if(isEnableClkout_M2)
        {
            clkHz = (UInt32) (coreDpll.f_dpll / (divhs * (UInt32)2U));
        }
        else
        {
            /* Misra C fix */
        }

    }

    return clkHz;
}

UInt32 Utils_getClkHz(Utils_ClkId clkId)
{
    UInt32 clkHz = 0;
    pmErrCode_t status = PM_FAIL;

    /**
     * \brief Holds the root clock frequencies specific for a given platform
     */
    UInt32 *rootClkFreqList;

    /**
     * \brief Holds the voltage domains information whose rails are shared
     */
    pmlibBoardVdRailShare_t *vdRailShareList;

    rootClkFreqList = PMLIBBoardConfigGetRootClks();
    vdRailShareList = PMLIBBoardConfigGetVdRailShareInfo();
    status = PMLIBClkRateInit(rootClkFreqList,vdRailShareList);

    if (PM_SUCCESS == status)
    {
        switch(clkId)
        {
            case UTILS_CLK_ID_EVE:
                status = PMLIBClkRateGet(PMHAL_PRCM_MOD_EVE1, PMHAL_PRCM_CLK_GENERIC, &clkHz);
                break;
            case UTILS_CLK_ID_DSP:
                status = PMLIBClkRateGet(PMHAL_PRCM_MOD_DSP1, PMHAL_PRCM_CLK_GENERIC, &clkHz);
                break;
            case UTILS_CLK_ID_IPU:
                status = PMLIBClkRateGet(PMHAL_PRCM_MOD_IPU1, PMHAL_PRCM_CLK_GENERIC, &clkHz);
                break;
            case UTILS_CLK_ID_A15:
                status = PMLIBClkRateGet(PMHAL_PRCM_MOD_MPU, PMHAL_PRCM_CLK_GENERIC, &clkHz);
                break;
            default:
                status = PM_FAIL;
                break;
        }
    }
    if (PM_SUCCESS != status)
    {
        clkHz = 0U;
    }


    return clkHz;
}

