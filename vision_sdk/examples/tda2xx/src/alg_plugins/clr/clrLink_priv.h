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
 * \file clrLink_priv.h Circular Light Detect Link private API/Data
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

#ifndef _CLR_LINK_PRIV_H_
#define _CLR_LINK_PRIV_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *  Include files
 *******************************************************************************
 */
#include <include/link_api/system.h>
#include <include/link_api/algorithmLink.h>
#include <include/link_api/algorithmLink_clr.h>
#include <include/link_api/algorithmLink_algPluginSupport.h>
#include "iclr_ti.h"
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
#define CLR_LINK_OUT_BUF_SIZE        (SystemUtils_align(sizeof(TI_CLR_output), 128))




/**
 *******************************************************************************
 *
 *   \brief Max number of buffers
 *
 *******************************************************************************
 */
#define CLR_LINK_MAX_NUM_OUTPUT    (8)


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
    AlgorithmLink_ClrCreateParams algLinkCreateParams;

    Void               *algClrHandle;
    TI_CLR_CreateParams  algClrCreateParams;

    TI_CLR_InArgs        algClrInArgs;
    TI_CLR_OutArgs       algClrOutArgs;

    IVISION_InBufs      algClrInBufs;
    IVISION_OutBufs     algClrOutBufs;

    IVISION_BufDesc     algClrInBufDescImage;
    IVISION_BufDesc    *algClrInBufDesc[1];

    IVISION_BufDesc     algClrOutBufDescClrOut;
    IVISION_BufDesc    *algClrOutBufDesc[1];

    System_LinkChInfo inChInfo;

    System_Buffer buffers[CLR_LINK_MAX_NUM_OUTPUT];
    /**< System buffers to exchange data with next link */

    System_MetaDataBuffer metaDataBuffers[CLR_LINK_MAX_NUM_OUTPUT];
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

} AlgorithmLink_ClrObj;



/*******************************************************************************
 *  Algorithm Link Private Functions
 *******************************************************************************
 */
Int32 AlgorithmLink_clrCreate(void * pObj,
                                              void * pCreateParams);
Int32 AlgorithmLink_clrProcess(void * pObj);
Int32 AlgorithmLink_clrControl(void * pObj,
                                               void * pControlParams);
Int32 AlgorithmLink_clrStop(void * pObj);
Int32 AlgorithmLink_clrDelete(void * pObj);
Int32 AlgorithmLink_clrPrintStatistics(void *pObj,
                AlgorithmLink_ClrObj *pClrObj);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/*@}*/

/* Nothing beyond this point */
