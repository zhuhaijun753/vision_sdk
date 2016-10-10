/*
 *******************************************************************************
 *
 * Copyright (C) 2015 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 *
 *
 *  File: sgxRender3Dutils.c
 *
 *  Custom utilities for sgxRender3D links
 *  
 *******************************************************************************/

#include "sgxRender3Dutils.h"
#include <examples/tda2xx/src/alg_plugins/commonutils/ldc_lut_tda2x.c>

/*=======================================================================
*
* Name: UInt32 project3DPointToImage(Point3D_f *xyzc, Point3D_f *xyd)
*
* Description: given point in camera coordinates, find pixel location
* 			    in image
*
=======================================================================*/
UInt32 project3DPointToImage(Point3D_f *xyzc, Point3D_f *xyd)
{
	UInt32 status = 0;

	/*check if point is behind camera*/
	if (xyzc->z <= 0)
		return 1;

	Flouble rc = SQRT(xyzc->x*xyzc->x + xyzc->y*xyzc->y);
	Flouble rcInv = 1/(rc+FLOUBLE_EPSILON);
	Flouble theta = ATAN(rc/xyzc->z);

	/*Look-up table */
	Flouble ind = theta/ldcLUT_U2D_step;
	if (ind >= ldcLUT_U2D_length-1)
		return 1;
	Int32 N = (Int32)ind;
	Flouble indMinN = ind - (Flouble)N;
	Flouble rd = (1.0f - indMinN)*ldcLUT_U2D_table[N] + indMinN * ldcLUT_U2D_table[N + 1];

	xyd->x = (Flouble)ldcLUT_distortionCenters[0] + rd*xyzc->x*rcInv;
	xyd->y = (Flouble)ldcLUT_distortionCenters[1] + rd*xyzc->y*rcInv;
	xyd->z = 0;

	return status;

}



