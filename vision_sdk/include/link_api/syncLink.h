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
 *   \ingroup FRAMEWORK_MODULE_API
 *   \defgroup SYNC_LINK_API Sync Link API
 *
 *   Sync link is a connector link. Sync Link is particularly targeted to a set
 *   of specific use cases where there is a need for a set of video frames from
 *   multiple channels to be in sync (Frames captured at approximately at the
 *   same time)
 *
 *   @{
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \file syncLink.h
 *
 * \brief Sync link API public header file.
 *
 * \version 0.0 (Aug 2013) : [NN] First version
 *
 *******************************************************************************
 */

#ifndef SYNC_LINK_H_
#define SYNC_LINK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <include/link_api/system.h>

/*******************************************************************************
 *  Defines
 *******************************************************************************
 */

/* @{ */


/**
 *******************************************************************************
 *
 *   \brief Number of channels supported by sync link.
 *
 *******************************************************************************
 */
#define SYNC_LINK_MAX_CHANNELS (8U)

/* @} */



/*******************************************************************************
 *  Data structures
 *******************************************************************************
*/

/**
 *******************************************************************************
 * \brief Sync link create time parameters.
 *******************************************************************************
*/
typedef struct
{
    System_LinkInQueParams    inQueParams;
    /**< Input queue information */

    System_LinkOutQueParams   outQueParams;
    /**< output queue information */

    UInt32   syncDelta;
    /**< Delta on which to sync frames, in msec's */

    UInt32   syncThreshold;
    /**< Threshold after which buffers from local queue can be dropped, in msecs */

} SyncLink_CreateParams;

/*******************************************************************************
 *  Functions Prototypes
 *******************************************************************************
 */
static inline void SyncLink_CreateParams_Init(SyncLink_CreateParams *pPrm);

/*******************************************************************************
 *  Functions
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \brief Init function for sync link. This function does the following for each
 *   sync link,
 *  - Creates a task for the link
 *  - Registers this link with the system
 *
 * \return  SYSTEM_LINK_STATUS_SOK
 *
 *******************************************************************************
 */
Int32 SyncLink_init(void);

/**
 *******************************************************************************
 *
 * \brief De-init function for sync link. This function de-registers this link
 *  from the system
 *
 * \return  SYSTEM_LINK_STATUS_SOK
 *
 *******************************************************************************
 */
Int32 SyncLink_deInit(void);

/**
 *******************************************************************************
 *
 * \brief Sync link set default parameters for create time params
 *   This function does the following
 *      - memset create params object
 *
 * \param  pPrm  [OUT]  SyncLink Create time Params
 *
 *******************************************************************************
 */
static inline void SyncLink_CreateParams_Init(SyncLink_CreateParams *pPrm)
{
/* MISRA.ONEDEFRULE.FUNC
 * MISRAC_2004 Rule 8.5
 * Function Definition in header file
 * KW State: Defer -> Waiver -> Case by case
 * MISRAC_WAIVER: This function initializes the create time argument to default.
 *  Defining this in the header file aids maintainability.
 */
    memset(pPrm, 0, sizeof(SyncLink_CreateParams));

    pPrm->syncThreshold = (UInt32) 0xFFFFFFFFU;
    pPrm->syncDelta = 1U;

    return;
}


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif


/*@}*/

