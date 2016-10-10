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
 * \file chains_lvdsVipMultiCamCapture.c
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
 *             Capture (VIP) 4CH 30fps 1280x720
 *                   |
 *                 Sync
 *                   |
 *                 IPC OUT
 *                   |
 *                 IPC IN
 *                   |
 *               EpSink (A15)
 *
 *
 *******************************************************************************
*/

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */

#include "chains_lvdsVipMultiCamCapture_priv.h"
#include <linux/examples/common/chains_common.h>
#include <vivi_preview.h>
#include "../../ti-plugins/include/preview_chain.h"

#define CAPTURE_SENSOR_WIDTH      (1280)
#define CAPTURE_SENSOR_HEIGHT     (720)

#define SYNC_DELTA_IN_MSEC                   (16)
#define SYNC_DROP_THRESHOLD_IN_MSEC          (33)

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

    chains_lvdsVipMultiCamCaptureObj ucObj;

    UInt32  num_captures;
    UInt32  captureOutWidth;
    UInt32  captureOutHeight;

}chains_lvdsVipMultiCamCaptureAppObj;

chains_lvdsVipMultiCamCaptureAppObj gLvdsVipMultiCamObj;

static Void chains_lvdsVipMultiCam_SetSyncPrm(SyncLink_CreateParams *pPrm)
{
    pPrm->syncDelta = SYNC_DELTA_IN_MSEC;
    pPrm->syncThreshold = SYNC_DROP_THRESHOLD_IN_MSEC;
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
Void chains_lvdsVipMultiCamCapture_SetAppPrms(chains_lvdsVipMultiCamCaptureObj *pUcObj,Void *appObj)
{
   EpLink_CreateParams  *pEndp;
   chains_lvdsVipMultiCamCaptureAppObj *pObj
            = (chains_lvdsVipMultiCamCaptureAppObj *)appObj;

    pObj->captureOutWidth  = CAPTURE_SENSOR_WIDTH;
    pObj->captureOutHeight = CAPTURE_SENSOR_HEIGHT;

    ChainsCommon_MultiCam_SetCapturePrms(
                    &pUcObj->CapturePrm,
                    pObj->num_captures);
    chains_lvdsVipMultiCam_SetSyncPrm(&pUcObj->SyncPrm);
    /* We have one epLink in the chain; we will configure it now */
    /* sink1 */
    pEndp = &pUcObj->EpSinkPrm;
    pEndp->epType = EP_SINK;
    pEndp->chainId = PREVIEW_LVDS_MULTI_CAM;
    strcpy(pEndp->plugName, PREVIEW_PLUGIN_NAME);
}

Void chains_lvdsVipMultiCamCapture_InitApp(Void)
{
    chains_lvdsVipMultiCamCaptureAppObj *pObj
        = (chains_lvdsVipMultiCamCaptureAppObj *)&gLvdsVipMultiCamObj;

    pObj->num_captures = 4;
    chains_lvdsVipMultiCamCapture_SetLinkId(&pObj->ucObj);
}

Int32 chains_lvdsVipMultiCamCapture_CreateApp(struct vivi_sink *sink, struct vivi_source *source)
{
    chains_lvdsVipMultiCamCaptureObj *pUcObj;

    chains_lvdsVipMultiCamCapture_InitApp();

    /* Let's set the epLink ids now */
    pUcObj = &gLvdsVipMultiCamObj.ucObj;
    sink->eplink_id[0] = pUcObj->EpSinkLinkID;

    return (chains_lvdsVipMultiCamCapture_Create(&gLvdsVipMultiCamObj.ucObj, &gLvdsVipMultiCamObj));
}

Int32 chains_lvdsVipMultiCamCapture_StartApp(Void)
{
    chains_lvdsVipMultiCamCaptureAppObj *pObj
        = (chains_lvdsVipMultiCamCaptureAppObj *)&gLvdsVipMultiCamObj;

    return (chains_lvdsVipMultiCamCapture_Start(&pObj->ucObj));
}

Int32 chains_lvdsVipMultiCamCapture_StopApp(Void)
{
    chains_lvdsVipMultiCamCaptureAppObj *pObj
        = (chains_lvdsVipMultiCamCaptureAppObj *)&gLvdsVipMultiCamObj;

    return (chains_lvdsVipMultiCamCapture_Stop(&pObj->ucObj));
}

Int32 chains_lvdsVipMultiCamCapture_DeleteApp(Void)
{
    chains_lvdsVipMultiCamCaptureAppObj *pObj
        = (chains_lvdsVipMultiCamCaptureAppObj *)&gLvdsVipMultiCamObj;

    return (chains_lvdsVipMultiCamCapture_Delete(&pObj->ucObj));
}
