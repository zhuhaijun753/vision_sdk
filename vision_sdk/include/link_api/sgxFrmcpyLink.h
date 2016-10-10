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
 * \ingroup SAMPLE_LINUX_MODULE_API
 * \defgroup SGXFRMCPY_LINK_API SgxFrmcpy Link API
 *
 * \brief  This module has the interface for using SgxFrmcpy Link
 *
 *         SgxFrmcpy Link is used to feed video frames to SGX for
 *         rendering.
 *
 * @{
 *
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \file sgxFrmcpyLink.h
 *
 * \brief SgxFrmcpy Link API
 *
 * \version 0.0 (Jun 2014) : [SS] First version
 *
 *******************************************************************************
 */

#ifndef SGXFRMCPY_LINK_H_
#define SGXFRMCPY_LINK_H_

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

/* @{ */
/**
 *******************************************************************************
 *
 * \brief Macro defining SGX Link egl context sharing command.
 *
 *
 *******************************************************************************
 */
#define SGXLINK_CMD_ACQUIRE_EGL_INFO                       (0x8000U)

/* @} */


/*******************************************************************************
 *  Enum's
 *******************************************************************************
 */

typedef enum {

    SGXFRMCPY_RENDER_TYPE_1x1 = 0,
    /**< Display video rendered full-screen on the display */

    SGXFRMCPY_RENDER_TYPE_2x2 = 1,
    /**< Display video rendered full-screen as 2x2 mosiac on the display */

    SGXFRMCPY_RENDER_TYPE_3D_CUBE = 2,
    /**< Display video rendered as a rotating 3D cube */

    SGXFRMCPY_RENDER_TYPE_MAX = 3,
    /**< Max value for this enum */

    SGXFRMCPY_RENDER_TYPE_FORCE_32BITS = 0x7FFFFFFF
    /**< value to force enum to be 32-bit */

} SgxFrmcpy_RenderType;

/*******************************************************************************
 *  Data structures
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 *   \brief Structure containing the SgxFrmcpy link create time parameters
 *
 *          This structure is used to create and configure a SgxFrmcpy link
 *          instance.
 *
 *******************************************************************************
*/
typedef struct
{
    UInt32 displayWidth;
    /**< Display width */

    UInt32 displayHeight;
    /**< Display height */

    SgxFrmcpy_RenderType renderType;
    /**< type of rendering to do using OpenGL */

    System_BufferType inBufType;
    /**< Input buffer type can be
     *   SYSTEM_BUFFER_TYPE_VIDEO_FRAME
     *   or
     *   SYSTEM_BUFFER_TYPE_VIDEO_FRAME_CONTAINER
     */

    System_LinkInQueParams inQueParams;
    /**< SgxFrmcpy link input queue information */

    System_LinkOutQueParams outQueParams;
    /**< SgxFrmcpy link Output queue information */

    Bool bEglInfoInCreate;
    /**< Flag to determine whether eglInfo is passed in Create */
    Void *EglInfo;
    /**< if bEglInfoInCreate == TRUE, the eglInfo is passed in this struct */

} SgxFrmcpyLink_CreateParams;

/*******************************************************************************
 *  Functions Prototypes
 *******************************************************************************
 */
static inline Void SgxFrmcpyLink_CreateParams_Init(
                                 SgxFrmcpyLink_CreateParams *prm);

/*******************************************************************************
 *  Functions
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 *   \brief SgxFrmcpy link register and init function
 *
 *          For each sgxFrmcpy instance (VID1, VID2, VID3 or GRPX1)
 *          - Creates link task
 *          - Registers as a link with the system API
 *
 *   \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
*/
Int32 SgxFrmcpyLink_init(void);

/**
 *******************************************************************************
 *
 *   \brief SgxFrmcpy link de-register and de-init function
 *
 *          For each sgxFrmcpy instance (VID1, VID2, VID3 or GRPX1)
 *          - Deletes link task
 *          - De-registers as a link with the system API
 *
 *   \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
*/
Int32 SgxFrmcpyLink_deInit(void);

/**
 *******************************************************************************
 *
 *   \brief Function to initialize the SgxFrmcpy Link Create Params
 *
 *          Sets default values for SgxFrmcpy link create time parameters
 *          User/App can override these default values later.
 *
 *   \param prm [IN] SgxFrmcpy Link create parameters
 *
 *   \return void
 *
 *******************************************************************************
*/
static inline Void SgxFrmcpyLink_CreateParams_Init(
                                 SgxFrmcpyLink_CreateParams *prm)
{
    memset(prm, 0, sizeof(SgxFrmcpyLink_CreateParams));

    prm->renderType = SGXFRMCPY_RENDER_TYPE_1x1;
    prm->inBufType = SYSTEM_BUFFER_TYPE_VIDEO_FRAME;
}


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/* @} */

/* Nothing beyond this point */
