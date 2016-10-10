/*
 *******************************************************************************
 *
 * Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */

#ifndef SYSTEM_TRACE_H_
#define SYSTEM_TRACE_H_


#include <xdc/runtime/System.h>
#include <ti/sysbios/knl/Task.h>
#include <osal/bsp_osal.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *******************************************************************************
 *
 *  \ingroup SYSTEM_LINK_API
 *  \defgroup SYSTEM_LINK_TRACE_API  System Link Trace, Debug, Utility APIs
 *
 *  Utility APIs for system tracing, debug and other function's
 *
 *  @{
 *
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 *  \file system_trace.h
 *  \brief  System Link Trace, Debug, Utility APIs
 *
 *******************************************************************************
 */


/*******************************************************************************
 *  Functions
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 *  \brief Prints to Shared memory and CCS console
 *
 *  This function prints the provided formatted string to shared memory and CCS
 *  console
 *
 *  \param format       [IN] Formatted string followed by variable arguments
 *
 *  \return SYSTEM_LINK_STATUS_SOK on success, else appropriate error code on
 *          failure.
 *
 *******************************************************************************
 */
/* MISRA.PTR.ARITH
 * MISRAC_2004_Rule_16.1
 * MISRAC_WAIVER: Can be included for Debug, Utility, Trace Functions
 */
Int32 Vps_printf(const char * format, ... );

#define UTILS_COMPILETIME_ASSERT(condition)                               \
   do {                                                                   \
           typedef char ErrorCheck##__FILE__##__LINE__[((condition) == TRUE) ? 1 : -1]; \
      } while(0)

#define UTILS_ISERROR(errCode)                                  ((errCode) < 0)

extern volatile Int32 g_AssertFailLoop;

static inline void UTILS_assertLocal(Bool   condition,
                                     const Char  *str,
                                     const Char  *fileName,
                                     Int32  lineNum);

static inline void UTILS_assertErrorLocal(Bool   condition,
                                          UInt32 statusVar,
                                          UInt32 errorCode,
                                          UInt32 linkID,
                                          UInt32 channelID,
                                          Char  *str,
                                          Char  *fileName,
                                          Int32  lineNum);

#define UTILS_assert(y)                                     \
    (UTILS_assertLocal((Bool) (y), (const Char *) # y, \
                       (const Char *) __FILE__, (Int32) __LINE__))
#define UTILS_assertError(condition, statusVar, errorCode, linkID, channelID)  \
    (UTILS_assertErrorLocal((Bool) (condition),                                \
                            (UInt32) (statusVar),                              \
                            (UInt32) (errorCode),                              \
                            (UInt32) (linkID),                                 \
                            (UInt32) (channelID),                              \
                            (Char *) #condition,                               \
                            (Char *) __FILE__, (Int32) __LINE__))

static inline void UTILS_assertLocal(Bool   condition,
                                     const Char  *str,
                                     const Char  *fileName,
                                     Int32  lineNum)
{
#ifdef UTILS_ASSERT_ENABLE
    if (condition == FALSE) {
        Vps_printf(" Assertion @ Line: %d in %s: %s : failed !!!\n",
                lineNum, fileName, str);
        System_printf(" Assertion @ Line: %d in %s: %s : failed !!!\n",
                lineNum, fileName, str);
        while(g_AssertFailLoop)
        {
            BspOsal_sleep(1U);
        }
    }
#endif
    return;
}

/*
* MISRA.ONEDEFRULE.FUNC
* MISRAC_2004_Rule_8.5
* Waiver for static inline functions defined in header
* file doing init of interface structure and small
* functions for performance reasons
*/

static inline void UTILS_assertErrorLocal(Bool   condition,
                                          UInt32 statusVar,
                                          UInt32 errorCode,
                                          UInt32 linkID,
                                          UInt32 channelID,
                                          Char  *str,
                                          Char  *fileName,
                                          Int32  lineNum)
{
#ifdef UTILS_ASSERT_ENABLE
    if (condition == FALSE) {
        statusVar = errorCode;
        Vps_printf(" LINK ERR::linkID:%x::channelID:%d::errorCode:%d::FileName:%s::linuNum:%d::errorCondition:%s\n",
                     linkID, channelID, statusVar, fileName, lineNum, str);
    }
#endif
    return;
}

/**
 *******************************************************************************
 *
 *  \brief Get current timestamp in msec
 *
 *  \return timestamp in msec
 *
 *******************************************************************************
 */
UInt64 Utils_getCurGlobalTimeInMsec(void);

/**
 *******************************************************************************
 *
 *  \brief Get current timestamp in usec
 *
 *  \return timestamp in msec
 *
 *******************************************************************************
 */
UInt64 Utils_getCurGlobalTimeInUsec(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif  /* ifndef
         * SYSTEM_TRACE_H_
         */
/* @} */
