/*
 *******************************************************************************
 *
 * Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */

/*
 *******************************************************************************
 *
 * \file chains_common_surround_view.h
 *
 * \brief This file contains common utility functions used by SV use cases
 *
 *******************************************************************************
 */

#ifndef _CHAINS_COMMON_SRV_H_
#define _CHAINS_COMMON_SRV_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *  Includes
 *******************************************************************************
 */
#include <examples/tda2xx/include/chains.h>
#include <src/utils_common/include/utils_temperature.h>
#include "include/link_api/system_common.h"
#include <include/link_api/algorithmLink_srvCommon.h>

/*******************************************************************************
 *  Defines
 *******************************************************************************
 */

#define SRV_DIR_3X                       "/TDA3X"
#define SRV_DIR_2X                       "/TDA2X"
#define SRV_FILENAME_LUT                 "LUT.BIN"
#define SRV_FILENAME_LUTIDX              "LUT_IDX.BIN"
#define SRV_FILENAME_V2W                 "V2W.BIN"
#define SRV_FILENAME_V2WIDX              "V2W_IDX.BIN"
#define SRV_IMG_FILE_FRONT               "FRONT"
#define SRV_IMG_FILE_RIGHT               "RIGHT"
#define SRV_IMG_FILE_BACK                "BACK"
#define SRV_IMG_FILE_LEFT                "LEFT"
#define SRV_CALMAT_FILE                  "CALMAT.BIN"
#define SRV_PERSMAT_FILE                 "PERSMAT.BIN"
#define SRV_CHARTPOS_FILE                "CHARTPOS.BIN"
#define SRV_DISPLAYFRAME_FILE            "DIS_FR.YUV"
#define SRV_FILENAME_GPULUT              "GPULUT.BIN"

#define SRV_MEDIA_SIZE_HEADER            (1024U)
#define SRV_MEDIA_SIZE_MAXIDX            (10U * 1024U)
#define SRV_MEDIA_SIZE_METADATA          (128U)
#define SRV_LDC_VIEW2WORLDMESH_MAX_SIZE  (150U * 1024U)
#define SRV_LDC_CARIMAGE_MAX_SIZE        (150U * 1024U)
#define SRV_CALMAT_SIZE                  (12U * 4U)
#define SRV_PERSMAT_SIZE                 (9U * 4U)
#define SRV_FILEWRITE_CHUNKSIZE          (96U * 1024U)   /* Bytes */
#define SRV_FILEREAD_CHUNKSIZE           (100U * 1024U)  /* Bytes */
#define SRV_CHARTPOS_SIZE                (256U)
#define SRV_CORNERPOINT_SIZE             (1024U)
#define SRV_PERSISTENT_BUF_SIZE          (8U * MB)
#define SRV_SRCATCH_BUF_SIZE             (1U * MB)
#define SRV_3D_LDC_SUB_SAMPLE_RATIO      (8U)
#define SRV_3D_GPU_SUB_SAMPLE_RATIO      (2U)

/*******************************************************************************
 *  Structures
 *******************************************************************************
 */
/**
 *******************************************************************************
 *
 *  \brief  ChainsCommon_SurroundView_mediaObj
 *
 *        This structure contains all the LinksId's and create Params.
 *        The same is passed to all create, start, stop functions.
 *
 *******************************************************************************
*/
typedef struct
{
    Int32 fp;
    Int32 fpIdx;
    Int32 fpv2w;
    Int32 fpv2wIdx;
    UInt32 filePtr;
    UInt32 *idxBuf;
    UInt32 idxBufPtr;
    UInt32 srvBufSize[100U];

} ChainsCommon_SurroundView_mediaObj;

/**
 *******************************************************************************
 *
 *  \brief  SV_MEDIA_FILE
 *
 *        This structure contains all the LinksId's and create Params.
 *        The same is passed to all create, start, stop functions.
 *
 *******************************************************************************
*/
typedef enum
{
    SRV_MEDIA_FILE_LUT,
    SRV_MEDIA_FILE_V2W

}SRV_MEDIA_FILE;

/*******************************************************************************
 *  Functions
 *******************************************************************************
 */

Int32 ChainsCommon_SurroundView_mediaInit(
            ChainsCommon_SurroundView_mediaObj *pMediaObj,
            UInt32 *pLutIdxFilePresent,
            UInt32 *pV2MIdxFilePresent,
            UInt32 *pNumViewPoints,
            SRV_MEDIA_FILE fileType);
Int32 ChainsCommon_SurroundView_mediaDeinit(
            ChainsCommon_SurroundView_mediaObj *pMediaObj);
Int32 ChainsCommon_SurroundView_fileRead(
            Int32 fileHndl,
            UInt8 *addr,
            UInt32 readSize,
            UInt32 *actualReadSize);
Int32 ChainsCommon_SurroundView_fileWrite(
            Int32 fileHndl,
            UInt8 *addr,
            UInt32 size);
Int32 ChainsCommon_SurroundView_fileWriteNumViewPoints(
            ChainsCommon_SurroundView_mediaObj *pMediaObj,
            UInt32 numViewPoints,
            SRV_MEDIA_FILE fileType);
Int32 ChainsCommon_SurroundView_mediaReadBlock(
            ChainsCommon_SurroundView_mediaObj *pMediaObj,
            AlgorithmLink_SrvCommonViewPointParams *pViewPointParams,
            UInt32 viewPointIdx,
            SRV_MEDIA_FILE fileType);
Int32 ChainsCommon_SurroundView_mediaWriteBlock(
            ChainsCommon_SurroundView_mediaObj *pMediaObj,
            AlgorithmLink_SrvCommonViewPointParams *pViewPointParams,
            UInt32 chunkSize,
            UInt32 offsetChunk,
            SRV_MEDIA_FILE fileType);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

/* @} */

/**
 *******************************************************************************
 *
 *   \defgroup EXAMPLES_API Example code implementation
 *
 *******************************************************************************
 */
