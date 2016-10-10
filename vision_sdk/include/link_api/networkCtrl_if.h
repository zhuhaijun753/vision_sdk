 /*
 *******************************************************************************
 *
 * Copyright (C) 2014s Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \file networkCtrl_if.h
 *
 * \brief Header and command interface between PC application
 *        and target application
 *
 *        This file will be common between the PC application
 *        and target application, hence it should not include any target
 *        specific data types and include files
 *
 *******************************************************************************
 */

#ifndef NETWORK_CTRL_IF_H_
#define NETWORK_CTRL_IF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/*******************************************************************************
 *  Defines
 *******************************************************************************
 */

/*******************************************************************************
 *  \brief TCP/IP port on which the server is listening
 *******************************************************************************
 */
#define NETWORK_CTRL_SERVER_PORT    (5000U)



/*******************************************************************************
 *  \brief Default TCP/IP port on which the network RX link will listen
 *******************************************************************************
 */
#define NETWORK_RX_SERVER_PORT      (6000U)

/*******************************************************************************
 *  \brief Default TCP/IP port on which the network TX link will listen
 *******************************************************************************
 */
#define NETWORK_TX_SERVER_PORT      (7000U)

/*******************************************************************************
 *  \brief Header packet magic number to confirm received packet is a header
 *******************************************************************************
 */
#define NETWORK_CTRL_HEADER         (0x1234ABCDU)
#define NETWORK_RX_HEADER           (0x5678ABCDU)
#define NETWORK_TX_HEADER           (0xABCD4321U)

/*******************************************************************************
 *  \brief Max length of command string
 *******************************************************************************
 */
#define NETWORK_CTRL_CMD_STRLEN_MAX     (64U)

/*******************************************************************************
 *  \brief Flag that is set in the 'flags' field of the header to indicate
 *         this packet is a ACK packet for a previously send command
 *******************************************************************************
 */
#define NETWORK_CTRL_FLAG_ACK            (0x00000001U)



/*******************************************************************************
 *  Data structure's
 *******************************************************************************
 */

typedef struct {

    uint32_t    header;
    /**< Header magic number NETWORK_CTRL_HEADER */

    char         cmd[NETWORK_CTRL_CMD_STRLEN_MAX];
    /**< Command, specified as a string of char's */

    uint32_t    returnValue;
    /**< Return value that is set by the command handler */

    uint32_t    flags;
    /**< command specified flags, see NETWORK_CTRL_FLAG_* */

    uint32_t    prmSize;
    /**< Size of input parameters in units of bytes.
     *   Can be 0 if no parameters need to sent for a command
     */

} NetworkCtrl_CmdHeader;

/**
 *******************************************************************************
 *
 * \brief Types of payload's that can be exchanged over network
 *
 *******************************************************************************
 */
#define NETWORK_RX_TYPE_META_DATA                    (0x1U)
#define NETWORK_RX_TYPE_BITSTREAM_MJPEG              (0x2U)
#define NETWORK_RX_TYPE_VIDEO_FRAME_YUV422I_YUYV     (0x8U)
#define NETWORK_RX_TYPE_VIDEO_FRAME_YUV420SP_UV      (0x9U)


typedef struct {

    uint32_t    header;
    /**< Header magic number NETWORK_RX_HEADER */

    uint32_t    payloadType;
    /**< Payload type NETWORK_RX_TYPE_* */

    uint32_t    chNum;
    /**< channel ID */

    uint32_t    dataSize;
    /**< Size of payload data in bytes */

    uint32_t    width;
    /**< Width of video frame */

    uint32_t    height;
    /**< Height of video frame */

    uint32_t    pitch[2U];
    /**< Pitch of video frame in bytes */

} NetworkRx_CmdHeader;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/* @} */


