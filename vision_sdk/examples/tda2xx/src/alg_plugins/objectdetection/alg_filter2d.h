/*
 *******************************************************************************
 *
 * Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */

#ifndef _FILTER2D_H_
#define _FILTER2D_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *  Include files
 *******************************************************************************
 */
#include <include/link_api/system.h>
#include <include/link_api/algorithmLink.h>
#include <examples/tda2xx/src/alg_plugins/common/include/alg_ivision.h>
#include <src/utils_common/include/utils_mem.h>
#include <ifilter_2d_ti.h>
#include <iclahe_ti.h>


/*******************************************************************************
 *  Enums
 *******************************************************************************
 */

/*******************************************************************************
 *  Defines
 *******************************************************************************
 */

/*******************************************************************************
 *  Data structures
 *******************************************************************************
 */
typedef struct {
    Void * handle;

    CLAHE_TI_CreateParams prms;
    CLAHE_TI_InArgs inArgs;
    CLAHE_TI_outArgs outArgs;

    IVISION_InBufs    inBufs;
    IVISION_OutBufs   outBufs;
    IVISION_BufDesc   inBufDesc[CLAHE_TI_BUFDESC_IN_TOTAL];
    IVISION_BufDesc   outBufDesc[CLAHE_TI_BUFDESC_OUT_TOTAL];
    IVISION_BufDesc   *inBufDescList[CLAHE_TI_BUFDESC_IN_TOTAL];
    IVISION_BufDesc   *outBufDescList[CLAHE_TI_BUFDESC_OUT_TOTAL];

    Void  *lutBufAddr[2];
    UInt32 lutBufSize;

    UInt32 lutBufId;

} Alg_ClaheObj;

typedef struct
{
    Void * handle;

    FILTER_2D_CreateParams prms;
    FILTER_2D_InArgs inArgs;
    FILTER_2D_OutArgs outArgs;

    IVISION_InBufs    inBufs;
    IVISION_OutBufs   outBufs;
    IVISION_BufDesc   inBufDesc;
    IVISION_BufDesc   outBufDesc;
    IVISION_BufDesc   *inBufDescList[FILTER_2D_TI_BUFDESC_IN_TOTAL];
    IVISION_BufDesc   *outBufDescList[FILTER_2D_TI_BUFDESC_OUT_TOTAL];

    Void  *tmpBufAddr;

    Void  *outBufAddr[2];
    UInt32 outBufSize[2];
    UInt32 outPitch[2];
    UInt32 outWidth;
    UInt32 outHeight;

    Alg_ClaheObj claheObj;

    Bool enableClahe;
    Bool enableFilter2d;

} Alg_Filter2dObj;

/*******************************************************************************
 *  Functions
 *******************************************************************************
 */
Int32 Alg_filter2dCreate(
            Alg_Filter2dObj *pObj,
            UInt32 inWidth,
            UInt32 inHeight,
            UInt32 inPitch[],
            Bool allocTmpBuf,
            Bool enableClahe,
            Bool enableFilter2d
);

Int32 Alg_filter2dProcess(
            Alg_Filter2dObj *pObj,
            Void *inBufAddr[],
            Void *outBufAddr[]
        );

Int32 Alg_filter2dDelete(Alg_Filter2dObj *pObj);

Int32 Alg_claheCreate(
            Alg_ClaheObj *pObj,
            UInt32 inWidth,
            UInt32 inHeight,
            UInt32 inPitch,
            UInt32 outPitch
            );

Int32 Alg_claheProcess(
            Alg_ClaheObj *pObj,
            Void *inBufAddr,
            Void *outBufAddr
        );

Int32 Alg_claheDelete(Alg_ClaheObj *pObj);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/*@}*/

/* Nothing beyond this point */
