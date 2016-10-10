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
 * \ingroup SAMPLE_LINUX_MODULE_API
 * \defgroup SGX3DSFM_LINK_API Sgx3Dsfm Link API
 *
 * \brief  This module has the interface for using Sgx3Dsfm Link
 *
 *         Sgx3Dsfm Link is used to display maps/objects using SGX.
 *         The rendered output will be pushed to display via DRM.
 *
 * @{
 *
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \file sgx3DsfmLink.h
 *
 * \brief Sgx3Dsfm Link API
 *
 * \version 0.0 (Dec 2015) : [MM] First version
 *
 *******************************************************************************
 */

#ifndef SGX3DSFM_LINK_H_
#define SGX3DSFM_LINK_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */
#include <include/link_api/system.h>

/*******************************************************************************
 *  Defines
 *******************************************************************************
 */
/**
 *******************************************************************************
 *
 * \brief Macro defining SGX Link egl context sharing command.
 *
 *
 *******************************************************************************
 */
#define SGXLINK_CMD_ACQUIRE_EGL_INFO                       (0x8000U)

/* @{ */

/**
 *******************************************************************************
 * \brief Enum for the input Q IDs
 *
 * SUPPORTED in ALL platforms
 *
 *******************************************************************************
*/
typedef enum
{
    SGX3DSFM_LINK_IPQID_MULTIVIEW = 0x0,
    /**< QueueId for multiview images */

    SGX3DSFM_LINK_IPQID_EGOPOSE = 1,
    /**< QueueId for vehicle pose buffer */

    SGX3DSFM_LINK_IPQID_BOXES = 2,
    /**< QueueId for object boxes buffer */

    SGX3DSFM_LINK_IPQID_MAXIPQ = 3,
    /**< Maximum number of input queues */

    SGX3DSFM_LINK_IPQID_FORCE32BITS = 0x7FFFFFFF
    /**< To make sure enum is 32 bits */

}Sgx3DsfmLink_InputQueId;

/* @} */

/*******************************************************************************
 *  Data structures
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 *   \brief Structure containing the Sgx3Dsfm link create time parameters
 *
 *          This structure is used to create and configure a Sgx3Dsfm link
 *          instance.
 *
 *******************************************************************************
*/
typedef struct
{
    UInt32                   inFramesHeight;
    /**< height of the input (captured) frames */
    UInt32                   inFramesWidth;
    /**< width of the input (captured) frames */
    UInt32                   numCams;
    /**< number of input camera streams */
    UInt32                   numInQue;
    /**< Number of inputs queue's */
    System_BufferType        inBufType[SGX3DSFM_LINK_IPQID_MAXIPQ];
    /**< Input buffer type can be
         SYSTEM_BUFFER_TYPE_METADATA
         OR
         SYSTEM_BUFFER_TYPE_VIDEO_FRAME_CONTAINER
     */
    System_LinkInQueParams   inQueParams[SGX3DSFM_LINK_IPQID_MAXIPQ];
    /**< Sgx3Dsfm link input queue information */

    System_LinkOutQueParams outQueParams;
    /**< SgxTest link Output queue information */

    Bool bEglInfoInCreate;
    /**< Flag to determine whether eglInfo is passed in Create */
    Void *EglInfo;
    /**< if bEglInfoInCreate == TRUE, the eglInfo is passed in this struct */

} Sgx3DsfmLink_CreateParams;

/*******************************************************************************
 *  Functions Prototypes
 *******************************************************************************
 */
static inline Void Sgx3DsfmLink_CreateParams_Init(
                                Sgx3DsfmLink_CreateParams *prm);

/*******************************************************************************
 *  Functions
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 *   \brief Sgx3Dsfm link register and init function
 *
 *          For each sgx3Dsfm instance
 *          - Creates link task
 *          - Registers as a link with the system API
 *
 *   \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
*/
Int32 Sgx3DsfmLink_init(void);

/**
 *******************************************************************************
 *
 *   \brief Sgx3Dsfm link de-register and de-init function
 *
 *          For each sgx3Dsfm instance
 *          - Deletes link task
 *          - De-registers as a link with the system API
 *
 *   \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
*/
Int32 Sgx3DsfmLink_deInit(void);

/**
 *******************************************************************************
 *
 *   \brief Function to initialize the Sgx3Dsfm Link Create Params
 *
 *          Sets default values for Sgx3Dsfm link create time parameters
 *          User/App can override these default values later.
 *
 *   \param prm [IN] Sgx3Dsfm Link create parameters
 *
 *   \return void
 *
 *******************************************************************************
*/
static inline Void Sgx3DsfmLink_CreateParams_Init(
                                Sgx3DsfmLink_CreateParams *prm)
{
    memset(prm, 0, sizeof(Sgx3DsfmLink_CreateParams));
}


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/* @} */

/* Nothing beyond this point */
