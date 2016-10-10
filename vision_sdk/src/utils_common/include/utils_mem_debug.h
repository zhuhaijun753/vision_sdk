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
 * \defgroup UTILS_MEM_DEBUG_API Memory allocator debug log API
 *
 * \brief  APIs to allocate system buffer memory from a predefined memory pool
 *
 * @{
 *
 *******************************************************************************
*/

/**
 *******************************************************************************
 *
 * \file utils_mem_debug.h
 *
 * \brief Memory allocator debug log API
 *
 * \version 0.0 First version
 * \version 0.1 Updates as per code review comments
 *
 *******************************************************************************
*/

#ifndef UTILS_MEM_DEBUG_H_
#define UTILS_MEM_DEBUG_H_

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */
#include <src/utils_common/include/utils.h>

/*******************************************************************************
 *  Defines
 *******************************************************************************
 */

/**
 *******************************************************************************
 * \brief Control to enable memory log
 *******************************************************************************
*/
#define UTILS_MEM_ENABLE_MEMLOG                                           (TRUE)

/**
 *******************************************************************************
 * \brief Control to enable SRheap memory log
 *******************************************************************************
*/
#define UTILS_MEM_ENABLE_MEMLOG_SRHEAP                                    (TRUE)

/**
 *******************************************************************************
 * \brief Maximum number of heaps
 *******************************************************************************
*/
#define  UTILS_MEM_MAXHEAPS                                                 (8U)


#define UTILS_MEMLOG_USED_START()

#define UTILS_MEMLOG_USED_END(pUsedSize)

#define UTILS_MEMLOG_FREE_START()

#define UTILS_MEMLOG_FREE_END(pAllocSize,id)

#define UTILS_MEMLOG_PRINT(str,pUsedSizeArray,sizeOfArray)

#endif

/* @} */
