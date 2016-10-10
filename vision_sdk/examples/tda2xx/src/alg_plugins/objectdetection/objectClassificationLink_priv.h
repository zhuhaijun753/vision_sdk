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
 * \file objectClassificationLink_priv.h Object Classificaiton Algorithm Link
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

#ifndef _OBJECTCLASSICIFICATION_LINK_PRIV_H_
#define _OBJECTCLASSICIFICATION_LINK_PRIV_H_

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
#include <include/link_api/algorithmLink_objectDetection.h>
#include <include/link_api/algorithmLink_objectClassification.h>
#include <include/link_api/algorithmLink_algPluginSupport.h>
#include <examples/tda2xx/src/alg_plugins/common/include/alg_ivision.h>
#include <src/utils_common/include/utils_prf.h>
#include "iobjclass_ti.h"
#include "iobjdet_ti.h"
#include <src/utils_common/include/utils_link_stats_if.h>
#include "objectDetectionLink_priv.h"

/*******************************************************************************
 *  Enums
 *******************************************************************************
 */

/*******************************************************************************
 *  Defines
 *******************************************************************************
 */

#define ALGORITHM_LINK_OBJECTCLASSIFICATION_TMP_OUTPUT_SIZE  (36*36*3*32*2)

/**
 *******************************************************************************
 *
 *   \ingroup LINK_API_CMD
 *   \addtogroup ALGORITHM_LINK_OBJECTCLASSIFICATION_CMD Algorithm Plugin:
 *                   Object classification Control Commands
 *   @{
 *
 *******************************************************************************
 */


/**
 *******************************************************************************
 *
 *   \brief Alg Link Config CMD: Create OC alg
 *                               This is a compulsory ioctl to instantiate Alg.
 *
 *   \param AlgorithmLink_ImgPyramidImgPyramidInfoControlParams [IN]
 *
 *   \return SYSTEM_STATUS_SOK on success
 *
 *******************************************************************************
 */
#define ALGORITHM_LINK_OBJECTCLASSIFICATION_CMD_CREATE_ALG               (0x1000)


/**
 *******************************************************************************
 *
 *   \brief Max number of buffers
 *
 *   Feature Plane Classification alg takes an input frame from feature plane
 *   computation link and generates an output buffer which is a meta data
 *   buffer. This macro defines the maximum number of such buffers this
 *   link can handle
 *
 *   SUPPORTED in ALL platforms
 *
 *******************************************************************************
 */
#define OBJECTCLASSICIFICATION_LINK_MAX_NUM_OUTPUT    (8)

/*******************************************************************************
 *  Data structures
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 *   \brief Structure containing feature plane Classification algorithm link
 *          parameters
 *
 *          This structure holds any algorithm parameters specific to this link.
 *
 *******************************************************************************
*/
typedef struct
{
    Void *handle;
    /**< Handle to the algorithm */
    AlgorithmLink_ObjectClassificationCreateParams algLinkCreateParams;
    /**< Create params of feature plane Classification algorithm link */
    TI_OC_CreateParams algCreateParams;
    /**< Create parameters for the algorithm */
    TI_OC_InArgs inArgs;
    /**< inArgs for the algorithm */
    TI_OC_OutArgs   outArgs;
    /**< outArgs for the algorithm */
    IVISION_InBufs    inBufs;
    /**< input buffers for the algorithm */
    IVISION_OutBufs   outBufs;
    /**< output buffers for the algorithm */
    IVISION_BufDesc   inBufImgPyramid[TI_OC_MAX_TOTAL_SCALES];
    /**< input buffer image pyramid */
    IVISION_BufDesc   inBufDesc[TI_OC_IN_BUFDESC_TOTAL];
    /**< input buffer descriptor */
    IVISION_BufDesc   outBufDesc[TI_OC_OUT_BUFDESC_TOTAL];
    /**< output buffer descriptor */
    IVISION_BufDesc   *inBufDescList[TI_OC_IN_BUFDESC_TOTAL];
    /**< list of input buffer descriptors */
    IVISION_BufDesc   *outBufDescList[TI_OC_OUT_BUFDESC_TOTAL];
    /**< list of input buffer descriptors */
    System_Buffer buffers[OBJECTCLASSICIFICATION_LINK_MAX_NUM_OUTPUT];
    /**< System buffers to exchange data with next link */
    System_MetaDataBuffer ocOutput[OBJECTCLASSICIFICATION_LINK_MAX_NUM_OUTPUT];
    /**< Payload for the system buffers */
    UInt32  outBufferSize;
    /**< Size of each output buffer */
    UInt32  numScales;
    /**< Number of scales to use for image pyramid,
         Copied from Create Parameters  */

    Void *pTmpOutput;
    /**< Buffer to hold intermidiate output from algorithm */
    TI_OC_outputList  detectionListLocal;
    /**< Temporary list to hold OC output */
    AlgorithmLink_InputQueueInfo  inputQInfo;
    /**< All the information about input Queue*/
    AlgorithmLink_OutputQueueInfo outputQInfo;
    /**< All the information about output Queue*/
    UInt32                        frameDropCounter;
    /**< Counter to keep track of number of frame drops */
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
} AlgorithmLink_ObjectClassificationObj;





/*******************************************************************************
 *  Algorithm Link Private Functions
 *******************************************************************************
 */
Int32 AlgorithmLink_objectClassificationCreate(void * pObj,
                                              void * pCreateParams);
Int32 AlgorithmLink_objectClassificationProcess(void * pObj);
Int32 AlgorithmLink_objectClassificationControl(void * pObj,
                                               void * pControlParams);
Int32 AlgorithmLink_objectClassificationStop(void * pObj);
Int32 AlgorithmLink_objectClassificationDelete(void * pObj);
Int32 AlgorithmLink_objectClassificationPrintStatistics(void *pObj,
                AlgorithmLink_ObjectClassificationObj *pObjectClassificationObj);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/*@}*/

/* Nothing beyond this point */
