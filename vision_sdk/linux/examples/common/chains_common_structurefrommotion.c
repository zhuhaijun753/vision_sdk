/*
 *******************************************************************************
 *
 * Copyright (C) 2015 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */
#include <linux/examples/common/chains_common.h>

/**
 *******************************************************************************
 *
 * \brief   Set SfM create parameters
 *
 *******************************************************************************
*/
Void ChainsCommon_StructureFromMotion_SetParams(
		SfmMain_CreateParams *mainprms,
		SfmLinearTwoView_CreateParams *ltvprms,
		SfmMap_CreateParams *mapprms,
		UInt32 numCams,
		UInt32 maxNumPoints)
{
    //Sfm Main
	mainprms->numCams = numCams ; //number of cameras in use
    mainprms->maxNumPoints = maxNumPoints;  //maximum number of points that will be processed

    mainprms->resetAfterNumIdleFrames = 600;

    mainprms->tuning_prms.y_roi[0] = 280;
    mainprms->tuning_prms.y_roi[1] = 235;
    mainprms->tuning_prms.y_roi[2] = 250;
    mainprms->tuning_prms.y_roi[3] = 210;
    mainprms->tuning_prms.y_roi[4] = 0; //NA
    mainprms->tuning_prms.y_roi[5] = 0; //NA

    mainprms->tuning_prms.em_fisheyeRadius 	= 500;
    mainprms->tuning_prms.em_thresh_init 		= 4;
    mainprms->tuning_prms.em_num_iter 			= 4;
    mainprms->tuning_prms.em_min_mag 			= 5e-1;
    mainprms->tuning_prms.em_min_num_inliers 	= 5;

    mainprms->tuning_prms.gpem_roi_xmin = 110;
    mainprms->tuning_prms.gpem_roi_ymin = 240;
    mainprms->tuning_prms.gpem_roi_width = 500;

    mainprms->tuning_prms.fc_max_radius 	= 500;
    mainprms->tuning_prms.fc_min_numTracks = 30;

    mainprms->tuning_prms.fc_max_y[0] = 1e30;
    mainprms->tuning_prms.fc_max_y[1] = 660;
    mainprms->tuning_prms.fc_max_y[2] = 1e30;
    mainprms->tuning_prms.fc_max_y[3] = 700;
    mainprms->tuning_prms.fc_max_y[4] = 0; //NA
    mainprms->tuning_prms.fc_max_y[5] = 0; //NA

    mainprms->tuning_prms.maxHeightGround = 10;
    mainprms->tuning_prms.movingAvgWindow  = 3;

    mainprms->motionDetect_prms.stillPointsRatioTresh = 0.1f;
    mainprms->motionDetect_prms.pixelDiffThresh = 256 / 128;
    mainprms->motionDetect_prms.numCamsInMotionThresh = 2;

    //Sfm Linear Two View
    ltvprms->numCams = numCams ; //number of cameras in use
    ltvprms->maxNumPoints = maxNumPoints;  //maximum number of points that will be processed

    ltvprms->tuning_prms.ransac_errorMeasure 	= 0;
    ltvprms->tuning_prms.ransac_iters 			= 200;
    ltvprms->tuning_prms.ransac_minNumInliers 	= 16;
    ltvprms->tuning_prms.ransac_points 			= 9;
    ltvprms->tuning_prms.ransac_threshold 		= 0.002;
    ltvprms->tuning_prms.ransac_maxCondNumber 	= 2000;
    ltvprms->maxNumInliers = 1024;

    //Sfm Mapping
	mapprms->numInPoints = numCams * maxNumPoints;
    mapprms->dx = 20;
	mapprms->dy = 20;
	mapprms->xmin = -900;
	mapprms->xmax =  900;
	mapprms->ymin = -1000; //was -500
	mapprms->ymax =  2000; //was -2500
	mapprms->minConfid = 0.85;
	mapprms->maxHeight = 500;
	mapprms->minHeight = 70;
	mapprms->minProbPerCell = 2;
	mapprms->dilateRange = 2;
	mapprms->erodeRange = 1;
	mapprms->maxNumBoxes = 12;
	mapprms->minSumProbs = 10;
	mapprms->minDistBoxTracking = 120;

}

