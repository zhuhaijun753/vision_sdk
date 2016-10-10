/*
 *******************************************************************************
 *
 * Copyright (C) 2016 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */

/**
 *******************************************************************************
 * \file chains_vipSingleCamOpenCVCannySgxDisplay.c
 *
 * \brief  Usecase file implementation of capture OpenCV Canny on A15 display usecase.
 *
 *         Usecase file for single camera view usecase.
 *
 *         Capture --> OpenCV Canny (A15) -->SgxDisplay Link(A15)
 *
 *
 *         In this use-case we capture 1 CH of video from OV1063x 720p30
 *         and send it to A15 using IPC_OUT, IPC_IN. A15 is running OpenCV Canny
 *         Algorithm Link and then SgxDisplay
 *         Link which will render the frames and display via DRM
 *
 *         The data flow diagram is shown below
 *
 *             Capture (VIP) 1CH 30fps 1280x720 or 60fp 1920x1080
 *                   |
 *                   |
 *                 IPC OUT
 *                   |
 *                 IPC IN
                     |
                OpenCV Canny (A15)
 *                   |
 *               SgxDisplay (A15)
 *
 * \version 0.0 (March 2016) : [SN] First version ported for linux.
 *
 *******************************************************************************
*/

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */

#include "chains_vipSingleCamOpenCVCannySgxDisplay_priv.h"
#include <linux/examples/tda2xx/include/chains.h>
#include <linux/examples/common/chains_common.h>
#include <linux/src/system/system_gbm_allocator.h>

#define CAPTURE_SENSOR_WIDTH      (1280)
#define CAPTURE_SENSOR_HEIGHT     (720)
#define LCD_DISPLAY_WIDTH         (800)
#define LCD_DISPLAY_HEIGHT        (480)

/**
 *******************************************************************************
 *
 *  \brief  SingleCameraOpenCVCannyObject
 *
 *        This structure contains all the LinksId's and create Params.
 *        The same is passed to all create, start, stop functions.
 *
 *******************************************************************************
*/
typedef struct {

    chains_vipSingleCamOpenCVCannySgxDisplayObj ucObj;

    /**< Link Id's and device IDs to use for this use-case */
    UInt32  appCtrlLinkId;

    UInt32  captureOutWidth;
    UInt32  captureOutHeight;

    Chains_Ctrl *chainsCfg;

}chains_vipSingleCamOpenCVCannySgxDisplayAppObj;

static struct control_srv_egl_ctx chainsEglParams = {
   .get_egl_native_display = gbm_allocator_get_native_display,
   .get_egl_native_buffer = gbm_allocator_get_native_buffer,
   .destroy_egl_native_buffer = gbm_allocator_destroy_native_buffer,
};

/**
 *******************************************************************************
 *
 * \brief   Set SGXDISPLAY Link Parameters
 *
 *          It is called in Create function.

 *
 * \param   pPrm    [IN]    IpcLink_CreateParams
 *
 *******************************************************************************
*/
Void chains_vipSingleCamOpenCVCannySgxDisplay_SetSgxDisplayLinkPrms (
                                  SgxFrmcpyLink_CreateParams *prms,
                                  UInt32 width, UInt32 height)
{
    prms->displayWidth = width;
    prms->displayHeight = height;
    prms->renderType = SGXFRMCPY_RENDER_TYPE_1x1;
    prms->inBufType = SYSTEM_BUFFER_TYPE_VIDEO_FRAME;
    prms->bEglInfoInCreate = TRUE;
    prms->EglInfo = (void *)&chainsEglParams;
}

/**
 *******************************************************************************
 *
 * \brief   Set OpenCV Canny Alg parameters
 *
 *          It is called in Create function.
 *          In this function alg link params are set
 *          The algorithm which is to run on core is set to
 *          baseClassCreate.algId. The input width and height to alg are set.
 *          Number of input buffers required by alg are also set here.
 *
 *
 * \param   pPrm    [IN]    AlgorithmLink_OpenCVCannyCreateParams
 * \param   chainsCfg    [IN]    Chains_Ctrl
 *
 *******************************************************************************
*/

Void chains_vipSingleCamOpenCVCannySgxDisplay_SetOpenCVCannyAlgPrms(
                                    AlgorithmLink_OpenCVCannyCreateParams *pPrm,
                                     Chains_Ctrl *chainsCfg)
{
    pPrm->baseClassCreate.algId = ALGORITHM_LINK_A15_ALG_OPENCVCANNY;
        
    pPrm->maxWidth    = CAPTURE_SENSOR_WIDTH;
    pPrm->maxHeight   = CAPTURE_SENSOR_HEIGHT;

    pPrm->numOutputFrames = 3;
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
Void chains_vipSingleCamOpenCVCannySgxDisplay_SetAppPrms(chains_vipSingleCamOpenCVCannySgxDisplayObj *pUcObj,Void *appObj)
{
    UInt32 displayWidth, displayHeight;

    chains_vipSingleCamOpenCVCannySgxDisplayAppObj *pObj
            = (chains_vipSingleCamOpenCVCannySgxDisplayAppObj*)appObj;

    pObj->captureOutWidth  = CAPTURE_SENSOR_WIDTH;
    pObj->captureOutHeight = CAPTURE_SENSOR_HEIGHT;

    ChainsCommon_SingleCam_SetCapturePrms(&(pUcObj->CapturePrm),
            CAPTURE_SENSOR_WIDTH,
            CAPTURE_SENSOR_HEIGHT,
            pObj->captureOutWidth,
            pObj->captureOutHeight,
            pObj->chainsCfg->captureSrc
            );

    ChainsCommon_GetDisplayWidthHeight(
        pObj->chainsCfg->displayType,
        &displayWidth,
        &displayHeight
        );

    ChainsCommon_SetGrpxSrcPrms(&pUcObj->GrpxSrcPrm,
                                 displayWidth,
                                 displayHeight
                                );

    pUcObj->Display_GrpxPrm.rtParams.tarWidth       = displayWidth;
    pUcObj->Display_GrpxPrm.rtParams.tarHeight      = displayHeight;
    pUcObj->Display_GrpxPrm.rtParams.posX           = 0;
    pUcObj->Display_GrpxPrm.rtParams.posY           = 0;
    pUcObj->Display_GrpxPrm.displayId               = DISPLAY_LINK_INST_DSS_GFX1;

    chains_vipSingleCamOpenCVCannySgxDisplay_SetSgxDisplayLinkPrms
                    (&pUcObj->SgxFrmcpyPrm,
                     displayWidth,
                     displayHeight
                    );

    chains_vipSingleCamOpenCVCannySgxDisplay_SetOpenCVCannyAlgPrms(&pUcObj->Alg_OpenCVCannyPrm,
                                                            pObj->chainsCfg);

    ChainsCommon_SetDisplayPrms(&pUcObj->Display_M4Prm,
                                NULL,
                                pObj->chainsCfg->displayType,
                                displayWidth,
                                displayHeight
                               );

    ChainsCommon_StartDisplayCtrl(
        pObj->chainsCfg->displayType,
        displayWidth,
        displayHeight
        );
}

/**
 *******************************************************************************
 *
 * \brief   Start the capture display Links
 *
 *          Function sends a control command to capture and display link to
 *          to Start all the required links . Links are started in reverse
 *          order as information of next link is required to connect.
 *          System_linkStart is called with LinkId to start the links.
 *
 * \param   pObj  [IN] chains_vipSingleCam_SgxDisplayAppObj
 *
 *
 *******************************************************************************
*/
Void chains_vipSingleCamOpenCVCannySgxDisplay_StartApp(chains_vipSingleCamOpenCVCannySgxDisplayAppObj *pObj)
{
    ChainsCommon_statCollectorReset();
    ChainsCommon_memPrintHeapStatus();

    chains_vipSingleCamOpenCVCannySgxDisplay_Start(&pObj->ucObj);

    ChainsCommon_prfLoadCalcEnable(TRUE, FALSE, FALSE);
}

/**
 *******************************************************************************
 *
 * \brief   Stop the capture display Links
 *
 *          Function sends a control command to capture and display link to
 *          to Start all the required links . Links are started in reverce
 *          order as information of next link is required to connect.
 *          System_linkStart is called with LinkId to start the links.
 *
 * \param   pObj  [IN] chains_vipSingleCam_SgxDisplayAppObj
 *
 *
 *******************************************************************************
*/
Void chains_vipSingleCamOpenCVCannySgxDisplay_StopApp(chains_vipSingleCamOpenCVCannySgxDisplayAppObj *pObj)
{

    chains_vipSingleCamOpenCVCannySgxDisplay_Stop(&pObj->ucObj);

    chains_vipSingleCamOpenCVCannySgxDisplay_Delete(&pObj->ucObj);

    ChainsCommon_StopDisplayCtrl();

    ChainsCommon_prfLoadCalcEnable(FALSE, FALSE, FALSE);

}

/**
 *******************************************************************************
 *
 * \brief   Single Channel Capture OpenCV Canny Display usecase function
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
Void chains_vipSingleCamOpenCVCannySgxDisplay(Chains_Ctrl *chainsCfg)
{
    char ch;
    UInt32 done = FALSE;
    chains_vipSingleCamOpenCVCannySgxDisplayAppObj chainsObj;

    chainsObj.chainsCfg = chainsCfg;

    chains_vipSingleCamOpenCVCannySgxDisplay_Create(&chainsObj.ucObj, &chainsObj);

    chains_vipSingleCamOpenCVCannySgxDisplay_StartApp(&chainsObj);

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
                ChainsCommon_prfCpuLoadPrint();
                ChainsCommon_statCollectorPrint();
                chains_vipSingleCamOpenCVCannySgxDisplay_printStatistics(&chainsObj.ucObj);
                chains_vipSingleCamOpenCVCannySgxDisplay_printBufferStatistics(&chainsObj.ucObj);
                break;
            default:
                Vps_printf("\nUnsupported option '%c'. Please try again\n", ch);
                break;
        }
    }

    chains_vipSingleCamOpenCVCannySgxDisplay_StopApp(&chainsObj);

}

