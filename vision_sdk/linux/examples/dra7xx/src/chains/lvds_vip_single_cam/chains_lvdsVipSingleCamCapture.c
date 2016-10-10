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
 * \file chains_vipSingleCameraView.c
 *
 * \brief  Usecase file implementation of capture display usecase.
 *
 *         Usecase file for single camere view usecase.
 *
 *         Capture --> EpSink Link(A15)
 *
 *
 *         In this use-case we capture 1 CH of video from OV1063x 720p30
 *         and send it to A15 using IPC_OUT, IPC_IN. A15 is running SgxDisplay
 *         Link which will render the frames and display via DRM
 *
 *         The data flow daigram is shown below
 *
 *             Capture (VIP) 1CH 30fps 1280x720 or 60fp 1920x1080
 *                   |
 *                   |
 *                 IPC OUT
 *                   |
 *                 IPC IN
 *                   |
 *               EpSink (A15)
 *
 * \version 0.0 (Jun 2014) : [YM] First version ported for linux.
 *
 *******************************************************************************
*/

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */

#include "chains_lvdsVipSingleCamCapture_priv.h"
#include <linux/examples/common/chains_common.h>
#include <vivi_preview.h>
#include "../../ti-plugins/include/preview_chain.h"

#define CAPTURE_SENSOR_WIDTH      (1280)
#define CAPTURE_SENSOR_HEIGHT     (720)

/**
 *******************************************************************************
 *
 *  \brief  SingleCameraViewObject
 *
 *        This structure contains all the LinksId's and create Params.
 *        The same is passed to all create, start, stop functions.
 *
 *******************************************************************************
*/
typedef struct {

    chains_lvdsVipSingleCamCaptureObj ucObj;

    UInt32  captureOutWidth;
    UInt32  captureOutHeight;

}chains_lvdsVipSingleCamCaptureAppObj;

chains_lvdsVipSingleCamCaptureAppObj gLvdsVipSingleCamObj;

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
Void chains_lvdsVipSingleCamCapture_SetAppPrms(chains_lvdsVipSingleCamCaptureObj *pUcObj,Void *appObj)
{
   EpLink_CreateParams  *pEndp;
   chains_lvdsVipSingleCamCaptureAppObj *pObj
            = (chains_lvdsVipSingleCamCaptureAppObj *)appObj;

    pObj->captureOutWidth  = CAPTURE_SENSOR_WIDTH;
    pObj->captureOutHeight = CAPTURE_SENSOR_HEIGHT;

    ChainsCommon_SingleCam_SetCapturePrms(&(pUcObj->CapturePrm),
            CAPTURE_SENSOR_WIDTH,
            CAPTURE_SENSOR_HEIGHT,
            pObj->captureOutWidth,
            pObj->captureOutHeight,
            CHAINS_CAPTURE_SRC_OV10635
            );
    /* We have one epLink in the chain; we will configure it now */
    /* sink1 */
    pEndp = &pUcObj->EpSinkPrm;
    pEndp->epType = EP_SINK;
    pEndp->chainId = PREVIEW_LVDS_SINGLE_CAM;
    strcpy(pEndp->plugName, PREVIEW_PLUGIN_NAME);
}

Void chains_lvdsVipSingleCamCapture_InitApp(Void)
{
    chains_lvdsVipSingleCamCaptureAppObj *pObj
        = (chains_lvdsVipSingleCamCaptureAppObj *)&gLvdsVipSingleCamObj;

    chains_lvdsVipSingleCamCapture_SetLinkId(&pObj->ucObj);
}

Int32 chains_lvdsVipSingleCamCapture_CreateApp(struct vivi_sink *sink, struct vivi_source *source)
{
    chains_lvdsVipSingleCamCaptureObj *pUcObj;

    chains_lvdsVipSingleCamCapture_InitApp();

    /* Let's set the epLink ids now */
    pUcObj = &gLvdsVipSingleCamObj.ucObj;
    sink->eplink_id[0] = pUcObj->EpSinkLinkID;

    return (chains_lvdsVipSingleCamCapture_Create(&gLvdsVipSingleCamObj.ucObj, &gLvdsVipSingleCamObj));
}

Int32 chains_lvdsVipSingleCamCapture_StartApp(Void)
{
    chains_lvdsVipSingleCamCaptureAppObj *pObj
        = (chains_lvdsVipSingleCamCaptureAppObj *)&gLvdsVipSingleCamObj;

    return (chains_lvdsVipSingleCamCapture_Start(&pObj->ucObj));
}

Int32 chains_lvdsVipSingleCamCapture_StopApp(Void)
{
    chains_lvdsVipSingleCamCaptureAppObj *pObj
        = (chains_lvdsVipSingleCamCaptureAppObj *)&gLvdsVipSingleCamObj;

    return (chains_lvdsVipSingleCamCapture_Stop(&pObj->ucObj));
}

Int32 chains_lvdsVipSingleCamCapture_DeleteApp(Void)
{
    chains_lvdsVipSingleCamCaptureAppObj *pObj
        = (chains_lvdsVipSingleCamCaptureAppObj *)&gLvdsVipSingleCamObj;

    return (chains_lvdsVipSingleCamCapture_Delete(&pObj->ucObj));
}
