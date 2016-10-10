/*
 *******************************************************************************
 *
 * Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */

#include <vivi_plugin.h>
#include <vivi_preview.h>
#include <preview_priv.h>
#include "include/preview_chain.h"

struct preview_chain {
    struct vivi_chain_info info;
    struct preview_chain_func func;
};

static const struct preview_chain preview_ctab[PREVIEW_NUM_CHAINS] =
{
    {
        .info = {
            "lvds-vip-single-cam",
            PREVIEW_LVDS_SINGLE_CAM,
            PREVIEW_LVDS_SINGLE_CAM_NUM_EP_SINK,
            PREVIEW_LVDS_SINGLE_CAM_NUM_EP_SOURCE
        },
        .func = {
           {
                &chains_lvdsVipSingleCamCapture_CreateApp,
                &chains_lvdsVipSingleCamCapture_StartApp,
                &chains_lvdsVipSingleCamCapture_StopApp,
                &chains_lvdsVipSingleCamCapture_DeleteApp
            }
        }
    },
    {
        .info = {
            "lvds-vip-multi-cam",
            PREVIEW_LVDS_MULTI_CAM,
            PREVIEW_LVDS_MULTI_CAM_NUM_EP_SINK,
            PREVIEW_LVDS_MULTI_CAM_NUM_EP_SOURCE
        },
        .func = {
           {
                &chains_lvdsVipMultiCamCapture_CreateApp,
                &chains_lvdsVipMultiCamCapture_StartApp,
                &chains_lvdsVipMultiCamCapture_StopApp,
                &chains_lvdsVipMultiCamCapture_DeleteApp
            }
        }
    },
};

DEFINE_PLUGIN_NUM_CHAINS_FUNC(PREVIEW_PNAME_TOKEN)
{
    return (PREVIEW_NUM_CHAINS);
}

DEFINE_PLUGIN_CHAIN_INFO_FUNC(PREVIEW_PNAME_TOKEN, chain_info, idx)
{
    if (idx < PREVIEW_NUM_CHAINS) {
        strcpy(chain_info->cname, preview_ctab[idx].info.cname);
        chain_info->id = preview_ctab[idx].info.id;
        chain_info->num_sink_eplink = preview_ctab[idx].info.num_sink_eplink;
        chain_info->num_source_eplink = preview_ctab[idx].info.num_source_eplink;
    }
}

DEFINE_PLUGIN_CHAIN_CONTROL_FUNC(PREVIEW_PNAME_TOKEN, chain_func, chain_id)
{
    if (chain_id < PREVIEW_NUM_CHAINS) {
        chain_func->create = preview_ctab[chain_id].func.create;
        chain_func->start = preview_ctab[chain_id].func.start;
        chain_func->stop = preview_ctab[chain_id].func.stop;
        chain_func->delete = preview_ctab[chain_id].func.delete;
        chain_func->init = preview_ctab[chain_id].func.init;
        chain_func->create_with_source_ep = preview_ctab[chain_id].func.create_with_source_ep;
    }
}
