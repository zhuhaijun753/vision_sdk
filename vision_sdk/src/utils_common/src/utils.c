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
 * \file utils.c
 *
 * \brief Utility functions implementation
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


/**
 *******************************************************************************
 * \brief Global variable to break out of a assert loop when debugging
 *******************************************************************************
*/
volatile Int32 g_AssertFailLoop = TRUE;

/*******************************************************************************
 *  Local Function Declarations
 *******************************************************************************
 */
Void Utils_commonErrorRaiseHook(Error_Block *eb);

/**
 *******************************************************************************
 *
 * \brief Function to set CPU Mhz with the OS
 *
 *        OS uses this to convert its timer tick to clock time
 *
 * \param freq [IN] Frequency in Hz
 *
 * \return SYSTEM_LINK_STATUS_SOK on success, else failure
 *
 *******************************************************************************
 */
Int32 Utils_setCpuFrequency (UInt32 freq)
{
    UInt cookie;
    Types_FreqHz cpuHz;
    Types_FreqHz oldCpuHz;

    BIOS_getCpuFreq(&oldCpuHz);

    cookie = Hwi_disable();
    cpuHz.lo = freq;
    cpuHz.hi = 0;
    ti_sysbios_BIOS_setCpuFreq(&cpuHz);
    Clock_tickStop();
    Clock_tickReconfig();
    Clock_tickStart();
    Hwi_restore(cookie);

    BIOS_getCpuFreq(&cpuHz);

    Vps_printf(" *** SYSTEM: CPU Frequency <ORG = %d Hz>, <NEW = %d Hz>\n",
                oldCpuHz.lo,
                cpuHz.lo);

    return SYSTEM_LINK_STATUS_SOK;
}

/**
 *******************************************************************************
 *
 * \brief Get current time in units of msec's
 *
 * \return Current time in units of msec's
 *
 *******************************************************************************
 */
UInt32 Utils_getCurTimeInMsec(void)
{
    static UInt32 cpuKhz = 500U*1000U; /* default */
    static Bool isInit = (Bool)FALSE;

    Types_Timestamp64 ts64;
    UInt64 curTs;

    if(!isInit)
    {
        /* do this only once */

        Types_FreqHz cpuHz;

        isInit = (Bool)TRUE;

        Timestamp_getFreq(&cpuHz);

        cpuKhz = cpuHz.lo / 1000U; /* convert to Khz */

        Vps_printf(" *** UTILS: CPU KHz = %d Khz ***\n", cpuKhz);
    }

    Timestamp_get64(&ts64);

    curTs = (UInt64) ts64.hi;
    curTs = (UInt64) curTs << (UInt64)16U;
    curTs = (UInt64) curTs << (UInt64)16U;
    curTs = curTs | (UInt64)ts64.lo;

    return (UInt32)(curTs/(UInt64)cpuKhz);
}

/**
 *******************************************************************************
 *
 * \brief Get current time in units of micro sec's
 *
 * \return Current time in units of micro sec's
 *
 *******************************************************************************
 */
UInt64 Utils_getCurTimeInUsec(void)
{
    static UInt32 cpuMhz = 500U; /* default */
    static Bool isInitialized = (Bool)FALSE;

    Types_Timestamp64 ts64;
    UInt64 curTs;

    if(!isInitialized)
    {
        /* do this only once */

        Types_FreqHz cpuHz;

        isInitialized = (Bool)TRUE;

        Timestamp_getFreq(&cpuHz);

        cpuMhz = cpuHz.lo / (1000U*1000U); /* convert to Mhz */

        Vps_printf(" *** UTILS: CPU MHz = %d Mhz ***\n", cpuMhz);
    }

    Timestamp_get64(&ts64);

    curTs = (UInt64) ts64.hi << (UInt64)16U;
    curTs = (UInt64) curTs << (UInt64)16U;
    curTs = (UInt64) curTs | (UInt64)ts64.lo;

    return (curTs/cpuMhz);

}

/**
 *******************************************************************************
 *
 * \brief Reset and init buffer skip context
 *
 * \param skipCtx           [OUT] Buffer skip context
 * \param inputFrameRate    [IN]  Expected input frame rate
 * \param outputFrameRate   [IN]  Required output frame rate
 *
 * \return None
 *
 *******************************************************************************
 */
Void Utils_resetSkipBufContext(Utils_BufSkipContext *skipCtx,
                               UInt32 inputFrameRate,
                               UInt32 outputFrameRate)
{
    skipCtx->firstTime = TRUE;
    skipCtx->inputFrameRate = inputFrameRate;
    skipCtx->outputFrameRate = outputFrameRate;
}

/**
 *******************************************************************************
 *
 * \brief This function tells the caller whether current buffer should be
 *        skipped or not
 *
 *        The function uses Utils_BufSkipContext to keep track of rate
 *        of incoming buffers and outgoing buffers and based on this tells
 *        if current buffer should be skipped (return TRUE) or not skipped
 *        i.e used by the application (return FALSE)
 *
 *        Application should call this function everytime a buffers is received
 *        or need to be output and this function will tell whether to skip
 *        this buffer or not
 *
 *        Application should make sure Utils_resetSkipBufContext() once before
 *        first buffer is received
 *
 *        NOTE, the Utils_BufSkipContext is just a counter, actuall buffer
 *        pointer need not be a input to this function.
 *
 * \param skipCtx           [IN/OUT] Buffer skip context
 *
 * \return TRUE, skip this buffer \n
 *         FALSE, no not skip this buffer
 *
 *******************************************************************************
 */
Bool Utils_doSkipBuf(Utils_BufSkipContext *skipCtx )
{
    Bool status = (Bool)FALSE;

    /*
     * if the target bufferrate has changed, first time case
     * needs to be visited?
     */
    if(0 != skipCtx->firstTime)
    {
        skipCtx->outCnt = 0;
        skipCtx->inCnt = 0;

        skipCtx->multipleCnt = skipCtx->inputFrameRate * skipCtx->outputFrameRate;
        skipCtx->firstTime = FALSE;
    }

    if (skipCtx->inCnt > skipCtx->outCnt)
    {
        skipCtx->outCnt += skipCtx->outputFrameRate;
        /* skip this frame, return true */
        status = (Bool)TRUE;
    }
    else
    {
        /* out will also be multiple */
        if (skipCtx->inCnt == skipCtx->multipleCnt)
        {
            /* reset to avoid overflow */
            skipCtx->inCnt = skipCtx->outCnt = 0;
        }

        skipCtx->inCnt += skipCtx->inputFrameRate;
        skipCtx->outCnt += skipCtx->outputFrameRate;
    }

    /* display this frame, hence return false */
    return status;
}


/**
 *******************************************************************************
 *
 * \brief Return the application initialization state of the
 *        specified core
 *
 * \param coreId    [IN]  Id of the core
 * \param pState    [OUT] Application initialization state
 *
 * \return returns 0 on success
 *
 *******************************************************************************
 */
Int32 Utils_getAppInitState(UInt32 coreId, UInt32 *pState)
{
    Int32 status;

    status = RemoteLog_getAppInitState(coreId, pState);

    return status;
}

/**
 *******************************************************************************
 *
 * \brief Set the application initialization state of the
 *        specified core
 *
 * \param coreId    [IN] Core ID of the core to send the char
 * \param state     [IN] Value of the application intialization state to set
 *
 * \return returns 0 on success
 *
 *******************************************************************************
 */
Int32 Utils_setAppInitState(UInt32 coreId, UInt32 state)
{
    Int32 status;

    status = RemoteLog_setAppInitState(coreId, state);

    return status;
}

Void Utils_commonErrorRaiseHook(Error_Block *eb)
{
    Vps_printf("\n");
    Vps_printf(" ### XDC ASSERT - ERROR CALLBACK START ### \n");

    if (eb != NULL) {
        if (0 != eb->msg) {
            Vps_printf("\n");
            Text_putSite(Error_getSite(eb), NULL, -1);
            if (0 != Text_isLoaded) {
                Vps_printf(eb->msg, eb->data.arg[0], eb->data.arg[1]);
            }
            else {
                Vps_printf("error {id:0x%x, args:[0x%x, 0x%x]}",
                    eb->id, eb->data.arg[0], eb->data.arg[1]);
            }
            Vps_printf("\n");
        }
    }

    Vps_printf(" ### XDC ASSERT - ERROR CALLBACK END ### \n");
    Vps_printf("\n");

    Error_print(eb);
}
