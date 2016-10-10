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
 * \file chains_common.c
 *
 * \brief  Board specific initializations through APP_CTRL link.
 *
 *         All peripherals thats are controlled from IPU like video sensors, d
 *         displays etc are controlled from A15 through APP_CTRL link commands
 *
 * \version 0.0 (Jun 2014) : [YM] First version implemented.
 *
 *******************************************************************************
*/

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */

#include <linux/examples/common/chains_common.h>

#define CAPTURE_SENSOR_WIDTH      (1280)
#define CAPTURE_SENSOR_HEIGHT     (720)
#define MAX_INPUT_STR_SIZE        (80)


/**
 *******************************************************************************
 *
 *  \brief  Common information related to all use-case
 *
 *******************************************************************************
*/
typedef struct {

    UInt32  displayCtrlLinkId;

    DisplayCtrlLink_ConfigParams    dctrlCfgPrms;
    DisplayCtrlLink_OvlyPipeParams  pipeParams[4];
    DisplayCtrlLink_OvlyParams      ovlyParams;
} Chains_CommonObj;

Chains_CommonObj gChains_commonObj;

/**
 *******************************************************************************
 *
 * \brief   Read a charater from UART or CCS console
 *
 * \return character that is read
 *
 *******************************************************************************
*/
char Chains_readChar()
{
    Int8 ch[80];

    fflush(stdin);
    fgets((char*)ch, MAX_INPUT_STR_SIZE, stdin);
    if(ch[1] != '\n' || ch[0] == '\n')
    ch[0] = '\n';

    return ch[0];
}

/**
 *******************************************************************************
 *
 * \brief   Initializes / starts peripherals remotely with APP_CTRL commands
 *
 *
 *******************************************************************************
 */
Int32 ChainsCommon_appCtrlCommonInit()
{

    Int32 status;

    status = System_linkControl(SYSTEM_LINK_ID_APP_CTRL,
                                APP_CTRL_LINK_CMD_SET_DMM_PRIORITIES,
                                NULL,
                                0,
                                TRUE);
    OSA_assert(status == SYSTEM_LINK_STATUS_SOK);

    status = System_linkControl(SYSTEM_LINK_ID_APP_CTRL,
                                APP_CTRL_LINK_CMD_STAT_COLLECTOR_INIT,
                                NULL,
                                0,
                                TRUE);
    OSA_assert(status == SYSTEM_LINK_STATUS_SOK);

    return status;
}

/**
 *******************************************************************************
 *
 * \brief   DeInitializes and stops peripheral remotely with APP_CTRL commands
 *
 *
 *******************************************************************************
 */
Int32 ChainsCommon_appCtrlCommonDeInit()
{

    Int32 status = SYSTEM_LINK_STATUS_SOK;

    status = System_linkControl(SYSTEM_LINK_ID_APP_CTRL,
                                APP_CTRL_LINK_CMD_STAT_COLLECTOR_DEINIT,
                                NULL,
                                0,
                                TRUE);
    OSA_assert(status == SYSTEM_LINK_STATUS_SOK);

    return status;
}
/**
 *******************************************************************************
 *
 * \brief   Set Capture Create Parameters for single camera capture mode
 *
 * \param   pPrm         [IN]  CaptureLink_CreateParams
 *
 *******************************************************************************
*/
Void ChainsCommon_SingleCam_SetCapturePrms(
                        CaptureLink_CreateParams *pPrm,
                        UInt32 captureInWidth,
                        UInt32 captureInHeight,
                        UInt32 captureOutWidth,
                        UInt32 captureOutHeight,
                        Chains_CaptureSrc captureSrc
                        )
{
    UInt32 i, streamId;

    CaptureLink_VipInstParams *pInstPrm;
    CaptureLink_InParams *pInprms;
    CaptureLink_OutParams *pOutprms;
    CaptureLink_VipScParams *pScPrms;
    CaptureLink_VipPortConfig    *pPortCfg;

    memset(pPrm, 0, sizeof(*pPrm));
    /* For SCV usecase number of camera is always 1 */
    pPrm->numVipInst = 1;
    pPrm->numDssWbInst = 0;

    for (i=0; i<SYSTEM_CAPTURE_VIP_INST_MAX; i++)
    {
        pInstPrm = &pPrm->vipInst[i];
        pInprms = &pInstPrm->inParams;
        pInstPrm->vipInstId     =   i;
#ifdef TDA2EX_BUILD
        //This change is done for supporting tda2ex
        pInstPrm->vipInstId     =   SYSTEM_CAPTURE_INST_VIP1_SLICE2_PORTA;
#endif
        if(captureSrc == CHAINS_CAPTURE_SRC_OV10635)
        {


            pInstPrm->videoIfMode   =   SYSTEM_VIFM_SCH_DS_HSYNC_VSYNC;
            pInstPrm->videoIfWidth  =   SYSTEM_VIFW_8BIT;
            pInstPrm->bufCaptMode   =   SYSTEM_CAPT_BCM_FRM_DROP;
            pInstPrm->numStream     =   1;

            pInprms->width      =   captureInWidth;
            pInprms->height     =   captureInHeight;
            pInprms->dataFormat =   SYSTEM_DF_YUV422P;
            pInprms->scanFormat =   SYSTEM_SF_PROGRESSIVE;

        }
        else if (captureSrc == CHAINS_CAPTURE_SRC_HDMI_720P)
        {


            pInstPrm->videoIfMode   =   SYSTEM_VIFM_SCH_DS_AVID_VSYNC;
            pInstPrm->videoIfWidth  =   SYSTEM_VIFW_16BIT;
            pInstPrm->bufCaptMode   =   SYSTEM_CAPT_BCM_FRM_DROP;
            pInstPrm->numStream     =   1;

            pInprms->width      =   1280;
            pInprms->height     =    720;
            pInprms->dataFormat =   SYSTEM_DF_YUV422P;
            pInprms->scanFormat =   SYSTEM_SF_PROGRESSIVE;

        }
        else if (captureSrc == CHAINS_CAPTURE_SRC_HDMI_1080P)
        {


            pInstPrm->videoIfMode   =   SYSTEM_VIFM_SCH_DS_AVID_VSYNC;
            pInstPrm->videoIfWidth  =   SYSTEM_VIFW_16BIT;
            pInstPrm->bufCaptMode   =   SYSTEM_CAPT_BCM_FRM_DROP;
            pInstPrm->numStream     =   1;

            pInprms->width      =   1920;
            pInprms->height     =   1080;
            pInprms->dataFormat =   SYSTEM_DF_YUV422P;
            pInprms->scanFormat =   SYSTEM_SF_PROGRESSIVE;
        }
        else if(captureSrc == CHAINS_CAPTURE_SRC_AR0132RCCC)
        {
            pInstPrm->vipInstId     =   SYSTEM_CAPTURE_INST_VIP3_SLICE1_PORTA;
            pInstPrm->videoIfMode   =   SYSTEM_VIFM_SCH_DS_HSYNC_VSYNC;
            pInstPrm->videoIfWidth  =   SYSTEM_VIFW_16BIT;
            pInstPrm->bufCaptMode   =   SYSTEM_CAPT_BCM_FRM_DROP;
            pInstPrm->numStream     =   1;

            pInprms->width      =   captureInWidth;
            pInprms->height     =   captureInHeight;
            pInprms->dataFormat =   SYSTEM_DF_YUV422I_YUYV;
            pInprms->scanFormat =   SYSTEM_SF_PROGRESSIVE;
        }
        else
        {
            /* Nothing here. To avoid MISRA C warnings */
        }

        for (streamId = 0; streamId < CAPTURE_LINK_MAX_OUTPUT_PER_INST;
                streamId++)
        {
            pOutprms = &pInstPrm->outParams[streamId];
            pOutprms->width         =   captureOutWidth;
            pOutprms->height        =   captureOutHeight;
            pOutprms->dataFormat    =   SYSTEM_DF_YUV420SP_UV;
            pOutprms->maxWidth      =   pOutprms->width;
            pOutprms->maxHeight     =   pOutprms->height;
            if((pInprms->width != pOutprms->width) ||
                (pInprms->height != pOutprms->height))
            {
                pOutprms->scEnable      =   TRUE;
            }
            else
            {
                pOutprms->scEnable      =   FALSE;
            }
            pOutprms->subFrmPrms.subFrameEnable = FALSE;
            pOutprms->subFrmPrms.numLinesPerSubFrame = 0;

            if(captureSrc == CHAINS_CAPTURE_SRC_AR0132RCCC)
            {
                pOutprms->dataFormat    =   SYSTEM_DF_YUV422I_UYVY;
            }
        }
        pScPrms = &pInstPrm->scPrms;
        pScPrms->inCropCfg.cropStartX = 0;
        pScPrms->inCropCfg.cropStartY = 0;
        pScPrms->inCropCfg.cropWidth = pInprms->width;
        pScPrms->inCropCfg.cropHeight = pInprms->height;

        pScPrms->scCfg.bypass       = FALSE;
        pScPrms->scCfg.nonLinear    = FALSE;
        pScPrms->scCfg.stripSize    = 0;

        pScPrms->userCoeff = FALSE;

        /* pScPrms->scCoeffCfg is not reuquired in case
         * pScPrms->userCoeff is FALSE
         */
        pPortCfg = &pInstPrm->vipPortCfg;
        pPortCfg->syncType          =   SYSTEM_VIP_SYNC_TYPE_DIS_SINGLE_YUV;
        pPortCfg->ancCropEnable     =   FALSE;



        pPortCfg->intfCfg.clipActive    =   FALSE;
        pPortCfg->intfCfg.clipBlank     =   FALSE;
        pPortCfg->intfCfg.intfWidth     =   SYSTEM_VIFW_16BIT;

        pPortCfg->disCfg.fidSkewPostCnt     =   0;
        pPortCfg->disCfg.fidSkewPreCnt      =   0;
        pPortCfg->disCfg.lineCaptureStyle   =
                                SYSTEM_VIP_LINE_CAPTURE_STYLE_ACTVID;
        pPortCfg->disCfg.fidDetectMode      =   SYSTEM_VIP_FID_DETECT_MODE_PIN;
        pPortCfg->disCfg.actvidPol          =   SYSTEM_POL_HIGH;
        pPortCfg->disCfg.vsyncPol           =   SYSTEM_POL_HIGH;
        pPortCfg->disCfg.hsyncPol           =   SYSTEM_POL_HIGH;
        pPortCfg->disCfg.discreteBasicMode  =   TRUE;

        pPortCfg->comCfg.ctrlChanSel        =   SYSTEM_VIP_CTRL_CHAN_SEL_7_0;
        pPortCfg->comCfg.ancChSel8b         =
                                SYSTEM_VIP_ANC_CH_SEL_8B_LUMA_SIDE;
        pPortCfg->comCfg.pixClkEdgePol      =   SYSTEM_EDGE_POL_RISING;
        pPortCfg->comCfg.invertFidPol       =   FALSE;
        pPortCfg->comCfg.enablePort         =   FALSE;
        pPortCfg->comCfg.expectedNumLines   =   pInprms->height;
        pPortCfg->comCfg.expectedNumPix     =   pInprms->width;
        pPortCfg->comCfg.repackerMode       =   SYSTEM_VIP_REPACK_CBA_TO_CBA;
        pPortCfg->actCropEnable             =   TRUE;

        if ((captureSrc == CHAINS_CAPTURE_SRC_HDMI_720P) ||
            (captureSrc == CHAINS_CAPTURE_SRC_HDMI_1080P))
        {
            pPortCfg->actCropEnable         =   FALSE;
        }

        pPortCfg->actCropCfg.srcNum                     =   0;
        pPortCfg->actCropCfg.cropCfg.cropStartX         =   0;
        pPortCfg->actCropCfg.cropCfg.cropStartY         =   0;
        pPortCfg->actCropCfg.cropCfg.cropWidth          =   pInprms->width;
        pPortCfg->actCropCfg.cropCfg.cropHeight         =   pInprms->height;

        pPortCfg->ancCropCfg.srcNum                     =   0;
        pPortCfg->ancCropCfg.cropCfg.cropStartX         =   0;
        pPortCfg->ancCropCfg.cropCfg.cropStartY         =   0;
        pPortCfg->ancCropCfg.cropCfg.cropWidth          =   0;
        pPortCfg->ancCropCfg.cropCfg.cropHeight         =   0;

        if(captureSrc == CHAINS_CAPTURE_SRC_AR0132RCCC)
        {
            pPortCfg->disCfg.lineCaptureStyle   =   SYSTEM_VIP_LINE_CAPTURE_STYLE_HSYNC;
            pPortCfg->disCfg.fidDetectMode      =   SYSTEM_VIP_FID_DETECT_MODE_VSYNC;
            pPortCfg->disCfg.actvidPol          =   SYSTEM_POL_LOW;
            pPortCfg->disCfg.vsyncPol           =   SYSTEM_POL_LOW;
            pPortCfg->disCfg.hsyncPol           =   SYSTEM_POL_HIGH;
            pPortCfg->comCfg.ancChSel8b         =   SYSTEM_VIP_ANC_CH_SEL_DONT_CARE;
            pPortCfg->comCfg.pixClkEdgePol      =   SYSTEM_EDGE_POL_FALLING;
        }

        pInstPrm->numBufs = CAPTURE_LINK_NUM_BUFS_PER_CH_DEFAULT + 2;
    }
}

/**
 *******************************************************************************
 *
 * \brief   Set Capture Create Parameters
 *
 *          This function is used to set the capture params.
 *          It is called in Create function. It is advisable to have
 *          Chains_LvdsVipMultiCameraSgxDisplay_ResetLinkPrm prior to set params
 *          so all the default params get set.
 *          Capture Link Input,Output, Vip parameters are set here.
 *
 * \param   pPrm      [OUT]  CaptureLink_CreateParams
 * \param   portId    [IN]   VIP ports which needs to be configured
 *
 *******************************************************************************
*/
Void ChainsCommon_MultiCam_SetCapturePrms(
                        CaptureLink_CreateParams *pPrm,
                        UInt32 numLvdsCh)
{
    UInt32 i, streamId;

    CaptureLink_VipInstParams *pInstPrm;
    CaptureLink_InParams *pInprms;
    CaptureLink_OutParams *pOutprms;
    CaptureLink_VipScParams *pScPrm;
    CaptureLink_VipPortConfig    *pPortCfg;
    UInt32 portId[10];


    memset(pPrm, 0, sizeof(*pPrm));

    pPrm->numVipInst = numLvdsCh;
    pPrm->numDssWbInst = 0;

    portId[0] = SYSTEM_CAPTURE_INST_VIP1_SLICE1_PORTA;
    portId[1] = SYSTEM_CAPTURE_INST_VIP1_SLICE2_PORTA;
    portId[2] = SYSTEM_CAPTURE_INST_VIP2_SLICE1_PORTA;
    portId[3] = SYSTEM_CAPTURE_INST_VIP3_SLICE1_PORTA;
    portId[4] = SYSTEM_CAPTURE_INST_VIP3_SLICE2_PORTA;
    portId[5] = SYSTEM_CAPTURE_INST_VIP2_SLICE2_PORTB;

#ifdef TDA2EX_BUILD
    portId[0] = SYSTEM_CAPTURE_INST_VIP1_SLICE2_PORTA;
    portId[1] = SYSTEM_CAPTURE_INST_VIP1_SLICE1_PORTA;
    portId[2] = SYSTEM_CAPTURE_INST_VIP1_SLICE2_PORTB;
    portId[3] = SYSTEM_CAPTURE_INST_VIP1_SLICE1_PORTB;
#endif

    for (i=0; i<SYSTEM_CAPTURE_VIP_INST_MAX; i++)
    {
        pInstPrm = &pPrm->vipInst[i];
        pInstPrm->vipInstId     =   portId[i];
        pInstPrm->videoIfMode   =   SYSTEM_VIFM_SCH_DS_HSYNC_VSYNC;
        pInstPrm->videoIfWidth  =   SYSTEM_VIFW_8BIT;
        pInstPrm->bufCaptMode   =   SYSTEM_CAPT_BCM_FRM_DROP;
        pInstPrm->numStream     =   1;

        pInprms = &pInstPrm->inParams;

        pInprms->width      =   CAPTURE_SENSOR_WIDTH;
        pInprms->height     =   CAPTURE_SENSOR_HEIGHT;
        pInprms->dataFormat =   SYSTEM_DF_YUV422P;
        pInprms->scanFormat =   SYSTEM_SF_PROGRESSIVE;

        for (streamId = 0; streamId < CAPTURE_LINK_MAX_OUTPUT_PER_INST;
                streamId++)
        {
            pOutprms = &pInstPrm->outParams[streamId];
            pOutprms->width         =   pInprms->width;
            pOutprms->height        =   pInprms->height;
            pOutprms->dataFormat    =   SYSTEM_DF_YUV420SP_UV;
            pOutprms->maxWidth      =   pOutprms->width;
            pOutprms->maxHeight     =   pOutprms->height;
            pOutprms->scEnable      =   FALSE;

            /* sub-frame not supported, set to FALSE */
            pOutprms->subFrmPrms.subFrameEnable = FALSE;
            pOutprms->subFrmPrms.numLinesPerSubFrame = 0;

        }
        pScPrm = &pInstPrm->scPrms;
        pScPrm->inCropCfg.cropStartX = 0;
        pScPrm->inCropCfg.cropStartY = 0;
        pScPrm->inCropCfg.cropWidth = pInprms->width;
        pScPrm->inCropCfg.cropHeight = pInprms->height;

        pScPrm->scCfg.bypass       = FALSE;
        pScPrm->scCfg.nonLinear    = FALSE;
        pScPrm->scCfg.stripSize    = 0;

        pScPrm->userCoeff = FALSE;

        /* pScPrm->scCoeffCfg is not reuquired in case
         * pScPrm->userCoeff is FALSE
         */
        pPortCfg = &pInstPrm->vipPortCfg;
        pPortCfg->syncType          =   SYSTEM_VIP_SYNC_TYPE_DIS_SINGLE_YUV;
        pPortCfg->ancCropEnable     =   FALSE;

        pPortCfg->intfCfg.clipActive    =   FALSE;
        pPortCfg->intfCfg.clipBlank     =   FALSE;
        pPortCfg->intfCfg.intfWidth     =   SYSTEM_VIFW_16BIT;

        pPortCfg->disCfg.fidSkewPostCnt     =   0;
        pPortCfg->disCfg.fidSkewPreCnt      =   0;
        pPortCfg->disCfg.lineCaptureStyle   =
                                SYSTEM_VIP_LINE_CAPTURE_STYLE_ACTVID;
        pPortCfg->disCfg.fidDetectMode      =   SYSTEM_VIP_FID_DETECT_MODE_PIN;
        pPortCfg->disCfg.actvidPol          =   SYSTEM_POL_HIGH;
        pPortCfg->disCfg.vsyncPol           =   SYSTEM_POL_HIGH;
        pPortCfg->disCfg.hsyncPol           =   SYSTEM_POL_HIGH;
        pPortCfg->disCfg.discreteBasicMode  =   TRUE;

        pPortCfg->comCfg.ctrlChanSel        =   SYSTEM_VIP_CTRL_CHAN_SEL_7_0;
        pPortCfg->comCfg.ancChSel8b         =
                            SYSTEM_VIP_ANC_CH_SEL_8B_LUMA_SIDE;
        pPortCfg->comCfg.pixClkEdgePol      =   SYSTEM_EDGE_POL_RISING;
        pPortCfg->comCfg.invertFidPol       =   FALSE;
        pPortCfg->comCfg.enablePort         =   FALSE;
        pPortCfg->comCfg.expectedNumLines   =   pInprms->height;
        pPortCfg->comCfg.expectedNumPix     =   pInprms->width;
        pPortCfg->comCfg.repackerMode       =   SYSTEM_VIP_REPACK_CBA_TO_CBA;

        pPortCfg->actCropEnable                         =   TRUE;
        pPortCfg->actCropCfg.srcNum                     =   0;
        pPortCfg->actCropCfg.cropCfg.cropStartX         =   0;
        pPortCfg->actCropCfg.cropCfg.cropStartY         =   0;
        pPortCfg->actCropCfg.cropCfg.cropWidth          =   pInprms->width;
        pPortCfg->actCropCfg.cropCfg.cropHeight         =   pInprms->height;

        pPortCfg->ancCropCfg.srcNum                     =   0;
        pPortCfg->ancCropCfg.cropCfg.cropStartX         =   0;
        pPortCfg->ancCropCfg.cropCfg.cropStartY         =   0;
        pPortCfg->ancCropCfg.cropCfg.cropWidth          =   0;
        pPortCfg->ancCropCfg.cropCfg.cropHeight         =   0;

        pInstPrm->numBufs = CAPTURE_LINK_NUM_BUFS_PER_CH_DEFAULT + 2;
    }
}

/**
 *******************************************************************************
 *
 * \brief   Load Calculation enable/disable
 *
 *          This functions enables load profiling. A control command
 *          SYSTEM_COMMON_CMD_CPU_LOAD_CALC_START is passed to chianed links.
 *          If parameter Enable is set true Load profiling is enabled.
 *          If printStatus is set true a System CMD to Print CPU load,
 *          Task Laod and Heap status information is sent
 *          While creating enable = TRUE , printStatus & printTskLoad = FALSE
 *          While deleting enable = FALSE , printStatus & printTskLoad = TRUE
 *
 * \param   enable               [IN]   is set true Load profiling
 *
 * \param   printStatus          [IN] true a System CMD
 *
 * \param   printTskLoad         [IN]  true a Print CPU load
 *
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *******************************************************************************
*/
Int32 ChainsCommon_prfLoadCalcEnable(Bool enable, Bool printStatus, Bool printTskLoad)
{
    UInt32 procId, linkId;


    for(procId=0; procId<SYSTEM_PROC_MAX; procId++)
    {
        if(    System_isProcEnabled(procId)==FALSE
            || procId == System_getSelfProcId()
           )
        {
            continue;
        }

        linkId = SYSTEM_MAKE_LINK_ID(procId, SYSTEM_LINK_ID_PROCK_LINK_ID);

        if(enable)
        {
            System_linkControl(
                linkId,
                SYSTEM_COMMON_CMD_CPU_LOAD_CALC_START,
                NULL,
                0,
                TRUE
            );
        }
        else
        {
            System_linkControl(
                linkId,
                SYSTEM_COMMON_CMD_CPU_LOAD_CALC_STOP,
                NULL,
                0,
                TRUE
            );
            if(printStatus)
            {
                SystemCommon_PrintStatus printStatus;

                memset(&printStatus, 0, sizeof(printStatus));

                printStatus.printCpuLoad = TRUE;
                printStatus.printTskLoad = printTskLoad;
                System_linkControl(
                    linkId,
                    SYSTEM_COMMON_CMD_PRINT_STATUS,
                    &printStatus,
                    sizeof(printStatus),
                    TRUE
                );
                OSA_waitMsecs(100);
            }
            System_linkControl(
                linkId,
                SYSTEM_COMMON_CMD_CPU_LOAD_CALC_RESET,
                NULL,
                0,
                TRUE
            );
        }
    }

    return SYSTEM_LINK_STATUS_SOK;
}

/**
 *******************************************************************************
 *
 * \brief   Print Load Calculation.
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *******************************************************************************
*/
Int32 ChainsCommon_prfCpuLoadPrint()
{
    UInt32 procId, linkId;
    SystemCommon_PrintStatus printStatus;

    memset(&printStatus, 0, sizeof(printStatus));

    printStatus.printCpuLoad = TRUE;
    printStatus.printTskLoad = TRUE;

    for(procId=0; procId<SYSTEM_PROC_MAX; procId++)
    {
        if(    System_isProcEnabled(procId)==FALSE
            || procId == System_getSelfProcId()
           )
        {
            continue;
        }

        linkId = SYSTEM_MAKE_LINK_ID(procId, SYSTEM_LINK_ID_PROCK_LINK_ID);

        System_linkControl(
            linkId,
            SYSTEM_COMMON_CMD_PRINT_STATUS,
            &printStatus,
            sizeof(printStatus),
            TRUE
        );
        OSA_waitMsecs(100);
    }

    return SYSTEM_LINK_STATUS_SOK;
}

/**
 *******************************************************************************
 *
 * \brief   Print Memory Heap Statistics
 *
 *          This function send a system control message
 *          SYSTEM_COMMON_CMD_CPU_LOAD_CALC_START to all cores.
 *
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *******************************************************************************
*/
Int32 ChainsCommon_memPrintHeapStatus()
{
    UInt32 procId, linkId;
    SystemCommon_PrintStatus printStatus;

    memset(&printStatus, 0, sizeof(printStatus));

    printStatus.printHeapStatus = TRUE;

    for(procId=0; procId<SYSTEM_PROC_MAX; procId++)
    {
        if(    System_isProcEnabled(procId)==FALSE
            || procId == System_getSelfProcId()
           )
        {
            continue;
        }

        linkId = SYSTEM_MAKE_LINK_ID(procId, SYSTEM_LINK_ID_PROCK_LINK_ID);

        System_linkControl(
                linkId,
                SYSTEM_COMMON_CMD_PRINT_STATUS,
                &printStatus,
                sizeof(printStatus),
                TRUE
            );
    }

    return SYSTEM_LINK_STATUS_SOK;
}

/**
 *******************************************************************************
 *
 * \brief   This function Print the statCollector output
 *          SYSTEM_COMMON_CMD_PRINT_STAT_COLL send to only IPU1_0 core.
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *******************************************************************************
*/
Int32 ChainsCommon_statCollectorPrint()
{
    UInt32 linkId;

    linkId = IPU1_0_LINK(SYSTEM_LINK_ID_PROCK_LINK_ID);

        System_linkControl(
            linkId,
            SYSTEM_COMMON_CMD_PRINT_STAT_COLL,
            NULL,
            0,
            TRUE
        );
        OSA_waitMsecs(100);


    return SYSTEM_LINK_STATUS_SOK;
}

/**
 *******************************************************************************
 *
 * \brief   This function Reset the statCollector registers
 *          SYSTEM_COMMON_CMD_RESET_STAT_COLL send to only IPU1_0 core.
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *******************************************************************************
*/
Int32 ChainsCommon_statCollectorReset()
{
    UInt32 linkId;

    linkId = IPU1_0_LINK(SYSTEM_LINK_ID_PROCK_LINK_ID);

        System_linkControl(
            linkId,
            SYSTEM_COMMON_CMD_RESET_STAT_COLL,
            NULL,
            0,
            TRUE
        );
        OSA_waitMsecs(100);

    return SYSTEM_LINK_STATUS_SOK;
}

/**
 *******************************************************************************
 *
 * \brief   Show CPU status from remote CPUs
 *
 *******************************************************************************
 */
void ChainsCommon_printCpuStatus()
{
    if(System_isProcEnabled(SYSTEM_IPU_PROC_PRIMARY))
    {
        System_linkControl(
            SYSTEM_LINK_ID_IPU1_0,
            SYSTEM_COMMON_CMD_CORE_STATUS,
            NULL,
            0,
            TRUE
        );
    }
    if(System_isProcEnabled(SYSTEM_PROC_DSP1))
    {
        System_linkControl(
            SYSTEM_LINK_ID_DSP1,
            SYSTEM_COMMON_CMD_CORE_STATUS,
            NULL,
            0,
            TRUE
        );
    }
    if(System_isProcEnabled(SYSTEM_PROC_DSP2))
    {
        System_linkControl(
            SYSTEM_LINK_ID_DSP2,
            SYSTEM_COMMON_CMD_CORE_STATUS,
            NULL,
            0,
            TRUE
        );
    }
    if(System_isProcEnabled(SYSTEM_PROC_EVE1))
    {
        System_linkControl(
            SYSTEM_LINK_ID_EVE1,
            SYSTEM_COMMON_CMD_CORE_STATUS,
            NULL,
            0,
            TRUE
        );
    }
    if(System_isProcEnabled(SYSTEM_PROC_EVE2))
    {
        System_linkControl(
            SYSTEM_LINK_ID_EVE2,
            SYSTEM_COMMON_CMD_CORE_STATUS,
            NULL,
            0,
            TRUE
        );
    }
    if(System_isProcEnabled(SYSTEM_PROC_EVE3))
    {
        System_linkControl(
            SYSTEM_LINK_ID_EVE3,
            SYSTEM_COMMON_CMD_CORE_STATUS,
            NULL,
            0,
            TRUE
        );
    }
    if(System_isProcEnabled(SYSTEM_PROC_EVE4))
    {
        System_linkControl(
            SYSTEM_LINK_ID_EVE4,
            SYSTEM_COMMON_CMD_CORE_STATUS,
            NULL,
            0,
            TRUE
        );
    }
}

/**
 *******************************************************************************
 *
 * \brief   Return W x H of a given display type
 *
 *******************************************************************************
*/
Void ChainsCommon_GetDisplayWidthHeight(
        Chains_DisplayType displayType,
        UInt32 *displayWidth,
        UInt32 *displayHeight)
{
    switch(displayType)
    {
        case CHAINS_DISPLAY_TYPE_LCD_7_INCH:
            *displayWidth = 800;
            *displayHeight = 480;
            break;
        case CHAINS_DISPLAY_TYPE_LCD_10_INCH:
            *displayWidth = 1280;
            *displayHeight = 800;
            break;
        case CHAINS_DISPLAY_TYPE_LDC_10_INCH_LCDCTRL_TC358778_MIPI_DSI_1920_1200:
            *displayWidth = 1920;
            *displayHeight = 1200;
            break;
        case CHAINS_DISPLAY_TYPE_HDMI_720P:
            *displayWidth = 1280;
            *displayHeight = 720;
            break;
        case CHAINS_DISPLAY_TYPE_HDMI_1080P:
            *displayWidth = 1920;
            *displayHeight = 1080;
            break;
        case CHAINS_DISPLAY_TYPE_FPD:
            *displayWidth = 1280;
            *displayHeight = 720;
            break;
        default:
            OSA_assert(0);
            break;
    }
}

/**
 *******************************************************************************
 *
 * \brief   Setups Display Controller to dual display on On-chip HDMI and
 *          LCD
 *
 *          Configure such that VID2 and VID3 pipe goto LCD
 *          And VID1 and GRPX pipe goes to HDMI
 *
 *          HDMI is hardcoded to 1080p60 resolution
 *          LCD is hardcoded to 800x480 resolution
 *
 *          NOTE, this is just a sample config use by example use-cases
 *          User's can configure it different in their use-case
 *
 *******************************************************************************
*/


Int32 ChainsCommon_DualDisplay_StartDisplayCtrl(
Chains_DisplayType lcdType, UInt32 displayLCDWidth, UInt32 displayLCDHeight)
{
    Int32 status;
    DisplayCtrlLink_ConfigParams *pPrm = &gChains_commonObj.dctrlCfgPrms;
    DisplayCtrlLink_VencInfo *pVInfo;
    DisplayCtrlLink_OvlyParams *pOvlyPrms;
    DisplayCtrlLink_OvlyPipeParams *pPipeOvlyPrms;
    DisplayCtrlLink_OvlyParams ovlyPrms[2];

    /* Set the link id */
    gChains_commonObj.displayCtrlLinkId = SYSTEM_LINK_ID_DISPLAYCTRL;

    /* Number of valid entries in vencInfo array */
    pPrm->numVencs = 2;
    /* Bitmask of tied vencs. Two vencs, which uses same pixel clock and whose vsync are synchronized, can be tied together. */
    pPrm->tiedVencs = 0;
    /* Activate the HDMI BSP layer in the Dctrl link. This is not required if there is no HDMI display in use. */
    pPrm->deviceId = DISPLAYCTRL_LINK_USE_HDMI;

    /* Configure HDMI display */
    pVInfo                                  = &pPrm->vencInfo[0];
    pVInfo->vencId                          = SYSTEM_DCTRL_DSS_VENC_HDMI;
    pVInfo->outputPort                      = SYSTEM_DCTRL_DSS_HDMI_OUTPUT;
    pVInfo->vencOutputInfo.vsPolarity       = SYSTEM_DCTRL_POLARITY_ACT_HIGH;
    pVInfo->vencOutputInfo.hsPolarity       = SYSTEM_DCTRL_POLARITY_ACT_HIGH;

    /* Below are of dont care for EVM LCD */
    pVInfo->vencOutputInfo.fidPolarity      = SYSTEM_DCTRL_POLARITY_ACT_LOW;
    pVInfo->vencOutputInfo.actVidPolarity   = SYSTEM_DCTRL_POLARITY_ACT_HIGH;

    /* We use 1080p60 - set to SYSTEM_STD_720P_60 for 720p! */
    pVInfo->mInfo.standard                  = SYSTEM_STD_1080P_60;

    /* Configure HDMI overlay parameters */
    pVInfo->mode                            = 0;
    pVInfo->isInputPipeConnected[0]         = TRUE;
    pVInfo->isInputPipeConnected[1]         = FALSE;
    pVInfo->isInputPipeConnected[2]         = FALSE;
    pVInfo->isInputPipeConnected[3]         = TRUE;
    pVInfo->writeBackEnabledFlag            = FALSE;

    pVInfo->vencOutputInfo.dataFormat       = SYSTEM_DF_RGB24_888;
    pVInfo->vencOutputInfo.dvoFormat        = SYSTEM_DCTRL_DVOFMT_GENERIC_DISCSYNC;
    pVInfo->vencOutputInfo.videoIfWidth     = SYSTEM_VIFW_24BIT;

    pVInfo->vencOutputInfo.pixelClkPolarity = SYSTEM_DCTRL_POLARITY_ACT_HIGH;
    pVInfo->vencOutputInfo.aFmt             = SYSTEM_DCTRL_A_OUTPUT_MAX;

    pOvlyPrms                               = &ovlyPrms[0];
    pOvlyPrms->vencId                       = pVInfo->vencId;
    pOvlyPrms->deltaLinesPerPanel           = 0;
    pOvlyPrms->alphaBlenderEnable           = 0;
    pOvlyPrms->backGroundColor              = 0x10;
    pOvlyPrms->colorKeyEnable               = 1;
    pOvlyPrms->colorKeySel                  = SYSTEM_DSS_DISPC_TRANS_COLOR_KEY_SRC;
    pOvlyPrms->ovlyOptimization             = SYSTEM_DSS_DISPC_OVLY_FETCH_ALLDATA;
    pOvlyPrms->transColorKey                = DRAW2D_TRANSPARENT_COLOR;

/* Configure LCD */
    pVInfo                                  = &pPrm->vencInfo[1];
    pVInfo->vencId                          = SYSTEM_DCTRL_DSS_VENC_LCD1;
    pVInfo->outputPort                      = SYSTEM_DCTRL_DSS_DPI1_OUTPUT;
    pVInfo->vencOutputInfo.vsPolarity       = SYSTEM_DCTRL_POLARITY_ACT_LOW;
    pVInfo->vencOutputInfo.hsPolarity       = SYSTEM_DCTRL_POLARITY_ACT_LOW;

    /* Below are of dont care for EVM LCD */
    pVInfo->vencOutputInfo.fidPolarity      = SYSTEM_DCTRL_POLARITY_ACT_HIGH;
    pVInfo->vencOutputInfo.actVidPolarity   = SYSTEM_DCTRL_POLARITY_ACT_HIGH;

    pVInfo->mInfo.standard                  = SYSTEM_STD_CUSTOM;
    pVInfo->mInfo.scanFormat                = SYSTEM_SF_PROGRESSIVE;
    pVInfo->mInfo.width                     = displayLCDWidth;
    pVInfo->mInfo.height                    = displayLCDHeight;

    if(lcdType == CHAINS_DISPLAY_TYPE_LCD_7_INCH)
    {
    pVInfo->mInfo.pixelClock                = 29232u;
    pVInfo->mInfo.fps                       = 60U;
    pVInfo->mInfo.hFrontPorch               = 40u;
    pVInfo->mInfo.hBackPorch                = 40u;
    pVInfo->mInfo.hSyncLen                  = 48u;
    pVInfo->mInfo.vFrontPorch               = 13u;
    pVInfo->mInfo.vBackPorch                = 29u;
    pVInfo->mInfo.vSyncLen                  = 3u;
    pVInfo->vencDivisorInfo.divisorLCD      = 1;
    pVInfo->vencDivisorInfo.divisorPCD      = 4;
    }
    else if(lcdType == CHAINS_DISPLAY_TYPE_LCD_10_INCH)
    {
        pVInfo->mInfo.pixelClock                =   74500U;
        pVInfo->mInfo.fps                       =   60U;
        pVInfo->mInfo.hBackPorch                =   80U;
        pVInfo->mInfo.hSyncLen                  =   62U;
        pVInfo->mInfo.hFrontPorch               =   48U;
        pVInfo->mInfo.vBackPorch                =   12U;
        pVInfo->mInfo.vSyncLen                  =   35U;
        pVInfo->mInfo.vFrontPorch               =   6U;
        pVInfo->vencDivisorInfo.divisorLCD      =   1;
        pVInfo->vencDivisorInfo.divisorPCD      =   1;
    }
    else if (lcdType ==
    CHAINS_DISPLAY_TYPE_LDC_10_INCH_LCDCTRL_TC358778_MIPI_DSI_1920_1200)
    {
        pVInfo->mInfo.pixelClock                =   147000U;
        pVInfo->mInfo.fps                       =   60U;
        pVInfo->mInfo.hBackPorch                =   30U;
        pVInfo->mInfo.hSyncLen                  =   16U;
        pVInfo->mInfo.hFrontPorch               =   32U;
        pVInfo->mInfo.vBackPorch                =   16U;
        pVInfo->mInfo.vSyncLen                  =   2U;
        pVInfo->mInfo.vFrontPorch               =   7U;
        pVInfo->vencDivisorInfo.divisorLCD      =   1;
        pVInfo->vencDivisorInfo.divisorPCD      =   1;
    }
    else if (lcdType == CHAINS_DISPLAY_TYPE_FPD)
    {
        pPrm->deviceId = DISPLAYCTRL_LINK_USE_LCD;
        pVInfo->vencId = SYSTEM_DCTRL_DSS_VENC_LCD3;
        pVInfo->outputPort = SYSTEM_DCTRL_DSS_DPI3_OUTPUT;
        pVInfo->mInfo.width                     =   displayLCDWidth;
        pVInfo->mInfo.height                    =   displayLCDHeight;
        pVInfo->mInfo.pixelClock                =   69300; /* modified as per panel-tlc59108.c form linux*/
        pVInfo->mInfo.fps                       =   60U;
        pVInfo->mInfo.hBackPorch                =   44U;
        pVInfo->mInfo.hSyncLen                  =   32U;
        pVInfo->mInfo.hFrontPorch               =   48U;
        pVInfo->mInfo.vBackPorch                =   12U;
        pVInfo->mInfo.vSyncLen                  =   7U;
        pVInfo->mInfo.vFrontPorch               =   4U;
        pVInfo->vencDivisorInfo.divisorLCD      =   1;
        pVInfo->vencDivisorInfo.divisorPCD      =   1;
        pVInfo->vencOutputInfo.vsPolarity       =   SYSTEM_DCTRL_POLARITY_ACT_LOW;
        pVInfo->vencOutputInfo.hsPolarity       =   SYSTEM_DCTRL_POLARITY_ACT_LOW;

        /* Below are of dont care for EVM LCD */
        pVInfo->vencOutputInfo.fidPolarity      =   SYSTEM_DCTRL_POLARITY_ACT_LOW;
        pVInfo->vencOutputInfo.actVidPolarity   =   SYSTEM_DCTRL_POLARITY_ACT_HIGH;

        pVInfo->vencOutputInfo.dataFormat       =   SYSTEM_DF_RGB24_888;
        pVInfo->vencOutputInfo.dvoFormat        =
                                    SYSTEM_DCTRL_DVOFMT_GENERIC_DISCSYNC;
        pVInfo->vencOutputInfo.videoIfWidth     =   SYSTEM_VIFW_24BIT;

        pVInfo->vencOutputInfo.pixelClkPolarity =   SYSTEM_DCTRL_POLARITY_ACT_HIGH;
        pVInfo->vencOutputInfo.aFmt             =   SYSTEM_DCTRL_A_OUTPUT_MAX;

        /* Configure overlay params */
        pOvlyPrms                               = &ovlyPrms[1];
        pOvlyPrms->vencId                       = pVInfo->vencId;
        pOvlyPrms->deltaLinesPerPanel           = 0;
        pOvlyPrms->alphaBlenderEnable           = 0;
        pOvlyPrms->backGroundColor              = 0x10;
        pOvlyPrms->colorKeyEnable               = 1;
        pOvlyPrms->colorKeySel                  = SYSTEM_DSS_DISPC_TRANS_COLOR_KEY_SRC;
        pOvlyPrms->ovlyOptimization             = SYSTEM_DSS_DISPC_OVLY_FETCH_ALLDATA;
        pOvlyPrms->transColorKey                = DRAW2D_TRANSPARENT_COLOR;
    }
    else
    {
        UTILS_assert(NULL);
    }
    /* Configure LCD overlay params */
    pVInfo->mode = 0;
    pVInfo->isInputPipeConnected[0]         = FALSE;
    pVInfo->isInputPipeConnected[1]         = TRUE;
    pVInfo->isInputPipeConnected[2]         = TRUE;
    pVInfo->isInputPipeConnected[3]         = FALSE;
    pVInfo->writeBackEnabledFlag            = FALSE;

    pVInfo->vencOutputInfo.dataFormat       = SYSTEM_DF_RGB24_888;
    pVInfo->vencOutputInfo.dvoFormat        = SYSTEM_DCTRL_DVOFMT_GENERIC_DISCSYNC;
    pVInfo->vencOutputInfo.videoIfWidth     = SYSTEM_VIFW_24BIT;

    pVInfo->vencOutputInfo.pixelClkPolarity = SYSTEM_DCTRL_POLARITY_ACT_LOW;
    pVInfo->vencOutputInfo.aFmt             = SYSTEM_DCTRL_A_OUTPUT_MAX;

    if(lcdType != CHAINS_DISPLAY_TYPE_FPD)
    {
        pOvlyPrms                               = &ovlyPrms[1];
        pOvlyPrms->vencId                       = pVInfo->vencId;
        pOvlyPrms->deltaLinesPerPanel           = 0;
        pOvlyPrms->alphaBlenderEnable           = 0;
        pOvlyPrms->backGroundColor              = 0x10;
        pOvlyPrms->colorKeyEnable               = 1;
        pOvlyPrms->colorKeySel                  = SYSTEM_DSS_DISPC_TRANS_COLOR_KEY_SRC;
        pOvlyPrms->ovlyOptimization             = SYSTEM_DSS_DISPC_OVLY_FETCH_ALLDATA;
        pOvlyPrms->transColorKey                = DRAW2D_TRANSPARENT_COLOR;
    }
    /* Setting HDMI overlay pipe parameters */
    pPipeOvlyPrms                           = &gChains_commonObj.pipeParams[0];
    pPipeOvlyPrms->pipeLine                 = SYSTEM_DSS_DISPC_PIPE_VID1;
    pPipeOvlyPrms->globalAlpha              = 0xFF;
    pPipeOvlyPrms->preMultiplyAlpha         = 0;
    pPipeOvlyPrms->zorderEnable             = TRUE;
    pPipeOvlyPrms->zorder                   = SYSTEM_DSS_DISPC_ZORDER0;

    /* Setting LCD overlay pipe parameters */
    pPipeOvlyPrms                           = &gChains_commonObj.pipeParams[1];
    pPipeOvlyPrms->pipeLine                 = SYSTEM_DSS_DISPC_PIPE_VID2;
    pPipeOvlyPrms->globalAlpha              = 0xFF;
    pPipeOvlyPrms->preMultiplyAlpha         = 0;
    pPipeOvlyPrms->zorderEnable             = TRUE;
    pPipeOvlyPrms->zorder                   = SYSTEM_DSS_DISPC_ZORDER0;

    /* Setting PIP overlay pipe parameters on LCD */
    pPipeOvlyPrms                           = &gChains_commonObj.pipeParams[2];
    pPipeOvlyPrms->pipeLine                 = SYSTEM_DSS_DISPC_PIPE_VID3;
    pPipeOvlyPrms->globalAlpha              = 0xFF;
    pPipeOvlyPrms->preMultiplyAlpha         = 0;
    pPipeOvlyPrms->zorderEnable             = TRUE;
    pPipeOvlyPrms->zorder                   = SYSTEM_DSS_DISPC_ZORDER2;

    /* Configure graphics overlay for HDMI */
    pPipeOvlyPrms                           = &gChains_commonObj.pipeParams[3];
    pPipeOvlyPrms->pipeLine                 = SYSTEM_DSS_DISPC_PIPE_GFX1;
    pPipeOvlyPrms->globalAlpha              = 0xFF;
    pPipeOvlyPrms->preMultiplyAlpha         = 0;
    pPipeOvlyPrms->zorderEnable             = TRUE;
    pPipeOvlyPrms->zorder                   = SYSTEM_DSS_DISPC_ZORDER3;

/* Create, configure and start the Display Ctrl link */
    status = System_linkCreate(gChains_commonObj.displayCtrlLinkId,
                                NULL,
                                0);
    UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

    status = System_linkControl(gChains_commonObj.displayCtrlLinkId,
                                DISPLAYCTRL_LINK_CMD_SET_CONFIG,
                                &gChains_commonObj.dctrlCfgPrms,
                                sizeof(DisplayCtrlLink_ConfigParams),
                                TRUE);
    UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

    status = System_linkControl(gChains_commonObj.displayCtrlLinkId,
                                DISPLAYCTRL_LINK_CMD_SET_OVLY_PARAMS,
                                &ovlyPrms[0],
                                sizeof(DisplayCtrlLink_OvlyParams),
                                TRUE);
    UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

    status = System_linkControl(gChains_commonObj.displayCtrlLinkId,
                                DISPLAYCTRL_LINK_CMD_SET_OVLY_PARAMS,
                                &ovlyPrms[1],
                                sizeof(DisplayCtrlLink_OvlyParams),
                                TRUE);
    UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

    status = System_linkControl(gChains_commonObj.displayCtrlLinkId,
                                DISPLAYCTRL_LINK_CMD_SET_OVLY_PIPELINE_PARAMS,
                                &gChains_commonObj.pipeParams[0],
                                sizeof(DisplayCtrlLink_OvlyPipeParams),
                                TRUE);
    UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

    status = System_linkControl(gChains_commonObj.displayCtrlLinkId,
                                DISPLAYCTRL_LINK_CMD_SET_OVLY_PIPELINE_PARAMS,
                                &gChains_commonObj.pipeParams[1],
                                sizeof(DisplayCtrlLink_OvlyPipeParams),
                                TRUE);
    UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

    status = System_linkControl(gChains_commonObj.displayCtrlLinkId,
                                DISPLAYCTRL_LINK_CMD_SET_OVLY_PIPELINE_PARAMS,
                                &gChains_commonObj.pipeParams[2],
                                sizeof(DisplayCtrlLink_OvlyPipeParams),
                                TRUE);
    UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

    status = System_linkControl(gChains_commonObj.displayCtrlLinkId,
                                DISPLAYCTRL_LINK_CMD_SET_OVLY_PIPELINE_PARAMS,
                                &gChains_commonObj.pipeParams[3],
                                sizeof(DisplayCtrlLink_OvlyPipeParams),
                                TRUE);
    UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

    return status;
}


/**
 *******************************************************************************
 *
 * \brief   Set display link parameters in the case of single video pipe
 *          and one graphics pipe
 *
 *******************************************************************************
*/
Void ChainsCommon_SetDisplayPrms(
                                DisplayLink_CreateParams *pPrm_Video,
                                DisplayLink_CreateParams *pPrm_Grpx,
                                Chains_DisplayType displayType,
                                UInt32 displayWidth,
                                UInt32 displayHeight)
{
    if(pPrm_Video)
    {
        pPrm_Video->rtParams.tarWidth         = displayWidth;
        pPrm_Video->rtParams.tarHeight        = displayHeight;
        pPrm_Video->displayId                 = DISPLAY_LINK_INST_DSS_VID1;
    }

    if(pPrm_Grpx)
    {
        pPrm_Grpx->displayId                  = DISPLAY_LINK_INST_DSS_GFX1;
    }
}

/**
 *******************************************************************************
 *
 * \brief   Set Display Controller Create Parameters
 *
 * \param   pPrm         [IN]    DisplayCtrlLink_ConfigParams
 *
 *******************************************************************************
*/
static Void ChainsCommon_SetDctrlConfig(
                                DisplayCtrlLink_ConfigParams *pPrm,
                                DisplayCtrlLink_OvlyParams *ovlyPrms,
                                DisplayCtrlLink_OvlyPipeParams *pipeOvlyPrms,
                                UInt32 displayType,
                                UInt32 displayWidth,
                                UInt32 displayHeight)
{
    DisplayCtrlLink_VencInfo *pVInfo;

    pPrm->numVencs = 1;
    pPrm->tiedVencs = 0;

    pVInfo = &pPrm->vencInfo[0];

    pVInfo->tdmMode = DISPLAYCTRL_LINK_TDM_DISABLE;
    if(displayType == CHAINS_DISPLAY_TYPE_LCD_7_INCH)
    {
        pPrm->deviceId = DISPLAYCTRL_LINK_USE_LCD;
        pVInfo->vencId = SYSTEM_DCTRL_DSS_VENC_LCD1;
        pVInfo->outputPort = SYSTEM_DCTRL_DSS_DPI1_OUTPUT;
        pVInfo->vencOutputInfo.vsPolarity    =   SYSTEM_DCTRL_POLARITY_ACT_LOW;
        pVInfo->vencOutputInfo.hsPolarity    =   SYSTEM_DCTRL_POLARITY_ACT_LOW;

        /* Below are of dont care for EVM LCD */
        pVInfo->vencOutputInfo.fidPolarity      =   SYSTEM_DCTRL_POLARITY_ACT_LOW;
        pVInfo->vencOutputInfo.actVidPolarity   =   SYSTEM_DCTRL_POLARITY_ACT_LOW;

        pVInfo->mInfo.standard                  =   SYSTEM_STD_CUSTOM;
        pVInfo->mInfo.width                     =   displayWidth;
        pVInfo->mInfo.height                    =   displayHeight;
        pVInfo->mInfo.scanFormat                =   SYSTEM_SF_PROGRESSIVE;
        pVInfo->mInfo.pixelClock                =   29232u;
        pVInfo->mInfo.fps                       =   60U;
        pVInfo->mInfo.hFrontPorch               =   40u;
        pVInfo->mInfo.hBackPorch                =   40u;
        pVInfo->mInfo.hSyncLen                  =   48u;
        pVInfo->mInfo.vFrontPorch               =   13u;
        pVInfo->mInfo.vBackPorch                =   29u;
        pVInfo->mInfo.vSyncLen                  =   3u;
        pVInfo->vencDivisorInfo.divisorLCD      =   1;
        pVInfo->vencDivisorInfo.divisorPCD      =   4;

        pVInfo->vencOutputInfo.dataFormat       =   SYSTEM_DF_RGB24_888;
        pVInfo->vencOutputInfo.dvoFormat        =
                                    SYSTEM_DCTRL_DVOFMT_GENERIC_DISCSYNC;
        pVInfo->vencOutputInfo.videoIfWidth     =   SYSTEM_VIFW_24BIT;

        pVInfo->vencOutputInfo.pixelClkPolarity =   SYSTEM_DCTRL_POLARITY_ACT_HIGH;
        pVInfo->vencOutputInfo.aFmt             =   SYSTEM_DCTRL_A_OUTPUT_MAX;

        /* Configure overlay params */

        ovlyPrms->vencId                       = SYSTEM_DCTRL_DSS_VENC_LCD1;
    }
    else if(displayType == CHAINS_DISPLAY_TYPE_LCD_10_INCH)
    {
        pPrm->deviceId = DISPLAYCTRL_LINK_USE_LCD;
        pVInfo->vencId = SYSTEM_DCTRL_DSS_VENC_LCD1;
        pVInfo->outputPort = SYSTEM_DCTRL_DSS_DPI1_OUTPUT;
        pVInfo->vencOutputInfo.vsPolarity    =   SYSTEM_DCTRL_POLARITY_ACT_LOW;
        pVInfo->vencOutputInfo.hsPolarity    =   SYSTEM_DCTRL_POLARITY_ACT_LOW;

        /* Below are of dont care for EVM LCD */
        pVInfo->vencOutputInfo.fidPolarity      =   SYSTEM_DCTRL_POLARITY_ACT_HIGH;
        pVInfo->vencOutputInfo.actVidPolarity   =   SYSTEM_DCTRL_POLARITY_ACT_HIGH;

        pVInfo->mInfo.standard                  =   SYSTEM_STD_CUSTOM;
        pVInfo->mInfo.width                     =   displayWidth;
        pVInfo->mInfo.height                    =   displayHeight;
        pVInfo->mInfo.scanFormat                =   SYSTEM_SF_PROGRESSIVE;
        pVInfo->mInfo.pixelClock                =   74500U;
        pVInfo->mInfo.fps                       =   60U;
        pVInfo->mInfo.hBackPorch                =   80U;
        pVInfo->mInfo.hSyncLen                  =   62U;
        pVInfo->mInfo.hFrontPorch               =   48U;
        pVInfo->mInfo.vBackPorch                =   12U;
        pVInfo->mInfo.vSyncLen                  =   35U;
        pVInfo->mInfo.vFrontPorch               =   6U;
        pVInfo->vencDivisorInfo.divisorLCD      =   1;
        pVInfo->vencDivisorInfo.divisorPCD      =   1;

        pVInfo->vencOutputInfo.dataFormat       =   SYSTEM_DF_RGB24_888;
        pVInfo->vencOutputInfo.dvoFormat        =
                                    SYSTEM_DCTRL_DVOFMT_GENERIC_DISCSYNC;
        pVInfo->vencOutputInfo.videoIfWidth     =   SYSTEM_VIFW_24BIT;

        pVInfo->vencOutputInfo.pixelClkPolarity =   SYSTEM_DCTRL_POLARITY_ACT_LOW;
        pVInfo->vencOutputInfo.aFmt             =   SYSTEM_DCTRL_A_OUTPUT_MAX;

        /* Configure overlay params */

        ovlyPrms->vencId                       = SYSTEM_DCTRL_DSS_VENC_LCD1;
    }
    else if (displayType ==
        CHAINS_DISPLAY_TYPE_LDC_10_INCH_LCDCTRL_TC358778_MIPI_DSI_1920_1200)
    {
        pPrm->deviceId = DISPLAYCTRL_LINK_USE_LCD;
        pVInfo->vencId = SYSTEM_DCTRL_DSS_VENC_LCD1;
        pVInfo->outputPort = SYSTEM_DCTRL_DSS_DPI1_OUTPUT;
        pVInfo->vencOutputInfo.vsPolarity    =   SYSTEM_DCTRL_POLARITY_ACT_LOW;
        pVInfo->vencOutputInfo.hsPolarity    =   SYSTEM_DCTRL_POLARITY_ACT_LOW;

        /* Below are of dont care for EVM LCD */
        pVInfo->vencOutputInfo.fidPolarity      =   SYSTEM_DCTRL_POLARITY_ACT_HIGH;
        pVInfo->vencOutputInfo.actVidPolarity   =   SYSTEM_DCTRL_POLARITY_ACT_HIGH;

        pVInfo->mInfo.standard                  =   SYSTEM_STD_CUSTOM;
        pVInfo->mInfo.width                     =   displayWidth;
        pVInfo->mInfo.height                    =   displayHeight;
        pVInfo->mInfo.scanFormat                =   SYSTEM_SF_PROGRESSIVE;
        pVInfo->mInfo.pixelClock                =   147000U;
        pVInfo->mInfo.fps                       =   60U;
        pVInfo->mInfo.hBackPorch                =   32U;
        pVInfo->mInfo.hSyncLen                  =   16U;
        pVInfo->mInfo.hFrontPorch               =   32U;
        pVInfo->mInfo.vBackPorch                =   16U;
        pVInfo->mInfo.vSyncLen                  =   2U;
        pVInfo->mInfo.vFrontPorch               =   7U;
        pVInfo->vencDivisorInfo.divisorLCD      =   1;
        pVInfo->vencDivisorInfo.divisorPCD      =   1;

        pVInfo->vencOutputInfo.dataFormat       =   SYSTEM_DF_RGB24_888;
        pVInfo->vencOutputInfo.dvoFormat        =
                                    SYSTEM_DCTRL_DVOFMT_GENERIC_DISCSYNC;
        pVInfo->vencOutputInfo.videoIfWidth     =   SYSTEM_VIFW_24BIT;

        pVInfo->vencOutputInfo.pixelClkPolarity =   SYSTEM_DCTRL_POLARITY_ACT_LOW;
        pVInfo->vencOutputInfo.aFmt             =   SYSTEM_DCTRL_A_OUTPUT_MAX;

        /* Configure overlay params */

        ovlyPrms->vencId                       = SYSTEM_DCTRL_DSS_VENC_LCD1;
    }
    else if(displayType == CHAINS_DISPLAY_TYPE_HDMI_720P
            || displayType == CHAINS_DISPLAY_TYPE_HDMI_1080P)
    {
        pPrm->deviceId = DISPLAYCTRL_LINK_USE_HDMI;
        pVInfo->vencId = SYSTEM_DCTRL_DSS_VENC_HDMI;
        pVInfo->outputPort = SYSTEM_DCTRL_DSS_HDMI_OUTPUT;
        pVInfo->vencOutputInfo.vsPolarity       =   SYSTEM_DCTRL_POLARITY_ACT_HIGH;
        pVInfo->vencOutputInfo.hsPolarity       =   SYSTEM_DCTRL_POLARITY_ACT_HIGH;

        /* Below are of dont care for EVM LCD */
        pVInfo->vencOutputInfo.fidPolarity      =   SYSTEM_DCTRL_POLARITY_ACT_LOW;
        pVInfo->vencOutputInfo.actVidPolarity   =   SYSTEM_DCTRL_POLARITY_ACT_HIGH;

        pVInfo->vencOutputInfo.dataFormat       =   SYSTEM_DF_RGB24_888;
        pVInfo->vencOutputInfo.dvoFormat        =
                                    SYSTEM_DCTRL_DVOFMT_GENERIC_DISCSYNC;
        pVInfo->vencOutputInfo.videoIfWidth     =   SYSTEM_VIFW_24BIT;

        pVInfo->vencOutputInfo.pixelClkPolarity =   SYSTEM_DCTRL_POLARITY_ACT_HIGH;
        pVInfo->vencOutputInfo.aFmt             =   SYSTEM_DCTRL_A_OUTPUT_MAX;

        /* Configure overlay params */

        ovlyPrms->vencId                        = pVInfo->vencId;
    }
    else if(displayType == CHAINS_DISPLAY_TYPE_FPD)
    {
        pPrm->deviceId = DISPLAYCTRL_LINK_USE_LCD;
        pVInfo->vencId = SYSTEM_DCTRL_DSS_VENC_LCD3;
        pVInfo->outputPort = SYSTEM_DCTRL_DSS_DPI3_OUTPUT;
        pVInfo->mInfo.width                     =   displayWidth;
        pVInfo->mInfo.height                    =   displayHeight;
        pVInfo->mInfo.pixelClock                =   74250;
        pVInfo->mInfo.fps                       =   60U;
        pVInfo->mInfo.hBackPorch                =   148U;
        pVInfo->mInfo.hSyncLen                  =   44U;
        pVInfo->mInfo.hFrontPorch               =   88U;
        pVInfo->mInfo.vBackPorch                =   15U;
        pVInfo->mInfo.vSyncLen                  =   5U;
        pVInfo->mInfo.vFrontPorch               =   2U;
        pVInfo->vencDivisorInfo.divisorLCD      =   1;
        pVInfo->vencDivisorInfo.divisorPCD      =   1;
        pVInfo->vencOutputInfo.vsPolarity       =   SYSTEM_DCTRL_POLARITY_ACT_HIGH;
        pVInfo->vencOutputInfo.hsPolarity       =   SYSTEM_DCTRL_POLARITY_ACT_HIGH;

        /* Below are of dont care for EVM LCD */
        pVInfo->vencOutputInfo.fidPolarity      =   SYSTEM_DCTRL_POLARITY_ACT_LOW;
        pVInfo->vencOutputInfo.actVidPolarity   =   SYSTEM_DCTRL_POLARITY_ACT_HIGH;

        pVInfo->vencOutputInfo.dataFormat       =   SYSTEM_DF_RGB24_888;
        pVInfo->vencOutputInfo.dvoFormat        =
                                    SYSTEM_DCTRL_DVOFMT_GENERIC_DISCSYNC;
        pVInfo->vencOutputInfo.videoIfWidth     =   SYSTEM_VIFW_24BIT;

        pVInfo->vencOutputInfo.pixelClkPolarity =   SYSTEM_DCTRL_POLARITY_ACT_HIGH;
        pVInfo->vencOutputInfo.aFmt             =   SYSTEM_DCTRL_A_OUTPUT_MAX;

        /* Configure overlay params */

        ovlyPrms->vencId                        = pVInfo->vencId;
    }
    else
    {
        OSA_assert(0);
    }

    if(displayType == CHAINS_DISPLAY_TYPE_HDMI_720P)
    {
        pVInfo->mInfo.standard                  =   SYSTEM_STD_720P_60;

    }
    else if (displayType == CHAINS_DISPLAY_TYPE_HDMI_1080P)
    {
        pVInfo->mInfo.standard                  =   SYSTEM_STD_1080P_60;
    }
    else if (displayType == CHAINS_DISPLAY_TYPE_FPD)
    {
        pVInfo->mInfo.standard                  =   SYSTEM_STD_720P_60;
    }

    /* TODO Dont know what to set here */
    pVInfo->mode = 0;
    pVInfo->isInputPipeConnected[0] =
                    TRUE;
    pVInfo->isInputPipeConnected[1] =
                    TRUE;
    pVInfo->isInputPipeConnected[2] =
                    TRUE;
    pVInfo->isInputPipeConnected[3] =
                    TRUE;
    pVInfo->writeBackEnabledFlag = FALSE;


    /* Setting other overlay parameters common to both Venc */

    ovlyPrms->deltaLinesPerPanel   = 0;
    ovlyPrms->alphaBlenderEnable   = 0;
    ovlyPrms->backGroundColor      = 0x10;
    ovlyPrms->colorKeyEnable       = 1;
    ovlyPrms->colorKeySel          = SYSTEM_DSS_DISPC_TRANS_COLOR_KEY_SRC;
    ovlyPrms->ovlyOptimization     = SYSTEM_DSS_DISPC_OVLY_FETCH_ALLDATA;
    ovlyPrms->transColorKey        = DRAW2D_TRANSPARENT_COLOR;

    /* Setting overlay pipe parameters */

    pipeOvlyPrms[0].pipeLine = SYSTEM_DSS_DISPC_PIPE_VID1;
    pipeOvlyPrms[0].globalAlpha = 0xFF;
    pipeOvlyPrms[0].preMultiplyAlpha = 0;
    pipeOvlyPrms[0].zorderEnable = TRUE;
    pipeOvlyPrms[0].zorder = SYSTEM_DSS_DISPC_ZORDER0;

    pipeOvlyPrms[1].pipeLine = SYSTEM_DSS_DISPC_PIPE_VID2;
    pipeOvlyPrms[1].globalAlpha = 0xFF;
    pipeOvlyPrms[1].preMultiplyAlpha = 0;
    pipeOvlyPrms[1].zorderEnable = TRUE;
    pipeOvlyPrms[1].zorder = SYSTEM_DSS_DISPC_ZORDER1;

    pipeOvlyPrms[2].pipeLine = SYSTEM_DSS_DISPC_PIPE_VID3;
    pipeOvlyPrms[2].globalAlpha = 0xFF;
    pipeOvlyPrms[2].preMultiplyAlpha = 0;
    pipeOvlyPrms[2].zorderEnable = TRUE;
    pipeOvlyPrms[2].zorder = SYSTEM_DSS_DISPC_ZORDER2;

    pipeOvlyPrms[3].pipeLine = SYSTEM_DSS_DISPC_PIPE_GFX1;
    pipeOvlyPrms[3].globalAlpha = 0xFF;
    pipeOvlyPrms[3].preMultiplyAlpha = 0;
    pipeOvlyPrms[3].zorderEnable = TRUE;
    pipeOvlyPrms[3].zorder = SYSTEM_DSS_DISPC_ZORDER3;

}

/**
 *******************************************************************************
 *
 * \brief   Configure and start display controller
 *
 *******************************************************************************
*/
Int32 ChainsCommon_StartDisplayCtrl(UInt32 displayType,
                                UInt32 displayWidth,
                                UInt32 displayHeight)
{
    Int32 status=0;

    gChains_commonObj.displayCtrlLinkId = SYSTEM_LINK_ID_DISPLAYCTRL;

    ChainsCommon_SetDctrlConfig(
        &gChains_commonObj.dctrlCfgPrms,
        &gChains_commonObj.ovlyParams,
        &gChains_commonObj.pipeParams[0],
        displayType,
        displayWidth,
        displayHeight
        );

    /* There are no createtime Params for display controller */
    status = System_linkCreate(gChains_commonObj.displayCtrlLinkId,
                                NULL,
                                0);

    status = System_linkControl(gChains_commonObj.displayCtrlLinkId,
                                DISPLAYCTRL_LINK_CMD_SET_CONFIG,
                                &gChains_commonObj.dctrlCfgPrms,
                                sizeof(DisplayCtrlLink_ConfigParams),
                                TRUE);
    OSA_assert(status == SYSTEM_LINK_STATUS_SOK);

    status = System_linkControl(gChains_commonObj.displayCtrlLinkId,
                                DISPLAYCTRL_LINK_CMD_SET_OVLY_PARAMS,
                                &gChains_commonObj.ovlyParams,
                                sizeof(DisplayCtrlLink_OvlyParams),
                                TRUE);
    OSA_assert(status == SYSTEM_LINK_STATUS_SOK);

    status = System_linkControl(gChains_commonObj.displayCtrlLinkId,
                                DISPLAYCTRL_LINK_CMD_SET_OVLY_PIPELINE_PARAMS,
                                &gChains_commonObj.pipeParams[0],
                                sizeof(DisplayCtrlLink_OvlyPipeParams),
                                TRUE);
    OSA_assert(status == SYSTEM_LINK_STATUS_SOK);

    status = System_linkControl(gChains_commonObj.displayCtrlLinkId,
                                DISPLAYCTRL_LINK_CMD_SET_OVLY_PIPELINE_PARAMS,
                                &gChains_commonObj.pipeParams[1],
                                sizeof(DisplayCtrlLink_OvlyPipeParams),
                                TRUE);
    OSA_assert(status == SYSTEM_LINK_STATUS_SOK);

    status = System_linkControl(gChains_commonObj.displayCtrlLinkId,
                            DISPLAYCTRL_LINK_CMD_SET_OVLY_PIPELINE_PARAMS,
                            &gChains_commonObj.pipeParams[2],
                            sizeof(DisplayCtrlLink_OvlyPipeParams),
                            TRUE);
    OSA_assert(status == SYSTEM_LINK_STATUS_SOK);

    status = System_linkControl(gChains_commonObj.displayCtrlLinkId,
                                DISPLAYCTRL_LINK_CMD_SET_OVLY_PIPELINE_PARAMS,
                                &gChains_commonObj.pipeParams[3],
                                sizeof(DisplayCtrlLink_OvlyPipeParams),
                                TRUE);
    OSA_assert(status == SYSTEM_LINK_STATUS_SOK);

    return status;
}

/**
 *******************************************************************************
 *
 * \brief   Stop and Delete display controller
 *
 *******************************************************************************
*/
Int32 ChainsCommon_StopDisplayCtrl()
{
    Int32 status=0;

    status = System_linkControl(gChains_commonObj.displayCtrlLinkId,
                                DISPLAYCTRL_LINK_CMD_CLR_CONFIG,
                                &gChains_commonObj.dctrlCfgPrms,
                                sizeof(DisplayCtrlLink_ConfigParams),
                                TRUE);
    OSA_assert(status == SYSTEM_LINK_STATUS_SOK);

    status = System_linkDelete(gChains_commonObj.displayCtrlLinkId);
    OSA_assert(status == SYSTEM_LINK_STATUS_SOK);

    return status;
}

/**
 *******************************************************************************
 *
 * \brief   Set display link parameters in the case of single video pipe
 *          and one graphics pipe
 *
 *******************************************************************************
*/
Void ChainsCommon_SetGrpxSrcPrms(
                                GrpxSrcLink_CreateParams *pPrm,
                                UInt32 displayWidth,
                                UInt32 displayHeight
                                )
{
    pPrm->grpxBufInfo.dataFormat  = SYSTEM_DF_BGR16_565;
    pPrm->grpxBufInfo.height   = displayHeight;
    pPrm->grpxBufInfo.width    = displayWidth;

    pPrm->logoDisplayEnable = TRUE;
    pPrm->logoParams.startX = 40;
    pPrm->logoParams.startY = 40;

    pPrm->statsDisplayEnable = TRUE;

    pPrm->statsPrintParams.startX = pPrm->logoParams.startX;
    pPrm->statsPrintParams.startY = displayHeight - 134 - pPrm->logoParams.startY;
}

