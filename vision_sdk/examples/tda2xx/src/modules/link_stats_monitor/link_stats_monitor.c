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
 * \file utils_link_stats_monitor.c
 *
 * \brief  This file implements monitor portion of the link statistics.
 *
 *         This file reads link statistics from the shared memory and prints
 *         them on the console every 5 seconds.
 *         It runs an independent task
 *
 * \version 0.1 (Mar 2015) : First version
 *
 *******************************************************************************
*/
/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */
#include <src/utils_common/include/utils_link_stats_if.h>
#include <src/utils_common/include/utils.h>
#include <include/link_api/system_common.h>
#include <src/links_common/system/system_priv_common.h>

#include <examples/tda2xx/include/link_stats_monitor.h>

/*******************************************************************************
 *  Defines
 *******************************************************************************
 */



/*******************************************************************************
 *  Function Definitions
 *******************************************************************************
 */

int32_t calcCoreLoad(System_LinkStatsCorePrfObj *pCoreLoadObj,
                     int32_t *hwiLoad,
                     int32_t *swiLoad,
                     int32_t *cpuLoad,
                     int32_t *lowPower)
{
    int32_t  retVal = SYSTEM_LINK_STATUS_SOK;
    uint64_t totalTime, time64, temp;

    totalTime = pCoreLoadObj->accPrfLoadObj.totalTimeLo & 0xFFFFFFFFU;
    temp = pCoreLoadObj->accPrfLoadObj.totalTimeHi & 0xFFFFFFFFU;
    totalTime |= (temp << 32);

    if (0 != totalTime)
    {
        time64 = pCoreLoadObj->accPrfLoadObj.totalHwiThreadTimeLo & 0xFFFFFFFFU;
        temp = pCoreLoadObj->accPrfLoadObj.totalHwiThreadTimeHi & 0xFFFFFFFFU;
        time64 |= (temp << 32);
        *hwiLoad = (time64 * 1000) / totalTime;

        time64 = pCoreLoadObj->accPrfLoadObj.totalSwiThreadTimeLo & 0xFFFFFFFFU;
        temp = pCoreLoadObj->accPrfLoadObj.totalSwiThreadTimeHi & 0xFFFFFFFFU;
        time64 |= (temp << 32);
        *swiLoad = (time64 * 1000) / totalTime;

        time64 = pCoreLoadObj->accPrfLoadObj.totalIdlTskTimeLo & 0xFFFFFFFFU;
        temp = pCoreLoadObj->accPrfLoadObj.totalIdlTskTimeHi & 0xFFFFFFFFU;
        time64 |= (temp << 32);
        *cpuLoad = 1000 - ((time64 * 1000) / totalTime);

        time64 = pCoreLoadObj->accPrfLoadObj.totalLowPowerTimeLo & 0xFFFFFFFFU;
        temp = pCoreLoadObj->accPrfLoadObj.totalLowPowerTimeHi & 0xFFFFFFFFU;
        time64 = time64 | (temp << 32);
        *lowPower = (time64 * 1000) / totalTime;
    }
    else
    {
        retVal = SYSTEM_LINK_STATUS_EFAIL;
    }

    return retVal;
}

Void Chains_printLoadInfo(
    uint32_t procId,
    System_LinkStatsCorePrfObj *pCoreLoadObj)
{
    int32_t hwiLoad, swiLoad, tskLoad, lowPower;
    int32_t hndlId, cpuLoad, totalLoad = 0, otherLoad = 0;
    uint32_t maxTskHndl;
    System_LinkStatsPrfLoadObj *pHndl;
    System_LinkStatsPrfLoadObj *pPrfLoadObjStartPtr;
    uint64_t totalTime, time64, temp;

    hwiLoad = swiLoad = -1;

    if (NULL != pCoreLoadObj)
    {
        totalTime = pCoreLoadObj->accPrfLoadObj.totalTimeLo & 0xFFFFFFFFU;
        temp = pCoreLoadObj->accPrfLoadObj.totalTimeHi & 0xFFFFFFFFU;
        totalTime |= (temp << 32);

        if(SYSTEM_LINK_STATUS_SOK ==
           calcCoreLoad(pCoreLoadObj, &hwiLoad, &swiLoad, &cpuLoad, &lowPower))
        {
            Vps_printf(" LOAD: CPU: %d.%d%% HWI: %d.%d%%, SWI:%d.%d%%, Low Power:%d.%d%% \n",
                        cpuLoad/10, cpuLoad%10,
                        hwiLoad/10, hwiLoad%10,
                        swiLoad/10, swiLoad%10,
                        lowPower/10, lowPower%10);
        }
        Vps_printf(" \n");

        pPrfLoadObjStartPtr = Utils_linkStatsGetGetMaxPrfLoadInst(
            procId, &maxTskHndl);
        for (hndlId = 0; hndlId < maxTskHndl; hndlId++)
        {
            pHndl = &pPrfLoadObjStartPtr[hndlId];

            if (TRUE == pHndl->isAlloc)
            {
                tskLoad = -1;

                time64 = pHndl->totalTskThreadTimeLo & 0xFFFFFFFFU;
                temp = pHndl->totalTskThreadTimeHi & 0xFFFFFFFFU;
                time64 |= (temp << 32);
                tskLoad = (time64 * 1000) / totalTime;

                totalLoad += tskLoad;

                if (tskLoad)
                {
                    Vps_printf(" LOAD: TSK: %-20s: %d.%d%% \r\n",
                           pHndl->name,
                           tskLoad/10,
                           tskLoad%10);
                }
            }
        }

        otherLoad = cpuLoad - totalLoad;

        Vps_printf(" LOAD: TSK: %-20s: %d.%d%% \r\n",
               "MISC",
               otherLoad/10,
               otherLoad%10);
    }

    Vps_printf(" \n");
}

Void Chains_printHeapInfo(uint32_t procId, System_LinkStatsCorePrfObj *pCoreLoadObj)
{
    Utils_MemHeapStats *heapStats;

    Vps_printf
          (" SYSTEM: SW Message Box Msg Pool, Free Msg Count = %d \r\n",
                Utils_mbxGetFreeMsgCount());

    Vps_printf(" \n");

    Vps_printf
          (" SYSTEM: Sempahores Objects, %4d of %4d free \r\n",
             pCoreLoadObj->freeSemaphoreObjs, pCoreLoadObj->maxSemaphoreObjs);
    Vps_printf
          (" SYSTEM: Task Objects      , %4d of %4d free \r\n",
             pCoreLoadObj->freeTaskObjs, pCoreLoadObj->maxTaskObjs);
    Vps_printf
          (" SYSTEM: Clock Objects     , %4d of %4d free \r\n",
             pCoreLoadObj->freeClockObjs, pCoreLoadObj->maxClockObjs);
    Vps_printf
          (" SYSTEM: Hwi Objects       , %4d of %4d free \r\n",
             pCoreLoadObj->freeHwiObjs, pCoreLoadObj->maxHwiObjs);

    Vps_printf(" \n");

    heapStats = &pCoreLoadObj->heapStats[UTILS_HEAPID_L2_LOCAL];
    if(heapStats->heapSize != 0)
    {
        Vps_printf
          (" SYSTEM: Heap = %-20s @ 0x%08x, Total size = %d B (%d KB), Free size = %d B (%d KB)\r\n",
             heapStats->heapName,
             heapStats->heapAddr,
             heapStats->heapSize,
             heapStats->heapSize/KB,
             heapStats->freeSize,
             heapStats->freeSize/KB);
    }

    heapStats = &pCoreLoadObj->heapStats[UTILS_HEAPID_DDR_CACHED_LOCAL];
    Vps_printf
          (" SYSTEM: Heap = %-20s @ 0x%08x, Total size = %d B (%d KB), Free size = %d B (%d KB)\r\n",
             heapStats->heapName,
             heapStats->heapAddr,
             heapStats->heapSize,
             heapStats->heapSize/KB,
             heapStats->freeSize,
             heapStats->freeSize/KB
             );

    if(procId == SYSTEM_IPU_PROC_PRIMARY)
    {
        /* print SR heap free space info from IPU1-0 side only,
         * to avoid duplicate print's
         */

        heapStats = &pCoreLoadObj->heapStats[UTILS_HEAPID_OCMC_SR];
        Vps_printf
              (" SYSTEM: Heap = %-20s @ 0x%08x, Total size = %d B (%d KB), Free size = %d B (%d KB)\r\n",
                 heapStats->heapName,
                 heapStats->heapAddr,
                 heapStats->heapSize,
                 heapStats->heapSize/KB,
                 heapStats->freeSize,
                 heapStats->freeSize/KB
                 );

        heapStats = &pCoreLoadObj->heapStats[UTILS_HEAPID_DDR_CACHED_SR_ECC_ASIL];
        Vps_printf
              (" SYSTEM: Heap = %-20s @ 0x%08x, Total size = %d B (%d KB), Free size = %d B (%d KB)\r\n",
                 heapStats->heapName,
                 heapStats->heapAddr,
                 heapStats->heapSize,
                 heapStats->heapSize/KB,
                 heapStats->freeSize,
                 heapStats->freeSize/KB
                 );

        heapStats = &pCoreLoadObj->heapStats[UTILS_HEAPID_DDR_CACHED_SR_ECC_QM];
        Vps_printf
              (" SYSTEM: Heap = %-20s @ 0x%08x, Total size = %d B (%d KB), Free size = %d B (%d KB)\r\n",
                 heapStats->heapName,
                 heapStats->heapAddr,
                 heapStats->heapSize,
                 heapStats->heapSize/KB,
                 heapStats->freeSize,
                 heapStats->freeSize/KB
                 );

        heapStats = &pCoreLoadObj->heapStats[UTILS_HEAPID_DDR_CACHED_SR_NON_ECC_ASIL];
        Vps_printf
              (" SYSTEM: Heap = %-20s @ 0x%08x, Total size = %d B (%d KB), Free size = %d B (%d KB)\r\n",
                 heapStats->heapName,
                 heapStats->heapAddr,
                 heapStats->heapSize,
                 heapStats->heapSize/KB,
                 heapStats->freeSize,
                 heapStats->freeSize/KB
                 );

        heapStats = &pCoreLoadObj->heapStats[UTILS_HEAPID_DDR_CACHED_SR];
        Vps_printf
              (" SYSTEM: Heap = %-20s @ 0x%08x, Total size = %d B (%d MB), Free size = %d B (%d MB)\r\n",
                 heapStats->heapName,
                 heapStats->heapAddr,
                 heapStats->heapSize,
                 heapStats->heapSize/MB,
                 heapStats->freeSize,
                 heapStats->freeSize/MB
                 );

        heapStats = &pCoreLoadObj->heapStats[UTILS_HEAPID_DDR_NON_CACHED_SR0];
        Vps_printf
              (" SYSTEM: Heap = %-20s @ 0x%08x, Total size = %d B (%d MB), Free size = %d B (%d MB)\r\n",
                 heapStats->heapName,
                 heapStats->heapAddr,
                 heapStats->heapSize,
                 heapStats->heapSize/MB,
                 heapStats->freeSize,
                 heapStats->freeSize/MB
                 );
    }
}

Void Chains_linkStatsMonitorCmdHandler(uint32_t cmd, Void *pPrm)
{
    int32_t status = 0;
    int32_t hwiLoad, swiLoad, cpuLoad, lowPower;
    Utils_SystemLoadStats *pStats;
    uint32_t procId;
    uint32_t linkCmd = 0;
    System_LinkStatsCorePrfObj *pCoreLoadObj;

    if ((SYSTEM_LINK_CMD_PRINT_CORE_PRF_LOAD == cmd) ||
        (SYSTEM_LINK_CMD_GET_CORE_PRF_LOAD == cmd))
    {
        /* Send the command to copy core load to each core */
        for(procId=0; procId<SYSTEM_PROC_MAX; procId++)
        {
            if(System_isProcEnabled(procId)==FALSE)
                continue;

            pCoreLoadObj = Utils_linkStatsGetPrfLoadInst(procId);
            UTILS_assert(NULL != pCoreLoadObj);

            /* clear any pending ACK command */
            Utils_linkStatsRecvCommand(
                    &pCoreLoadObj->linkToSrcCmdObj,
                    &linkCmd);

            Utils_linkStatsSendCommand(
                &pCoreLoadObj->srcToLinkCmdObj,
                LINK_STATS_CMD_COPY_CORE_LOAD);
        }

        /* Waiting for 1 second here to make sure each core sees this command
           Assumption is that load update time interval is 500ms for
           each core. */
        Task_sleep(1000);

        /* Send the command to copy core load to each core */
        for(procId=0; procId<SYSTEM_PROC_MAX; procId++)
        {
            if(System_isProcEnabled(procId)==FALSE)
                continue;

            pCoreLoadObj = Utils_linkStatsGetPrfLoadInst(procId);
            UTILS_assert(NULL != pCoreLoadObj);

            status = Utils_linkStatsRecvCommand(
                &pCoreLoadObj->linkToSrcCmdObj,
                &linkCmd);

            if(SYSTEM_LINK_CMD_PRINT_CORE_PRF_LOAD == cmd)
            {
                Vps_printf(" \n");
                Vps_printf(" CPU [%-7s] Statistics, \n",
                    System_getProcName(procId));
                Vps_printf(" ************************* \n");

                if (0 != status)
                {
                    Vps_printf(
                        " CPU: %s: Unable to get CPU statistics !!!\n",
                        System_getProcName(procId));
                }

                Vps_printf(" \n");
                /* Command is received, so print the core and task statistics */
                if ((0 == status) &&
                    (LINK_STATS_CMD_COPY_CORE_LOAD_DONE == linkCmd))
                {
                    Chains_printLoadInfo(procId, pCoreLoadObj);
                    Chains_printHeapInfo(procId, pCoreLoadObj);
                }
                Vps_printf(" \n");
            }
            else if(SYSTEM_LINK_CMD_GET_CORE_PRF_LOAD == cmd)
            {
                pStats = (Utils_SystemLoadStats *) pPrm;
                calcCoreLoad(pCoreLoadObj, &hwiLoad, &swiLoad, &cpuLoad, &lowPower);
                if ((0 == status) &&
                    (LINK_STATS_CMD_COPY_CORE_LOAD_DONE == linkCmd))
                {
                    pStats[procId].totalLoadParams.integerValue    = cpuLoad/10;
                    pStats[procId].totalLoadParams.fractionalValue = cpuLoad%10;
                }
                else
                {
                    pStats[procId].totalLoadParams.integerValue    = 0;
                    pStats[procId].totalLoadParams.fractionalValue = 0;
                }
            }
            else
            {
                /* Empty */
            }
        }
    }
}

/**
 *******************************************************************************
 *
 * \brief Initializes the Link Stats monitor.
 *        Creates a task to print link statistics at every 60 seconds
 *        Creates a timer to give a callback at every 60seconds
 *        Initializes other parameters
 *
 * \return  returns 0 on success
 *
 *******************************************************************************
 */
int32_t Chains_linkStatsMonitorInit()
{
    SystemLink_registerHandler(Chains_linkStatsMonitorCmdHandler);
    return SYSTEM_LINK_STATUS_SOK;
}

/**
 *******************************************************************************
 *
 * \brief DeInitializes the Link Stats monitor.
 *        Deletes the task and timer
 *
 * \return  returns 0 on success
 *
 *******************************************************************************
 */
Void Chains_linkStatsMonitorDeInit()
{
    SystemLink_unregisterHandler(Chains_linkStatsMonitorCmdHandler);
}

