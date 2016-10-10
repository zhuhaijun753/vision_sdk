/*
 *******************************************************************************
 *
 * Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */
#include "chains_fastBoot_issIspSimcop_pd_Display_priv.h"
#include <examples/tda2xx/include/chains_common.h>

#define ANALYTICS_ON            (1U)
#define ANALYTICS_OFF           (0U)

Void chains_fastBoot_stopIpuLoadCalculation(Void);
Void chains_fastBoot_stopDspEveLoadCalculation(Void);
Void chains_fastBoot_switchDspEveOn(chains_fastBoot_issIspSimcop_pd_DisplayAppObj *pObj,
                                              Utils_BootSlaves_Params *bootParams);
Void chains_fastBoot_switchDspEveOff(chains_fastBoot_issIspSimcop_pd_DisplayAppObj *pObj);
