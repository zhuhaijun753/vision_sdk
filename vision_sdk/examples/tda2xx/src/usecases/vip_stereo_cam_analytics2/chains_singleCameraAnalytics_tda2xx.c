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
#include <examples/tda2xx/include/chains.h>

Void Chains_vipStereoCameraAnalytics2(Chains_Ctrl *chainsCfg)
{
    chainsCfg->captureSrc = CHAINS_CAPTURE_SRC_HDMI_1080P;
    Chains_fileRxCameraAnalyticsTda2xx(chainsCfg);
}

