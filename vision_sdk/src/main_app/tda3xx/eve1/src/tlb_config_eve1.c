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
 * \file tlb_config_eve1.c
 *
 * \brief  This file implements the MMU configuration of EVE1
 *
 * \version 0.0 (Jul 2013) : [SS] First version
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
#include <mmu.h>
#include <soc.h>

/*******************************************************************************
 *  DEFINES
 ***************************************************************************** */
void eve1MmuConfig(void);
void eveCommonMmuConfig(UInt32 baseAddr);

/* There are only 32 TLB entries in EVE */
#define EVE_TLB_NUM_ENTRIES (32U)

/* To allow skipping of configuration for some TLB entries */
#define INVALID_PG          (0xFFFFFFFFU)

#ifdef TDA3XX_64MB_DDR
const UInt32 tlbMapping[EVE_TLB_NUM_ENTRIES*2U] =
{

    /* PHYS   ,  VIRT   */
    0x41000000, 0x41000000, /* 00 - OCMC CBUF */
    INVALID_PG, INVALID_PG, /* 01 - Programmed by SBL 0x0 - entry point */
    INVALID_PG, INVALID_PG, /* 02 - Programmed by SBL for entry point page */
    INVALID_PG, INVALID_PG, /* 03 - Programmed by SBL 0x40000000 - 0x40000000 */
    INVALID_PG, INVALID_PG, /* 04 - Programmed by SBL 0x48000000 - 0x48000000 */
    0x42000000, 0x42000000, /* 05 - Required for IPC mailboxes */
    0x4A000000, 0x4A000000, /* 06 - VIP and peripheral accesses */
    0x81000000, 0x81000000, /* 07 - For SR1 */
    0x82000000, 0x82000000, /* 08 - For SR1 */
    0x83000000, 0x83000000, /* 09 - For SR1 */
    INVALID_PG, INVALID_PG, /* 10 - Free entry */
    INVALID_PG, INVALID_PG, /* 11 - Free entry */
    INVALID_PG, INVALID_PG, /* 12 - Free entry */
    INVALID_PG, INVALID_PG, /* 13 - Free entry */
    INVALID_PG, INVALID_PG, /* 14 - Free entry */
    INVALID_PG, INVALID_PG, /* 15 - Free entry */
    INVALID_PG, INVALID_PG, /* 16 - Free entry */
    INVALID_PG, INVALID_PG, /* 17 - Free entry */
    INVALID_PG, INVALID_PG, /* 18 - Free entry */
    INVALID_PG, INVALID_PG, /* 19 - Free entry */
    INVALID_PG, INVALID_PG, /* 20 - Free entry */
    INVALID_PG, INVALID_PG, /* 21 - Free entry */
    INVALID_PG, INVALID_PG, /* 22 - Free entry */
    INVALID_PG, INVALID_PG, /* 23 - Free entry */
    INVALID_PG, INVALID_PG, /* 24 - Free entry */
    INVALID_PG, INVALID_PG, /* 25 - Free entry */
    INVALID_PG, INVALID_PG, /* 26 - Free entry */
    INVALID_PG, INVALID_PG, /* 27 - Free entry */
    INVALID_PG, INVALID_PG, /* 28 - Free entry */
    INVALID_PG, INVALID_PG, /* 29 - Free entry */
    INVALID_PG, INVALID_PG, /* 30 - Free entry */
    0x54000000, 0x54000000, /* 31 - Needed for RTI */

};
#else
const UInt32 tlbMapping[EVE_TLB_NUM_ENTRIES*2U] =
{
    /* PHYS   ,  VIRT   */
    0x41000000, 0x41000000, /* 00 - OCMC CBUF */
    INVALID_PG, INVALID_PG, /* 01 - Programmed by SBL 0x0 - entry point */
    INVALID_PG, INVALID_PG, /* 02 - Programmed by SBL for entry point page */
    INVALID_PG, INVALID_PG, /* 03 - Programmed by SBL 0x40000000 - 0x40000000 */
    INVALID_PG, INVALID_PG, /* 04 - Programmed by SBL 0x48000000 - 0x48000000 */
    0x42000000, 0x42000000, /* 05 - Required for IPC mailboxes */
    0x4A000000, 0x4A000000, /* 06 - VIP and peripheral accesses */
    0x85000000, 0x85000000, /* 07 - For SR1 */
    0x86000000, 0x86000000, /* 08 - For SR1 */
    0x87000000, 0x87000000, /* 09 - For SR1 */
    0x88000000, 0x88000000, /* 10 - For SR1 */
    0x89000000, 0x89000000, /* 11 - For SR1 */
    0x8A000000, 0x8A000000, /* 12 - For SR1 */
    0x8B000000, 0x8B000000, /* 13 - For SR1 */
    0x8C000000, 0x8C000000, /* 14 - For SR1 */
    0x8D000000, 0x8D000000, /* 15 - For SR1 */
    0x8E000000, 0x8E000000, /* 16 - For SR1 */
    0x8F000000, 0x8F000000, /* 17 - For SR1 */
    0x90000000, 0x90000000, /* 18 - For SR1 */
    0x91000000, 0x91000000, /* 19 - For SR1 */
    0x92000000, 0x92000000, /* 20 - For SR1 */
    0x93000000, 0x93000000, /* 21 - For SR1 */
    0x94000000, 0x94000000, /* 22 - For SR1 */
    0x95000000, 0x95000000, /* 23 - For SR1 */
    0x96000000, 0x96000000, /* 24 - For SR1 */
    0x97000000, 0x97000000, /* 25 - For SR1 */
    0x98000000, 0x98000000, /* 26 - For SR1 */
    0x99000000, 0x99000000, /* 27 - For SR1 */
    0x9A000000, 0x9A000000, /* 28 - For SR1 */
    0x9B000000, 0x9B000000, /* 29 - For SR1 */
    0x9F000000, 0x9F000000, /* 30 - For SR0 */
    0x54000000, 0x54000000, /* 31 - Needed for RTI */

};
#endif

/**
 *******************************************************************************
 *
 * \brief This function implements the MMU configuration of EVE
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
        tlbEntry.virtAddr       = tlbMapping[i*2U + 1U];
        MMUTlbEntrySet(baseAddr,
                       i,
                       (const MMU_TlbEntry_t *) &tlbEntry);
    }
    /*Enable MMU*/
    MMUEnable(baseAddr);
}

/**
 *******************************************************************************
 *
 * \brief This function implements the MMU configuration of EVE1
 *
 * \return  void
 *
 *******************************************************************************
 */
void eve1MmuConfig(void)
{
    eveCommonMmuConfig(SOC_EVE_MMU0_BASE);
}
