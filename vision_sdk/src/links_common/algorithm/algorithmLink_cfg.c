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
 * \file algorithmLink_cfg.c
 *
 * \brief  This file has some configuration of algorithm link
 *
 *         Functions in this file will be called by use case or application
 *
 * \version 0.0 (Aug 2013) : [PS] First version
 *
 *******************************************************************************
*/

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */
#include "algorithmLink_priv.h"
#include "algorithmLink_cfg.h"
#include <include/link_api/algorithmLink.h>
#include <include/link_api/algorithmLink_edgeDetection.h>
#include <include/link_api/algorithmLink_frameCopy.h>
#include <include/link_api/algorithmLink_safeFrameCopy.h>
#include <include/link_api/algorithmLink_dmaSwMs.h>
#include <include/link_api/algorithmLink_colorToGray.h>
#include <include/link_api/algorithmLink_synthesis.h>
#include <include/link_api/algorithmLink_geometricAlignment.h>
#include <include/link_api/algorithmLink_geometricAlignment3D.h>
#include <include/link_api/algorithmLink_photoAlignment.h>
#include <include/link_api/algorithmLink_autoChartDetect.h>
#include <include/link_api/algorithmLink_poseEstimate.h>
#include <include/link_api/algorithmLink_ultrasonicFusion.h>
#include <include/link_api/algorithmLink_denseOpticalFlow.h>
#include <include/link_api/algorithmLink_vectorToImage.h>
#include <include/link_api/algorithmLink_featurePlaneComputation.h>
#include <include/link_api/algorithmLink_objectDetection.h>
#include <include/link_api/algorithmLink_objectClassification.h>
#include <include/link_api/algorithmLink_objectDraw.h>
#include <include/link_api/algorithmLink_sfmLinearTwoView.h>
#include <include/link_api/algorithmLink_sfmMain.h>
#include <include/link_api/algorithmLink_sfmMap.h>
#include <include/link_api/algorithmLink_sparseOpticalFlow.h>
#include <include/link_api/algorithmLink_laneDetect.h>
#include <include/link_api/algorithmLink_subframeCopy.h>
#include <include/link_api/algorithmLink_softIsp.h>
#include <include/link_api/algorithmLink_remapMerge.h>
#include <include/link_api/algorithmLink_issAewb.h>
#include <include/link_api/algorithmLink_census.h>
#include <include/link_api/algorithmLink_disparityHamDist.h>
#include <include/link_api/algorithmLink_stereoPostProcess.h>
#include <include/link_api/algorithmLink_crc.h>
#include <include/link_api/algorithmLink_sfm.h>
#include <include/link_api/algorithmLink_fcw.h>
#include <include/link_api/algorithmLink_clr.h>
#include <include/link_api/algorithmLink_imgPyramid.h>
#include <include/link_api/algorithmLink_imagePreProcess.h>
#include <include/link_api/algorithmLink_fpCompute.h>
#include <include/link_api/algorithmLink_sceneObstructionDetect.h>
#include <include/link_api/algorithmLink_deWarp.h>

#ifdef RADAR_INCLUDE
#include <include/link_api/algorithmLink_radarProcess.h>
#include <include/link_api/algorithmLink_CRFAlignment.h>
#endif
#include <osal/bsp_safety_osal.h>
#ifdef ENABLE_OPENCV
#include <include/link_api/algorithmLink_opencvCanny.h>
#endif
#ifdef OPENCL_INCLUDE
#include <include/link_api/algorithmLink_openClframeCopy.h>
#include <include/link_api/algorithmLink_openClCannyEdge.h>
#endif

/*******************************************************************************
 *  Declaring gAlgorithmLinkFuncTable for the current core
 *******************************************************************************
 */
AlgorithmLink_FuncTable gAlgorithmLinkFuncTable[ALGORITHM_LINK_ALG_MAXNUM];

/**
 *******************************************************************************
 * \brief Display Link object, stores all link related information
 *******************************************************************************
 */
AlgorithmLink_Obj gAlgorithmLink_obj[ALGORITHM_LINK_OBJ_MAX];
/**
 *******************************************************************************
 * \brief Maintain FFI modes for all algos
 *******************************************************************************
 */
UInt32 gAlgorithmLinkFFIMode[ALGORITHM_LINK_ALG_MAXNUM];

/**
 *******************************************************************************
 *
 * \brief Initializing alg plugins
 *
 *        This function needs to be called by the use case / application.
 *        This function will inturn call the algorithm specific init functions
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
 */
Int32 AlgorithmLink_initAlgPlugins(void)
{
    UInt32 i;
    memset(gAlgorithmLinkFuncTable, 0, sizeof(gAlgorithmLinkFuncTable));
    for(i = 0U; i < ALGORITHM_LINK_ALG_MAXNUM; i++)
    {
        gAlgorithmLinkFFIMode[i] = BSP_SAFETY_OSAL_MODE_ASIL;
    }

#ifdef BUILD_DSP
    /** For all alorithms on Dsp */
#ifdef ISS_INCLUDE
    AlgorithmLink_sceneObstructionDetect_initPlugin();
#endif
    AlgorithmLink_FrameCopy_initPlugin();
    AlgorithmLink_SafeFrameCopy_initPlugin();
    AlgorithmLink_ColorToGray_initPlugin();
    AlgorithmLink_DmaSwMs_initPlugin();
    AlgorithmLink_ObjectDetection_initPlugin();
    AlgorithmLink_ObjectClassification_initPlugin();
    AlgorithmLink_vectorToImage_initPlugin();
    AlgorithmLink_sfmLinearTwoView_initPlugin();
    AlgorithmLink_sfmMain_initPlugin();
    AlgorithmLink_sfmMap_initPlugin();
    AlgorithmLink_laneDetect_initPlugin();
    AlgorithmLink_objectDraw_initPlugin();
    AlgorithmLink_sfm_initPlugin();
    AlgorithmLink_fcw_initPlugin();
    AlgorithmLink_clr_initPlugin();
#ifdef RADAR_INCLUDE
    AlgorithmLink_RadarProcess_initPlugin();
    AlgorithmLink_CRFAlignment_initPlugin();
#endif
    /** For 64MB DDR configuration on TDA3xx, surround view is not included, since surround view
      * significant DDR memory needs */
#ifndef TDA3XX_64MB_DDR
    #ifdef ALG_SV_INCLUDE
    AlgorithmLink_Synthesis_initPlugin();
    AlgorithmLink_pAlign_initPlugin();
    AlgorithmLink_gAlign_initPlugin();
    AlgorithmLink_gAlign3D_initPlugin();
    AlgorithmLink_UltrasonicFusion_initPlugin();
    AlgorithmLink_acDetect_initPlugin();
    AlgorithmLink_pEstimate_initPlugin();
    #endif
#endif

    #ifdef TDA2XX_FAMILY_BUILD
    AlgorithmLink_StereoPostProcess_initPlugin();
    #endif
#endif

#ifdef BUILD_ARP32
    /** For all alorithms on Eve */
    AlgorithmLink_FrameCopy_initPlugin();
    AlgorithmLink_SafeFrameCopy_initPlugin();
    AlgorithmLink_softIsp_initPlugin();
    #ifdef TDA2XX_FAMILY_BUILD
    AlgorithmLink_census_initPlugin();
    AlgorithmLink_disparityHamDist_initPlugin();
    #endif
    AlgorithmLink_EdgeDetection_initPlugin();
    AlgorithmLink_DenseOptFlow_initPlugin();
    AlgorithmLink_featurePlaneComputation_initPlugin();
    AlgorithmLink_sparseOpticalFlow_initPlugin();
    AlgorithmLink_SubframeCopy_initPlugin();
    #ifdef TDA2XX_FAMILY_BUILD
        #ifdef BUILD_ARP32_2
        AlgorithmLink_RemapMerge_initPlugin();
        #endif
    #endif
    AlgorithmLink_imagePreProcess_initPlugin();
    AlgorithmLink_fpCompute_initPlugin();
#endif

#ifdef BUILD_M4
    /** For all alorithms on IPU (M4) */
    AlgorithmLink_DmaSwMs_initPlugin();
#endif

#if ((defined(BUILD_M4_0) && defined(IPU_PRIMARY_CORE_IPU1)) || (defined(BUILD_M4_2) && defined(IPU_PRIMARY_CORE_IPU2)))
    /** For all alorithms on IPU (M4) */
    AlgorithmLink_objectDraw_initPlugin();

    #ifdef TDA3XX_FAMILY_BUILD
        #ifdef ISS_INCLUDE
        AlgorithmLink_sceneObstructionDetect_initPlugin();
        AlgorithmLink_issAewb1_initPlugin();
        AlgLink_DeWarpPlugin_init();
        #endif
    #endif

    #ifdef ALG_CRC_INCLUDE
    AlgorithmLink_Crc_initPlugin();
    #endif
#endif

#ifdef BUILD_M4_1
    /** For all alorithms on IPU (M4) */
    AlgorithmLink_objectDraw_initPlugin();
#endif

#ifdef BUILD_A15
    /** For all alorithms on A15 */
#ifdef ENABLE_OPENCV
    AlgorithmLink_OpenCVCanny_initPlugin();
#endif
    AlgorithmLink_FrameCopy_initPlugin();
    AlgorithmLink_SafeFrameCopy_initPlugin();
#ifdef OPENCL_INCLUDE
    AlgorithmLink_OpenCLFrameCopy_initPlugin();
    AlgorithmLink_OpenCLCannyEdge_initPlugin();
#if 0
    AlgorithmLink_OpenCLObjectDetection_initPlugin();
#endif
#endif
    AlgorithmLink_DmaSwMs_initPlugin();
    AlgorithmLink_objectDraw_initPlugin();
#endif

#ifdef BUILD_M4
    /* Image Pyramid plugin is supported only one M4 */
    AlgorithmLink_imgPyramidComputation_initPlugin();
#endif


    return SYSTEM_LINK_STATUS_SOK;
}

/* Nothing beyond this point */
