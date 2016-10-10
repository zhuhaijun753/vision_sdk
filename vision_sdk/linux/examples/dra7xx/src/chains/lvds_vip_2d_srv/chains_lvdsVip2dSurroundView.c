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
#include "chains_lvdsVip2dSurroundView_priv.h"
#include <linux/examples/common/chains_common.h>
#include <vivi_srv.h>
#include "../../ti-plugins/include/srv_chain.h"
#include "osa_file.h"
#include <osa_mem.h>
#include <linux/src/system/system_priv_ipc.h>

#define SYNTH_ALG_2D_SRV_LUT_FILENAME "/home/root/synthtable.bin"

/**
 *******************************************************************************
 *
 *  \brief  Use-case object
 *
 *        This structure contains all the LinksId's and create Params.
 *        The same is passed to all create, start, stop functions.
 *
 *******************************************************************************
*/
enum synctype {
    LOOSE_SYNC,
    TIGHT_SYNC
};


typedef struct {
    chains_lvdsVip2dSurroundViewObj ucObj;

    UInt32 num_captures;
    UInt32 num_buffers_per_capture;
    UInt32 portId[VIDEO_SENSOR_MAX_LVDS_CAMERAS];
    UInt32 capture_width;
    UInt32 capture_height;

    UInt32 svInWidth;
    UInt32 svInHeight;
    UInt32 svOutWidth;
    UInt32 svOutHeight;
    UInt32 svCarImageWidth;
    UInt32 svCarImageHeight;
    UInt32 svNumViews;
    UInt32 svOutputMode;
    Chain_Common_SRV_CalibParams gaCalibPrm;
    UInt32 enableCarOverlayInAlg;
    UInt32 synthLUTSize;
    UInt8 *synthLUTPtr;
    /**< Set to 1, if DSP need to create the car image, apply only for 2D SRV */
} Chains_LvdsVip2dSurroundViewAppObj;

Chains_LvdsVip2dSurroundViewAppObj gLvdsVip2dSurroundViewObj;

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
Void chains_lvdsVip2dSurroundView_SetAppPrms(
            chains_lvdsVip2dSurroundViewObj *pUcObj, Void *appObj)
{
    UInt32 i;
    CaptureLink_CreateParams  *pVipCapture;
    EpLink_CreateParams  *pEndp;
    Chains_LvdsVip2dSurroundViewAppObj *pObj;
    UInt32 tmpReadSize=0;
    UInt8 *virt_addr;
    int osa_ret=OSA_SOK;

    pObj = (Chains_LvdsVip2dSurroundViewAppObj*)appObj;
    pVipCapture = &pUcObj->CapturePrm;

    ChainsCommon_MultiCam_SetCapturePrms(&pUcObj->CapturePrm,
                             pObj->num_captures);

    for (i=0; i<SYSTEM_CAPTURE_VIP_INST_MAX; i++) {
        pVipCapture->vipInst[i].numBufs = pObj->num_buffers_per_capture;
    }

    ChainsCommon_SurroundView_SetSyncPrm(&pUcObj->Sync_svPrm, pObj->num_captures,
                                              TIGHT_SYNC);

    ChainsCommon_SurroundView_SetSynthParams(&pUcObj->Alg_SynthesisPrm,
                                              pObj->svInWidth, pObj->svInHeight,
                                              pObj->svOutWidth, pObj->svOutHeight,
                                              pObj->svNumViews,
                                              pObj->svCarImageWidth,
                                              pObj->svCarImageHeight,
                                              pObj->svOutputMode,
                                              pObj->enableCarOverlayInAlg);

    UTILS_assert(pUcObj->Alg_SynthesisPrm.synthLUTPtr != NULL);
    pObj->synthLUTPtr = pUcObj->Alg_SynthesisPrm.synthLUTPtr;
    pObj->synthLUTSize = pUcObj->Alg_SynthesisPrm.synthLUTSize;

    virt_addr = (UInt8 *)OSA_memPhys2Virt((unsigned int)pObj->synthLUTPtr,
                      OSA_MEM_REGION_TYPE_AUTO);
    osa_ret = OSA_fileReadFile(SYNTH_ALG_2D_SRV_LUT_FILENAME,
                   virt_addr,
                   pObj->synthLUTSize,&tmpReadSize);
    pUcObj->Alg_SynthesisPrm.useSuppliedLUT = 0;
    pUcObj->Alg_SynthesisPrm.saveCalculatedLUT = 1;
    if (OSA_SOK == osa_ret) {
        if(tmpReadSize == pObj->synthLUTSize) {
        OSA_memCacheWb((unsigned int)virt_addr,
                ((unsigned int)virt_addr)+pObj->synthLUTSize);
        OSA_memCacheInv((unsigned int)virt_addr, pObj->synthLUTSize);

            pUcObj->Alg_SynthesisPrm.useSuppliedLUT = 1;
            pUcObj->Alg_SynthesisPrm.saveCalculatedLUT = 0;
        }
    } else {
        OSA_memCacheInv((unsigned int)virt_addr, pObj->synthLUTSize);
    }

    ChainsCommon_SurroundView_SetGAlignParams(&pUcObj->Alg_GeoAlignPrm,
                                              pObj->svInWidth, pObj->svInHeight,
                                              pObj->svOutWidth, pObj->svOutHeight,
                                              pObj->svNumViews,
                                              pObj->svCarImageWidth,
                                              pObj->svCarImageHeight,
                                              pObj->svOutputMode,
                                              &pObj->gaCalibPrm);

    ChainsCommon_SurroundView_SetPAlignParams(&pUcObj->Alg_PhotoAlignPrm,
                                              pObj->svInWidth, pObj->svInHeight,
                                              pObj->svOutWidth, pObj->svOutHeight,
                                              pObj->svNumViews,
                                              pObj->svCarImageWidth,
                                              pObj->svCarImageHeight,
                                              pObj->svOutputMode);

    /* We have two epLinks in the chain; we will configure them now */
    /* sink2 */
    pEndp = &pUcObj->epSink_2Prm;
    pEndp->epType = EP_SINK;
    pEndp->chainId = LVDS_SRV_2D_880x1080;
    strcpy(pEndp->plugName, SRV_PLUGIN_NAME);

    /* sink1 */
    pEndp = &pUcObj->epSink_1Prm;
    pEndp->epType = EP_SINK;
    pEndp->chainId = LVDS_SRV_2D_880x1080;
    strcpy(pEndp->plugName, SRV_PLUGIN_NAME);
}

Void chains_lvdsVip2dSurroundView_saveLUT()
{
    Chains_LvdsVip2dSurroundViewAppObj *pObj;
    chains_lvdsVip2dSurroundViewObj *pUcObj;
    UInt8 *virt_addr = NULL;

    pObj = (Chains_LvdsVip2dSurroundViewAppObj*)&gLvdsVip2dSurroundViewObj;
    pUcObj = &pObj->ucObj;

    UTILS_assert(pObj->synthLUTPtr != NULL);
    virt_addr = (UInt8 *)OSA_memPhys2Virt((unsigned int)pObj->synthLUTPtr,
                          OSA_MEM_REGION_TYPE_AUTO);
    printf("Virtual address is 0x%08x\n",(unsigned int)virt_addr);
    if(pUcObj->Alg_SynthesisPrm.saveCalculatedLUT==1) {
        OSA_fileWriteFile(SYNTH_ALG_2D_SRV_LUT_FILENAME,
                  virt_addr,
                  pObj->synthLUTSize);
    }
}

#define SV2D_CARBOX_WIDTH 190
#define SV2D_CARBOX_HEIGHT 360

Void chains_lvdsVip2dSurroundView_InitApp()
{
    Bool startWithCalibration, userGaSgxLut;
    Chains_LvdsVip2dSurroundViewAppObj *pObj
        = (Chains_LvdsVip2dSurroundViewAppObj*)&gLvdsVip2dSurroundViewObj;
    Chain_Common_SRV_CalibParams * gaCalibPrm = &gLvdsVip2dSurroundViewObj.gaCalibPrm;

    /* Capture related initialization */
    pObj->num_captures = 4;
    pObj->num_buffers_per_capture = 6;
    pObj->capture_width = LVDS_CAPTURE_WIDTH;
    pObj->capture_height = LVDS_CAPTURE_HEIGHT;

    pObj->portId[0] = SYSTEM_CAPTURE_INST_VIP1_SLICE1_PORTA;
    pObj->portId[1] = SYSTEM_CAPTURE_INST_VIP1_SLICE2_PORTA;
    pObj->portId[2] = SYSTEM_CAPTURE_INST_VIP2_SLICE1_PORTA;
    pObj->portId[3] = SYSTEM_CAPTURE_INST_VIP3_SLICE1_PORTA;
    pObj->portId[4] = SYSTEM_CAPTURE_INST_VIP3_SLICE2_PORTA;
    pObj->portId[5] = SYSTEM_CAPTURE_INST_VIP2_SLICE2_PORTB;

    /* SRV Algo related initialization */
    pObj->svInWidth = SV_INPUT_WIDTH;
    pObj->svInHeight = SV_INPUT_HEIGHT;
    pObj->svOutWidth = 880;
    pObj->svOutHeight = 1080;
    pObj->svCarImageWidth = SV2D_CARBOX_WIDTH;
    pObj->svCarImageHeight = SV2D_CARBOX_HEIGHT;
    pObj->svNumViews = SV_NUM_VIEWS;
    pObj->svOutputMode = ALGORITHM_LINK_SRV_OUTPUT_2D;
    pObj->enableCarOverlayInAlg = 1;

    /* Set startWithCalibration = TRUE to start the demo with calibration.
       Else it will use the previously calibrated LUTs */
    startWithCalibration = TRUE;
    userGaSgxLut = FALSE;
    ChainsCommon_SurroundView_InitCalibration(gaCalibPrm, startWithCalibration, userGaSgxLut, pObj->svOutputMode);
}

Int32 chains_lvdsVip2dSurroundView_CreateApp(struct vivi_sink *sink, struct vivi_source *source)
{
    Int32 status;
    chains_lvdsVip2dSurroundViewObj *pUcObj;

    chains_lvdsVip2dSurroundView_InitApp();
    status = chains_lvdsVip2dSurroundView_Create(&gLvdsVip2dSurroundViewObj.ucObj, &gLvdsVip2dSurroundViewObj);

    chains_lvdsVip2dSurroundView_saveLUT();
    /* Let's set the epLink ids now */
    pUcObj = &gLvdsVip2dSurroundViewObj.ucObj;
    sink->eplink_id[0] = pUcObj->epSink_2LinkID;
    sink->eplink_id[1] = pUcObj->epSink_1LinkID;

    return status;
}

Int32 chains_lvdsVip2dSurroundView_StartApp()
{
    Int32 status;

    ChainsCommon_memPrintHeapStatus();

    status = chains_lvdsVip2dSurroundView_Start(&gLvdsVip2dSurroundViewObj.ucObj);

    ChainsCommon_prfLoadCalcEnable(TRUE, FALSE, FALSE);

    return status;
}

Int32 chains_lvdsVip2dSurroundView_StopApp()
{
    Int32 status;

    status = chains_lvdsVip2dSurroundView_Stop(&gLvdsVip2dSurroundViewObj.ucObj);

    ChainsCommon_prfLoadCalcEnable(FALSE, TRUE, TRUE);

    return status;
}

Int32 chains_lvdsVip2dSurroundView_DeleteApp()
{
    Int32 status;
    Chain_Common_SRV_CalibParams *gaCalibPrm = &gLvdsVip2dSurroundViewObj.gaCalibPrm;
    Chains_LvdsVip2dSurroundViewAppObj *pObj;

    pObj = (Chains_LvdsVip2dSurroundViewAppObj*)&gLvdsVip2dSurroundViewObj;

    System_ipcMemFree(OSA_HEAPID_DDR_CACHED_SR1,
              (UInt32) pObj->synthLUTPtr,
              pObj->synthLUTSize);

    status = chains_lvdsVip2dSurroundView_Delete(&gLvdsVip2dSurroundViewObj.ucObj);

    ChainsCommon_SurroundView_StopCalibration(gaCalibPrm, pObj->svOutputMode);

    return status;
}

Int32 chains_lvdsVip2dSurroundView_PauseApp()
{
    Int32 status;
    chains_lvdsVip2dSurroundViewObj *pObj = &gLvdsVip2dSurroundViewObj.ucObj;

    status = System_linkStop(pObj->CaptureLinkID);
    UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

    return status;
}

Int32 chains_lvdsVip2dSurroundView_ResumeApp()
{
    Int32 status;
    chains_lvdsVip2dSurroundViewObj *pObj = &gLvdsVip2dSurroundViewObj.ucObj;

    status = System_linkStart(pObj->CaptureLinkID);
    UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

    return status;
}

Void chains_lvdsVip2dSurroundView_CalibApp()
{
    ChainsCommon_SurroundView_MenuCalibration(&gLvdsVip2dSurroundViewObj.gaCalibPrm, ALGORITHM_LINK_SRV_OUTPUT_2D);
}

Void chains_lvdsVip2dSurroundView_StatApp()
{
    chains_lvdsVip2dSurroundView_printStatistics(&gLvdsVip2dSurroundViewObj.ucObj);
    ChainsCommon_memPrintHeapStatus();
}
