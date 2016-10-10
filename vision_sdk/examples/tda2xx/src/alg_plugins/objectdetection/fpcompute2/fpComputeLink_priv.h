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
 * \ingroup ALGORITHM_LINK_API
 * \defgroup ALGORITHM_LINK_IMPL Algorithm Link Implementation
 *
 * @{
 */

/**
 *******************************************************************************
 *
 * \file featurePlaneCompLink_priv.h Feature Plane Computation Algorithm Link
 *       private API/Data structures
 *
 * \brief  This link private header file has defined
 *         - Algorithm link instance/handle object
 *         - All the local data structures
 *         - Algorithm plug in function interfaces
 *
 * \version 0.0 (Nov 2015) : [YM] First version
 *
 *******************************************************************************
 */

#ifndef _FPCOMPUTE_LINK_PRIV_H_
#define _FPCOMPUTE_LINK_PRIV_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *  Include files
 *******************************************************************************
 */
#include <include/link_api/system.h>
#include <include/link_api/algorithmLink.h>
#include <include/link_api/algorithmLink_fpCompute.h>
#include <include/link_api/algorithmLink_imgPyramid.h>
#include <include/link_api/algorithmLink_imagePreProcess.h>
#include <include/link_api/algorithmLink_algPluginSupport.h>
#include <src/utils_common/include/utils_prf.h>
#include <examples/tda2xx/src/alg_plugins/common/include/alg_ivision.h>
#include <include/link_api/system_common.h>
#include <src/utils_common/include/utils_mem.h>
#include <iti_pd_feature_plane_computation_ti.h>
#include <src/utils_common/include/utils_link_stats_if.h>


/*******************************************************************************
 *  Enums
 *******************************************************************************
 */

/*******************************************************************************
 *  Defines
 *******************************************************************************
 */

#define FPCOMPUTE_LINK_MAX_NUM_OUTPUT    (8)


/*******************************************************************************
 *  Data structures
 *******************************************************************************
 */

typedef struct
{
    Void * handle;

    PD_FEATURE_PLANE_COMPUTATION_CreateParams prms;
    PD_FEATURE_PLANE_COMPUTATION_InArgs inArgs;
    PD_FEATURE_PLANE_COMPUTATION_OutArgs outArgs;

    IVISION_InBufs    inBufs;
    IVISION_OutBufs   outBufs;
    IVISION_BufDesc   inBufDesc[ALGORITHLINK_IMGPYRAMID_MAX_SCALES];
    IVISION_BufDesc   outBufDesc;
    IVISION_BufDesc   *inBufDescList[ALGORITHLINK_IMGPYRAMID_MAX_SCALES];
    IVISION_BufDesc   *outBufDescList[PD_FEATURE_PLANE_COMPUTATION_BUFDESC_OUT_TOTAL];

} Alg_FPComputeObj;


/**
 *******************************************************************************
 *
 *   \brief Structure containing feature plane computation algorithm link
 *          parameters
 *
 *          This structure holds any algorithm parameters specific to this link.
 *
 *******************************************************************************
*/
typedef struct
{
    Alg_FPComputeObj algFPComputeObj;

    AlgorithmLink_FPComputeCreateParams algLinkCreateParams;
    /**< Create params of feature plane computation algorithm link */

    System_Buffer buffers[FPCOMPUTE_LINK_MAX_NUM_OUTPUT];
    /**< System buffers to exchange data with next link */

    System_MetaDataBuffer featurePlanes[FPCOMPUTE_LINK_MAX_NUM_OUTPUT];
    /**< Payload for the system buffers */

    UInt32  outBufferSize;
    /**< Size of each output buffer */

    AlgorithmLink_InputQueueInfo  inputQInfo;
    /**< All the information about input Queue*/

    AlgorithmLink_OutputQueueInfo outputQInfo;
    /**< All the information about output Queue*/

    System_LinkChInfo inChInfo;
    /**< Information about input channel */

    System_LinkStatistics   *linkStatsInfo;
    /**< Pointer to the Link statistics information,
         used to store below information
            1, min, max and average latency of the link
            2, min, max and average latency from source to this link
            3, links statistics like frames captured, dropped etc
        Pointer is assigned at the link create time from shared
        memory maintained by utils_link_stats layer */

    Bool isFirstFrameRecv;
    /**< Flag to indicate if first frame is received, this is used as trigger
     *   to start stats counting
     */

    UInt64 fpComputeTime;
    UInt64 frameCount;

} AlgorithmLink_FPComputeObj;

/*******************************************************************************
 *  Algorithm Link Private Functions
 *******************************************************************************
 */
Int32 AlgorithmLink_fpComputeCreate(void * pObj,
                                              void * pCreateParams);
Int32 AlgorithmLink_fpComputeProcess(void * pObj);
Int32 AlgorithmLink_fpComputeControl(void * pObj,
                                               void * pControlParams);
Int32 AlgorithmLink_fpComputeStop(void * pObj);
Int32 AlgorithmLink_fpComputeDelete(void * pObj);
Int32 AlgorithmLink_fpComputePrintStatistics(void *pObj,
                AlgorithmLink_FPComputeObj *pFPComputeObj);

Int32 Alg_fpComputeCreate(
            Alg_FPComputeObj *pObj,
            AlgorithmLink_ImgPyramidInfo * pImgpmfInfo,
            UInt32 *outBufSize);

Int32 Alg_fpComputeProcess(
            Alg_FPComputeObj *pObj,
            Void *inBufAddr,
            Void *outBufAddr);

Int32 Alg_fpComputeDelete(
            Alg_FPComputeObj *pObj);



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/*@}*/

/* Nothing beyond this point */
