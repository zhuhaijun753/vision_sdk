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
 * \file fcwLink_priv.h Fcw Link private API/Data
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

#ifndef _FCW_LINK_PRIV_H_
#define _FCW_LINK_PRIV_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *  Include files
 *******************************************************************************
 */
#include <include/link_api/system.h>
#include <include/link_api/algorithmLink.h>
#include <include/link_api/algorithmLink_fcw.h>
#include <include/link_api/algorithmLink_algPluginSupport.h>
#include "ifcw_ti.h"
#include "igpe_ti.h"
#include <src/utils_common/include/utils_prf.h>
#include <src/utils_common/include/utils_dma.h>
#include <examples/tda2xx/src/alg_plugins/common/include/alg_ivision.h>
#include <src/utils_common/include/utils_link_stats_if.h>
#include "sfmLink_priv.h"

/*******************************************************************************
 *  Enums
 *******************************************************************************
 */


/*******************************************************************************
 *  Defines
 *******************************************************************************
 */
#define FCW_LINK_FCW_OUT_BUF_SIZE        (SystemUtils_align(sizeof(AlgorithmLink_FcwOutput), 128))



/**
 *******************************************************************************
 *
 *   \brief Max number of buffers
 *
 *******************************************************************************
 */
#define FCW_LINK_MAX_NUM_OUTPUT    (8)


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
    AlgorithmLink_FcwCreateParams algLinkCreateParams;

    Void               *algGpeHandle;
    Void               *algFcwHandle;
    GPE_TI_CreateParams algGpeCreateParams;
    FCW_TI_CreateParams algFcwCreateParams;


    GPE_TI_InArgs       algGpeInArgs;
    GPE_TI_OutArgs      algGpeOutArgs;

    IVISION_InBufs      algGpeInBufs;
    IVISION_OutBufs     algGpeOutBufs;

    IVISION_BufDesc     algGpeInBufDescObjDesc[GPE_TI_IN_BUFDESC_TOTAL];
    IVISION_BufDesc    *algGpeInBufDesc[GPE_TI_IN_BUFDESC_TOTAL];

    IVISION_BufDesc     algGpeOutBufDescGpeObjs;
    IVISION_BufDesc    *algGpeOutBufDesc[1];


    FCW_TI_InArgs       algFcwInArgs;
    FCW_TI_OutArgs      algFcwOutArgs;

    IVISION_InBufs      algFcwInBufs;
    IVISION_OutBufs     algFcwOutBufs;

    IVISION_BufDesc     algFcwInBufDescObjDesc;
    IVISION_BufDesc    *algFcwInBufDesc[1];

    IVISION_BufDesc     algFcwOutBufDescFcwObjs;
    IVISION_BufDesc    *algFcwOutBufDesc[1];

    System_LinkChInfo inChInfo;

    System_Buffer buffers[FCW_LINK_MAX_NUM_OUTPUT];
    /**< System buffers to exchange data with next link */

    System_MetaDataBuffer metaDataBuffers[FCW_LINK_MAX_NUM_OUTPUT];
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

} AlgorithmLink_FcwObj;

/**
 *******************************************************************************
 *
 *   \brief Structure containing algorithm output
 *
 *******************************************************************************
*/
typedef struct {

    FCW_TI_OD_input objectDetectOut;
    FCW_TI_output fcwOut;

} AlgorithmLink_FcwOutput;

/*******************************************************************************
 *  Algorithm Link Private Functions
 *******************************************************************************
 */
Int32 AlgorithmLink_fcwCreate(void * pObj,
                                              void * pCreateParams);
Int32 AlgorithmLink_fcwProcess(void * pObj);
Int32 AlgorithmLink_fcwControl(void * pObj,
                                               void * pControlParams);
Int32 AlgorithmLink_fcwStop(void * pObj);
Int32 AlgorithmLink_fcwDelete(void * pObj);
Int32 AlgorithmLink_fcwPrintStatistics(void *pObj,
                AlgorithmLink_FcwObj *pFcwObj);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/*@}*/

/* Nothing beyond this point */
