/*
 *******************************************************************************
 *
 * Copyright (C) 2015 Texas Instruments Incorporated - http://www.ti.com/
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
 * \file sfmLinearTwoViewLink_priv.h SfM LinearTwoView Algorithm Link private
 *                            API/Data structures
 *
 * \brief  This link private header file has defined
 *         - Algorithm link instance/handle object
 *         - All the local data structures
 *         - Algorithm plug in function interfaces
 *
 * \version 0.0 (Jun 2015) : [MM] First version
 *
 *******************************************************************************
 */

#ifndef _SFM_LINEARTWOVIEW_LINK_PRIV_H_
#define _SFM_LINEARTWOVIEW_LINK_PRIV_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *  Include files
 *******************************************************************************
 */
#include <include/link_api/system.h>
#include <include/link_api/algorithmLink.h>
#include <include/link_api/algorithmLink_sfmLinearTwoView.h>
#include <include/link_api/algorithmLink_algPluginSupport.h>
#include <src/utils_common/include/utils_prf.h>
#include <src/utils_common/include/utils_que.h>
#include <src/utils_common/include/utils_link_stats_if.h>
#include "./include/iSfmLinearTwoViewAlgo.h"


/*******************************************************************************
 *  Enums
 *******************************************************************************
 */

/*******************************************************************************
 *  Defines
 *******************************************************************************
 */

/*******************************************************************************
 *  Data structures
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 *   \brief Structure containing algorithm link parameters
 *
 *          This structure holds any algorithm parameters specific to this link.
 *
 *******************************************************************************
*/
typedef  struct
{
    void                   			 *algHandle;
    /**< Handle of the algorithm */
    AlgorithmLink_SfmLinearTwoViewCreateParams linkCreateParams;
    /**< Create params (link level)*/
    SfmLinearTwoView_ControlParams			  algControlParams;
    /**< Control params */
    AlgorithmLink_InputQueueInfo  inputQInfo[ALGLINK_SFMLTV_IPQID_MAXIPQ];
    /**< All the information about input Queues used */
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
     *   to start stats counting*/
    Bool isFirstOPGenerated;
    /**< Flag to indicate if first output is generated. This flag
     *   can be used to control any special processing for first time */
} AlgorithmLink_SfmLinearTwoViewObj;

/*******************************************************************************
 *  Algorithm Link Private Functions
 *******************************************************************************
 */
Int32 AlgorithmLink_sfmLinearTwoViewCreate(void * pObj, void * pCreateParams);
Int32 AlgorithmLink_sfmLinearTwoViewProcess(void * pObj);
Int32 AlgorithmLink_sfmLinearTwoViewControl(void * pObj, void * pControlParams);
Int32 AlgorithmLink_sfmLinearTwoViewStop(void * pObj);
Int32 AlgorithmLink_sfmLinearTwoViewDelete(void * pObj);
Int32 AlgorithmLink_sfmLinearTwoViewPrintStatistics(void *pObj, void *pAlgLinkObj);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/*@}*/

/* Nothing beyond this point */
