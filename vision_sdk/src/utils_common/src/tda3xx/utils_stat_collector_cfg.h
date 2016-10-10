/*
 *******************************************************************************
 *
 * Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */

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

#define UTILS_STAT_COLLECTOR_MSTR_CFG_ARRAY_0               \
{                                                           \
    /* set 0 */                                             \
    {                                                       \
        "SCI_EMIF_SYS",                                     \
        DATA_VIS_DATA_BW_MOD_EMIF1_RD_WR,                      \
        {                                                   \
            SCI_MSTR_THROUGHPUT,                            \
            SCI_EMIF_SYS,                                   \
            0                                               \
        }                                                   \
    },                                                      \
    {                                                       \
        "SCI_IPU",                                          \
        DATA_VIS_DATA_BW_MOD_IPU1,                           \
        {                                                   \
            SCI_MSTR_THROUGHPUT,                            \
            SCI_IPU,                                        \
            0                                               \
        }                                                   \
    },                                                      \
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
        "SCI_DSP2_MDMA",                                    \
        DATA_VIS_DATA_BW_MOD_DSP2_MDMA,                     \
        {                                                   \
            SCI_MSTR_THROUGHPUT,                            \
            SCI_DSP2_MDMA,                                  \
            0                                               \
        }                                                   \
    },                                                      \
    {                                                       \
        "SCI_DSS",                                          \
        DATA_VIS_DATA_BW_MOD_DSS,                           \
        {                                                   \
            SCI_MSTR_THROUGHPUT,                            \
            SCI_DSS,                                        \
            0                                               \
        }                                                   \
    },                                                      \
    {                                                       \
        "SCI_MMU",                                          \
        DATA_VIS_DATA_BW_MOD_MMU,                           \
        {                                                   \
            SCI_MSTR_THROUGHPUT,                            \
            SCI_MMU,                                        \
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
    {                                                       \
        "SCI_VIP_P1",                                       \
        DATA_VIS_DATA_BW_MOD_VIP1_P1,                        \
        {                                                   \
            SCI_MSTR_THROUGHPUT,                            \
            SCI_VIP_P1,                                     \
            0                                               \
        }                                                   \
    },                                                      \
                                                            \
    {                                                       \
        "SCI_VIP_P2",                                       \
        DATA_VIS_DATA_BW_MOD_VIP1_P2,                        \
        {                                                   \
            SCI_MSTR_THROUGHPUT,                            \
            SCI_VIP_P2,                                     \
            0                                               \
        }                                                   \
    },                                                      \
                                                            \
    {                                                       \
        "SCI_ISS_RT",                                       \
        DATA_VIS_DATA_BW_MOD_ISS_RT,                        \
        {                                                   \
            SCI_MSTR_THROUGHPUT,                            \
            SCI_ISS_RT,                                     \
            0                                               \
        }                                                   \
    },                                                      \
    {                                                       \
        "SCI_ISS_NRT2",                                     \
        DATA_VIS_DATA_BW_MOD_ISS_NRT2,                      \
        {                                                   \
            SCI_MSTR_THROUGHPUT,                            \
            SCI_ISS_NRT2,                                   \
            0                                               \
        }                                                   \
    },                                                      \
    {                                                       \
        "SCI_EVE_P1",                                       \
        DATA_VIS_DATA_BW_MOD_EVE1_P1,                        \
        {                                                   \
            SCI_MSTR_THROUGHPUT,                            \
            SCI_EVE_P1,                                     \
            0                                               \
        }                                                   \
    },                                                      \
                                                            \
    {                                                       \
        "SCI_EVE_P2",                                       \
        DATA_VIS_DATA_BW_MOD_EVE1_P2,                        \
        {                                                   \
            SCI_MSTR_THROUGHPUT,                            \
            SCI_EVE_P2,                                     \
            0                                               \
        }                                                   \
    },                                                      \
}

#define UTILS_STAT_COLLECTOR_MSTR_CFG_ARRAY_1               \
{                                                           \
    /* set 1 */                                             \
    {                                                       \
        "SCI_GMAC_SW",                                      \
        DATA_VIS_DATA_BW_MOD_GMAC_SW,                       \
        {                                                   \
            SCI_MSTR_THROUGHPUT,                            \
            SCI_GMAC_SW,                                    \
            0                                               \
        }                                                   \
    },                                                      \
    {                                                       \
        "SCI_ISS_NRT1",                                     \
        DATA_VIS_DATA_BW_MOD_ISS_NRT1,                      \
        {                                                   \
            SCI_MSTR_THROUGHPUT,                            \
            SCI_ISS_NRT1,                                   \
            0                                               \
        }                                                   \
    },                                                      \
    {                                                       \
        "SCI_DSP2_CFG",                                     \
        DATA_VIS_DATA_BW_MOD_DSP2_CFG,                      \
        {                                                   \
            SCI_MSTR_THROUGHPUT,                            \
            SCI_DSP2_CFG,                                   \
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
        "SCI_OCMC_RAM",                                     \
        DATA_VIS_DATA_BW_MOD_OCMC_RAM,                      \
        {                                                   \
            SCI_MSTR_THROUGHPUT,                            \
            SCI_OCMC_RAM,                                   \
            0                                               \
        }                                                   \
    },                                                      \
    {                                                       \
        "SCI_DSP1_CFG",                                     \
        DATA_VIS_DATA_BW_MOD_DSP1_CFG,                      \
        {                                                   \
            SCI_MSTR_THROUGHPUT,                            \
            SCI_DSP1_CFG,                                   \
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
        "SCI_GPMC",                                         \
        DATA_VIS_DATA_BW_MOD_GPMC,                          \
        {                                                   \
            SCI_MSTR_THROUGHPUT,                            \
            SCI_GPMC,                                       \
            0                                               \
        }                                                   \
    },                                                      \
    {                                                       \
        "SCI_MCASP",                                        \
        DATA_VIS_DATA_BW_MOD_MCASP,                         \
        {                                                   \
            SCI_MSTR_THROUGHPUT,                            \
            SCI_MCASP,                                      \
            0                                               \
        }                                                   \
    },                                                      \
    {                                                       \
        "SCI_EDMA_TC1_RD",                                  \
        DATA_VIS_DATA_BW_MOD_EDMA_TC1_RD,                   \
        {                                                   \
            SCI_MSTR_THROUGHPUT,                            \
            SCI_EDMA_TC1_RD,                                \
            0                                               \
        }                                                   \
    },                                                      \
                                                            \
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



