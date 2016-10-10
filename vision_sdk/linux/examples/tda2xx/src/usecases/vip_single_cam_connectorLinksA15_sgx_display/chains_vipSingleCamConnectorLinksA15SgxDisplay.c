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
 * \file chains_vipSingleCamConnectorLinksA15SgxDisplay.c
 *
 * \brief  Usecase file implementation of capture frame copy on A15 display usecase.
 *         Along with connector links on A15 - DUP, MERGE, SELECT, GATE
 *
 * \version 0.0 (Apr 2016) : [YM] First version created for HLOS.
 *
 *******************************************************************************
*/

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */

#include "chains_vipSingleCamConnectorLinksA15SgxDisplay_priv.h"
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
 *  \brief  SingleCameraConnectorLinksA15Object
 *
 *        This structure contains all the LinksId's and create Params.
 *        The same is passed to all create, start, stop functions.
 *
 *******************************************************************************
*/
typedef struct {

    chains_vipSingleCamConnectorLinksA15SgxDisplayObj ucObj;

    /**< Link Id's and device IDs to use for this use-case */
    UInt32  appCtrlLinkId;

    UInt32  captureOutWidth;
    UInt32  captureOutHeight;

    Chains_Ctrl *chainsCfg;

}chains_vipSingleCamConnectorLinksA15SgxDisplayAppObj;

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
Void chains_vipSingleCamConnectorLinksA15SgxDisplay_SetSgxDisplayLinkPrms (
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
 * \brief   Set Frame Copy Alg parameters
 *
 *          It is called in Create function.
 *          In this function alg link params are set
 *          The algorithm which is to run on core is set to
 *          baseClassCreate.algId. The input width and height to alg are set.
 *          Number of input buffers required by alg are also set here.
 *
 *
 * \param   pPrm    [IN]    AlgorithmLink_ConnectorLinksA15CreateParams
 * \param   chainsCfg    [IN]    Chains_Ctrl
 *
 *******************************************************************************
*/

Void chains_vipSingleCamConnectorLinksA15SgxDisplay_SetConnectorLinksA15AlgPrms(
                                      AlgorithmLink_FrameCopyCreateParams *pPrm,
                                      Chains_Ctrl *chainsCfg)
{
    pPrm->baseClassCreate.algId = ALGORITHM_LINK_A15_ALG_FRAMECOPY;

    pPrm->maxWidth    = CAPTURE_SENSOR_WIDTH;
    pPrm->maxHeight   = CAPTURE_SENSOR_HEIGHT;

    pPrm->numOutputFrames = 3;
}

Void chains_vipSingleCamConnectorLinksA15SgxDisplay_SetSelectLinkPrms(SelectLink_CreateParams *pPrm,
                                                                      GateLink_CreateParams * pGatePrms)
{
    pPrm->numOutQue = 2U;

    pPrm->outQueChInfo[0].outQueId   = 1;
    pPrm->outQueChInfo[0].numOutCh   = 1;
    pPrm->outQueChInfo[0].inChNum[0] = 1;

    pPrm->outQueChInfo[1].outQueId   = 0;
    pPrm->outQueChInfo[1].numOutCh   = 1;
    pPrm->outQueChInfo[1].inChNum[0] = 0;

    pGatePrms->prevLinkIsCreated = TRUE;

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
Void chains_vipSingleCamConnectorLinksA15SgxDisplay_SetAppPrms(chains_vipSingleCamConnectorLinksA15SgxDisplayObj *pUcObj,Void *appObj)
{
    UInt32 displayWidth, displayHeight;

    chains_vipSingleCamConnectorLinksA15SgxDisplayAppObj *pObj
            = (chains_vipSingleCamConnectorLinksA15SgxDisplayAppObj*)appObj;

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
    chains_vipSingleCamConnectorLinksA15SgxDisplay_SetSelectLinkPrms(&(pUcObj->Select_1Prm),
                                                                     &(pUcObj->Gate_1Prm));

    chains_vipSingleCamConnectorLinksA15SgxDisplay_SetSgxDisplayLinkPrms
                    (&pUcObj->SgxFrmcpyPrm,
                     displayWidth,
                     displayHeight
                    );

    chains_vipSingleCamConnectorLinksA15SgxDisplay_SetConnectorLinksA15AlgPrms(&pUcObj->Alg_FrameCopy_1Prm,
                                                            pObj->chainsCfg);
    chains_vipSingleCamConnectorLinksA15SgxDisplay_SetConnectorLinksA15AlgPrms(&pUcObj->Alg_FrameCopy_2Prm,
                                                            pObj->chainsCfg);

    ChainsCommon_SetGrpxSrcPrms(&pUcObj->GrpxSrcPrm,
                                 displayWidth,
                                 displayHeight
                                );

    pUcObj->Display_GrpxPrm.rtParams.tarWidth       = displayWidth;
    pUcObj->Display_GrpxPrm.rtParams.tarHeight      = displayHeight;
    pUcObj->Display_GrpxPrm.rtParams.posX           = 0;
    pUcObj->Display_GrpxPrm.rtParams.posY           = 0;
    pUcObj->Display_GrpxPrm.displayId               = DISPLAY_LINK_INST_DSS_GFX1;


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
 *          to Start all the required links . Links are started in reverce
 *          order as information of next link is required to connect.
 *          System_linkStart is called with LinkId to start the links.
 *
 * \param   pObj  [IN] chains_vipSingleCam_SgxDisplayAppObj
 *
 *
 *******************************************************************************
*/
Void chains_vipSingleCamConnectorLinksA15SgxDisplay_StartApp(chains_vipSingleCamConnectorLinksA15SgxDisplayAppObj *pObj)
{
    ChainsCommon_statCollectorReset();
    ChainsCommon_memPrintHeapStatus();

    chains_vipSingleCamConnectorLinksA15SgxDisplay_Start(&pObj->ucObj);

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
Void chains_vipSingleCamConnectorLinksA15SgxDisplay_StopApp(chains_vipSingleCamConnectorLinksA15SgxDisplayAppObj *pObj)
{

    chains_vipSingleCamConnectorLinksA15SgxDisplay_Stop(&pObj->ucObj);

    chains_vipSingleCamConnectorLinksA15SgxDisplay_Delete(&pObj->ucObj);

    ChainsCommon_StopDisplayCtrl();

    ChainsCommon_prfLoadCalcEnable(FALSE, FALSE, FALSE);

}

/**
 *******************************************************************************
 *
 * \brief   Single Channel Capture Frame Copy Display usecase function
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
Void chains_vipSingleCamConnectorLinksA15SgxDisplay(Chains_Ctrl *chainsCfg)
{
    char ch;
    UInt32 done = FALSE;
    chains_vipSingleCamConnectorLinksA15SgxDisplayAppObj chainsObj;

    chainsObj.chainsCfg = chainsCfg;

    chains_vipSingleCamConnectorLinksA15SgxDisplay_Create(&chainsObj.ucObj, &chainsObj);

    chains_vipSingleCamConnectorLinksA15SgxDisplay_StartApp(&chainsObj);

    while(!done)
    {
        ch = Chains_menuRunTime();

        switch(ch)
        {
            case '0':
                done = TRUE;
                break;
            case '1':
                /* Send command to toggle operation status */
                System_linkControl(
                        chainsObj.ucObj.Gate_1LinkID,
                        GATE_LINK_CMD_SET_OPERATION_MODE_ON,
                        NULL,
                        0,
                        TRUE);
                break;
            case '2':
                /* Send command to toggle operation status */
                System_linkControl(
                        chainsObj.ucObj.Gate_1LinkID,
                        GATE_LINK_CMD_SET_OPERATION_MODE_OFF,
                        NULL,
                        0,
                        TRUE);
                break;
            case 'p':
            case 'P':
                ChainsCommon_prfCpuLoadPrint();
                ChainsCommon_statCollectorPrint();
                chains_vipSingleCamConnectorLinksA15SgxDisplay_printStatistics(&chainsObj.ucObj);
                chains_vipSingleCamConnectorLinksA15SgxDisplay_printBufferStatistics(&chainsObj.ucObj);
                break;
            default:
                Vps_printf("\nUnsupported option '%c'. Please try again\n", ch);
                break;
        }
    }

    chains_vipSingleCamConnectorLinksA15SgxDisplay_StopApp(&chainsObj);

}

