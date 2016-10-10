/*
 *******************************************************************************
 *
 * Copyright (C) 2016 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */

#include "chains_issMultCaptIspDeWarp3dSv_priv.h"
#include "chains_issMultCaptIspDeWarp3dSv.h"
#include <examples/tda2xx/src/alg_plugins/surroundview/include/svCommonDefs.h>
#include <examples/tda2xx/include/chains_common.h>
#include <examples/tda2xx/include/chains_common_surround_view.h>

/*******************************************************************************
 *  Defines
 *******************************************************************************
 */
/**
 *******************************************************************************
 * \brief Enable the LDC slice parameters locally
 *
 *******************************************************************************
*/
#define LOCAL_LDC_SLICE_PRM

/*******************************************************************************
 *  Global Variables
 *******************************************************************************
 */
static ChainsCommon_SurroundView_mediaObj gSrvMediaObj;

#ifdef LOCAL_LDC_SLICE_PRM
    extern SRV_UC_3D_LDC_SLICE_PRM gLdcSlicePrmNo;

    AlgorithmLink_ldcSlicePrm \
    gLdcSlicePrm[SRV_UC_3D_LDC_SLICE_PRM_MAX][ALGLINK_SRV_COMMON_MAX_CAMERAS] =
    {
        /* 0:1 slice with 16x16 block size */
        {
            {   1,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,100,100,16,16,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,0,0,0,0,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,0,0,0,0,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,0,0,0,0,
            },
            {   1,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,100,100,16,16,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,0,0,0,0,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,0,0,0,0,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,0,0,0,0,
            },
            {   1,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,100,100,16,16,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,0,0,0,0,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,0,0,0,0,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,0,0,0,0,
            },
            {   1,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,100,100,16,16,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,0,0,0,0,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,0,0,0,0,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,0,0,0,0,
            },
        },

        /* 1:1 slice with 16x8 block size */
        {
            {   1,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,100,100,16,8,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,0,0,0,0,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,0,0,0,0,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,0,0,0,0,
            },
            {   1,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,100,100,16,8,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,0,0,0,0,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,0,0,0,0,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,0,0,0,0,
            },
            {   1,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,100,100,16,8,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,0,0,0,0,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,0,0,0,0,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,0,0,0,0,
            },
            {   1,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,100,100,16,8,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,0,0,0,0,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,0,0,0,0,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,0,0,0,0,
            },
        },

        /* 2:2 slices with 16x16 and 16x8 block sizes */
        {
            {   2,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,100,50,16,16,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,50,100,50,16,8,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,0,0,0,0,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,0,0,0,0,
            },
            {   2,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,50,100,16,8,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                50,0,50,100,16,16,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,0,0,0,0,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,0,0,0,0,
            },
            {   2,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,100,50,16,8,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,50,100,50,16,16,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,0,0,0,0,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,0,0,0,0,
            },
            {   2,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,50,100,16,16,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                50,0,50,100,16,8,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,0,0,0,0,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,0,0,0,0,
            },
        },

        /* 3:2 slices with 16x16 and 16x4 block sizes */
        {
            {   2,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,100,50,16,16,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,50,100,50,16,8,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,0,0,0,0,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,0,0,0,0,
            },
            {   2,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,50,100,16,8,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                50,0,50,100,16,16,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,0,0,0,0,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,0,0,0,0,
            },
            {   2,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,100,50,16,8,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,50,100,50,16,16,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,0,0,0,0,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,0,0,0,0,
            },
            {   2,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,50,100,16,16,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                50,0,50,100,16,8,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,0,0,0,0,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,0,0,0,0,
            },
        },

        /* 4:1 slice with 16x4 block size */
        {
            {   1,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,100,100,16,4,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,0,0,0,0,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,0,0,0,0,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,0,0,0,0,
            },
            {   1,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,100,100,16,4,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,0,0,0,0,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,0,0,0,0,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,0,0,0,0,
            },
            {   1,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,100,100,16,4,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,0,0,0,0,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,0,0,0,0,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,0,0,0,0,
            },
            {   1,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,100,100,16,4,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,0,0,0,0,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,0,0,0,0,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,0,0,0,0,
            },
        },

        /* 5:2 slices with 16x8 and 16x4 block sizes */
        {
            {   2,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,100,100,16,8,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_ABS,
                248,400,224,32,16,4,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,0,0,0,0,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,0,0,0,0,
            },
            {   2,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,100,100,16,8,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_ABS,
                0,392,32,224,16,4,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,0,0,0,0,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,0,0,0,0,
            },
            {   2,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,100,100,16,8,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_ABS,
                248,0,224,32,16,4,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,0,0,0,0,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,0,0,0,0,
            },
            {   2,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,100,100,16,8,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_ABS,
                288,392,32,224,16,4,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,0,0,0,0,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,0,0,0,0,
            },
        },

        /* 6:3 slices with 16x16, 16x8 and 16x4 block sizes */
        {
            {   3,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,100,100,16,16,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_ABS,
                216,320,320,112,16,8,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_ABS,
                248,400,224,32,16,4,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,0,0,0,0,
            },
            {   3,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,100,100,16,16,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_ABS,
                0,216,112,576,16,8,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_ABS,
                0,392,32,224,16,4,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,0,0,0,0,
            },
            {   3,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,100,100,16,16,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_ABS,
                216,0,320,112,16,8,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_ABS,
                248,0,224,32,16,4,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,0,0,0,0,
            },
            {   3,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,100,100,16,16,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_ABS,
                208,216,112,576,16,8,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_ABS,
                288,392,32,224,16,4,
                ALGLINK_SRV_LDC_SLICE_PRMTYPE_PERCENT,
                0,0,0,0,0,0,
            },
        },
    };
#endif

/*******************************************************************************
 *  Local Functions
 *******************************************************************************
 */
Int32 mediaReadLdcViewPointParams(
                chains_issMultCaptIspDewarpSv_DisplayAppObj *pChainsObj,
                ChainsCommon_SurroundView_mediaObj *pMediaObj,
                AlgorithmLink_SrvCommonViewPointParams *pSrvViewPointsParam,
                System_VideoFrameCompositeBuffer *pVidCompBuf,
                UInt32 numViewPoints,
                UInt32 numCameras,
                UInt32 ldcLutSize,
                UInt32 carImgSize)
{
    Int32 retVal;
    UInt32 cnt, idx;
    UInt32 rawMemSize, alignedLdcLutSize, alignedCarImgSize, alignedHeaderSize;
    AlgorithmLink_SrvCommonViewPointParams *pLocalSrvParams;
    System_VideoFrameCompositeBuffer *pLocalVidCompBuf;
    UInt32 start,end;

    start = Utils_getCurTimeInMsec();

    for (cnt = 0; cnt < numViewPoints; cnt ++)
    {
        if (1U == cnt)
        {
            /*
             *  Start the SRV use case when the first view point parameters are
             *  available
             */
            BspOsal_semPost(pChainsObj->startSrvDemo);

            /* Wait for the complete chain creation */
            BspOsal_semWait(pChainsObj->startSrvDemo, BSP_OSAL_WAIT_FOREVER);
        }

        if (0U == (cnt % 5U))
        {
            Vps_printf(" SRV_MEDIA: Reading LDC LUT for view point %d ...\n",cnt);
        }

        pLocalSrvParams = pSrvViewPointsParam + cnt;
        pLocalVidCompBuf = pVidCompBuf + cnt;

        pLocalSrvParams->pCompBuf = pLocalVidCompBuf;

        /* DSP will use this CAR Image, hence aliging on 128 boundary */

        alignedLdcLutSize = SystemUtils_align(ldcLutSize, 128U);
        alignedCarImgSize = SystemUtils_align(carImgSize, 128U);
        alignedHeaderSize = SystemUtils_align(SRV_MEDIA_SIZE_HEADER, 128U);

        /* Max size */
        rawMemSize = alignedHeaderSize + (alignedLdcLutSize * numCameras) +
                            alignedCarImgSize;

        pLocalSrvParams->baseBufAddr =
                        ChainsCommon_SurroundView_memAlloc( rawMemSize, 128U);
        pMediaObj->srvBufSize[cnt] = rawMemSize;

        retVal = ChainsCommon_SurroundView_mediaReadBlock(pMediaObj,
                                                            pLocalSrvParams,
                                                            cnt,
                                                            SRV_MEDIA_FILE_LUT);
        if (SYSTEM_LINK_STATUS_SOK == retVal)
        {
            pLocalSrvParams->carImgPtr = (UInt32 *)
                    ((UInt32)pLocalSrvParams->baseBufAddr + alignedHeaderSize);

            for (idx = 0; idx < numCameras; idx++)
            {
                pLocalVidCompBuf->metaBufAddr[idx] = (Void*)
                    ((UInt32) pLocalSrvParams->baseBufAddr + alignedHeaderSize
                        + alignedCarImgSize + (alignedLdcLutSize * idx));
            }

            pLocalVidCompBuf->numFrames = numCameras;
        }

        UTILS_assert(SYSTEM_LINK_STATUS_SOK == retVal);

#ifdef LOCAL_LDC_SLICE_PRM
        memcpy(
            &pLocalSrvParams->slicePrm,
            &gLdcSlicePrm[gLdcSlicePrmNo],
            sizeof(AlgorithmLink_ldcSlicePrm) * ALGLINK_SRV_COMMON_MAX_CAMERAS);
#endif
    }

    end = Utils_getCurTimeInMsec();

    Vps_printf(" SRV_MEDIA: Time taken to read %d view point data = %d msec\n",
               numViewPoints, (end - start));

    return retVal;
}

/*******************************************************************************
 *  Functions
 *******************************************************************************
 */

Void chains_issMultCaptIspDeWarp3dSv_MediaTask(UArg arg1, UArg arg2)
{
    UInt32 lineOffset, ldcLutSize;
    Int32 status = SYSTEM_LINK_STATUS_SOK;
    chains_issMultCaptIspDewarpSv_DisplayAppObj *pChainsObj;

    UTILS_COMPILETIME_ASSERT (SRV_MEDIA_SIZE_HEADER >= \
                                sizeof(AlgorithmLink_SrvCommonViewPointParams));

    pChainsObj = (chains_issMultCaptIspDewarpSv_DisplayAppObj *)arg1;
    UTILS_assert(NULL != pChainsObj);

    lineOffset = (SRV_UC_3D_LDC_OUTPUT_WIDTH_MAX /
                    (1U << VPS_ISS_LDC_LUT_DOWN_SCALE_FACTOR_8)) + 1U;
    lineOffset += 15;
    lineOffset &= ~0xF;
    lineOffset *= 4;

    ldcLutSize = lineOffset * ((SRV_UC_3D_LDC_OUTPUT_HEIGHT_MAX /
                                (1U << VPS_ISS_LDC_LUT_DOWN_SCALE_FACTOR_8))
                                + 1U);

    status = ChainsCommon_SurroundView_mediaInit(&gSrvMediaObj,
                                                &pChainsObj->lutIdxFilePresent,
                                                &pChainsObj->v2WIdxFilePresent,
                                                &pChainsObj->numViewPoints,
                                                SRV_MEDIA_FILE_LUT);
    UTILS_assert(SYSTEM_LINK_STATUS_SOK == status);
    UTILS_assert((TRUE == pChainsObj->lutIdxFilePresent) ||
                 (TRUE == pChainsObj->v2WIdxFilePresent));

    while (SYSTEM_LINK_STATUS_SOK == status)
    {
        if(pChainsObj->lutIdxFilePresent == TRUE)
        {
            /*
             *  The LDC LUT are available in the file therefore allocate the
             *  buffers and read the LUTs.
             */
            status = mediaReadLdcViewPointParams(
                                        pChainsObj,
                                        &gSrvMediaObj,
                                        &pChainsObj->algViewParams[0U],
                                        &pChainsObj->ldcMeshTableContainer[0U],
                                        pChainsObj->numViewPoints,
                                        SRV_UC_3D_NUM_CAMERAS,
                                        ldcLutSize,
                                        SRV_LDC_CARIMAGE_MAX_SIZE);
            UTILS_assert(SYSTEM_LINK_STATUS_SOK == status);
        }

        if(pChainsObj->lutIdxFilePresent == TRUE)
        {
            pChainsObj->freeToSwitch = TRUE;
        }

        break;
    }

    ChainsCommon_SurroundView_mediaDeinit(&gSrvMediaObj);
}

Int32 chains_issMultCaptIspDeWarp3dSv_FreeBufs(
                chains_issMultCaptIspDewarpSv_DisplayAppObj *pChainsObj)
{
    UInt32 cnt;
    ChainsCommon_SurroundView_mediaObj *pMediaObj = &gSrvMediaObj;

    for(cnt = 0;cnt < pChainsObj->numViewPoints;cnt ++)
    {
        if(pChainsObj->algViewParams[cnt].baseBufAddr != NULL)
        {
            ChainsCommon_SurroundView_memFree(
                                    pChainsObj->algViewParams[cnt].baseBufAddr,
                                    pMediaObj->srvBufSize[cnt]);
        }
    }

    return SYSTEM_LINK_STATUS_SOK;
}
