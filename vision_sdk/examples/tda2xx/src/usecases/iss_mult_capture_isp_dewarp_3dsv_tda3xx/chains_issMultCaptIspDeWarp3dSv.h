/*
*******************************************************************************
*
* Copyright (C) 2016 Texas Instruments Incorporated - http://www.ti.com/
* ALL RIGHTS RESERVED
*
*******************************************************************************
*/


#ifndef CHAINS_ISSMULTICAPTISPSIMCOPSRV_H_
#define CHAINS_ISSMULTICAPTISPSIMCOPSRV_H_

#include "chains_issMultCaptIspDeWarp3dSv_priv.h"
#include "chains_issMultCaptIspDeWarp3dSv_grpx_priv.h"
#include <examples/tda2xx/src/alg_plugins/surroundview/include/svCommonDefs.h>
#include <examples/tda2xx/include/chains_common.h>

/* Maximum No of view points */
#define SRV_UC_3D_MAX_NUM_VIEW_POINTS           (100U)

/* SRV Num Views/Channels */
#define SRV_UC_3D_NUM_CAMERAS                   (4U)

/* Parameters of these is used while creating */
#define SRV_UC_3D_DEFAULT_VIEW_TO_USE           (0U)

#define SRV_UC_3D_MESH_TABLE_PITCH(w, r)((((((w)/(r))+1) + 15U) & ~15U) * (4U))
/**< LDC Mesh table pitch */

#define SRV_UC_3D_DEWARP_NUM_OUTPUT_BUFS        (3U)
/**< Number of output buffer that would be allocated at DeWarp */
#define SRV_UC_3D_ISP_NUM_OUTPUT_BUFS           (3U)
/**< Number of output buffer that would be allocated at ISP */

/* Capture Resolution of AR0140 sensor */
#define SRV_UC_3D_CAPT_SENSOR_AR140_WIDTH       (1280U)
#define SRV_UC_3D_CAPT_SENSOR_AR140_HEIGHT      (800U)

/* Capture Resolution of OV10640 sensor */
#define SRV_UC_3D_CAPT_SENSOR_OV10640_WIDTH     (1280U)
#define SRV_UC_3D_CAPT_SENSOR_OV10640_HEIGHT    (720U)

#define SRV_UC_3D_ISP_OUTPUT_WIDTH_W_PAD    (SRV_UC_3D_CAPT_SENSOR_AR140_WIDTH)
#define SRV_UC_3D_ISP_OUTPUT_HEIGHT_W_PAD   (1008U)

/* Maximum output size from LDC / SIMCOP */
#define SRV_UC_3D_LDC_OUTPUT_WIDTH_MAX      (1280U)
#define SRV_UC_3D_LDC_OUTPUT_HEIGHT_MAX     (1008U)

/* Maximum output size from SYNTH */
#define SRV_UC_3D_SYNTH_OUTPUT_WIDTH_MAX    (752U)
#define SRV_UC_3D_SYNTH_OUTPUT_HEIGHT_MAX   (1008U)

/* LDC Frm size and other parameters */
#define SRV_UC_3D_LDC_BLOCK_WIDTH           (16U)
#define SRV_UC_3D_LDC_BLOCK_HEIGHT          (16U)

/* SRV Car Box Size */
#define SRV_UC_3D_CARBOX_WIDTH              (0U)
#define SRV_UC_3D_CARBOX_HEIGHT             (0U)

/**
 *******************************************************************************
 *
 *  \brief  SRV_UC_3D_LDC_SLICE_PRM
 *
 *        This enum contains the index to the slice parameter set structure
 *        'gLdcSlicePrm'.
 *
 *******************************************************************************
*/
typedef enum
{
    SRV_UC_3D_LDC_SLICE_PRM_1_16x16,
    /* 1 slice with 16x16 block size */
    SRV_UC_3D_LDC_SLICE_PRM_1_16x8,
    /* 1 slice with 16x8 block size */
    SRV_UC_3D_LDC_SLICE_PRM_2_16x16_16x8,
    /* 2 slices with 16x16 and 16x8 block size */
    SRV_UC_3D_LDC_SLICE_PRM_2_16x16_16x4,
    /* 2 slices with 16x16 and 16x4 block size */
    SRV_UC_3D_LDC_SLICE_PRM_1_16x4,
    /* 1 slice with 16x4 block size */
    SRV_UC_3D_LDC_SLICE_PRM_2_16x8_16x4,
    /* 2 slices with 16x8 and 16x4 block size */
    SRV_UC_3D_LDC_SLICE_PRM_3_16x16_16x8_16x4,
    /* 3 slices with 16x16, 16x8 and 16x4 block size */
    SRV_UC_3D_LDC_SLICE_PRM_MAX,
    /* Maximum No of Slice Parameters */
}SRV_UC_3D_LDC_SLICE_PRM;

/**
 *******************************************************************************
 *
 *  \brief  chains_issMultCaptIspDewarpSv_DisplayAppObj
 *
 *        This structure contains all the LinksId's and create Params.
 *        The same is passed to all create, start, stop functions.
 *
 *******************************************************************************
*/
typedef struct {

    chains_issMultCaptIspDeWarp3dSvObj ucObj;
    chains_issMultCaptIspDeWarp3dSv_grpxObj ucGrpxObj;

    IssIspConfigurationParameters ispConfig;

    IssM2mSimcopLink_ConfigParams simcopConfig;
    vpsissldcConfig_t             ldcCfg;
    vpsissvtnfConfig_t            vtnfCfg;
    AlgLink_DeWarpConfigParams    deWarpSimcopCfg;

    UInt32  captureOutWidth;
    UInt32  captureOutHeight;
    UInt32  displayWidth;
    UInt32  displayHeight;

    Chains_Ctrl *chainsCfg;


    Bool bypassVtnf;
    Bool bypassLdc;

    AlgorithmLink_SrvCommonViewPointParams \
        algViewParams[SRV_UC_3D_MAX_NUM_VIEW_POINTS];
    System_VideoFrameCompositeBuffer \
        ldcMeshTableContainer[SRV_UC_3D_MAX_NUM_VIEW_POINTS];

    UInt32  freeToSwitch;
    UInt32  numViewPoints;

    BspOsal_SemHandle startSrvDemo;
    /**< Synchronization sempahore used to signal start computing LDC LUTs */
    BspOsal_TaskHandle mediaTaskHndl;
    /**< Media task handle */

    BspOsal_SemHandle dspCreate;
    /**< Synchronization sempahore used to starte DSP link creation */
    BspOsal_TaskHandle dspCreateTaskHndl;
    /**< DSP create task handle */

    AlgLink_DeWarpSaveFrame deWarpSaveFrame;

    UInt32 lutIdxFilePresent;
    /**< Flag to indicate if LUT Index file is present on MMCSD card */
    UInt32 v2WIdxFilePresent;
    /**< Flag to indicate if View to World Index file is present on
        MMCSD card */

} chains_issMultCaptIspDewarpSv_DisplayAppObj;

Void chains_issMultCaptIspDeWarp3dSv_MediaTask(UArg arg1, UArg arg2);
/**< Task to interface to media */
Int32 chains_issMultCaptIspDeWarp3dSv_FreeBufs(
                    chains_issMultCaptIspDewarpSv_DisplayAppObj *pChainsObj);

#endif /* CHAINS_ISSMULTICAPTISPSIMCOPSRV_H_ */
