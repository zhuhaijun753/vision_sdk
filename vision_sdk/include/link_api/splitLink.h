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
 *   \ingroup SAMPLE_MODULE_API
 *   \defgroup SPLIT_LINK_API Split Link API
 *
 *
 *   Split link is a connector link. It takes each channel of higher resolution
 *   video and splits it not "numSplits" number of channels with lower
 *   resolution video.
 *
 *   @{
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \file splitLink.h
 *
 * \brief Split link API public header file.
 *
 * \version 0.0 (Jul 2013) : [NN] First version
 *
 *******************************************************************************
 */

#ifndef SPLIT_LINK_H_
#define SPLIT_LINK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <include/link_api/system.h>

/*******************************************************************************
 *  Defines
 *******************************************************************************
 */

 /* @{ */

/* @} */

/*******************************************************************************
 *  Data structure's
 *******************************************************************************
*/


/**
 *******************************************************************************
 * \brief SPLIT link configuration parameters.
 *
 *
 * SUPPORTED in ALL platforms
 *
 *******************************************************************************
*/
typedef struct
{
    System_LinkInQueParams   inQueParams;
    /**< Input queue information */

    System_LinkOutQueParams   outQueParams;
    /**< output queue information */

    UInt32  numSplits;
    /**< Number into which the input buffer is split - default is 2 */

} SplitLink_CreateParams;

/*******************************************************************************
 *  Functions Prototypes
 *******************************************************************************
 */
static inline void SplitLink_CreateParams_Init(SplitLink_CreateParams *pPrm);

/*******************************************************************************
 *  Functions
 *******************************************************************************
 */

 /**
 *******************************************************************************
 *
 * \brief Init function for SPLIT link. This function does the following for each
 *   SPLIT link,
 *  - Creates a task for the link
 *  - Registers this link with the system
 *
 * \return  SYSTEM_LINK_STATUS_SOK
 *
 *******************************************************************************
 */
Int32 SplitLink_init(void);

/**
 *******************************************************************************
 *
 * \brief De-init function for SPLIT link. This function de-registers this link
 *  from the system
 *
 * \return  SYSTEM_LINK_STATUS_SOK
 *
 *******************************************************************************
 */
Int32 SplitLink_deInit(void);

/**
 *******************************************************************************
 *
 * \brief Split link set default parameters for create time params
 *   This function does the following
 *      - memset create params object
 * \param  pPrm  [OUT]  SplitLink Create time Params
 *
 *******************************************************************************
 */
static inline void SplitLink_CreateParams_Init(SplitLink_CreateParams *pPrm)
{
/* MISRA.ONEDEFRULE.FUNC
 * MISRAC_2004 Rule 8.5
 * Function Definition in header file
 * KW State: Defer -> Waiver -> Case by case
 * MISRAC_WAIVER: This function initializes the create time argument to default.
 *  Defining this in the header file aids maintainability.
 */
    memset(pPrm, 0, sizeof(SplitLink_CreateParams));
    pPrm->numSplits = 2U;
    return;
}


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif


/*@}*/

