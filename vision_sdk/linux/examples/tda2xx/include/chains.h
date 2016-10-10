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
 *
 * \ingroup EXAMPLES_API
 * \defgroup EXAMPLES_CHAIN APIs for selecting the usecase chain.
 *
 * \brief  APIs for selecting the required usecase chain and run
 *         time menu configurations .
 *         It also provide API's for instrumentation of load and heap usage
 *
 * @{
 *
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \file chains.h
 *
 * \brief APIs for selecting the required usecase chain.
 *
 * \version 0.0 (May 2014) : [YM] First version ported to linux
 *
 *******************************************************************************
 */

#ifndef _CHAINS_H_
#define _CHAINS_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *  Include files
 *******************************************************************************
 */

#include <linux/examples/common/chains_common.h>


/**
 *******************************************************************************
 *
 *  \brief  Chain Parameters.
 *
 *******************************************************************************
*/
typedef struct {

    UInt32 algProcId;
    /**<  Processor ID on which algorithm runs for
     *    - Frame copy algorithm use-case
     *    - DMA SW MS algorithm use-case
     */

    UInt32 numLvdsCh;
    /**< Number of channels of LVDS to enable */

    Chains_DisplayType displayType;
    /**< LCD/HDM display */

    Chains_CaptureSrc captureSrc;
    /**< OV/HDMI-720p/HDMI-1080p capture */

    AlgorithmLink_SrvOutputModes svOutputMode;
    /**< To slect 2D vs 3D Surround View (SRV) Alg */
    UInt32 numPyramids;
    /**< numPyramids - used to select Alg with One/Two Pyramid Mode in  Dense Optical Flow*/
    Bool enableCarOverlayInAlg;
    /**< Set to 1, if DSP need to create the car image, apply only for 2D SRV */

    Bool enableAutoCalib;
    /**< Set to 1, if auto calibration is called to get initial calibration matrix */

    Bool enablePersmatUpdate;
    /**< Set to 1, if initial calibration matrix is updated by Harris corner detection + BRIEF */

} Chains_Ctrl;


/*******************************************************************************
 *  Functions
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \brief   Single Channel Capture to Frame Copy on A15 to Sgx Display usecase function
 *
 *          This functions creates capture Link on IPU and Alg link on A15
 *          connects them through IPC links
 *
 * \param   chainsCfg         [IN] Chains_Ctrl
 *
 *******************************************************************************
*/

Void chains_vipSingleCamFrameCopySgxDisplay(Chains_Ctrl *chainsCfg);

/**
 *******************************************************************************
 *
 * \brief   Single Channel Capture to OpenCV Canny on A15 to Sgx Display usecase function
 *
 *          This functions creates capture Link on IPU and Alg link on A15
 *          connects them through IPC links
 *
 * \param   chainsCfg         [IN] Chains_Ctrl
 *
 *******************************************************************************
*/

Void chains_vipSingleCamOpenCVCannySgxDisplay(Chains_Ctrl *chainsCfg);

/**
 *******************************************************************************
 *
 * \brief   Single Channel Capture to Sgx Display usecase function
 *
 *          This functions creates capture Link on IPU and NULL link on A15
 *          connects them through IPC links
 *
 * \param   chainsCfg         [IN] Chains_Ctrl
 *
 *******************************************************************************
*/
Void chains_vipSingleCam_SgxDisplay(Chains_Ctrl *chainsCfg);

/**
 *******************************************************************************
 *
 * \brief   4ch Channel LVDS Capture to Display usecase function
 *
 * \param   chainsCfg         [IN] Chains_Ctrl
 *
 *******************************************************************************
*/
Void chains_lvdsVipMultiCam_SgxDisplay(Chains_Ctrl *chainsCfg);

/**
 *******************************************************************************
 *
 * \brief   4ch Channel LVDS Capture, 3D SRV, DRM Display usecase function
 *
 * \param   chainsCfg         [IN] Chains_Ctrl
 *
 *******************************************************************************
*/
Void chains_lvdsVipMultiCam_Sgx3Dsrv(Chains_Ctrl *chainsCfg);

/**
 *******************************************************************************
 *
 * \brief   4ch Channel LVDS Capture, Autocalibration and 3D SRV
 *
 * \param   chainsCfg         [IN] Chains_Ctrl
 *
 *******************************************************************************
*/
Void chains_lvdsVipMultiCam_Sgx3DsrvACalib(Chains_Ctrl *usecaseCfg);

/**
 *******************************************************************************
 *
 * \brief   3D Perception demo (4 channel Structure from motion (SfM))
 *
 * \param   chainsCfg         [IN] Chains_Ctrl
 *
 *******************************************************************************
*/
Void chains_lvdsVipMultiCam_3d_perception(Chains_Ctrl *chainsCfg);

/**
 *******************************************************************************
 *
 * \brief   2ch 2560x720 OV490 Capture, 3D SRV, DRM Display usecase function
 *
 * \param   chainsCfg         [IN] Chains_Ctrl
 *
 *******************************************************************************
*/
Void chains_ov490VipMultiCam_Sgx3Dsrv(Chains_Ctrl *chainsCfg);

/**
 *******************************************************************************
 *
 * \brief   Creates capture + encode + Decoder + sgxDisplay chain
 *
 * \param   chainsCfg       [IN]   Chains_Ctrl
 *
 *******************************************************************************
*/
Void chains_vipSingleCam_Enc_Dec_SgxDisplay(Chains_Ctrl *chainsCfg);

/**
 *******************************************************************************
 *
 * \brief   AvbRx + Decoder + sgxDisplay chain
 *
 * \param   chainsCfg       [IN]   Chains_Ctrl
 *
 *******************************************************************************
*/
Void Chains_AvbRxDecodeSgxDisplay(Chains_Ctrl *chainsCfg);

/**
 *******************************************************************************
 *
 * \brief   CApture + EVE framecopy + sgxDisplay chain
 *
 * \param   chainsCfg       [IN]   Chains_Ctrl
 *
 *******************************************************************************
*/
Void chains_vipSingleCamFrmcpyEve_SgxDisplay(Chains_Ctrl *chainsCfg);

/**
 *******************************************************************************
 *
 * \brief   Single Channel Capture to Sgx to display on IPU usecase function
 *
 * \param   chainsCfg         [IN] Chains_Ctrl
 *
 *******************************************************************************
*/
Void chains_vipSingleCam_Sgx_IpuDisplay(Chains_Ctrl *chainsCfg);

/**
 *******************************************************************************
 *
 * \brief   Single Channel Capture to Sgx to dual display on IPU usecase function
 *
 * \param   chainsCfg         [IN] Chains_Ctrl
 *
 *******************************************************************************
 */
Void chains_vipSingleCam_SGX_IpuDualDisplay(Chains_Ctrl *chainsCfg);

/**
 *******************************************************************************
 *
 * \brief   Single Channel Capture to Sgx with all connector links and framecopy
 *
 * \param   chainsCfg         [IN] Chains_Ctrl
 *
 *******************************************************************************
 */
Void chains_vipSingleCamConnectorLinksA15SgxDisplay(Chains_Ctrl *chainsCfg);

/**
 *******************************************************************************
 *
 * \brief   Run time Menu selection
 *
 *          This functions displays the run time options available
 *          And receives user input and calls corrosponding functions run time
 *          Instrumentation logs are printing routine is called in same function
 *
 *
 * \return  char input by uart on success
 *******************************************************************************
*/
Char Chains_menuRunTime();


Void Chains_nullSrc_DecodeDisplay(Chains_Ctrl *chainsCfg);

/**
 *******************************************************************************
 *
 * \brief   Set default values to chains control parameters
 *
 *******************************************************************************
*/
static inline Void Chains_Ctrl_Init(Chains_Ctrl *pPrm)
{
    memset(pPrm, 0, sizeof(Chains_Ctrl));

    pPrm->algProcId = SYSTEM_PROC_DSP1;
    pPrm->displayType = CHAINS_DISPLAY_TYPE_HDMI_1080P;
    pPrm->captureSrc = CHAINS_CAPTURE_SRC_OV10635;
    /* TBD : Initialize numLvdsCh */
    pPrm->svOutputMode = ALGORITHM_LINK_SRV_OUTPUT_3D;
    pPrm->numPyramids = 0;
    pPrm->enableCarOverlayInAlg = 0;
    pPrm->enableAutoCalib = 0;
    pPrm->enablePersmatUpdate = 0;
}


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

/* @} */

/**
 *******************************************************************************
 *
 *   \defgroup EXAMPLES_API Example code implementation
 *
 *******************************************************************************
 */

