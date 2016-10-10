/*
 *******************************************************************************
 *
 * Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */
#include "chains_vipSingleCameraAnalytics2_priv.h"
#include <examples/tda2xx/include/chains_common.h>

/**
 *******************************************************************************
 *
 *  \brief  SingleCameraAnalyticsObject
 *
 *        This structure contains all the LinksId's and create Params.
 *        The same is passed to all create, start, stop functions.
 *
 *******************************************************************************
*/
typedef struct {

    chains_vipSingleCameraAnalytics2Obj ucObj;

    Chains_Ctrl *chainsCfg;

} Chains_VipSingleCameraAnalytics2AppObj;

Chains_VipSingleCameraAnalytics2AppObj gChains_vipSingleCameraAnalytics2AppObj;


/**
 *******************************************************************************
 *
 * \brief   Handler for System Events.
 *
 * \param   cmd[IN]     System Command
 * \param   pPrm[IN]    Pointer to the Data
 *
 *******************************************************************************
*/
Void Chains_vipSingleCameraAnalytics2_EventHandler(UInt32 cmd, Void *pPrm)
{
    chains_vipSingleCameraAnalytics2Obj *pUcObj =
        &gChains_vipSingleCameraAnalytics2AppObj.ucObj;


    if (UTILS_TEMP_CMD_EVENT_HOT == cmd)
    {
        ChainsCommon_tempHotEventHandler(pUcObj->CaptureLinkID, pPrm);
    }
    else if (UTILS_TEMP_CMD_EVENT_COLD == cmd)
    {
        ChainsCommon_tempColdEventHandler(pUcObj->CaptureLinkID, pPrm);
    }
    else
    {
        ; /* Nothing to do */
    }
}


/**
 *******************************************************************************
 *
 * \brief   Set link Parameters
 *
 *          It is called in Create function of the auto generated use-case file.
 *
 * \param pUcObj    [IN] Auto-generated usecase object
 * \param appObj    [IN] Application specific object
 *
 *******************************************************************************
*/
Void chains_vipSingleCameraAnalytics2_SetAppPrms(chains_vipSingleCameraAnalytics2Obj *pUcObj, Void *appObj)
{
    Chains_VipSingleCameraAnalytics2AppObj *pObj
        = (Chains_VipSingleCameraAnalytics2AppObj*)appObj;

    ChainsCommon_Analytics2_SetPrms(
        &pUcObj->CapturePrm,
        &pUcObj->Alg_SparseOpticalFlowPrm,
        &pUcObj->Alg_SfmPrm,
        &pUcObj->Alg_ImagePreProcessPrm,
        &pUcObj->Alg_ImgPyramidPrm,
        &pUcObj->Alg_FPComputePrm,
        &pUcObj->Alg_ObjectDetectionPrm,
        &pUcObj->Alg_ObjectClassificationPrm,
        &pUcObj->Sync_FcwPrm,
        &pUcObj->Alg_FcwPrm,
        &pUcObj->Alg_LaneDetectPrm,
        &pUcObj->Alg_ClrPrm,
        &pUcObj->Sync_algPrm,
        &pUcObj->Alg_ObjectDrawPrm,
        &pUcObj->Sync_objectDetectPrm,
        &pUcObj->Display_VideoPrm,
        &pUcObj->GrpxSrcPrm,
        &pUcObj->Display_GrpxPrm,
        pObj->chainsCfg->captureSrc,
        pObj->chainsCfg->displayType,
        pUcObj->Alg_ImgPyramidLinkID,
        pUcObj->Alg_FPComputeLinkID,
        pUcObj->Alg_ObjectDrawLinkID,
        pUcObj->Alg_ObjectClassificationLinkID,
        pUcObj->Alg_LaneDetectLinkID,
        FALSE
        );
}

/**
 *******************************************************************************
 *
 * \brief   Start the capture display Links
 *
 *          Function sends a control command to capture and display link to
 *          to Start all the required links . Links are started in reverce
 *          order as information of next link is required to connect.
 *          System_linkStart is called with LinkId to start the links.
 *
 * \param   pObj  [IN] Chains_VipSingleCameraAnalytics2AppObj
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
*/
Void chains_vipSingleCameraAnalytics2_StartApp(Chains_VipSingleCameraAnalytics2AppObj *pObj)
{

    /* Initialize the sytem to take up temprature events */
    Utils_tempConfigInit();
    SystemLink_registerHandler(Chains_vipSingleCameraAnalytics2_EventHandler);

    Chains_memPrintHeapStatus();

    ChainsCommon_Analytics2_Start(FALSE);

    chains_vipSingleCameraAnalytics2_Start(&pObj->ucObj);

    Chains_prfLoadCalcEnable(TRUE, FALSE, FALSE);
}

/**
 *******************************************************************************
 *
 * \brief   Delete the capture display Links
 *
 *          Function sends a control command to capture and display link to
 *          to delete all the prior created links
 *          System_linkDelete is called with LinkId to delete the links.
 *
 * \param   pObj   [IN]   Chains_VipSingleCameraAnalytics2AppObj
 *
 *******************************************************************************
*/
Void chains_vipSingleCameraAnalytics2_StopAndDeleteApp(Chains_VipSingleCameraAnalytics2AppObj *pObj)
{
    chains_vipSingleCameraAnalytics2_Stop(&pObj->ucObj);
    chains_vipSingleCameraAnalytics2_Delete(&pObj->ucObj);

    ChainsCommon_Analytics2_Stop(FALSE);

    /* Print the HWI, SWI and all tasks load */
    /* Reset the accumulated timer ticks */
    Chains_prfLoadCalcEnable(FALSE, TRUE, TRUE);

    /* De-init the temperature configuration */
    Utils_tempConfigDeInit();
    SystemLink_unregisterHandler(Chains_vipSingleCameraAnalytics2_EventHandler);
}

/**
 *******************************************************************************
 *
 * \brief   Single Channel Capture Display usecase function
 *
 *          This functions executes the create, start functions
 *
 *          Further in a while loop displays run time menu and waits
 *          for user inputs to print the statistics or to end the demo.
 *
 *          Once the user inputs end of demo stop and delete
 *          functions are executed.
 *
 * \param   chainsCfg       [IN]   Chains_Ctrl
 *
 *******************************************************************************
*/
Void Chains_vipSingleCameraAnalytics2(Chains_Ctrl *chainsCfg)
{
    char ch;
    UInt32 done = FALSE;

    gChains_vipSingleCameraAnalytics2AppObj.chainsCfg = chainsCfg;

    chains_vipSingleCameraAnalytics2_Create(
            &gChains_vipSingleCameraAnalytics2AppObj.ucObj,
            &gChains_vipSingleCameraAnalytics2AppObj);

    chains_vipSingleCameraAnalytics2_StartApp(
            &gChains_vipSingleCameraAnalytics2AppObj);

    while(!done)
    {
        ch = ChainsCommon_menuRunTime_SingleCamAnalytics();

        switch(ch)
        {
            case '0':
                done = TRUE;
                break;
            case 'p':
            case 'P':
                ChainsCommon_PrintStatistics();
                chains_vipSingleCameraAnalytics2_printStatistics(
                        &gChains_vipSingleCameraAnalytics2AppObj.ucObj);
                break;
            case 't':
            case 'T':
                ChainsCommon_thermalConfig();
            default:
                Vps_printf("\nUnsupported option '%c'. Please try again\n", ch);
                break;
        }
    }

    chains_vipSingleCameraAnalytics2_StopAndDeleteApp(
            &gChains_vipSingleCameraAnalytics2AppObj);
}

