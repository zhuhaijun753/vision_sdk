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
#include "chains_nullSrc_Display_priv.h"
#include <linux/examples/tda2xx/include/chains.h>
#include <linux/examples/common/chains_common.h>


#define MAX_NUMBER_OF_CHANNELS    (1)
#define ENCDEC_MAX_FRAME_WIDTH    (1920)
#define ENCDEC_MAX_FRAME_HEIGHT   (1080)

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
typedef struct {

    chains_nullSrc_DisplayObj ucObj;

    UInt32  displayWidth;
    UInt32  displayHeight;

    Chains_Ctrl *chainsCfg;

    UInt32 codecType;
} Chains_NullSrcDisplayAppObj;

Void chains_nullSrcDisplay_SetNullSrcPrms(Chains_NullSrcDisplayAppObj *pObj,
                        NullSrcLink_CreateParams *pPrm)
{
    UInt32 chId;
    System_LinkChInfo *pChInfo;

    pPrm->outQueInfo.numCh = 1;

    pPrm->timerPeriodMilliSecs = 32;

    for (chId = 0; chId < pPrm->outQueInfo.numCh; chId++)
    {
        pPrm->channelParams[chId].numBuffers = 4;

        pChInfo = &pPrm->outQueInfo.chInfo[chId];

        pChInfo->flags = System_Link_Ch_Info_Set_Flag_Data_Format(pChInfo->flags,
                                                    SYSTEM_DF_YUV420SP_UV);
        pChInfo->flags = System_Link_Ch_Info_Set_Flag_Scan_Format(pChInfo->flags,
                                                    SYSTEM_SF_PROGRESSIVE);
        pChInfo->flags = System_Link_Ch_Info_Set_Flag_Buf_Type(pChInfo->flags,
                                               /*SYSTEM_BUFFER_TYPE_VIDEO_FRAME*/
                                               SYSTEM_BUFFER_TYPE_BITSTREAM);
        pChInfo->width = ENCDEC_MAX_FRAME_WIDTH;//pObj->displayWidth;
        pChInfo->height = ENCDEC_MAX_FRAME_HEIGHT;//pObj->displayHeight;
        pChInfo->startX = 0;
        pChInfo->startY = 0;
        pChInfo->pitch[0] = SystemUtils_align(pChInfo->width, 32);
        pChInfo->pitch[1] = SystemUtils_align(pChInfo->width, 32);

        pPrm->channelParams[chId].fileReadMode = NULLSRC_LINK_FILEREAD_RUN_TIME;

        strncpy(pPrm->channelParams[chId].nameDataFile, "/home/1920x1080_00.h264", 260);
        strncpy(pPrm->channelParams[chId].nameIndexFile, "/home/1920x1080_00.hdr", 260);
    }

    pPrm->dataRxMode = NULLSRC_LINK_DATA_RX_MODE_FILE;
}

/**
 *******************************************************************************
 *
 * \brief   Set Decode Create Parameters
 *
 * \param   pPrm         [IN]    DecodeLink_CreateParams
 *
 *******************************************************************************
*/
static Void chains_nullSrc_Display_SetDecodePrms(
                    Chains_NullSrcDisplayAppObj *pObj,
                    DecLink_CreateParams *pPrm)
{
    UInt32 chId;
    DecLink_ChCreateParams *decPrm;

    for (chId = 0; chId<MAX_NUMBER_OF_CHANNELS; chId++)
    {
        UTILS_assert (chId < DEC_LINK_MAX_CH);
        decPrm = &pPrm->chCreateParams[chId];

        decPrm->dpbBufSizeInFrames  = DEC_LINK_DPB_SIZE_IN_FRAMES_DEFAULT;
        decPrm->algCreateStatus     = DEC_LINK_ALG_CREATE_STATUS_CREATE;
        decPrm->decodeFrameType     = DEC_LINK_DECODE_ALL;

        decPrm->processCallLevel    = DEC_LINK_FRAMELEVELPROCESSCALL;
        decPrm->targetMaxWidth      = ENCDEC_MAX_FRAME_WIDTH;
        decPrm->targetMaxHeight     = ENCDEC_MAX_FRAME_HEIGHT;
        decPrm->numBufPerCh         = 6;
        decPrm->defaultDynamicParams.targetBitRate = 10*1000*1000;
        decPrm->defaultDynamicParams.targetFrameRate = 30;
        decPrm->fieldMergeDecodeEnable = FALSE;

        switch (pObj->codecType)
        {
            case '0': /* MJPEG */
                decPrm->format = SYSTEM_IVIDEO_MJPEG;
                decPrm->profile = 0;
                decPrm->displayDelay = 0;
                break;

            case '1': /* H264 */
                decPrm->format = SYSTEM_IVIDEO_H264HP;
                decPrm->profile = 3;
                decPrm->displayDelay = 0;
                break;

            default: /* D1 */
                printf("\r\nCodec Type: %d, returning \n", pObj->codecType);
                UTILS_assert(FALSE);
                break;
        }
    }
}

/**
 *******************************************************************************
 *
 * \brief   Set VPE Create Parameters
 *
 * \param   pPrm    [OUT]    VpeLink_CreateParams
 *
 *******************************************************************************
*/
static Void chains_nullSrc_Display_SetVPEPrms(
                    VpeLink_CreateParams *pPrm,
                    UInt32 numCh,
                    UInt32 displayWidth,
                    UInt32 displayHeight,
                    UInt32 inputWidth,
                    UInt32 inputHeight
                    )
{
    UInt16 chId;

    pPrm->enableOut[0] = TRUE;

    for(chId = 0; chId < numCh; chId++)
    {
        pPrm->chParams[chId].outParams[0].numBufsPerCh = 4;

        pPrm->chParams[chId].outParams[0].width = displayWidth;
        pPrm->chParams[chId].outParams[0].height = displayHeight;
        pPrm->chParams[chId].outParams[0].dataFormat = SYSTEM_DF_YUV420SP_UV;

        pPrm->chParams[chId].scCfg.bypass       = FALSE;
        pPrm->chParams[chId].scCfg.nonLinear    = FALSE;
        pPrm->chParams[chId].scCfg.stripSize    = 0;

        pPrm->chParams[chId].scCropCfg.cropStartX = 32;
        pPrm->chParams[chId].scCropCfg.cropStartY = 24;
        pPrm->chParams[chId].scCropCfg.cropWidth = inputWidth-32;
        pPrm->chParams[chId].scCropCfg.cropHeight = inputHeight-24;
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
Void chains_nullSrc_Display_SetAppPrms(chains_nullSrc_DisplayObj *pUcObj, Void *appObj)
{
    Chains_NullSrcDisplayAppObj *pObj
        = (Chains_NullSrcDisplayAppObj*)appObj;

    ChainsCommon_GetDisplayWidthHeight(
        pObj->chainsCfg->displayType,
        &pObj->displayWidth,
        &pObj->displayHeight
        );


    chains_nullSrcDisplay_SetNullSrcPrms(pObj, &pUcObj->NullSourcePrm);

    chains_nullSrc_Display_SetDecodePrms(pObj, &pUcObj->DecodePrm);

    ChainsCommon_SetDisplayPrms(&pUcObj->DisplayPrm,
                                NULL,
                                pObj->chainsCfg->displayType,
                                pObj->displayWidth,
                                pObj->displayHeight
                               );

    chains_nullSrc_Display_SetVPEPrms(
        &pUcObj->VPEPrm,
        1,
        ENCDEC_MAX_FRAME_WIDTH,
        ENCDEC_MAX_FRAME_HEIGHT,
        ENCDEC_MAX_FRAME_WIDTH,
        ENCDEC_MAX_FRAME_HEIGHT);

    ChainsCommon_StartDisplayCtrl(
        pObj->chainsCfg->displayType,
        pObj->displayWidth,
        pObj->displayHeight
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
 * \param   pObj  [IN] Chains_VipSingleCameraViewObj
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
*/
Void chains_nullSrc_Display_StartApp(Chains_NullSrcDisplayAppObj *pObj)
{
    ChainsCommon_statCollectorReset();
    ChainsCommon_memPrintHeapStatus();

    chains_nullSrc_Display_Start(&pObj->ucObj);

    ChainsCommon_prfLoadCalcEnable(TRUE, FALSE, FALSE);
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
 * \param   pObj   [IN]   Chains_VipSingleCameraViewObj
 *
 *******************************************************************************
*/
Void chains_nullSrc_Display_StopAndDeleteApp(Chains_NullSrcDisplayAppObj *pObj)
{
    chains_nullSrc_Display_Stop(&pObj->ucObj);
    chains_nullSrc_Display_Delete(&pObj->ucObj);

    ChainsCommon_StopDisplayCtrl();

    /* Print the HWI, SWI and all tasks load */
    /* Reset the accumulated timer ticks */
    ChainsCommon_prfLoadCalcEnable(FALSE, TRUE, TRUE);
}

/**
 *******************************************************************************
 * \brief Run Time Menu string for codec Type Selection.
 *******************************************************************************
 */
char chains_nullSrc_Display_codecTypeSelect[] = {
    "\r\n "
    "\r\n ========================================="
    "\r\n Chains Run-time Codec Type Selection Menu"
    "\r\n ========================================="
    "\r\n "
    "\r\n Enter '0' for MJPEG "
    "\r\n "
    "\r\n Enter '1' for H.264 "
    "\r\n "
    "\r\n Enter Choice: "
    "\r\n "
};

/**
 *******************************************************************************
 *
 * \brief   Single Channel Capture Display usecase function
 *`
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
Void Chains_nullSrc_DecodeDisplay(Chains_Ctrl *chainsCfg)
{
    char ch = 'a';
    UInt32 done = FALSE;
    Chains_NullSrcDisplayAppObj chainsObj;

    chainsObj.chainsCfg = chainsCfg;

    while(!done)
    {
        Vps_printf(chains_nullSrc_Display_codecTypeSelect);
        chainsObj.codecType = Chains_readChar();

        switch(chainsObj.codecType)
        {
            case '0':
                done = TRUE;
                break;
            case '1':
                done = TRUE;
                break;
            default:
                Vps_printf("\nUnsupported option '%c'. Please try again\n",
                           chainsObj.codecType);
                break;
        }
    }

    chains_nullSrc_Display_Create(&chainsObj.ucObj, &chainsObj);

    chains_nullSrc_Display_StartApp(&chainsObj);

    done = FALSE;
    while(!done)
    {
        ch = '1';
        ch = Chains_menuRunTime();
        switch(ch)
        {
            case '0':
                done = TRUE;
                break;

            case 'p':
            case 'P':
                ChainsCommon_PrintStatistics();
                chains_nullSrc_Display_printStatistics(&chainsObj.ucObj);
                break;
            default:
                Vps_printf("\nUnsupported option '%c'. Please try again\n", ch);
                break;
        }
    }

    chains_nullSrc_Display_StopAndDeleteApp(&chainsObj);
}

