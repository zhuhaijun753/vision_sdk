/*
 *******************************************************************************
 *
 * Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */
#ifndef __PREVIEW_CHAIN_H__
#define __PREVIEW_CHAIN_H__

enum preview_chain_id {
    PREVIEW_LVDS_SINGLE_CAM,
    PREVIEW_LVDS_MULTI_CAM,
    PREVIEW_NUM_CHAINS,
};

/* preview-chain1-info */
#define PREVIEW_LVDS_SINGLE_CAM_NUM_EP_SINK   (1)
#define PREVIEW_LVDS_SINGLE_CAM_NUM_EP_SOURCE (0)
/* preview-chain2-info */
#define PREVIEW_LVDS_MULTI_CAM_NUM_EP_SINK   (1)
#define PREVIEW_LVDS_MULTI_CAM_NUM_EP_SOURCE (0)
/* preview-chain3-info */
#define PREVIEW_BUF_LOOP_NUM_EP_SINK   (1)
#define PREVIEW_BUF_LOOP_NUM_EP_SOURCE   (1)

Int32 chains_lvdsVipSingleCamCapture_CreateApp(struct vivi_sink *sink,
		struct vivi_source *source);
Int32 chains_lvdsVipSingleCamCapture_StartApp();
Int32 chains_lvdsVipSingleCamCapture_StopApp();
Int32 chains_lvdsVipSingleCamCapture_DeleteApp();

Int32 chains_lvdsVipMultiCamCapture_CreateApp(struct vivi_sink *sink,
		struct vivi_source *source);
Int32 chains_lvdsVipMultiCamCapture_StartApp();
Int32 chains_lvdsVipMultiCamCapture_StopApp();
Int32 chains_lvdsVipMultiCamCapture_DeleteApp();

Int32 chains_bufLoop_StartApp(Void);
Int32 chains_bufLoop_StopApp(Void);
Int32 chains_bufLoop_DeleteApp(Void);
Void chains_bufLoop_InitSourceEpApp(struct vivi_sink *sink,
		struct vivi_source *source);
Int32 chains_bufLoop_CreateSourceEpApp(struct vivi_sink *sink,
		struct vivi_source *source, struct vivi_source_config *sourceCfg[]);


#endif /* __PREVIEW_CHAIN_H__ */
