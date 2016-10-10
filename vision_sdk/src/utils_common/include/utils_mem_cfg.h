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
 * \ingroup UTILS_MEM_API
 * \defgroup UTILS_MEM_CFG_API Memory heap configuration parameters
 *
 * \brief  Parameters to enable / disable L2 heap and set size of L2 heap
 *
 * @{
 *
 *******************************************************************************
*/

/**
 *******************************************************************************
 *
 * \file utils_mem_cfg.h
 *
 * \brief Memory heap configuration parameters
 *
 * \version 0.0 First version
 *
 *******************************************************************************
*/

#ifndef UTILS_MEM_CFG_H_
#define UTILS_MEM_CFG_H_

/* The L2 RAM size for DSP is visible to all cores for save and restore functionality
 * while trying to disable and re-enable DSP during power off and power on.
 */
#define UTILS_MEM_L2RAM_SIZE    (224U * 1024U)

#ifdef BUILD_DSP
    #define ENABLE_HEAP_L2
    /* 0x800 is used by stack, remaining is for HEAP, 0x40 is used for PMLIBDoWfi */
    #define UTILS_MEM_HEAP_L2_SIZE  (UTILS_MEM_L2RAM_SIZE - 0x840U)
#endif

#ifdef BUILD_ARP32
    #define ENABLE_HEAP_L2
    #define UTILS_MEM_HEAP_L2_SIZE  (24*1024)
#endif

#if ((defined(BUILD_M4_0) && defined(IPU_PRIMARY_CORE_IPU1)) || (defined(BUILD_M4_2) && defined(IPU_PRIMARY_CORE_IPU2)))

#define ENABLE_HEAP_SR0

/*
 * Make sure value for UTILS_MEM_HEAP_OCMC_SIZE is <=
 *    OCMC1_SIZE specified in the SoC specific .xs file
 *    utils_mem_ipu1_0.c will allocate this section at start of OCMC_RAM1
 *    In case of TDA2x/TDA2Ex HS devices, first 4*kB of OCMC_RAM1 is never
 *    available. This constraint will be forced for GP devices for simplicity.
 *    As a result, actual memory available in OCMC Heap will be
 *    (UTILS_MEM_HEAP_OCMC_SIZE - 4*kB) in case of TDA2x/TDA2Ex
 *
 * Make sure value for UTILS_MEM_HEAP_DDR_CACHED_SIZE is <=
 *    SR1_FRAME_BUFFER_SIZE specified in the SoC specific .xs file
 *
 * The .xs file to look at depends on SoC, A15 OS and DDR memory config selected
 *
 * SoC          A15 OS DDR config           .xs file
 * =================================================
 * TDA2XX_BUILD Bios   TDA2XX_512MB_DDR     vision_sdk\build\tda2xx\mem_segment_definition_512mb_bios.xs
 * TDA2XX_BUILD Linux  A15_TARGET_OS_LINUX  vision_sdk\build\tda2xx\mem_segment_definition_1024mb_linux.xs
 *
 * TDA3XX_BUILD NA     TDA3XX_64MB_DDR      vision_sdk\build\tda3xx\mem_segment_definition_64mb.xs
 * TDA3XX_BUILD NA     none                 vision_sdk\build\tda3xx\mem_segment_definition_512mb.xs
 *
 * TDA2EX_BUILD Bios   TDA2EX_512MB_DDR     vision_sdk\build\tda2ex\mem_segment_definition_512mb_bios.xs
 * TDA2EX_BUILD Linux  A15_TARGET_OS_LINUX  vision_sdk\build\tda2ex\mem_segment_definition_1024mb_linux.xs
 *
 */

/*
 * OCMC_CBUF based use-cases use OCMC_RAM1.
 * When these are used, OCMC_RAM1 cannot be used as HEAP.
 * as the CBUF usage can destroy heap related data structures.
 *
 * If user wants to use, OCMC_RAM1 as heap, un-comment the following line.
 * In that case, CBUF-based use-cases (SRV/DeWarp/Sub-FrameCopy) cannot be used.
 */
/* #define UTILS_MEM_HEAP_OCMC_SIZE              (512*1024) */

/* Same sizes for TDA2X/TDA2EX/TDA3X */
#ifdef ECC_FFI_INCLUDE
#define UTILS_MEM_HEAP_DDR_ECC_ASIL_SIZE      (1*1024*1024)
#define UTILS_MEM_HEAP_DDR_ECC_QM_SIZE        (40*1024*1024)
#define UTILS_MEM_HEAP_DDR_NON_ECC_ASIL_SIZE  (1*1024*1024)
#else
#define UTILS_MEM_HEAP_DDR_ECC_ASIL_SIZE      (4*1024)
#define UTILS_MEM_HEAP_DDR_ECC_QM_SIZE        (4*1024)
#define UTILS_MEM_HEAP_DDR_NON_ECC_ASIL_SIZE  (4*1024)
#endif

#ifdef TDA2XX_BUILD
    #ifdef A15_TARGET_OS_BIOS
        #ifdef TDA2XX_512MB_DDR
            #ifdef ECC_FFI_INCLUDE
                #define UTILS_MEM_HEAP_DDR_CACHED_SIZE      (309*1024*1024)
            #else
              #ifdef OPENCL_INCLUDE
                #define UTILS_MEM_HEAP_DDR_CACHED_SIZE      (249*1024*1024)
              #else
                #define UTILS_MEM_HEAP_DDR_CACHED_SIZE      (350*1024*1024)
              #endif
            #endif
        #endif
    #endif
    #ifdef A15_TARGET_OS_LINUX
        #define UTILS_MEM_HEAP_DDR_CACHED_SIZE          (250*1024*1024)
    #endif
#endif

#ifdef TDA2EX_BUILD
    #ifdef A15_TARGET_OS_BIOS
        #ifdef TDA2EX_512MB_DDR
            #ifdef ECC_FFI_INCLUDE
                #define UTILS_MEM_HEAP_DDR_CACHED_SIZE      (309*1024*1024)
            #else
                #define UTILS_MEM_HEAP_DDR_CACHED_SIZE      (350*1024*1024)
            #endif
        #endif
    #endif

    #ifdef A15_TARGET_OS_LINUX
        #define UTILS_MEM_HEAP_DDR_CACHED_SIZE          (250*1024*1024)
    #endif
#endif

#ifdef TDA3XX_BUILD
    #ifdef TDA3XX_64MB_DDR
        #define UTILS_MEM_HEAP_DDR_CACHED_SIZE          (35*1024*1024)
    #else
        /*
         *  For 3D SRV View 2 world generation IPU1_0 CODE memory in excess of 30 MB
         *  is required and accordingly the SR1 is reduced by 34 MB.
         */
        #ifdef ECC_FFI_INCLUDE
            #ifdef TDA3X_3DSRV_V2W_GEN
                #define UTILS_MEM_HEAP_DDR_CACHED_SIZE      ((309 - 34)*1024*1024)
            #else
                #define UTILS_MEM_HEAP_DDR_CACHED_SIZE      (309*1024*1024)
            #endif
        #else
            #ifdef TDA3X_3DSRV_V2W_GEN
                #define UTILS_MEM_HEAP_DDR_CACHED_SIZE      ((350 - 34)*1024*1024)
            #else
                #define UTILS_MEM_HEAP_DDR_CACHED_SIZE      (350*1024*1024)
            #endif
        #endif
    #endif
#endif

#endif /* #ifdef BUILD_M4_0 */

#endif /* UTILS_MEM_CFG_H_ */

/* @} */
