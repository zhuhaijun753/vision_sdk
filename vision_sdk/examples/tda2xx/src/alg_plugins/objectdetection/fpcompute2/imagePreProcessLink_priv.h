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
 * \file imagePreProcessLink_priv.h Image pre-process Algorithm Link
 *       private API/Data structures
 *
 * \brief  This link private header file has defined
 *         - Algorithm link instance/handle object
 *         - All the local data structures
 *         - Algorithm plug in function interfaces
 *
 * \version 0.0 (Feb 2013) : [NN] First version
 *
 *******************************************************************************
 */

#ifndef _IMAGEPREPROCESS_LINK_PRIV_H_
#define _IMAGEPREPROCESS_LINK_PRIV_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *  Include files
 *******************************************************************************
 */
#include <include/link_api/system.h>
#include <include/link_api/algorithmLink.h>
#include <include/link_api/algorithmLink_imagePreProcess.h>
#include <include/link_api/algorithmLink_algPluginSupport.h>
#include <src/utils_common/include/utils_prf.h>
#include <examples/tda2xx/src/alg_plugins/common/include/alg_ivision.h>
#include <examples/tda2xx/src/alg_plugins/objectdetection/alg_filter2d.h>
#include <include/link_api/system_common.h>
#include <src/utils_common/include/utils_mem.h>
#include <src/utils_common/include/utils_link_stats_if.h>


/*******************************************************************************
 *  Enums
 *******************************************************************************
 */

/*******************************************************************************
 *  Defines
 *******************************************************************************
 */

#define IMAGEPREPROCESS_LINK_MAX_NUM_OUTPUT    (8)


/*******************************************************************************
 *  Data structures
 *******************************************************************************
 */






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
    Alg_Filter2dObj       algFilter2dObj;

    AlgorithmLink_ImagePreProcessCreateParams algLinkCreateParams;
    /**< Create params of feature plane computation algorithm link */

    System_Buffer buffers[IMAGEPREPROCESS_LINK_MAX_NUM_OUTPUT];
    /**< System buffers to exchange data with next link */

    System_VideoFrameBuffer videoFrames[IMAGEPREPROCESS_LINK_MAX_NUM_OUTPUT];
    /**< System buffer data structure to exchange buffers between links */

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

    UInt64 filter2dTime;
    UInt64 imagePreProcessTime;
    UInt64 frameCount;

} AlgorithmLink_ImagePreProcessObj;

/*******************************************************************************
 *  Algorithm Link Private Functions
 *******************************************************************************
 */
Int32 AlgorithmLink_imagePreProcessCreate(void * pObj,
                                              void * pCreateParams);
Int32 AlgorithmLink_imagePreProcessProcess(void * pObj);
Int32 AlgorithmLink_imagePreProcessControl(void * pObj,
                                               void * pControlParams);
Int32 AlgorithmLink_imagePreProcessStop(void * pObj);
Int32 AlgorithmLink_imagePreProcessDelete(void * pObj);
Int32 AlgorithmLink_imagePreProcessPrintStatistics(void *pObj,
                AlgorithmLink_ImagePreProcessObj *pImagePreProcessObj);



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/*@}*/

/* Nothing beyond this point */
