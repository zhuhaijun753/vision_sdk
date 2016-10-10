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
#include "chains_vipSingleCameraOpenCLCannyEdge_priv.h"
Void chains_vipSingleCameraOpenCLCannyEdge_SetLinkId(chains_vipSingleCameraOpenCLCannyEdgeObj *pObj){
       pObj->CaptureLinkID                  = SYSTEM_LINK_ID_CAPTURE_0;
       pObj->IPCOut_IPU1_0_A15_0_0LinkID    = IPU1_0_LINK (SYSTEM_LINK_ID_IPC_OUT_0);
       pObj->IPCIn_A15_0_IPU1_0_0LinkID     = A15_0_LINK (SYSTEM_LINK_ID_IPC_IN_0);
       pObj->Alg_OpenCLCannyEdgeLinkID      = A15_0_LINK (SYSTEM_LINK_ID_ALG_0);
       pObj->IPCOut_A15_0_IPU1_0_0LinkID    = A15_0_LINK (SYSTEM_LINK_ID_IPC_OUT_0);
       pObj->IPCIn_IPU1_0_A15_0_0LinkID     = IPU1_0_LINK (SYSTEM_LINK_ID_IPC_IN_0);
       pObj->Display_VideoLinkID            = SYSTEM_LINK_ID_DISPLAY_0;
       pObj->GrpxSrcLinkID                  = IPU1_0_LINK (SYSTEM_LINK_ID_GRPX_SRC_0);
       pObj->Display_GrpxLinkID             = SYSTEM_LINK_ID_DISPLAY_1;
}

Void chains_vipSingleCameraOpenCLCannyEdge_ResetLinkPrms(chains_vipSingleCameraOpenCLCannyEdgeObj *pObj){
       CaptureLink_CreateParams_Init(&pObj->CapturePrm);
       IpcLink_CreateParams_Init(&pObj->IPCOut_IPU1_0_A15_0_0Prm);
       IpcLink_CreateParams_Init(&pObj->IPCIn_A15_0_IPU1_0_0Prm);
       AlgorithmLink_OpenCLCannyEdge_Init(&pObj->Alg_OpenCLCannyEdgePrm);
       IpcLink_CreateParams_Init(&pObj->IPCOut_A15_0_IPU1_0_0Prm);
       IpcLink_CreateParams_Init(&pObj->IPCIn_IPU1_0_A15_0_0Prm);
       DisplayLink_CreateParams_Init(&pObj->Display_VideoPrm);
       GrpxSrcLink_CreateParams_Init(&pObj->GrpxSrcPrm);
       DisplayLink_CreateParams_Init(&pObj->Display_GrpxPrm);
}

Void chains_vipSingleCameraOpenCLCannyEdge_SetPrms(chains_vipSingleCameraOpenCLCannyEdgeObj *pObj){
}

Void chains_vipSingleCameraOpenCLCannyEdge_ConnectLinks(chains_vipSingleCameraOpenCLCannyEdgeObj *pObj){

       //Capture -> IPCOut_IPU1_0_A15_0_0
       pObj->CapturePrm.outQueParams.nextLink = pObj->IPCOut_IPU1_0_A15_0_0LinkID;
       pObj->IPCOut_IPU1_0_A15_0_0Prm.inQueParams.prevLinkId = pObj->CaptureLinkID;
       pObj->IPCOut_IPU1_0_A15_0_0Prm.inQueParams.prevLinkQueId = 0;

       //IPCOut_IPU1_0_A15_0_0 -> IPCIn_A15_0_IPU1_0_0
       pObj->IPCOut_IPU1_0_A15_0_0Prm.outQueParams.nextLink = pObj->IPCIn_A15_0_IPU1_0_0LinkID;
       pObj->IPCIn_A15_0_IPU1_0_0Prm.inQueParams.prevLinkId = pObj->IPCOut_IPU1_0_A15_0_0LinkID;
       pObj->IPCIn_A15_0_IPU1_0_0Prm.inQueParams.prevLinkQueId = 0;

       //IPCIn_A15_0_IPU1_0_0 -> Alg_OpenCLCannyEdge
       pObj->IPCIn_A15_0_IPU1_0_0Prm.outQueParams.nextLink = pObj->Alg_OpenCLCannyEdgeLinkID;
       pObj->Alg_OpenCLCannyEdgePrm.inQueParams.prevLinkId = pObj->IPCIn_A15_0_IPU1_0_0LinkID;
       pObj->Alg_OpenCLCannyEdgePrm.inQueParams.prevLinkQueId = 0;

       //Alg_OpenCLCannyEdge -> IPCOut_A15_0_IPU1_0_0
       pObj->Alg_OpenCLCannyEdgePrm.outQueParams.nextLink = pObj->IPCOut_A15_0_IPU1_0_0LinkID;
       pObj->IPCOut_A15_0_IPU1_0_0Prm.inQueParams.prevLinkId = pObj->Alg_OpenCLCannyEdgeLinkID;
       pObj->IPCOut_A15_0_IPU1_0_0Prm.inQueParams.prevLinkQueId = 0;

       //IPCOut_A15_0_IPU1_0_0 -> IPCIn_IPU1_0_A15_0_0
       pObj->IPCOut_A15_0_IPU1_0_0Prm.outQueParams.nextLink = pObj->IPCIn_IPU1_0_A15_0_0LinkID;
       pObj->IPCIn_IPU1_0_A15_0_0Prm.inQueParams.prevLinkId = pObj->IPCOut_A15_0_IPU1_0_0LinkID;
       pObj->IPCIn_IPU1_0_A15_0_0Prm.inQueParams.prevLinkQueId = 0;

       //IPCIn_IPU1_0_A15_0_0 -> Display_Video
       pObj->IPCIn_IPU1_0_A15_0_0Prm.outQueParams.nextLink = pObj->Display_VideoLinkID;
       pObj->Display_VideoPrm.inQueParams.prevLinkId = pObj->IPCIn_IPU1_0_A15_0_0LinkID;
       pObj->Display_VideoPrm.inQueParams.prevLinkQueId = 0;

       //GrpxSrc -> Display_Grpx
       pObj->GrpxSrcPrm.outQueParams.nextLink = pObj->Display_GrpxLinkID;
       pObj->Display_GrpxPrm.inQueParams.prevLinkId = pObj->GrpxSrcLinkID;
       pObj->Display_GrpxPrm.inQueParams.prevLinkQueId = 0;

}

Int32 chains_vipSingleCameraOpenCLCannyEdge_Create(chains_vipSingleCameraOpenCLCannyEdgeObj *pObj, Void *appObj){

       Int32 status;

       chains_vipSingleCameraOpenCLCannyEdge_SetLinkId(pObj);
       chains_vipSingleCameraOpenCLCannyEdge_ResetLinkPrms(pObj);

       chains_vipSingleCameraOpenCLCannyEdge_SetPrms(pObj);
       chains_vipSingleCameraOpenCLCannyEdge_SetAppPrms(pObj, appObj);

       chains_vipSingleCameraOpenCLCannyEdge_ConnectLinks(pObj);
       status = System_linkCreate(pObj->CaptureLinkID, &pObj->CapturePrm, sizeof(pObj->CapturePrm));
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       status = System_linkCreate(pObj->IPCOut_IPU1_0_A15_0_0LinkID, &pObj->IPCOut_IPU1_0_A15_0_0Prm, sizeof(pObj->IPCOut_IPU1_0_A15_0_0Prm));
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       status = System_linkCreate(pObj->IPCIn_A15_0_IPU1_0_0LinkID, &pObj->IPCIn_A15_0_IPU1_0_0Prm, sizeof(pObj->IPCIn_A15_0_IPU1_0_0Prm));
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       status = System_linkCreate(pObj->Alg_OpenCLCannyEdgeLinkID, &pObj->Alg_OpenCLCannyEdgePrm, sizeof(pObj->Alg_OpenCLCannyEdgePrm));
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       status = System_linkCreate(pObj->IPCOut_A15_0_IPU1_0_0LinkID, &pObj->IPCOut_A15_0_IPU1_0_0Prm, sizeof(pObj->IPCOut_A15_0_IPU1_0_0Prm));
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       status = System_linkCreate(pObj->IPCIn_IPU1_0_A15_0_0LinkID, &pObj->IPCIn_IPU1_0_A15_0_0Prm, sizeof(pObj->IPCIn_IPU1_0_A15_0_0Prm));
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       status = System_linkCreate(pObj->Display_VideoLinkID, &pObj->Display_VideoPrm, sizeof(pObj->Display_VideoPrm));
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       status = System_linkCreate(pObj->GrpxSrcLinkID, &pObj->GrpxSrcPrm, sizeof(pObj->GrpxSrcPrm));
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       status = System_linkCreate(pObj->Display_GrpxLinkID, &pObj->Display_GrpxPrm, sizeof(pObj->Display_GrpxPrm));
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       return status;
}

Int32 chains_vipSingleCameraOpenCLCannyEdge_Start(chains_vipSingleCameraOpenCLCannyEdgeObj *pObj){

       Int32 status;

       status = System_linkStart(pObj->Display_GrpxLinkID);
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       status = System_linkStart(pObj->GrpxSrcLinkID);
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       status = System_linkStart(pObj->Display_VideoLinkID);
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       status = System_linkStart(pObj->IPCIn_IPU1_0_A15_0_0LinkID);
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       status = System_linkStart(pObj->IPCOut_A15_0_IPU1_0_0LinkID);
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       status = System_linkStart(pObj->Alg_OpenCLCannyEdgeLinkID);
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       status = System_linkStart(pObj->IPCIn_A15_0_IPU1_0_0LinkID);
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       status = System_linkStart(pObj->IPCOut_IPU1_0_A15_0_0LinkID);
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       status = System_linkStart(pObj->CaptureLinkID);
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       return status;
}

Int32 chains_vipSingleCameraOpenCLCannyEdge_Stop(chains_vipSingleCameraOpenCLCannyEdgeObj *pObj){

       Int32 status;

       status = System_linkStop(pObj->Display_GrpxLinkID);
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       status = System_linkStop(pObj->GrpxSrcLinkID);
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       status = System_linkStop(pObj->Display_VideoLinkID);
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       status = System_linkStop(pObj->IPCIn_IPU1_0_A15_0_0LinkID);
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       status = System_linkStop(pObj->IPCOut_A15_0_IPU1_0_0LinkID);
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       status = System_linkStop(pObj->Alg_OpenCLCannyEdgeLinkID);
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       status = System_linkStop(pObj->IPCIn_A15_0_IPU1_0_0LinkID);
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       status = System_linkStop(pObj->IPCOut_IPU1_0_A15_0_0LinkID);
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       status = System_linkStop(pObj->CaptureLinkID);
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       return status;
}

Int32 chains_vipSingleCameraOpenCLCannyEdge_Delete(chains_vipSingleCameraOpenCLCannyEdgeObj *pObj){

       Int32 status;

       status = System_linkDelete(pObj->Display_GrpxLinkID);
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       status = System_linkDelete(pObj->GrpxSrcLinkID);
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       status = System_linkDelete(pObj->Display_VideoLinkID);
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       status = System_linkDelete(pObj->IPCIn_IPU1_0_A15_0_0LinkID);
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       status = System_linkDelete(pObj->IPCOut_A15_0_IPU1_0_0LinkID);
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       status = System_linkDelete(pObj->Alg_OpenCLCannyEdgeLinkID);
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       status = System_linkDelete(pObj->IPCIn_A15_0_IPU1_0_0LinkID);
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       status = System_linkDelete(pObj->IPCOut_IPU1_0_A15_0_0LinkID);
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       status = System_linkDelete(pObj->CaptureLinkID);
       UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

       return status;
}

Void chains_vipSingleCameraOpenCLCannyEdge_printBufferStatistics(chains_vipSingleCameraOpenCLCannyEdgeObj *pObj){
       System_linkPrintBufferStatistics(pObj->CaptureLinkID);
       System_linkPrintBufferStatistics(pObj->IPCOut_IPU1_0_A15_0_0LinkID);
       Task_sleep(500);
       System_linkPrintBufferStatistics(pObj->IPCIn_A15_0_IPU1_0_0LinkID);
       System_linkPrintBufferStatistics(pObj->Alg_OpenCLCannyEdgeLinkID);
       System_linkPrintBufferStatistics(pObj->IPCOut_A15_0_IPU1_0_0LinkID);
       Task_sleep(500);
       System_linkPrintBufferStatistics(pObj->IPCIn_IPU1_0_A15_0_0LinkID);
       System_linkPrintBufferStatistics(pObj->Display_VideoLinkID);
       System_linkPrintBufferStatistics(pObj->GrpxSrcLinkID);
       System_linkPrintBufferStatistics(pObj->Display_GrpxLinkID);
       Task_sleep(500);
}

Void chains_vipSingleCameraOpenCLCannyEdge_printStatistics(chains_vipSingleCameraOpenCLCannyEdgeObj *pObj){
       System_linkPrintStatistics(pObj->CaptureLinkID);
       System_linkPrintStatistics(pObj->IPCOut_IPU1_0_A15_0_0LinkID);
       Task_sleep(500);
       System_linkPrintStatistics(pObj->IPCIn_A15_0_IPU1_0_0LinkID);
       System_linkPrintStatistics(pObj->Alg_OpenCLCannyEdgeLinkID);
       System_linkPrintStatistics(pObj->IPCOut_A15_0_IPU1_0_0LinkID);
       Task_sleep(500);
       System_linkPrintStatistics(pObj->IPCIn_IPU1_0_A15_0_0LinkID);
       System_linkPrintStatistics(pObj->Display_VideoLinkID);
       System_linkPrintStatistics(pObj->GrpxSrcLinkID);
       System_linkPrintStatistics(pObj->Display_GrpxLinkID);
       Task_sleep(500);
}
