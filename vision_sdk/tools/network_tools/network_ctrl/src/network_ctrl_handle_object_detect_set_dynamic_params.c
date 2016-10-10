/*
 *******************************************************************************
 *
 * Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */

#include "network_ctrl_priv.h"

void handleObjectDetectSetDynamicParams()
{
    unsigned int prmSize;
    unsigned int params[16];
    unsigned int i;

    for(i=0; i<sizeof(params)/sizeof(params[0]); i++)
    {
        params[i]  = atoi(gNetworkCtrl_obj.params[i]);
    }

    SendCommand(gNetworkCtrl_obj.command, &params,
                                    sizeof(params));
    RecvResponse(gNetworkCtrl_obj.command, &prmSize);
}

