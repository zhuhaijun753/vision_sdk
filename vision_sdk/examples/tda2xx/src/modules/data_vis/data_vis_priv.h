/*
 *******************************************************************************
 *
 * Copyright (C) 2015 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */



#ifndef DATA_VIS_PRIV_H_
#define DATA_VIS_PRIV_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *  Include files
 *******************************************************************************
 */

#include "data_vis.h"
#include <osal/bsp_osal.h>
#include <src/utils_common/include/utils_mem.h>

#define DATA_VIS_QUEUE_SIZE     (64*1000u)

typedef struct {

    UInt32 readIdx;
    UInt32 writeIdx;
    UInt32 count;
    DataVis_DataBwElement *data;

} DataVis_Queue;

typedef struct {

    UInt32 isSetPrmDone;

    DataVis_CreatePrm createPrm;

    DataVis_DataBwPrm userSetPrm;
    Bool resetDataCollector;
    Bool resetDraw;

    BspOsal_SemHandle lock;

    /* only used by drawing function's
     * Should not be used by data collection
     * and data logging function's
     */
    DataVis_DataBwPrm drawPrm;
    DataVis_DataBwElement *pDrawDataBwElements;
    UInt32 curDrawWrIdx;
    UInt32 curDrawNumElements;
    UInt64 startTimeInMsec;
    UInt16 x_prev[DATA_VIS_DATA_BW_MOD_MAX];
    UInt16 y_prev[DATA_VIS_DATA_BW_MOD_MAX];
    UInt16 x_cur[DATA_VIS_DATA_BW_MOD_MAX];
    UInt16 y_cur[DATA_VIS_DATA_BW_MOD_MAX];
    /* --- */

    DataVis_Queue queue;

} DataVis_Obj;


extern DataVis_Obj gDataVis_Obj;



Void DataVis_unlock();
Void DataVis_lock();

Void DataVis_queueDataBwCreate();
Void DataVis_queueDataBwDelete();

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/* @} */


