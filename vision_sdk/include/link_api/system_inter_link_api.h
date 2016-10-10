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
 *  \ingroup SYSTEM_LINK_API
 *  \defgroup SYSTEM_LINK_INTER_LINK_API  System API for inter link communication
 *
 *  Used by links to talk to each other. This API is not used by the
 *  users of the links. Typically used by a implementor of a link
 *
 *  @{
 *
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 *  \file system_inter_link_api.h
 *
 *  \brief System API for inter link communication
 *
 *******************************************************************************
 */

#ifndef SYSTEM_INTERNAL_LINK_API_H_
#define SYSTEM_INTERNAL_LINK_API_H_

/*******************************************************************************
 *  Include files
 *******************************************************************************
 */
#include <include/link_api/system.h>
#include <include/link_api/system_buffer.h>

/*******************************************************************************
 *  Defines
 *******************************************************************************
 */

/* None */

/**
 *******************************************************************************
 *
 *  \brief Structure that is exchanged across processor by IPC OUT/IN links
 *
 *******************************************************************************
 */
typedef struct {

    UInt64              srcTimestamp;
    /**< Timestamp updated at the source. This is the timestamp at which the
     *   source buffer was available, in units of usec's
     */
    UInt64              linkLocalTimestamp;
    /**< Timestamp at which the buffer was available at the input of the local
     *   link, in units of usec's
     */

    UInt64                ipcPrfTimestamp64[2];
    /**< Used to measure IPC overheads */

    UInt32  flags;
    /**< See SYSTEM_BUFFER_FLAG_* */

    UInt32  orgSystemBufferPtr;
    /**< Original system buffer pointer */

    UInt32              frameId;
    /**< Unique ID associated with a frame that is set by Source Link */

    UInt32              rsv0;
    /**< size of this structure MUST be multiple of 64-bit */

    UInt32  payload[SYSTEM_MAX_PAYLOAD_SIZE/sizeof(UInt32)];
    /**< Payload data for this buffer */

} System_IpcBuffer;

/**
 *******************************************************************************
 *
 *   \brief Maximum number of buffers in buffer list
 *
 *******************************************************************************
 */
#define SYSTEM_MAX_BUFFERS_IN_BUFFER_LIST       (64U)

/**
 *******************************************************************************
 *
 * \brief List of System_Buffer pointer
 *        In many cases a list of System_Buffers are exchanged at function
 *        boundary. This structure holds a list of System_Buffer pointers.
 *
 *******************************************************************************
 */
typedef struct {

    UInt32         numBuf;
    /**< Number of buffers in buffer list */

    System_Buffer *buffers[SYSTEM_MAX_BUFFERS_IN_BUFFER_LIST];
    /**< Pointer to individual buffers */

} System_BufferList;

/*******************************************************************************
 *  Functions Prototypes
 *******************************************************************************
 */
static inline UInt32 System_Buffer_Flag_Get_Buf_Type(UInt32 container);
static inline UInt32 System_Buffer_Flag_Set_Buf_Type(UInt32 container,UInt32 value);
static inline UInt32 System_Buffer_Flag_Get_Ch_Num(UInt32 container);
static inline UInt32 System_Buffer_Flag_Set_Ch_Num(UInt32 container,UInt32 value);
static inline UInt32 System_Buffer_Flag_Get_Payload_Size(UInt32 container);
static inline UInt32 System_Buffer_Flag_Set_Payload_Size(UInt32 container,UInt32 value);
/*******************************************************************************
 *  Functions
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 *  \brief Function that returns the LINKs output channel configurations
 *
 *******************************************************************************
 */
typedef Int32(*System_GetLinkInfoCb) (Void            *pTsk,
                                      System_LinkInfo *info);


/**
 *******************************************************************************
 *
 *   \brief Function expected to be called by the LINK
 *          to get the input frames
 *
 *******************************************************************************
 */
typedef Int32(*System_LinkGetOutputBuffersCb) (Void              *pTsk,
                                               UInt16             queId,
                                               System_BufferList *pList);

/**
 *******************************************************************************
 *
 *   \brief Function expected to be called by the LINK to return received frame,
 *          once processed by the LINK
 *
 *******************************************************************************
 */
typedef Int32(*System_LinkPutEmptyBuffersCb) (Void              *pTsk,
                                              UInt16             queId,
                                              System_BufferList *pList);

/**
 *******************************************************************************
 *
 *  \brief LINK Instance Info
 *
 *   Each LINK is expected to register with "system" with the following
 *   information.
 *   Using these links, the system would form a chain with multiple LINKs
 *
 *******************************************************************************
 */
typedef struct {
    Void *pTsk;

    System_LinkGetOutputBuffersCb linkGetFullBuffers;
    /**< Function expected to be called by the LINK to get the input frames */

    System_LinkPutEmptyBuffersCb  linkPutEmptyBuffers;
    /**< Function expected to be called by the LINK to return received frame,
         once processed by the LINK */

    System_GetLinkInfoCb           getLinkInfo;
    /**<  Function that returns the LINKs output channel configurations */

} System_LinkObj;

/**
 *******************************************************************************
 *
 *  \brief Function called by a link to get input frames
 *
 *  \param linkId  [IN]  Link ID of previous link
 *  \param queId   [IN]  Que ID of previous link
 *  \param pBufList   [OUT] List of buffers available for processing
 *                       from previous link
 *
 *  \return SYSTEM_LINK_STATUS_SOK on success, else error code
 *
 *******************************************************************************
 */
Int32 System_getLinksFullBuffers(UInt32             linkId,
                                 UInt16             queId,
                                 System_BufferList *pBufList);


/**
 *******************************************************************************
 *
 *  \brief Function called by a link to return received frame,
 *        once processed by the LINK
 *
 *  \param linkId  [IN] Link ID of previous link
 *  \param queId   [IN] Que ID of previous link
 *  \param pBufList   [IN] List of buffers released to previous link
 *
 *  \return SYSTEM_LINK_STATUS_SOK on success, else error code
 *
 *******************************************************************************
 */
Int32 System_putLinksEmptyBuffers(UInt32             linkId,
                                  UInt16             queId,
                                  System_BufferList *pBufList);

/**
 *******************************************************************************
 *
 *  \brief Send a command to a link
 *
 *  \param linkId  [IN] Link ID of previous link
 *  \param cmd     [IN] Command to send
 *  \param payload [IN] 32-bit payload pointer/value to send
 *
 *  \return SYSTEM_LINK_STATUS_SOK on success, else error code
 *
 *******************************************************************************
 */
Int32 System_sendLinkCmd(UInt32 linkId, UInt32 cmd, Void *payload);

/**
 *******************************************************************************
 *
 *  \brief Register a link with the system
 *
 *  \param linkId  [IN] Link ID of previous link
 *  \param pTskObj [IN] Information needed to chain the links together
 *
 *  \return SYSTEM_LINK_STATUS_SOK on success, else error code
 *
 *******************************************************************************
 */
Int32 System_registerLink(UInt32 linkId, const System_LinkObj * pTskObj);

static inline UInt32 System_Buffer_Flag_Get_Buf_Type(UInt32 container)
{
/* MISRA.ONEDEFRULE.FUNC
 * MISRAC_2004 Rule 8.5
 * Function Definition in header file
 * KW State: Defer -> Waiver -> Case by case
 * MISRAC_WAIVER: This function initializes the create time argument to default.
 *  Defining this in the header file aids maintainability.
 */

     return SystemUtils_unpack_bitfield(container, 0x0000000FU, 0U);

}

static inline UInt32 System_Buffer_Flag_Set_Buf_Type(UInt32 container,UInt32 value)
{
/* MISRA.ONEDEFRULE.FUNC
 * MISRAC_2004 Rule 8.5
 * Function Definition in header file
 * KW State: Defer -> Waiver -> Case by case
 * MISRAC_WAIVER: This function initializes the create time argument to default.
 *  Defining this in the header file aids maintainability.
 */

    container = SystemUtils_pack_bitfield(container, value, 0x0000000FU, 0U);
    return container;
}

static inline UInt32 System_Buffer_Flag_Get_Ch_Num(UInt32 container)
{
/* MISRA.ONEDEFRULE.FUNC
 * MISRAC_2004 Rule 8.5
 * Function Definition in header file
 * KW State: Defer -> Waiver -> Case by case
 * MISRAC_WAIVER: This function initializes the create time argument to default.
 *  Defining this in the header file aids maintainability.
 */

    return SystemUtils_unpack_bitfield(container, 0x00000FF0U, 4U);
}

static inline UInt32 System_Buffer_Flag_Set_Ch_Num(UInt32 container,UInt32 value)
{
/* MISRA.ONEDEFRULE.FUNC
 * MISRAC_2004 Rule 8.5
 * Function Definition in header file
 * KW State: Defer -> Waiver -> Case by case
 * MISRAC_WAIVER: This function initializes the create time argument to default.
 *  Defining this in the header file aids maintainability.
 */

    container = SystemUtils_pack_bitfield(container, value, 0x00000FF0U, 4U);
    return container;
}

static inline UInt32 System_Buffer_Flag_Get_Payload_Size(UInt32 container)
{
/* MISRA.ONEDEFRULE.FUNC
 * MISRAC_2004 Rule 8.5
 * Function Definition in header file
 * KW State: Defer -> Waiver -> Case by case
 * MISRAC_WAIVER: This function initializes the create time argument to default.
 *  Defining this in the header file aids maintainability.
 */

    return SystemUtils_unpack_bitfield(container, 0x00FFF000U, 12U);
}

static inline UInt32 System_Buffer_Flag_Set_Payload_Size(UInt32 container,UInt32 value)
{
/* MISRA.ONEDEFRULE.FUNC
 * MISRAC_2004 Rule 8.5
 * Function Definition in header file
 * KW State: Defer -> Waiver -> Case by case
 * MISRAC_WAIVER: This function initializes the create time argument to default.
 *  Defining this in the header file aids maintainability.
 */

    container = SystemUtils_pack_bitfield(container, value, 0x00FFF000U, 12U);
    return container;
}

#endif

/* @} */
