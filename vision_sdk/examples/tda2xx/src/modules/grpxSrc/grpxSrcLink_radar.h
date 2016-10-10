/*
 *******************************************************************************
 *
 * Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \ingroup GRPX_RADAR_LINK_API
 * \defgroup GRPX_RADAR_LINK_IMPL Grpx Src Link Implementation
 *
 * @{
 *
 *******************************************************************************
 */

 /**
 *******************************************************************************
 *
 * \file GrpxRadarLink_priv.h Grpx Src Link private API/Data structures
 *
 * \brief  This file is a private header file for grpx src link implementation
 *         This file lists the data structures, function prototypes which are
 *         implemented and used as a part of grpx src link.
 *
 * \version 0.0 (Oct 2013) : [NN] First version
 *
 *******************************************************************************
 */

#ifndef _GRPX_RADAR_LINK_PRIV_H_
#define _GRPX_RADAR_LINK_PRIV_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *  Include files
 *******************************************************************************
 */
#include <src/links_ipu/system/system_priv_ipu1_0.h>
#ifdef RADAR_INCLUDE
#include <include/link_api/algorithmLink_radarProcess.h>
#include <include/link_api/algorithmLink_CRFAlignment.h>
#endif
#include "grpxSrcLink_priv.h"
#include "examples/tda2xx/include/draw2d.h"
#include <src/utils_common/include/utils_prf.h>


/*******************************************************************************
 *  Defines
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \brief Maximum number of grpx src link objects
 *
 * SUPPORTED in ALL platforms
 *
 *******************************************************************************
 */
#define GRPX_RADAR_LINK_OBJ_MAX    (2)

/**
 *******************************************************************************
 *
 * \brief Max Number of buffers this link uses
 *
 * SUPPORTED in ALL platforms
 *
 *******************************************************************************
 */
#define GRPX_RADAR_LINK_MAX_OUT_FRAMES   (SYSTEM_LINK_FRAMES_PER_CH)

/**
 *******************************************************************************
 *
 * \brief Number of buffers this link uses
 *
 * SUPPORTED in ALL platforms
 *
 *******************************************************************************
 */
#define GRPX_RADAR_LINK_OUT_FRAMES       (1)

/**
 *******************************************************************************
 *
 * \brief Interval at which stats have to be calculated in Msec
 *
 * SUPPORTED in ALL platforms
 *
 *******************************************************************************
 */
#define GRPX_RADAR_LINK_LOAD_REFRESH_INTERVAL_MSEC (5000)

/**
 *******************************************************************************
 *
 * \brief Interval at which GrpxRadarLink processing happens
 *
 * SUPPORTED in ALL platforms
 *
 *******************************************************************************
 */
#define GRPX_RADAR_LINK_PROCESS_INTERVAL_MSEC (10)

/* String size for prints */
#define GRPX_RADAR_LINK_STR_SZ (128)

/**
*******************************************************************************
 * \brief Grpx src String properties
 *
 * SUPPORTED in ALL platforms
 *
*******************************************************************************
*/
typedef struct
{
    UInt32 startX;
    /**< X position where the stats print starts */

    UInt32 startY;
    /**< Y position where the stats print starts */

    UInt32 fontType;
    /**< font type */

    char string[128];
    /**< String to be displayed */

} GrpxRadarLink_StringInfo;

/*******************************************************************************
 *  Data structure's
 *******************************************************************************
 */
typedef struct {
    Utils_BufHndl bufOutQue;
    /**< Output buffer queue */

    System_Buffer buffers[GRPX_RADAR_LINK_MAX_OUT_FRAMES];
    /**< System buffer data structure to exchange buffers between links */

    System_VideoFrameBuffer videoFrames[GRPX_RADAR_LINK_MAX_OUT_FRAMES];
    /**< Payload for System buffers */

    Fvid2_Frame frames[GRPX_RADAR_LINK_MAX_OUT_FRAMES];
    /**< Fvid2_frames to hold buffers */

    UInt32 numFrames;
    /**< Number of output buffers, must be <= GRPX_RADAR_LINK_MAX_OUT_FRAMES */

    Fvid2_Format format;
    /**< Format object used for buffer creation */

}GrpxRadarLink_OutObj;

typedef struct {

    Utils_SystemLoadStats systemLoadStats[SYSTEM_PROC_MAX];
    /**< Load object for each processor in the system */

    UInt32                profilerState;
    /**< Variable to keep track of profiler state */

    UInt32                refreshInterval;
    /**< Variable to control load calculation period in Msec */

    UInt32                startTime;
    /**< Varaile to hold stats start time */


}GrpxRadar_StatsDisplayObj;

typedef struct
{
    GraphicsLink_StringPrintState    stringPrintState;
    /**< State of stirng print  */
    Int32  remainingDuration;
    /**< Time in ms that specified string needs to be dispalyed */
    GrpxRadarLink_StringInfo stringInfo;
    /**< String to be displayed */
} GrpxRadarLink_StringRunTimePrintObj;

typedef struct
{
    UInt32    distance;
    /**< Distance to radar sensor , in cm */
	Int32    angle;
    /**< Azimuth angle to the radar sensor  */
	Int32    relSpeed;
    /**< Relative speed of the target  */

	//zz (20150629)
	Int32   vel_threshold;

} GrpxRadarLink_targetInformation;

/*******************************************************************************
 *  Function's
 *******************************************************************************
 */

Void GrpxRadarLink_setObj(GrpxSrcLink_Obj *pObj);

Void GrpxRadarLink_setBRadarScopePrinted(Bool value);

Void GrpxRadarLink_setBRadarTargetUpdated(Bool value);

Void GrpxRadarLink_setInitTargetValues();

Void GrpxRadarLink_clearSingleTarget(GrpxSrcLink_Obj *pObj,
                    UInt32 centerX,
                    UInt32 centerY,
                    GrpxRadarLink_targetInformation target);

Void GrpxRadarLink_clearTargets(
					GrpxSrcLink_Obj *pObj,
					UInt32 centerX,
					UInt32 centerY);

Void GrpxRadarLink_targetUpdateValue(
					UInt32 targetNo,
					UInt32 value);

Int32 GrpxRadarLink_drawMonitoringGrid(GrpxSrcLink_Obj *pObj,
					UInt32 startX,
					UInt32 startY,
					UInt32 stopX,
					UInt32 stopY,
					UInt32 gridWidth
					);

Int32 GrpxRadarLink_drawSectorLines(GrpxSrcLink_Obj *pObj,
					UInt32 startX,
					UInt32 startY,
					UInt32 length,
					UInt32 degree
					);


Void GrpxRadarLink_drawPartCircle(GrpxSrcLink_Obj *pObj,
					UInt32 x,
					UInt32 y,
					UInt32 radius,
					UInt32 degrees
					);

Void GrpxRadarLink_drawLabels(GrpxSrcLink_Obj *pObj,
					UInt32 x,
					UInt32 y,
					UInt32 radius,
					UInt32 degrees);

Int32 GrpxRadarLink_drawCpuLoad(GrpxSrcLink_Obj *pObj,
                    UInt32 x, UInt32 y,
                    UInt32 barWidth,
                    UInt32 barHeight,
                    UInt32 padX,
                    UInt32 padY,
                    UInt32 fontIdx
                    );

Void GrpxRadarLink_plotTarget(GrpxSrcLink_Obj *pObj,
        Int32 x,
        Int32 y,
        Int32 radius,
        GrpxRadarLink_targetInformation *pTarget );

Void GrpxRadarLink_displayTargets(GrpxSrcLink_Obj *pObj,
					UInt32 x,
					UInt32 y,
					UInt32 radius);

Void GrpxRadarLink_drawLabel(
        GrpxSrcLink_Obj *pObj,
        UInt32 x,
        UInt32 y,
        char *text);


void sin_table_fill(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

