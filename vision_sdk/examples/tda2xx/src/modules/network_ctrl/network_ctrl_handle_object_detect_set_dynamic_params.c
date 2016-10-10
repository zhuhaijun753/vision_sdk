/*
 *******************************************************************************
 *
 * Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */


#include "network_ctrl_priv.h"
#include <include/link_api/algorithmLink_objectDetection.h>

Void NetworkCtrl_cmdHandlerObjectDetectSetDynamicParams(char *cmd, UInt32 prmSize)
{
    UInt32 linkId;
    AlgorithmLink_ObjectDetectEnableAlgParams algParams;
    UInt32 params[16];
    Int32 status;

    /* alloc tmp buffer for parameters */
    if(prmSize == sizeof(params))
    {
        /* read parameters */
        NetworkCtrl_readParams((UInt8*)(&params[0]), sizeof(params));

        algParams.baseClassControl.controlCmd = ALGORITHM_LINK_OBJECT_DETECT_CMD_ENABLE_ALG;
        algParams.baseClassControl.size= sizeof(algParams);
        algParams.enablePD  = params[0];
        algParams.detectorTypePD  = params[1];
        algParams.trackingMethodPD  = params[2];
        algParams.softCascadeThPD  = params[3];
        algParams.strongCascadeThPD  = params[4];
        algParams.enableTSR  = params[5];
        algParams.detectorTypeTSR  = params[6];
        algParams.trackingMethodTSR  = params[7];
        algParams.recognitionMethodTSR  = params[8];
        algParams.softCascadeThTSR  = params[9];
        algParams.strongCascadeThTSR  = params[10];
        algParams.enableVD  = params[11];
        algParams.detectorTypeVD  = params[12];
        algParams.trackingMethodVD  = params[13];
        algParams.softCascadeThVD  = params[14];
        algParams.strongCascadeThVD  = params[15];
        algParams.enableAdvConfig = TRUE;

        linkId = DSP1_LINK (SYSTEM_LINK_ID_ALG_0);

        status= System_linkControl(
            linkId,
            ALGORITHM_LINK_CMD_CONFIG,
            &algParams,
            sizeof(algParams),
            TRUE
        );

        UTILS_assert(0 == status);
    }
    else
    {
        Vps_printf(" NETWORK_CTRL: %s: Insufficient parameters (%d bytes) specified !!!\n", cmd, prmSize);
    }

    /* send response */
    NetworkCtrl_writeParams(NULL, 0, 0);
}
