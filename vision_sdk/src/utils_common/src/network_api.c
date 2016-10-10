/*
 *******************************************************************************
 *
 * Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */
#include <src/utils_common/include/network_api.h>


#define NETWORK_INVALID_SOCKET 0xFFFFFFFFU

Int32 Network_read(Network_SockObj *pObj, UInt8 *dataBuf, UInt32 *dataSize)
{
    Int32 status = 0;

#if ( defined(NDK_PROC_TO_USE_IPU1_0) && defined(BUILD_M4_0) ) || \
    ( defined(NDK_PROC_TO_USE_IPU1_1) && defined(BUILD_M4_1) ) || \
    ( defined(NDK_PROC_TO_USE_IPU2)   && defined(BUILD_M4_2) ) || \
    ( defined(NDK_PROC_TO_USE_A15_0)  && defined(BUILD_A15) )

    UInt32 tmpDataSize;
    Int32 actDataSize = 0;

    tmpDataSize = *dataSize;

    while(tmpDataSize > 0U)
    {

        /* MISRA.CAST.PTR_TO_INT
         * MISRAC_2004_Rule_11.3:Cast between a pointer and an integral type
         * State: Defer -> Waiver -> Case by case
         * connectedSockFd is defined based on type in NDK package
         */
        actDataSize = recv(pObj->connectedSockFd, (void *)dataBuf, tmpDataSize, 0);
        if(actDataSize <= 0)
        {
            *dataSize = 0U;
            status = -1;
            break;
        }
        else
        {
            dataBuf += actDataSize;
            tmpDataSize -= (UInt32)actDataSize;
        }
    }

#endif

    return status;
}


Int32 Network_write(Network_SockObj *pObj, UInt8 *dataBuf, UInt32 dataSize)
{
    Int32 status = 0;

#if ( defined(NDK_PROC_TO_USE_IPU1_0) && defined(BUILD_M4_0) ) || \
    ( defined(NDK_PROC_TO_USE_IPU1_1) && defined(BUILD_M4_1) ) || \
    ( defined(NDK_PROC_TO_USE_IPU2)   && defined(BUILD_M4_2) ) || \
    ( defined(NDK_PROC_TO_USE_A15_0)  && defined(BUILD_A15) )
    Int32 actDataSize=0;

    while(dataSize > 0 ) {
        actDataSize = (Int32)send(pObj->connectedSockFd, dataBuf, dataSize, 0);

        if(actDataSize <= 0)
        {
            break;
        }
        else
        {
            dataBuf += actDataSize;
            dataSize -= (UInt32)actDataSize;
        }
    }

    if( dataSize > 0 ) {
        status = -1;
    }
#endif

    return status;
}

Int32 Network_open(Network_SockObj *pObj, UInt32 port)
{
    Int32 status = 0;

#if ( defined(NDK_PROC_TO_USE_IPU1_0) && defined(BUILD_M4_0) ) || \
    ( defined(NDK_PROC_TO_USE_IPU1_1) && defined(BUILD_M4_1) ) || \
    ( defined(NDK_PROC_TO_USE_IPU2)   && defined(BUILD_M4_2) ) || \
    ( defined(NDK_PROC_TO_USE_A15_0)  && defined(BUILD_A15) )

    struct sockaddr_in   sin1;
    Int32 option = 1;

    pObj->connectedSockFd = (SOCKET)NETWORK_INVALID_SOCKET;
    pObj->port = port;
    pObj->sockFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if( pObj->sockFd == (SOCKET)NETWORK_INVALID_SOCKET)
    {
        Vps_printf(" NETWORK: Unable to open socket (port=%d)!!!\n", port);
        status = -1;
    }
    else
    {
        /* Bind to the specified Server port */
        bzero( &sin1, sizeof(struct sockaddr_in) );
        sin1.sin_family     = AF_INET;
        sin1.sin_addr.s_addr = INADDR_ANY;
        sin1.sin_port       = htons((UInt16)(pObj->port));

        setsockopt ( pObj->sockFd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof( option ) );

        if( bind( pObj->sockFd,(struct sockaddr *)&sin1, sizeof(sin1) ) < 0 )
        {
            Vps_printf(" NETWORK: Unable to bind() (port=%d) !!!\n", port);
            fdClose( pObj->sockFd );
            pObj->sockFd = (SOCKET)NETWORK_INVALID_SOCKET;
            status = -1;
        }
        else
        {
            if( listen( pObj->sockFd, 5 ) < 0 )
            {
                fdClose( pObj->sockFd );
                pObj->sockFd = (SOCKET)NETWORK_INVALID_SOCKET;
                status = -1;
            }
        }
    }

#endif

    return status;
}

Int32 Network_close(Network_SockObj *pObj, Bool closeServerSock)
{
#if ( defined(NDK_PROC_TO_USE_IPU1_0) && defined(BUILD_M4_0) ) || \
    ( defined(NDK_PROC_TO_USE_IPU1_1) && defined(BUILD_M4_1) ) || \
    ( defined(NDK_PROC_TO_USE_IPU2)   && defined(BUILD_M4_2) ) || \
    ( defined(NDK_PROC_TO_USE_A15_0)  && defined(BUILD_A15) )

    if(pObj->connectedSockFd != (SOCKET)NETWORK_INVALID_SOCKET)
    {
        fdClose( pObj->connectedSockFd );
        pObj->connectedSockFd = (SOCKET)NETWORK_INVALID_SOCKET;
    }

    if(closeServerSock)
    {
        if(pObj->sockFd != (SOCKET)NETWORK_INVALID_SOCKET)
        {
            fdClose( pObj->sockFd );
            pObj->sockFd = (SOCKET)NETWORK_INVALID_SOCKET;
        }
    }
#endif

    return 0;
}

Int32 Network_waitConnect(Network_SockObj *pObj, UInt32 timeout)
{
    Int32 status = 0;

#if ( defined(NDK_PROC_TO_USE_IPU1_0) && defined(BUILD_M4_0) ) || \
    ( defined(NDK_PROC_TO_USE_IPU1_1) && defined(BUILD_M4_1) ) || \
    ( defined(NDK_PROC_TO_USE_IPU2)   && defined(BUILD_M4_2) ) || \
    ( defined(NDK_PROC_TO_USE_A15_0)  && defined(BUILD_A15) )

    pObj->pollitem[0].fd = pObj->sockFd;
    pObj->pollitem[0].eventsRequested = POLLIN;

    if( fdPoll( pObj->pollitem, 1U, timeout ) == SOCKET_ERROR )
    {
        Vps_printf(" NETWORK: fdPoll() failed with SOCKET_ERROR (port=%d) !!!\n", pObj->port);
        status =  -1;
    }
    else
    {

        if( pObj->pollitem[0].eventsDetected == FALSE)
        {
            /* NO connection, retry */
            status =  0;
        }
        else
        {
            if( (UInt32)(pObj->pollitem[0].eventsDetected) & (UInt32)POLLNVAL )
            {
                Vps_printf(" NETWORK: fdPoll() failed with POLLNVAL (port=%d) !!!\n", pObj->port);
                status = -1;
            }
            else
            {

                if( (UInt32)(pObj->pollitem[0].eventsDetected) & (UInt32)POLLIN )
                {
                    pObj->connectedSockFd = accept( pObj->sockFd, 0, 0 );

                    if( pObj->connectedSockFd != (SOCKET)NETWORK_INVALID_SOCKET )
                    {
                        status = 1;
                    }
                }
            }
        }
    }
#endif
    /* NO connection, retry */
    return status;
}

Int32 Network_sessionOpen(BspOsal_TaskHandle handle)
{
#if ( defined(NDK_PROC_TO_USE_IPU1_0) && defined(BUILD_M4_0) ) || \
    ( defined(NDK_PROC_TO_USE_IPU1_1) && defined(BUILD_M4_1) ) || \
    ( defined(NDK_PROC_TO_USE_IPU2)   && defined(BUILD_M4_2) ) || \
    ( defined(NDK_PROC_TO_USE_A15_0)  && defined(BUILD_A15) )

    if(handle==NULL)
    {
        handle = Task_self();
    }

    /* Allocate the file environment for this task */
    fdOpenSession( handle );
#endif

    return 0;
}

Int32 Network_sessionClose(BspOsal_TaskHandle handle)
{
#if ( defined(NDK_PROC_TO_USE_IPU1_0) && defined(BUILD_M4_0) ) || \
    ( defined(NDK_PROC_TO_USE_IPU1_1) && defined(BUILD_M4_1) ) || \
    ( defined(NDK_PROC_TO_USE_IPU2)   && defined(BUILD_M4_2) ) || \
    ( defined(NDK_PROC_TO_USE_A15_0)  && defined(BUILD_A15) )

    if(handle==NULL)
    {
        handle = Task_self();
    }

    fdCloseSession( handle );

#endif

    return 0;
}

