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
 * \file sfmLink_priv.h Sfm Link private API/Data
 *       structures
 *
 * \brief  This link private header file has defined
 *         - Algorithm link instance/handle object
 *         - All the local data structures
 *         - Algorithm plug in function interfaces
 *
 * \version 0.0 (Apr 2013) : [NN] First version
 *
 *******************************************************************************
 */

#ifndef _SFM_LINK_PRIV_H_
#define _SFM_LINK_PRIV_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *  Include files
 *******************************************************************************
 */
#include <include/link_api/system.h>
#include <include/link_api/algorithmLink.h>
#include <include/link_api/algorithmLink_sfm.h>
#include <include/link_api/algorithmLink_algPluginSupport.h>
#include "isfm_ti.h"
#include <src/utils_common/include/utils_prf.h>
#include <src/utils_common/include/utils_dma.h>
#include <examples/tda2xx/src/alg_plugins/common/include/alg_ivision.h>
#include <src/utils_common/include/utils_link_stats_if.h>

/*******************************************************************************
 *  Enums
 *******************************************************************************
 */


/*******************************************************************************
 *  Defines
 *******************************************************************************
 */
#define SFM_LINK_SFM_POINTS_BUF_SIZE        (SystemUtils_align(sizeof(AlgorithmLink_SfmOutput), 128))


#define SFM_LINK_MAX_TRACK_POINTS           (1024)


/**
 *******************************************************************************
 *
 *   \brief Max number of buffers
 *
 *******************************************************************************
 */
#define SFM_LINK_MAX_NUM_OUTPUT    (8)


/*******************************************************************************
 *  Data structures
 *******************************************************************************
 */



/**
 *******************************************************************************
 *
 *   \brief Structure containing algorithm link
 *          parameters
 *
 *          This structure holds any algorithm parameters specific to this link.
 *
 *******************************************************************************
*/
typedef struct
{
    AlgorithmLink_SfmCreateParams algLinkCreateParams;

    Void               *algSfmHandle;
    SFM_TI_CreateParams algSfmCreateParams;

    SFM_TI_InArgs       algSfmInArgs;
    SFM_TI_OutArgs      algSfmOutArgs;

    IVISION_InBufs      algSfmInBufs;
    IVISION_OutBufs     algSfmOutBufs;

    IVISION_BufDesc     algSfmInBufDescSofPoints;
    IVISION_BufDesc    *algSfmInBufDesc[1];

    IVISION_BufDesc     algSfmOutBufDescSfmPoints;
    IVISION_BufDesc    *algSfmOutBufDesc[1];

    System_LinkChInfo inChInfo;

    UInt32              maxFrameId;

    System_Buffer buffers[SFM_LINK_MAX_NUM_OUTPUT];
    /**< System buffers to exchange data with next link */

    System_MetaDataBuffer metaDataBuffers[SFM_LINK_MAX_NUM_OUTPUT];
    /**< Payload for the system buffers */

    AlgorithmLink_InputQueueInfo  inputQInfo;
    /**< All the information about input Queue*/

    AlgorithmLink_OutputQueueInfo outputQInfo;
    /**< All the information about output Queue*/

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

} AlgorithmLink_SfmObj;

/**
 *******************************************************************************
 *
 *   \brief Structure containing algorithm output
 *
 *******************************************************************************
*/
typedef struct {

    UInt32 outNumPoints ;
    float  camExtPrm[3][4];
    SFM_TI_output  sfmOut[SFM_LINK_MAX_TRACK_POINTS];

} AlgorithmLink_SfmOutput;

/*******************************************************************************
 *  Algorithm Link Private Functions
 *******************************************************************************
 */
Int32 AlgorithmLink_sfmCreate(void * pObj,
                                              void * pCreateParams);
Int32 AlgorithmLink_sfmProcess(void * pObj);
Int32 AlgorithmLink_sfmControl(void * pObj,
                                               void * pControlParams);
Int32 AlgorithmLink_sfmStop(void * pObj);
Int32 AlgorithmLink_sfmDelete(void * pObj);
Int32 AlgorithmLink_sfmPrintStatistics(void *pObj,
                AlgorithmLink_SfmObj *pSfmObj);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/*@}*/

/* Nothing beyond this point */
