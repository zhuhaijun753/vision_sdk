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
 *  \ingroup FRAMEWORK_MODULE_API
 *  \defgroup PROCESSOR_LINK_API Processor Link API
 *
 *   Processor Link is a link which is created on every processor to handle
 *   generic commands which do not fall into any specific pre-defined link
 *   category like capture or display. This interface allows user/developer to
 *   extend new commands in the system without having to create a new link
 *   everytime. Normally this link does not exchange frames and usually
 *   handles control commands only.
 *
 *******************************************************************************
*/

/**
 *******************************************************************************
 *
 *   \ingroup PROCESSOR_LINK_API
 *   \defgroup SYSTEM_COMMON_LINK_API Processor Link API: Common Interfaces
 *
 *   This module defines the control commands that are applicable to
 *   all processors running in the system.
 *
 *   @{
*/

/**
 *
 *******************************************************************************
 *
 *   \file systemLink_common.h
 *   \brief Processor Link API: Common Interfaces
 *
 *******************************************************************************
*/


#ifndef SYSTEM_LINK_COMMON_H_
#define SYSTEM_LINK_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *  Include files
 *******************************************************************************
 */

#include <include/link_api/system.h>

/**
 *******************************************************************************
 *
 *  \ingroup LINK_API_CMD
 *  \addtogroup SYSTEM_COMMON_CMD Processor Link API: Common Control Commands
 *
 *   @{
*/

/**
 *******************************************************************************
 *
 *  \brief System CMD: Start CPU load computation
 *
 *  \param NONE
 *
 *******************************************************************************
*/
#define SYSTEM_COMMON_CMD_CPU_LOAD_CALC_START       (0x9000U)

/**
 *******************************************************************************
 *
 *  \brief System CMD: Stop CPU load computation
 *
 *  \param NONE
 *
 *******************************************************************************
*/
#define SYSTEM_COMMON_CMD_CPU_LOAD_CALC_STOP       (0x9001U)

/**
 *******************************************************************************
 *
 *  \brief System CMD: Reset CPU load computation
 *
 *  \param NONE
 *
 *******************************************************************************
*/
#define SYSTEM_COMMON_CMD_CPU_LOAD_CALC_RESET      (0x9002U)

/**
 *******************************************************************************
 *
 *  \brief System CMD: Print CPU load information
 *
 *  \param NONE
 *
 *******************************************************************************
*/
#define SYSTEM_COMMON_CMD_CPU_LOAD_CALC_PRINT      (0x9003U)

/**
 *******************************************************************************
 *
 *  \brief System CMD: Print CPU load, Task Load and Heap status information
 *
 *  \param NONE
 *
 *******************************************************************************
*/
#define SYSTEM_COMMON_CMD_PRINT_STATUS             (0x9004U)

/**
 *******************************************************************************
 *
 *  \brief Check's if core is active or not
 *
 *  \param NONE
 *
 *******************************************************************************
*/
#define SYSTEM_COMMON_CMD_CORE_STATUS               (0x9005U)

/**
 *******************************************************************************
 *
 *  \brief Gets Load on this core
 *
 *  \param NONE
 *
 *******************************************************************************
*/
#define SYSTEM_COMMON_CMD_GET_LOAD                  (0x9006U)

/**
 *******************************************************************************
 *
 *  \brief System CMD: Run EDMA LLD standalone tests
 *
 *  \param NONE
 *
 *******************************************************************************
*/
#define SYSTEM_COMMON_CMD_RUN_DMA_TEST              (0x9007U)

/**
 *******************************************************************************
 *
 *  \brief System CMD: Print Statistics standalone tests
 *
 *  \param NONE
 *
 *******************************************************************************
*/
#define SYSTEM_COMMON_CMD_PRINT_STAT_COLL              (0x9008U)

/**
 *******************************************************************************
 *
 *  \brief System CMD: Reset Statistics standalone tests
 *
 *  \param NONE
 *
 *******************************************************************************
*/
#define SYSTEM_COMMON_CMD_RESET_STAT_COLL              (0x9009U)


/**
 *******************************************************************************
 *
 *  \brief System CMD: Get IP address of the system, if enabled
 *
 *  \param SystemCommon_IpAddr  [OUT] System IP address
 *
 *******************************************************************************
*/
#define SYSTEM_COMMON_CMD_GET_IP_ADDR                 (0x900AU)

/**
 *******************************************************************************
 *
 *  \brief System CMD: Allocate buffer from heaps associated with System_HeapId
 *
 *  \param SystemCommon_AllocBuffer  [OUT] bufferPtr physical address
 *  \param SystemCommon_AllocBuffer  [IN]  System_heapId
 *  \param SystemCommon_AllocBuffer  [IN]  size
 *  \param SystemCommon_AllocBuffer  [IN]  align
 *
 *
 *******************************************************************************
*/
#define SYSTEM_COMMON_CMD_ALLOC_BUFFER                (0x900BU)
/**
 *******************************************************************************
 *
 *  \brief System CMD: Free buffer from heaps associated with System_HeapId
 *
 *  \param SystemCommon_FreeBuffer  [IN] bufferPtr physical address
 *  \param SystemCommon_FreeBuffer  [IN] System_heapId
 *  \param SystemCommon_FreeBuffer  [IN] size
 *
 *******************************************************************************
*/
#define SYSTEM_COMMON_CMD_FREE_BUFFER                 (0x900CU)


/**
 *******************************************************************************
 *
 *  \brief System CMD: Get proc Id where NDK is running
 *
 *  \param SystemCommon_NetProcId  [OUT] procId
 *
 *******************************************************************************
*/
#define SYSTEM_COMMON_CMD_NET_PROC_ID                  (0x900DU)

/**
 *******************************************************************************
 *
 *  \brief System CMD: Test the timers of all enabled core
 *
 *  \param NONE
 *
 *******************************************************************************
*/
#define SYSTEM_COMMON_CMD_RUN_TIMER_TEST                 (0x9010U)


/**
 *******************************************************************************
 *
 *  \brief Gets Load on SGX core
 *
 *  \param NONE
 *
 *******************************************************************************
*/
#define SYSTEM_COMMON_CMD_GET_SGX_LOAD                   (0x901AU)

/**
 *******************************************************************************
 *
 *  \brief Get Heap Stats from IPU - to be called from other cores
 *
 *  \param NONE
 *
 *******************************************************************************
*/
#define SYSTEM_COMMON_CMD_GET_HEAP_STATS                 (0x901BU)

/* @} */

/*******************************************************************************
 *  Define's
 *******************************************************************************
 */
#define SYSTEM_COMMON_MAX_CMD_HANDLERS                (8U)

/*******************************************************************************
 *  Enums
 *******************************************************************************
 */

/**
 *******************************************************************************
 * \brief Heap ID
 *******************************************************************************
*/
typedef enum
{
    SYSTEM_HEAPID_DDR_NON_CACHED_SR0 = 0,
    /**< Heap ID of heap in DDR
     *   - This is non-cached memory
     *   - This is shared across all CPUs
     *   - Recommended to be used for small information structures, that need to be
     *     shared across CPUs
     */

    SYSTEM_HEAPID_DDR_CACHED_SR1 = 1,
    /**< Heap ID of heap in DDR
     *   - This is cached memory
     *   - This is shared across all CPUs
     *   - Recommended to be used for big buffer allocations, that need to be shared
     *     across CPUs
     */

    SYSTEM_HEAPID_DDR_CACHED_SR1_NON_ECC_QM = 1,
    /**< Heap ID of heap in DDR
     *   - This is cached memory wih ECC disabled and QM access permissions
     *   - This is shared across all CPUs
     *   - Recommended to be used for big buffer allocations, that need to be shared
     *     across CPUs
     */

    SYSTEM_HEAPID_OCMC_SR2 = 2,
    /**< Heap ID of heap in DDR
     *   - This is cached or non-cached memory depending on CPU MMU setting
     *   - This is shared across all CPUs
     *   - Recommended to be used as fast access temporary memory by algorithms
     */

    SYSTEM_HEAPID_RESERVED1 = 3,
    /**< Heap ID of heap in DDR
     *   - This is cached memory
     *   - This is only visible to the local CPUs
     *   - Recommended to be used for small information, that is needed by the
     *     local algorithm
     */

    SYSTEM_HEAPID_RESERVED2 = 4,
    /**< Heap ID of heap in L2 Memory
     *   - This is cached or non-cached memory depending on CPU MMU setting
     *   - This is only visible to the local CPU
     *   - Recommended to be used as fast access temporary memory by algorithms
     */

    SYSTEM_HEAPID_DDR_CACHED_SR1_ECC_ASIL = 5,
    /**< Heap ID of heap in DDR
     *   - This is cached memory with ECC enabled and ASIL access permissions
     *   - This is shared across all CPUs
     *   - Recommended to be used for big buffer allocations, that need to be shared
     *     across CPUs
     *   - Valid only when ECC_FFI_INCLUDE is defined.
     *     Else will map to SYSTEM_HEAPID_DDR_CACHED_SR1
     */

    SYSTEM_HEAPID_DDR_CACHED_SR1_ECC_QM = 6,
    /**< Heap ID of heap in DDR
     *   - This is cached memory with ECC enabled and QM access permissions
     *   - This is shared across all CPUs
     *   - Recommended to be used for big buffer allocations, that need to be shared
     *     across CPUs
     *   - Valid only when ECC_FFI_INCLUDE is defined.
     *     Else will map to SYSTEM_HEAPID_DDR_CACHED_SR1
     */

    SYSTEM_HEAPID_DDR_CACHED_SR1_NON_ECC_ASIL = 7,
    /**< Heap ID of heap in DDR
     *   - This is cached memory with ECC disabled and ASIL access permissions
     *   - This is shared across all CPUs
     *   - Recommended to be used for big buffer allocations, that need to be shared
     *     across CPUs
     *   - Valid only when ECC_FFI_INCLUDE is defined.
     *     Else will map to SYSTEM_HEAPID_DDR_CACHED_SR1
     */

    SYSTEM_HEAPID_MAXNUMHEAPS = 8,
    /**< max Heap ID */

    SYSTEM_HEAPID_FORCE32BITS = 0x7FFFFFFFU
    /**< This should be the last value after the max enumeration value.
     *   This is to make sure enum size defaults to 32 bits always regardless
     *   of compiler.
     */

} System_HeapId;

/*******************************************************************************
 *  Data structure's
 *******************************************************************************
 */

/**
 *******************************************************************************
 *  \brief Callback to handle user defined commands
 *
 *  \param cmd [IN] Command that needs to be handled
 *  \param pPrm [IN/OUT] Parameters for this command
 *
 *******************************************************************************
 */
typedef Void (*SystemLink_CmdHandler)(UInt32 cmd, Void *pPrm);

/**
 *******************************************************************************
 *
 *  \brief System Common: Print CPU load, Task Load and Heap Staus..
 *
 *   Defines System Common parameters that are passed when system statistics
 *   print is requested..
 *
 *******************************************************************************
*/

typedef struct {

    UInt32 printCpuLoad;
    /**< [IN] Print CPU load of each core */

    UInt32 printTskLoad;
    /**< [IN] Print CPU loading of each Task*/

    UInt32 printHeapStatus;
    /**< [IN] Print Current Heap status of core*/

} SystemCommon_PrintStatus;


/**
 *******************************************************************************
 *
 *  \brief IP address of system
 *
 *******************************************************************************
 */
typedef struct {

    char ipAddr[32U];
    /**< IP address of the system as string */

} SystemCommon_IpAddr;

/**
 *******************************************************************************
 *
 *  \brief SystemCommon_AllocBuffer defines parameters used to
 *         allocates buffer from IPU managed heap
 *
 *******************************************************************************
 */
typedef struct {
    UInt32 bufferPtr;
    /**< [OUT] pointer to allocated buffer */
    UInt32 heapId;
    /**< [IN] Heap id of the heap to allocate from */
    UInt32 size;
    /**< [IN] Size of the buffer to be allocated */
    UInt32 align;
    /**< [IN] Alignment of the allocated buffer */
} SystemCommon_AllocBuffer;

/**
 *******************************************************************************
 *
 *  \brief SystemCommon_AllocBuffer defines parameters used to
 *         free buffer from IPU managed heap
 *
 *******************************************************************************
 */
typedef struct {
    UInt32 bufferPtr;
    /**< [IN] pointer to allocated buffer */
    UInt32 heapId;
    /**< [IN] Heap id of the heap to allocate from */
    UInt32 size;
    /**< [IN] Size of the buffer to be allocated */
} SystemCommon_FreeBuffer;

/**
 *******************************************************************************
 *
 *  \brief IP address of system
 *
 *******************************************************************************
 */
typedef struct {

    UInt32 procId;
    /**< procId where NDK is running */

} SystemCommon_NetProcId;


/*******************************************************************************
 *  Function's
 *******************************************************************************
 */




/**
 *******************************************************************************
 *  \brief Register user defined command handler in System Link
 *
 *         Multiple handler can be registered
 *         All handlers are called when a command is received
 *         Unsupported command should be ignored in handler
 *
 *  \param handler [IN] User defined handler
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
 */
Int32 SystemLink_registerHandler(SystemLink_CmdHandler handler);

/**
 *******************************************************************************
 *  \brief Un-Register user defined command handler in System Link
 *
 *  \param handler [IN] User defined handler to unregister
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
 */
Int32 SystemLink_unregisterHandler(SystemLink_CmdHandler handler);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/*@}*/


