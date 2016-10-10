/*
 *******************************************************************************
 *
 * Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */

#ifndef UTILS_STAT_COLLECTOR_CFG_H_
#define UTILS_STAT_COLLECTOR_CFG_H_

/**
 *******************************************************************************
 * \file utils_stat_collector_cfg.c
 *
 * \brief  This file has the implementataion of statCollector task.
 *
 *
 * \version 0.0 (Jan 2014) : [FM] First version
 *
 *******************************************************************************
*/



#define UTILS_STAT_COLLECTOR_SDRAM_CFG_ARRAY_0              \
{                                                           \
    {                                                       \
        "SCI_EMIF1 RD+WR",                                  \
        DATA_VIS_DATA_BW_MOD_EMIF1_RD_WR,                   \
        {                                                   \
            SCI_SDRAM_THROUGHPUT,                           \
            SCI_EMIF1,                                      \
            1,                                              \
            SCI_MASTID_ALL,                                 \
            0xff,                                           \
            SCI_RD_OR_WR_DONTCARE,                          \
            SCI_ERR_DONTCARE,                               \
                                                            \
        }                                                   \
    },                                                      \
    {                                                       \
        "SCI_EMIF2 RD+WR",                                  \
        DATA_VIS_DATA_BW_MOD_EMIF2_RD_WR,                   \
        {                                                   \
            SCI_SDRAM_THROUGHPUT,                           \
            SCI_EMIF2,                                      \
            1,                                              \
            SCI_MASTID_ALL,                                 \
            0xff,                                           \
            SCI_RD_OR_WR_DONTCARE,                          \
            SCI_ERR_DONTCARE,                               \
        }                                                   \
    },                                                      \
    {                                                       \
        "SCI_EMIF1 RD ONLY",                                \
        DATA_VIS_DATA_BW_MOD_EMIF1_RD_ONLY,                 \
        {                                                   \
            SCI_SDRAM_THROUGHPUT,                           \
            SCI_EMIF1,                                      \
            1,                                              \
            SCI_MASTID_ALL,                                 \
            0xff,                                           \
            SCI_RD_ONLY,                                    \
            SCI_ERR_DONTCARE,                               \
                                                            \
        }                                                   \
    },                                                      \
    {                                                       \
        "SCI_EMIF1 WR ONLY",                                \
        DATA_VIS_DATA_BW_MOD_EMIF1_WR_ONLY,                 \
        {                                                   \
            SCI_SDRAM_THROUGHPUT,                           \
            SCI_EMIF1,                                      \
            1,                                              \
            SCI_MASTID_ALL,                                 \
            0xff,                                           \
            SCI_WR_ONLY,                                    \
            SCI_ERR_DONTCARE,                               \
                                                            \
        }                                                   \
    },                                                      \
    {                                                       \
        "SCI_EMIF2 RD ONLY",                                \
        DATA_VIS_DATA_BW_MOD_EMIF2_RD_ONLY,                 \
        {                                                   \
            SCI_SDRAM_THROUGHPUT,                           \
            SCI_EMIF2,                                      \
            1,                                              \
            SCI_MASTID_ALL,                                 \
            0xff,                                           \
            SCI_RD_ONLY,                                    \
            SCI_ERR_DONTCARE,                               \
        }                                                   \
    },                                                      \
    {                                                       \
        "SCI_EMIF2 WR ONLY",                                \
        DATA_VIS_DATA_BW_MOD_EMIF2_WR_ONLY,                 \
        {                                                   \
            SCI_SDRAM_THROUGHPUT,                           \
            SCI_EMIF2,                                      \
            1,                                              \
            SCI_MASTID_ALL,                                 \
            0xff,                                           \
            SCI_WR_ONLY,                                    \
            SCI_ERR_DONTCARE,                               \
        }                                                   \
    },                                                      \
    {                                                       \
        "SCI_MA_MPU_P1",                                    \
        DATA_VIS_DATA_BW_MOD_MA_MPU_P1,                     \
        {                                                   \
            SCI_SDRAM_THROUGHPUT,                           \
            SCI_MA_MPU_P1,                                  \
            1,                                              \
            SCI_MASTID_ALL,                                 \
            0xff,                                           \
            SCI_RD_OR_WR_DONTCARE,                          \
            SCI_ERR_DONTCARE,                               \
        }                                                   \
    },                                                      \
    {                                                       \
        "SCI_MA_MPU_P2",                                    \
        DATA_VIS_DATA_BW_MOD_MA_MPU_P2,                     \
        {                                                   \
            SCI_SDRAM_THROUGHPUT,                           \
            SCI_MA_MPU_P2,                                  \
            1,                                              \
            SCI_MASTID_ALL,                                 \
            0xff,                                           \
            SCI_RD_OR_WR_DONTCARE,                          \
            SCI_ERR_DONTCARE,                               \
        }                                                   \
    },                                                      \
                                                            \
}

#define UTILS_STAT_COLLECTOR_MSTR_CFG_ARRAY_0               \
{                                                           \
    /* set 0 */                                             \
    {                                                       \
        "SCI_DSS",                                          \
        DATA_VIS_DATA_BW_MOD_DSS,                           \
        {                                                   \
            SCI_MSTR_THROUGHPUT,                            \
            SCI_DSS,                                        \
            0                                               \
        }                                                   \
    },                                                      \
                                                            \
    {                                                       \
        "SCI_IPU1",                                         \
        DATA_VIS_DATA_BW_MOD_IPU1,                          \
        {                                                   \
            SCI_MSTR_THROUGHPUT,                            \
            SCI_IPU1,                                       \
            0                                               \
        }                                                   \
    },                                                      \
                                                            \
    {                                                       \
        "SCI_VIP1_P1",                                      \
        DATA_VIS_DATA_BW_MOD_VIP1_P1,                       \
        {                                                   \
            SCI_MSTR_THROUGHPUT,                            \
            SCI_VIP1_P1,                                    \
            0                                               \
        }                                                   \
    },                                                      \
    {                                                       \
        "SCI_VIP1_P2",                                      \
        DATA_VIS_DATA_BW_MOD_VIP1_P2,                       \
        {                                                   \
            SCI_MSTR_THROUGHPUT,                            \
            SCI_VIP1_P2,                                    \
            0                                               \
        }                                                   \
    },                                                      \
                                                            \
    {                                                       \
        "SCI_VPE_P1",                                       \
        DATA_VIS_DATA_BW_MOD_VPE_P1,                        \
        {                                                   \
            SCI_MSTR_THROUGHPUT,                            \
            SCI_VPE_P1,                                     \
            0                                               \
        }                                                   \
    },                                                      \
    {                                                       \
        "SCI_VPE_P2",                                       \
        DATA_VIS_DATA_BW_MOD_VPE_P2,                        \
        {                                                   \
            SCI_MSTR_THROUGHPUT,                            \
            SCI_VPE_P1,                                     \
            0                                               \
        }                                                   \
    },                                                      \
                                                            \
    {                                                       \
        "SCI_DSP1_MDMA",                                    \
        DATA_VIS_DATA_BW_MOD_DSP1_MDMA,                     \
        {                                                   \
            SCI_MSTR_THROUGHPUT,                            \
            SCI_DSP1_MDMA,                                  \
            0                                               \
        }                                                   \
    },                                                      \
    {                                                       \
        "SCI_DSP1_EDMA",                                    \
        DATA_VIS_DATA_BW_MOD_DSP1_EDMA,                     \
        {                                                   \
            SCI_MSTR_THROUGHPUT,                            \
            SCI_DSP1_EDMA,                                  \
            0                                               \
        }                                                   \
    },                                                      \
                                                            \
    {                                                       \
        "SCI_DSP2_MDMA",                                    \
        DATA_VIS_DATA_BW_MOD_DSP2_MDMA,                     \
        {                                                   \
            SCI_MSTR_THROUGHPUT,                            \
            SCI_DSP2_MDMA,                                  \
            0                                               \
        }                                                   \
    },                                                      \
    {                                                       \
        "SCI_DSP2_EDMA",                                    \
        DATA_VIS_DATA_BW_MOD_DSP2_EDMA,                     \
        {                                                   \
            SCI_MSTR_THROUGHPUT,                            \
            SCI_DSP2_EDMA,                                  \
            0                                               \
        }                                                   \
    },                                                      \
                                                            \
    {                                                       \
        "SCI_EVE1_TC0",                                     \
        DATA_VIS_DATA_BW_MOD_EVE1_TC0,                      \
        {                                                   \
            SCI_MSTR_THROUGHPUT,                            \
            SCI_EVE1_TC0,                                   \
            0                                               \
        }                                                   \
    },                                                      \
                                                            \
    {                                                       \
        "SCI_EVE1_TC1",                                     \
        DATA_VIS_DATA_BW_MOD_EVE1_TC1,                      \
        {                                                   \
            SCI_MSTR_THROUGHPUT,                            \
            SCI_EVE1_TC1,                                   \
            0                                               \
        }                                                   \
    },                                                      \
                                                            \
    {                                                       \
        "SCI_EVE2_TC0",                                     \
        DATA_VIS_DATA_BW_MOD_EVE2_TC0,                      \
        {                                                   \
            SCI_MSTR_THROUGHPUT,                            \
            SCI_EVE2_TC0,                                   \
            0                                               \
        }                                                   \
    },                                                      \
                                                            \
    {                                                       \
        "SCI_EVE2_TC1",                                     \
        DATA_VIS_DATA_BW_MOD_EVE2_TC1,                      \
        {                                                   \
            SCI_MSTR_THROUGHPUT,                            \
            SCI_EVE2_TC1,                                   \
            0                                               \
        }                                                   \
    },                                                      \
                                                            \
    {                                                       \
        "SCI_EDMA_TC0_RD",                                  \
        DATA_VIS_DATA_BW_MOD_EDMA_TC0_RD,                   \
        {                                                   \
            SCI_MSTR_THROUGHPUT,                            \
            SCI_EDMA_TC0_RD,                                \
            0                                               \
        }                                                   \
    },                                                      \
    {                                                       \
        "SCI_EDMA_TC0_WR",                                  \
        DATA_VIS_DATA_BW_MOD_EDMA_TC0_WR,                   \
        {                                                   \
            SCI_MSTR_THROUGHPUT,                            \
            SCI_EDMA_TC0_WR,                                \
            0                                               \
        }                                                   \
    },                                                      \
                                                            \
    {                                                       \
        "SCI_EDMA_TC1_RD",                                  \
        DATA_VIS_DATA_BW_MOD_EDMA_TC1_RD,                   \
        {                                                   \
            SCI_MSTR_THROUGHPUT,                            \
            SCI_EDMA_TC1_RD,                                \
            0                                               \
        }                                                   \
    },                                                      \
    {                                                       \
        "SCI_EDMA_TC1_WR",                                  \
        DATA_VIS_DATA_BW_MOD_EDMA_TC1_WR,                   \
        {                                                   \
            SCI_MSTR_THROUGHPUT,                            \
            SCI_EDMA_TC1_WR,                                \
            0                                               \
        }                                                   \
    },                                                      \
}

#define UTILS_STAT_COLLECTOR_MSTR_CFG_ARRAY_1               \
{                                                           \
    /* set 1 */                                             \
    {                                                       \
        "SCI_VIP2_P1",                                      \
        DATA_VIS_DATA_BW_MOD_VIP2_P1,                       \
        {                                                   \
            SCI_MSTR_THROUGHPUT,                            \
            SCI_VIP2_P1,                                    \
            0                                               \
        }                                                   \
    },                                                      \
    {                                                       \
        "SCI_VIP2_P2",                                      \
        DATA_VIS_DATA_BW_MOD_VIP2_P2,                       \
        {                                                   \
            SCI_MSTR_THROUGHPUT,                            \
            SCI_VIP2_P2,                                    \
            0                                               \
        }                                                   \
    },                                                      \
    {                                                       \
        "SCI_VIP3_P1",                                      \
        DATA_VIS_DATA_BW_MOD_VIP3_P1,                       \
        {                                                   \
            SCI_MSTR_THROUGHPUT,                            \
            SCI_VIP3_P1,                                    \
            0                                               \
        }                                                   \
    },                                                      \
    {                                                       \
        "SCI_VIP3_P2",                                      \
        DATA_VIS_DATA_BW_MOD_VIP3_P2,                       \
        {                                                   \
            SCI_MSTR_THROUGHPUT,                            \
            SCI_VIP3_P2,                                    \
            0                                               \
        }                                                   \
    },                                                      \
    {                                                       \
        "SCI_EVE3_TC0",                                     \
        DATA_VIS_DATA_BW_MOD_EVE3_TC0,                      \
        {                                                   \
            SCI_MSTR_THROUGHPUT,                            \
            SCI_EVE3_TC0,                                   \
            0                                               \
        }                                                   \
    },                                                      \
                                                            \
    {                                                       \
        "SCI_EVE3_TC1",                                     \
        DATA_VIS_DATA_BW_MOD_EVE3_TC1,                      \
        {                                                   \
            SCI_MSTR_THROUGHPUT,                            \
            SCI_EVE3_TC1,                                   \
            0                                               \
        }                                                   \
    },                                                      \
                                                            \
    {                                                       \
        "SCI_EVE4_TC0",                                     \
        DATA_VIS_DATA_BW_MOD_EVE4_TC0,                      \
        {                                                   \
            SCI_MSTR_THROUGHPUT,                            \
            SCI_EVE4_TC0,                                   \
            0                                               \
        }                                                   \
    },                                                      \
                                                            \
    {                                                       \
        "SCI_EVE4_TC1",                                     \
        DATA_VIS_DATA_BW_MOD_EVE4_TC1,                      \
        {                                                   \
            SCI_MSTR_THROUGHPUT,                            \
            SCI_EVE4_TC1,                                   \
            0                                               \
        }                                                   \
    },                                                      \
                                                            \
    {                                                       \
        "SCI_IVA",                                          \
        DATA_VIS_DATA_BW_MOD_IVA,                           \
        {                                                   \
            SCI_MSTR_THROUGHPUT,                            \
            SCI_IVA,                                        \
            0                                               \
        }                                                   \
    },                                                      \
    {                                                       \
        "SCI_GPU_P1",                                       \
        DATA_VIS_DATA_BW_MOD_GPU_P1,                        \
        {                                                   \
            SCI_MSTR_THROUGHPUT,                            \
            SCI_GPU_P1,                                     \
            0                                               \
        }                                                   \
    },                                                      \
    {                                                       \
        "SCI_GPU_P2",                                       \
        DATA_VIS_DATA_BW_MOD_GPU_P2,                        \
        {                                                   \
            SCI_MSTR_THROUGHPUT,                            \
            SCI_GPU_P2,                                     \
            0                                               \
        }                                                   \
    },                                                      \
    {                                                       \
        "SCI_GMAC_SW",                                      \
        DATA_VIS_DATA_BW_MOD_GMAC_SW,                       \
        {                                                   \
            SCI_MSTR_THROUGHPUT,                            \
            SCI_GMAC_SW,                                    \
            0                                               \
        }                                                   \
    },                                                      \
}

#endif /* End of UTILS_STAT_COLLECTOR_CFG_H_ */
