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
#include "chains_fileRxCameraAnalytics_tda2xx_priv.h"
#include <examples/tda2xx/include/chains_common.h>

#define CAPTURE_SENSOR_WIDTH                                    1280
#define CAPTURE_SENSOR_HEIGHT                                   720

#define REMAP_WIDTH                                             896
#define REMAP_HEIGHT                                            384

#define STEREO_OUTPUT_WIDTH                                     640
#define STEREO_OUTPUT_HEIGHT                                    360

#define LIVE_CAMERA_DISPLAY_WIDTH                               640
#define LIVE_CAMERA_DISPLAY_HEIGHT                              360

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

    chains_fileRxCameraAnalytics_tda2xxObj ucObj;

    Chains_Ctrl *chainsCfg;

} Chains_FileRxCameraAnalyticsTda2xxAppObj;

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
Void chains_fileRxCameraAnalytics_tda2xx_SetAppPrms(chains_fileRxCameraAnalytics_tda2xxObj *pUcObj, Void *appObj)
{
    Chains_FileRxCameraAnalyticsTda2xxAppObj *pObj
        = (Chains_FileRxCameraAnalyticsTda2xxAppObj*)appObj;

    ChainsCommon_SetFileRxPrms(
        &pUcObj->NullSourcePrm,
        &pUcObj->DecodePrm,
        1920,
        1080,
        1,
        25);

    ChainsCommon_Analytics2_SetPrms(
        NULL,
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
        TRUE
        );

    pUcObj->Display_VideoPrm.displayId          = DISPLAY_LINK_INST_DSS_VID3;

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
 * \param   pObj  [IN] Chains_FileRxCameraAnalyticsTda2xxAppObj
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
*/
Void chains_fileRxCameraAnalytics_tda2xx_StartApp(Chains_FileRxCameraAnalyticsTda2xxAppObj *pObj)
{
    Chains_memPrintHeapStatus();

    ChainsCommon_Analytics2_Start(TRUE);

    chains_fileRxCameraAnalytics_tda2xx_Start(&pObj->ucObj);

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
 * \param   pObj   [IN]   Chains_FileRxCameraAnalyticsTda2xxAppObj
 *
 *******************************************************************************
*/
Void chains_fileRxCameraAnalytics_tda2xx_StopAndDeleteApp(Chains_FileRxCameraAnalyticsTda2xxAppObj *pObj)
{

    chains_fileRxCameraAnalytics_tda2xx_Stop(&pObj->ucObj);
    chains_fileRxCameraAnalytics_tda2xx_Delete(&pObj->ucObj);

    ChainsCommon_Analytics2_Stop(TRUE);

    /* Print the HWI, SWI and all tasks load */
    /* Reset the accumulated timer ticks */
    Chains_prfLoadCalcEnable(FALSE, TRUE, TRUE);
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
Void Chains_fileRxCameraAnalyticsTda2xx(Chains_Ctrl *chainsCfg)
{
    char ch;
    UInt32 done = FALSE;
    Chains_FileRxCameraAnalyticsTda2xxAppObj chainsObj;

    chainsObj.chainsCfg = chainsCfg;

    chains_fileRxCameraAnalytics_tda2xx_Create(&chainsObj.ucObj, &chainsObj);

    chains_fileRxCameraAnalytics_tda2xx_StartApp(&chainsObj);

    while(!done)
    {
        ch = Chains_menuRunTime();

        switch(ch)
        {
            case '0':
                done = TRUE;
                break;
            case 'p':
            case 'P':
                ChainsCommon_PrintStatistics();
                chains_fileRxCameraAnalytics_tda2xx_printStatistics(&chainsObj.ucObj);
                break;
            default:
                Vps_printf("\nUnsupported option '%c'. Please try again\n", ch);
                break;
        }
    }

    chains_fileRxCameraAnalytics_tda2xx_StopAndDeleteApp(&chainsObj);

}

