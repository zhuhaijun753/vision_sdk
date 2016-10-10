/*
 *******************************************************************************
 *
 * Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */
#include "chains_fastBoot_issIspSimcop_pd_Display_priv.h"
#include "chains_fastBoot_dspEvePowerDown.h"
#include <examples/tda2xx/include/chains_common.h>

/**
 *******************************************************************************
 *
 * \brief   Send commands to stop the CPU load calculation for IPU cores.
 *          This is done to have individual control on which CPU core's load
 *          can be enabled or disabled when DSP and EVEs are put to power down
 *          mode.
 *
 * \param   None
 *
 * \return  None
 *
 *******************************************************************************
*/
Void chains_fastBoot_stopIpuLoadCalculation(Void)
{
    UInt32 procId, linkId, i;
    UInt32 procIdArr[2U] = {SYSTEM_PROC_IPU1_0, SYSTEM_PROC_IPU1_1};
    /* Ensure that the CPU Load calculation for IPU is stopped */
    for (i = 0U; i < 2U; i++)
    {
        procId = procIdArr[i];
        if(System_isProcEnabled(procId))
        {
            linkId = linkId = SYSTEM_MAKE_LINK_ID(procId, SYSTEM_LINK_ID_PROCK_LINK_ID);
            System_linkControl(
                linkId,
                SYSTEM_COMMON_CMD_CPU_LOAD_CALC_STOP,
                NULL,
                0,
                TRUE
            );
            System_linkControl(
                linkId,
                SYSTEM_COMMON_CMD_CPU_LOAD_CALC_RESET,
                NULL,
                0,
                TRUE
            );
        }
    }
    return;
}

/**
 *******************************************************************************
 *
 * \brief   Send commands to stop the CPU load calculation for DSP & EVE cores.
 *          This is done to have individual control on which CPU core's load
 *          can be enabled or disabled when DSP and EVEs are put to power down
 *          mode.
 *
 * \param   None
 *
 * \return  None
 *
 *******************************************************************************
*/
Void chains_fastBoot_stopDspEveLoadCalculation(Void)
{
    UInt32 procId, linkId, i;
    UInt32 procIdArr[3U] = {SYSTEM_PROC_DSP1, SYSTEM_PROC_DSP2, SYSTEM_PROC_EVE1};
    for (i = 0U; i < 3U; i++)
    {
        procId = procIdArr[i];
        if(System_isProcEnabled(procId))
        {
            linkId = linkId = SYSTEM_MAKE_LINK_ID(procId, SYSTEM_LINK_ID_PROCK_LINK_ID);
            System_linkControl(
                linkId,
                SYSTEM_COMMON_CMD_CPU_LOAD_CALC_STOP,
                NULL,
                0,
                TRUE
            );
            System_linkControl(
                linkId,
                SYSTEM_COMMON_CMD_CPU_LOAD_CALC_RESET,
                NULL,
                0,
                TRUE
            );
        }
    }
    return;
}

/**
 *******************************************************************************
 *
 * \brief   Switch the DSP and EVE CPUs ON
 *
 *          Function first boots the DSP and EVE cores and then sends the IPC
 *          sync to the cores. The usecase is then created and started, gate
 *          links are switched on. Display link is switched to show analytics
 *          output.
 *
 * \param   pObj       [IN]   chains_fastBoot_issIspSimcop_pd_DisplayAppObj
 * \param   bootParams [IN]   Utils_BootSlaves_Params - To boot cores
 *
 *******************************************************************************
*/
Void chains_fastBoot_switchDspEveOn(chains_fastBoot_issIspSimcop_pd_DisplayAppObj *pObj,
                                              Utils_BootSlaves_Params *bootParams)
{
    DisplayLink_SwitchChannelParams displayPrm;
    UInt32 startTime, endTime, timeTaken;

    pObj->analyticsState = ANALYTICS_ON;
    startTime = Utils_getCurGlobalTimeInMsec();
    /* boot & sync other cores here */
    Utils_bootSlaves(bootParams);
    Utils_syncSlaves();

    /* create & start late usecase */
    chains_fastBoot_issIspSimcop_pd_Display_Create_UcLate(&pObj->ucObj, pObj);
    chains_fastBoot_issIspSimcop_pd_Display_StartApp_UcLate(pObj);
    /* Send command to toggle operation status */
    System_linkControl(
            pObj->ucObj.Gate_algFeatureComputeLinkID,
            GATE_LINK_CMD_SET_OPERATION_MODE_ON,
            NULL,
            0,
            TRUE);

    System_linkControl(
            pObj->ucObj.Gate_algCaptureLinkID,
            GATE_LINK_CMD_SET_OPERATION_MODE_ON,
            NULL,
            0,
            TRUE);

    System_linkControl(
            pObj->ucObj.Gate_algDrawLinkID,
            GATE_LINK_CMD_SET_OPERATION_MODE_ON,
            NULL,
            0,
            TRUE);

    displayPrm.activeChId = 1;

    System_linkControl(pObj->ucObj.Display_VideoLinkID,
                    DISPLAY_LINK_CMD_SWITCH_CH,
                    &displayPrm,
                    sizeof(displayPrm),
                    TRUE);
    endTime = Utils_getCurGlobalTimeInMsec();
    timeTaken = endTime - startTime;
    Vps_printf(" CHAINS: DSP and EVE power ON Time = %d ms\n", timeTaken);
}
/**
 *******************************************************************************
 *
 * \brief   Switch the DSP and EVE CPUs OFF
 *
 *          Function first switches off the gates, waits for the buffers to be
 *          release and then powers down the cores after detaching IPC. DSP and
 *          EVE applications are completely exitted before switching off the
 *          cores.
 *
 * \param   pObj       [IN]   chains_fastBoot_issIspSimcop_pd_DisplayAppObj
 *
 *******************************************************************************
*/
Void chains_fastBoot_switchDspEveOff(chains_fastBoot_issIspSimcop_pd_DisplayAppObj *pObj)
{
    DisplayLink_SwitchChannelParams displayPrm;
    UInt32 startTime, endTime, timeTaken;
    UInt32 numBuffersPending;

    startTime = Utils_getCurGlobalTimeInMsec();
    pObj->analyticsState = ANALYTICS_OFF;
    displayPrm.activeChId = 0;

    System_linkControl(pObj->ucObj.Display_VideoLinkID,
                    DISPLAY_LINK_CMD_SWITCH_CH,
                    &displayPrm,
                    sizeof(displayPrm),
                    TRUE);
    /* Send command to toggle operation status */
    System_linkControl(
            pObj->ucObj.Gate_algFeatureComputeLinkID,
            GATE_LINK_CMD_SET_OPERATION_MODE_OFF,
            NULL,
            0,
            TRUE);
    System_linkControl(
            pObj->ucObj.Gate_algCaptureLinkID,
            GATE_LINK_CMD_SET_OPERATION_MODE_OFF,
            NULL,
            0,
            TRUE);

    System_linkControl(
            pObj->ucObj.Gate_algDrawLinkID,
            GATE_LINK_CMD_SET_OPERATION_MODE_OFF,
            NULL,
            0,
            TRUE);
    /* Wait for the buffers in Gate links to be freed before
     * powering down DSP and EVE.
     */
    do
    {
        BspOsal_sleep(1U);
        System_linkControl(
            pObj->ucObj.Gate_algFeatureComputeLinkID,
            GATE_LINK_CMD_GET_BUFFER_FORWARD_COUNT,
            &numBuffersPending,
            sizeof(numBuffersPending),
            TRUE);
    } while (numBuffersPending != 0U);
    /* Wait for the buffers in Gate links to be freed before
     * powering down DSP and EVE.
     */
    do
    {
        BspOsal_sleep(1U);
        System_linkControl(
            pObj->ucObj.Gate_algCaptureLinkID,
            GATE_LINK_CMD_GET_BUFFER_FORWARD_COUNT,
            &numBuffersPending,
            sizeof(numBuffersPending),
            TRUE);
    } while (numBuffersPending != 0U);
    /* Wait for the buffers in Gate links to be freed before
     * powering down DSP and EVE.
     */
    do
    {
        BspOsal_sleep(1U);
        System_linkControl(
            pObj->ucObj.Gate_algDrawLinkID,
            GATE_LINK_CMD_GET_BUFFER_FORWARD_COUNT,
            &numBuffersPending,
            sizeof(numBuffersPending),
            TRUE);
    } while (numBuffersPending != 0U);
    /* Stop and delete the UC late chains */
    chains_fastBoot_issIspSimcop_pd_Display_StopAndDeleteApp_UcLate(pObj);
    /* Perform the IPC detach and shut down the cores */
    Utils_bootPowerDownSlaves();
    endTime = Utils_getCurGlobalTimeInMsec();
    timeTaken = endTime - startTime;
    Vps_printf(" CHAINS: DSP and EVE power OFF Time = %d ms\n", timeTaken);
}
