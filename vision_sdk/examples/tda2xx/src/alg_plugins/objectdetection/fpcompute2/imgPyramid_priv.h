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
 * \file imgPyramid_priv.h Algorithm Link private API/Data structures
 *
 *
 *******************************************************************************
 */

#ifndef IMGPMD_ALGLINK_PRIV_H_
#define IMGPMD_ALGLINK_PRIV_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *  Include files
 *******************************************************************************
 */
#include <include/link_api/system.h>
#include <include/link_api/algorithmLink.h>
#include <include/link_api/algorithmLink_imgPyramid.h>
#include <include/link_api/algorithmLink_algPluginSupport.h>
#include <src/utils_common/include/utils_prf.h>
#include <src/utils_common/include/utils_link_stats_if.h>

/*******************************************************************************
 *  Defines
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 *   \brief Max number of video frames needed for DMA SW MS algorithm
 *
 *******************************************************************************
 */
#define ALG_LINK_IMG_PYRAMID_MAX_OUT_BUF            (8)

#define ALG_IMG_PMD_Q_FORMATE_SCALE_RATIO           (12U)

#define ALG_IMG_PMD_BUFFER_ALIGN                    (32U)

#define ALG_IMG_PMD_SREACH_STEP                     (4U)

#define ALG_IMG_PMD_BUFFER_HEIGHT_ALIGN             (64U)

#define ALG_IMAGE_PYRAMID_MAX_SCALE_STEPS           (8U)

/*******************************************************************************
 *  Data structures
 *******************************************************************************
 */

/* Forward declaration of resizer function structure here,
   since image pyramid object is dependent on this structure and
   iResizer is dependent on image pyramid object */
typedef struct Alg_ImgPyramidRszDrvFxns_t Alg_ImgPyramidRszDrvFxns;

/**
 *******************************************************************************
 *
 *   \brief Structure containing image pyramid algorithm link specific parameters
 *
 *          This structure holds any algorithm parameters specific to this link.
 *
 *******************************************************************************
*/
typedef  struct
{
    System_LinkChInfo inputChInfo;
    /**< channel info of input */

    AlgorithmLink_ImgPyramidCreateParams algLinkCreateParams;
    /**< Create params of the image pyramid algorithm Link*/

    AlgorithmLink_OutputQueueInfo outputQInfo;
    /**< All the information about output Q */
    AlgorithmLink_InputQueueInfo inputQInfo;
    /**< All the information about input Q */

    AlgorithmLink_ImgPyramidFrameDesc *outFrmDesc
        [ALG_LINK_IMG_PYRAMID_MAX_OUT_BUF];
    AlgorithmLink_ImgPyramidFrameDesc inFrmDesc
        [ALGORITHLINK_IMGPYRAMID_MAX_SCALES];

    System_Buffer sysBuffers[ALG_LINK_IMG_PYRAMID_MAX_OUT_BUF];
    /**< System buffer data structure to exchange buffers between links */

    System_MetaDataBuffer metaDataBuffer[ALG_LINK_IMG_PYRAMID_MAX_OUT_BUF];
    /**< System Meta Data buffer */

    System_LinkStatistics *linkStatsInfo;
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

    UInt32      orgInputWidth;
    /**< Original/Input Frame Width */
    UInt32      orgInputHeight;
    /**< Original/Input Frame Height */

    Void       *pRszObj;

    Alg_ImgPyramidRszDrvFxns    *pRszDrvFxn;

    BspOsal_SemHandle             lock;
    /**< Semaphore handle */
} AlgorithmLink_ImgPmdObj;



/*******************************************************************************
 *  Algorithm Link Private Functions
 *******************************************************************************
 */

#ifdef ISS_INCLUDE
Void *Alg_imgPyramidIssRszCreateDrv(AlgorithmLink_ImgPmdObj *pObj, Void *data);
Void Alg_imgPyramidIssRszDeleteDrv(Void *pObj, Void *data);
Void Alg_imgPyramidIssRszProcessFrame(
    Void *pObj,
    AlgorithmLink_ImgPyramidFrameDesc *inFrmDesc1,
    AlgorithmLink_ImgPyramidFrameDesc *outFrmDesc1,
    AlgorithmLink_ImgPyramidFrameDesc *inFrmDesc2,
    AlgorithmLink_ImgPyramidFrameDesc *outFrmDesc2,
    Void *data);
#else
#ifdef VPE_INCLUDE
Void *Alg_imgPyramidVpeRszCreateDrv(AlgorithmLink_ImgPmdObj *pObj, Void *data);
Void Alg_imgPyramidVpeRszDeleteDrv(Void *pObj, Void *data);
Void Alg_imgPyramidVpeRszProcessFrame(
    Void *pObj,
    AlgorithmLink_ImgPyramidFrameDesc *inFrmDesc1,
    AlgorithmLink_ImgPyramidFrameDesc *outFrmDesc1,
    AlgorithmLink_ImgPyramidFrameDesc *inFrmDesc2,
    AlgorithmLink_ImgPyramidFrameDesc *outFrmDesc2,
    Void *data);
#endif
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/*@}*/

/* Nothing beyond this point */
