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
 * \file tlb_config_eve1.c
 *
 * \brief  This file implements common MMU configuration for all EVE
 *         in TDA2x as per VisionSDK requirments
 *
 * \version 0.0 (Aug 2015) : [CSG] First version
 *
 *******************************************************************************
*/

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Error.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/ipc/Ipc.h>

#include <tlb_config_eve_common.h>

/*******************************************************************************
 *  DEFINES
 ***************************************************************************** */

/* There are only 32 TLB entries in EVE */
#define EVE_TLB_NUM_ENTRIES (32U)

/* To allow skipping of configuration for some TLB entries */
#define INVALID_PG          (0xFFFFFFFFU)

const UInt32 tlbMapping[EVE_TLB_NUM_ENTRIES*2U] =
{
    /* PHYS   ,  VIRT   */
    0x41000000U, 0x41000000U, /* 00 - OCMC CBUF1 */
    INVALID_PG, INVALID_PG,   /* 01 - Programmed by SBL 0x0 - entry point */
    INVALID_PG, INVALID_PG,   /* 02 - Programmed by SBL for entry point page */
    INVALID_PG, INVALID_PG,   /* 03 - Programmed by SBL 0x40000000 - 0x40000000 */
    INVALID_PG, INVALID_PG,   /* 04 - Programmed by SBL 0x48000000 - 0x48000000 */
    0x42000000U, 0x42000000U, /* 05 - Required for IPC mailboxes */
    0x49000000U, 0x49000000U, /* 06 - OCMC CBUF2, CBUF3 */
    0x4A000000U, 0x4A000000U, /* 07 - VIP and peripheral accesses */
#ifdef A15_TARGET_OS_LINUX
    0x84000000U, 0x84000000U, /* 08 - For SR1 */
    0x85000000U, 0x85000000U, /* 09 - For SR1 */
    0x86000000U, 0x86000000U, /* 10 - For SR1 */
    0x87000000U, 0x87000000U, /* 11 - For SR1 */
    0x88000000U, 0x88000000U, /* 12 - For SR1 */
    0x89000000U, 0x89000000U, /* 13 - For SR1 */
    0x8A000000U, 0x8A000000U, /* 14 - For SR1 */
    0x8B000000U, 0x8B000000U, /* 15 - For SR1 */
    0x8C000000U, 0x8C000000U, /* 16 - For SR1 */
    0x8D000000U, 0x8D000000U, /* 17 - For SR1 */
    0x8E000000U, 0x8E000000U, /* 18 - For SR1 */
    0x8F000000U, 0x8F000000U, /* 19 - For SR1 */
    0x90000000U, 0x90000000U, /* 20 - For SR1 */
    0x91000000U, 0x91000000U, /* 21 - For SR1 */
    0x92000000U, 0x92000000U, /* 22 - For SR1 */
    0x93000000U, 0x93000000U, /* 23 - For SR1 */
    0x94000000U, 0x94000000U, /* 24 - For SR1 */
    0x95000000U, 0x95000000U, /* 25 - For SR1 */
    0x96000000U, 0x96000000U, /* 26 - For SR1 */
    0x97000000U, 0x97000000U, /* 27 - For SR1 */
    0x98000000U, 0x98000000U, /* 28 - For SR1 */
    0x99000000U, 0x99000000U, /* 29 - For SR1 */
    0x9A000000U, 0x9A000000U, /* 30 - For SR1 */
    /* TDA2XX Linux build is 1024MB_DDR */
    /* SR0 for 1024MB configuration */
    0xA0000000U, 0xA0000000U, /* 31 */
#else
    0x88000000U, 0x88000000U, /* 08 - For SR1 */
    0x89000000U, 0x89000000U, /* 09 - For SR1 */
    0x8A000000U, 0x8A000000U, /* 10 - For SR1 */
    0x8B000000U, 0x8B000000U, /* 11 - For SR1 */
    0x8C000000U, 0x8C000000U, /* 12 - For SR1 */
    0x8D000000U, 0x8D000000U, /* 13 - For SR1 */
    0x8E000000U, 0x8E000000U, /* 14 - For SR1 */
    0x8F000000U, 0x8F000000U, /* 15 - For SR1 */
    0x90000000U, 0x90000000U, /* 16 - For SR1 */
    0x91000000U, 0x91000000U, /* 17 - For SR1 */
    0x92000000U, 0x92000000U, /* 18 - For SR1 */
    0x93000000U, 0x93000000U, /* 19 - For SR1 */
    0x94000000U, 0x94000000U, /* 20 - For SR1 */
    0x95000000U, 0x95000000U, /* 21 - For SR1 */
    0x96000000U, 0x96000000U, /* 22 - For SR1 */
    0x97000000U, 0x97000000U, /* 23 - For SR1 */
    0x98000000U, 0x98000000U, /* 24 - For SR1 */
    0x99000000U, 0x99000000U, /* 25 - For SR1 */
    0x9A000000U, 0x9A000000U, /* 26 - For SR1 */
    0x9B000000U, 0x9B000000U, /* 27 - For SR1 */
    0x9C000000U, 0x9C000000U, /* 28 - For SR1 */
    0x9D000000U, 0x9D000000U, /* 29 - For SR1 */
    0x9E000000U, 0x9E000000U, /* 30 - For SR1 */
    /* SR0 for 512MB configuration */
    0x9F000000U, 0x9F000000U, /* 31 */
#endif

};

/**
 *******************************************************************************
 *
 * \brief This function implements the MMU configuration common for all EVEs
 *        in TDA2x as required by VisionSDK
 *
 * \return  void
 *
 *******************************************************************************
 */
void eveCommonMmuConfig(UInt32 baseAddr)
{
    UInt32 i;
    MMU_TlbEntry_t  tlbEntry;

    tlbEntry.valid          = 1U;
    tlbEntry.pageSize       = MMU_PAGESIZE_16MB;
    tlbEntry.endianness     = MMU_Little_Endian;
    tlbEntry.elementSize    = MMU_Byte_ElementSize; /* Unsupported - keep 0 */
    tlbEntry.tlbElementSize = MMU_TLB_ElementSize;  /* Unsupported - keep 0 */
    tlbEntry.preserve       = 0U; /* 0 => No protection against flush */

    for ( i = 0; i < EVE_TLB_NUM_ENTRIES; i++)
    {
        if(INVALID_PG == tlbMapping[i*2U])
        {
            continue;
        }
        tlbEntry.phyAddr        = tlbMapping[i*2U];
        tlbEntry.virtAddr       = tlbMapping[(i*2U) + 1U];
        MMUTlbEntrySet(baseAddr,
                       i,
                       (const MMU_TlbEntry_t *) &tlbEntry);
    }
    /*Enable MMU*/
    MMUEnable(baseAddr);
}
