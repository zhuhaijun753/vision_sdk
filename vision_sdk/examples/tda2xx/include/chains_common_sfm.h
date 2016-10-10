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
 * \file chains_common_iss.h
 *
 * \brief This file contains common functions and definitions for the ISS
 *
 *******************************************************************************
 */


#ifndef _CHAINS_COMMON_SFM_H_
#define _CHAINS_COMMON_SFM_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *  Includes
 *******************************************************************************
 */
#include <include/link_api/system.h>
#include <include/link_api/captureLink.h>
#include <include/link_api/decLink.h>
#include <include/link_api/algorithmLink_sparseOpticalFlow.h>
#include <include/link_api/algorithmLink_sfm.h>
#include <include/link_api/algorithmLink_objectDraw.h>

#define INPUT_FPS        (60)
/* Output FPS when the DSP and EVE are in OPP_NOM */
#define OUTPUT_FPS       (15)
/* Output FPS when the DSP and EVE are in OPP_HIGH */
/* #define OUTPUT_FPS       (20) */

typedef struct {

    UInt32 prevFrameId;
    UInt32 capturePitch;
    UInt32 frameSkipCount;

    Void *pCamPrmBuf;
    Void *pGroundPlaneEqBuf;

    UInt32 dummyFrameNum;
    Bool   useDummyFrameId;

    Utils_BufSkipContext skipFrames;

    UInt32 curFrameNum;

} Chains_SfmObj;

extern Chains_SfmObj gChains_sfmObj;

/*******************************************************************************
 *  Function's
 *******************************************************************************
 */
Void ChainsCommon_Sfm_SetCaptureCallback(CaptureLink_CreateParams *pPrm);
Void ChainsCommon_Sfm_SetDecLinkCallback(DecLink_CreateParams *pPrm);
Void ChainsCommon_Sfm_SetSfmPrm(
                    CaptureLink_CreateParams *pCapturePrm,
                    AlgorithmLink_SparseOpticalFlowCreateParams *pAlgSofPrm,
                    AlgorithmLink_SfmCreateParams *pSfmPrm,
                    AlgorithmLink_FcwCreateParams *pFcwPrm,
                    AlgorithmLink_ObjectDrawCreateParams *pDrawPrm,
                    UInt32 startX,
                    UInt32 startY,
                    UInt32 width,
                    UInt32 height,
                    UInt32 sfmTrackPoints,
                    char *sfmCamPrmFile
                    );

Void ChainsCommon_Sfm_Init(Bool useDummyFrameId);
Void ChainsCommon_Sfm_DeInit();

UInt32 ChainsCommon_Sfm_CaptureGetCurrentFrameNum();

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /* end of _CHAINS_COMMON_ISS_H_ */

/* @} */

