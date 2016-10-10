/*
 *******************************************************************************
 *
 * Copyright (C) 2015 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */

#ifndef __BOX_H
#define __BOX_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sgx3DsfmDefs.h"

/*init functions*/
int boxes_init(void);
/*draw functions*/
void boxes_draw(float boxes[][24], UInt32 numBoxes, Pose3D_f *pose, int viewID);
void boxes_inputImage_draw(float boxes[][24], UInt32 numBoxes, Pose3D_f *poses, UInt32 camNo);
void boxes_legend_draw();
/*supporting functions*/
void boxes_projectToInputImage(float boxes[][24], UInt32 numBoxes, Pose3D_f *poses, UInt32 camNo, float boxes_projected[][24], UInt32 *numBoxes_projected);
UInt32 project3DPointToImage(Point3D_f *xyzc, Point3D_f *xyd);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __BOX_H    */
