/*
*******************************************************************************
*
* Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/
* ALL RIGHTS RESERVED
*
*******************************************************************************
*/

/*
*******************************************************************************
*
* IMPORTANT NOTE:
*  This file is AUTO-GENERATED by Vision SDK use case generation tool
*
*******************************************************************************
*/
#include "chains_networkTxCapture_priv.h"
Void chains_networkTxCapture_SetLinkId(chains_networkTxCaptureObj *pObj){
       pObj->CaptureLinkID                  = SYSTEM_LINK_ID_CAPTURE;
       pObj->DupLinkID                      = IPU1_0_LINK (SYSTEM_LINK_ID_DUP_0);
       pObj->IPCOut_IPU1_0_A15_0_0LinkID    = IPU1_0_LINK (SYSTEM_LINK_ID_IPC_OUT_0);
       pObj->IPCIn_A15_0_IPU1_0_0LinkID     = A15_0_LINK (SYSTEM_LINK_ID_IPC_IN_0);
       pObj->NullLinkID                     = A15_0_LINK (SYSTEM_LINK_ID_NULL_0);
       pObj->Display_VideoLinkID            = SYSTEM_LINK_ID_DISPLAY_0;
       pObj->GrpxSrcLinkID                  = IPU1_0_LINK (SYSTEM_LINK_ID_GRPX_SRC_0);
       pObj->Display_GrpxLinkID             = SYSTEM_LINK_ID_DISPLAY_1;
}

Void chains_networkTxCapture_ResetLinkPrms(chains_networkTxCaptureObj *pObj){
       CaptureLink_CreateParams_Init(&pObj->CapturePrm);
       DupLink_CreateParams_Init(&pObj->DupPrm);
       IpcLink_CreateParams_Init(&pObj->IPCOut_IPU1_0_A15_0_0Prm);
       IpcLink_CreateParams_Init(&pObj->IPCIn_A15_0_IPU1_0_0Prm);
       NullLink_CreateParams_Init (&pObj->NullPrm);
       DisplayLink_CreateParams_Init(&pObj->Display_VideoPrm);
       GrpxSrcLink_CreateParams_Init(&pObj->GrpxSrcPrm);
       DisplayLink_CreateParams_Init(&pObj->Display_GrpxPrm);
}

Void chains_networkTxCapture_SetPrms(chains_networkTxCaptureObj *pObj){
       (pObj->DupPrm).numOutQue = 2;
       (pObj->NullPrm).numInQue = 1;
}

Void chains_networkTxCapture_ConnectLinks(chains_networkTxCaptureObj *pObj){

       //Capture -> Dup
       pObj->CapturePrm.outQueParams.nextLink = pObj->DupLinkID;
       pObj->DupPrm.inQueParams.prevLinkId = pObj->CaptureLinkID;
       pObj->DupPrm.inQueParams.prevLinkQueId = 0;

       //Dup -> Display_Video
       pObj->DupPrm.outQueParams[0].nextLink = pObj->Display_VideoLinkID;
       pObj->Display_VideoPrm.inQueParams.prevLinkId = pObj->DupLinkID;
       pObj->Display_VideoPrm.inQueParams.prevLinkQueId = 0;

       //Dup -> IPCOut_IPU1_0_A15_0_0
       pObj->DupPrm.outQueParams[1].nextLink = pObj->IPCOut_IPU1_0_A15_0_0LinkID;
       pObj->IPCOut_IPU1_0_A15_0_0Prm.inQueParams.prevLinkId = pObj->DupLinkID;
       pObj->IPCOut_IPU1_0_A15_0_0Prm.inQueParams.prevLinkQueId = 1;

       //IPCOut_IPU1_0_A15_0_0 -> IPCIn_A15_0_IPU1_0_0
       pObj->IPCOut_IPU1_0_A15_0_0Prm.outQueParams.nextLink = pObj->IPCIn_A15_0_IPU1_0_0LinkID;
       pObj->IPCIn_A15_0_IPU1_0_0Prm.inQueParams.prevLinkId = pObj->IPCOut_IPU1_0_A15_0_0LinkID;
       pObj->IPCIn_A15_0_IPU1_0_0Prm.inQueParams.prevLinkQueId = 0;

       //IPCIn_A15_0_IPU1_0_0 -> Null
       pObj->IPCIn_A15_0_IPU1_0_0Prm.outQueParams.nextLink = pObj->NullLinkID;
       pObj->NullPrm.inQueParams[0].prevLinkId = pObj->IPCIn_A15_0_IPU1_0_0LinkID;
       pObj->NullPrm.inQueParams[0].prevLinkQueId = 0;

       //GrpxSrc -> Display_Grpx
       pObj->GrpxSrcPrm.outQueParams.nextLink = pObj->Display_GrpxLinkID;
       pObj->Display_GrpxPrm.inQueParams.prevLinkId = pObj->GrpxSrcLinkID;
       pObj->Display_GrpxPrm.inQueParams.prevLinkQueId = 0;

}

Int32 chains_networkTxCapture_Create(chains_networkTxCaptureObj *pObj, Void *appObj){

       Int32 status;

       chains_networkTxCapture_SetLinkId(pObj);
       chains_networkTxCapture_ResetLinkPrms(pObj);

       chains_networkTxCapture_SetPrms(pObj);
       chains_networkTxCapture_SetAppPrms(pObj, appObj);

       chains_networkTxCapture_ConnectLinks(pObj);
       status = System_linkCreate(pObj->CaptureLinkID, &pObj->CapturePrm, sizeof(pObj->CapturePrm));
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       status = System_linkCreate(pObj->DupLinkID, &pObj->DupPrm, sizeof(pObj->DupPrm));
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       status = System_linkCreate(pObj->IPCOut_IPU1_0_A15_0_0LinkID, &pObj->IPCOut_IPU1_0_A15_0_0Prm, sizeof(pObj->IPCOut_IPU1_0_A15_0_0Prm));
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       status = System_linkCreate(pObj->IPCIn_A15_0_IPU1_0_0LinkID, &pObj->IPCIn_A15_0_IPU1_0_0Prm, sizeof(pObj->IPCIn_A15_0_IPU1_0_0Prm));
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       status = System_linkCreate(pObj->NullLinkID, &pObj->NullPrm, sizeof(pObj->NullPrm));
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       status = System_linkCreate(pObj->Display_VideoLinkID, &pObj->Display_VideoPrm, sizeof(pObj->Display_VideoPrm));
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       status = System_linkCreate(pObj->GrpxSrcLinkID, &pObj->GrpxSrcPrm, sizeof(pObj->GrpxSrcPrm));
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       status = System_linkCreate(pObj->Display_GrpxLinkID, &pObj->Display_GrpxPrm, sizeof(pObj->Display_GrpxPrm));
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       return status;
}

Int32 chains_networkTxCapture_Start(chains_networkTxCaptureObj *pObj){

       Int32 status;

       status = System_linkStart(pObj->Display_GrpxLinkID);
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       status = System_linkStart(pObj->GrpxSrcLinkID);
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       status = System_linkStart(pObj->Display_VideoLinkID);
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       status = System_linkStart(pObj->NullLinkID);
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       status = System_linkStart(pObj->IPCIn_A15_0_IPU1_0_0LinkID);
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       status = System_linkStart(pObj->IPCOut_IPU1_0_A15_0_0LinkID);
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       status = System_linkStart(pObj->DupLinkID);
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       status = System_linkStart(pObj->CaptureLinkID);
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       return status;
}

Int32 chains_networkTxCapture_Stop(chains_networkTxCaptureObj *pObj){

       Int32 status;

       status = System_linkStop(pObj->Display_GrpxLinkID);
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       status = System_linkStop(pObj->GrpxSrcLinkID);
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       status = System_linkStop(pObj->Display_VideoLinkID);
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       status = System_linkStop(pObj->NullLinkID);
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       status = System_linkStop(pObj->IPCIn_A15_0_IPU1_0_0LinkID);
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       status = System_linkStop(pObj->IPCOut_IPU1_0_A15_0_0LinkID);
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       status = System_linkStop(pObj->DupLinkID);
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       status = System_linkStop(pObj->CaptureLinkID);
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       return status;
}

Int32 chains_networkTxCapture_Delete(chains_networkTxCaptureObj *pObj){

       Int32 status;

       status = System_linkDelete(pObj->Display_GrpxLinkID);
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       status = System_linkDelete(pObj->GrpxSrcLinkID);
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       status = System_linkDelete(pObj->Display_VideoLinkID);
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       status = System_linkDelete(pObj->NullLinkID);
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       status = System_linkDelete(pObj->IPCIn_A15_0_IPU1_0_0LinkID);
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       status = System_linkDelete(pObj->IPCOut_IPU1_0_A15_0_0LinkID);
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       status = System_linkDelete(pObj->DupLinkID);
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       status = System_linkDelete(pObj->CaptureLinkID);
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       return status;
}

Void chains_networkTxCapture_printBufferStatistics(chains_networkTxCaptureObj *pObj){
       System_linkPrintBufferStatistics(pObj->CaptureLinkID);
       System_linkPrintBufferStatistics(pObj->DupLinkID);
       System_linkPrintBufferStatistics(pObj->IPCOut_IPU1_0_A15_0_0LinkID);
       Task_sleep(500);
       System_linkPrintBufferStatistics(pObj->IPCIn_A15_0_IPU1_0_0LinkID);
       System_linkPrintBufferStatistics(pObj->NullLinkID);
       Task_sleep(500);
       System_linkPrintBufferStatistics(pObj->Display_VideoLinkID);
       System_linkPrintBufferStatistics(pObj->GrpxSrcLinkID);
       System_linkPrintBufferStatistics(pObj->Display_GrpxLinkID);
       Task_sleep(500);
}

Void chains_networkTxCapture_printStatistics(chains_networkTxCaptureObj *pObj){
       System_linkPrintStatistics(pObj->CaptureLinkID);
       System_linkPrintStatistics(pObj->DupLinkID);
       System_linkPrintStatistics(pObj->IPCOut_IPU1_0_A15_0_0LinkID);
       Task_sleep(500);
       System_linkPrintStatistics(pObj->IPCIn_A15_0_IPU1_0_0LinkID);
       System_linkPrintStatistics(pObj->NullLinkID);
       Task_sleep(500);
       System_linkPrintStatistics(pObj->Display_VideoLinkID);
       System_linkPrintStatistics(pObj->GrpxSrcLinkID);
       System_linkPrintStatistics(pObj->Display_GrpxLinkID);
       Task_sleep(500);
}
