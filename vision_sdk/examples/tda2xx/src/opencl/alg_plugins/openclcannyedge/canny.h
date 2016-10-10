/*
 *******************************************************************************
 *
 * Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */
#ifndef _OPENCL_CANNY_H_
#define _OPENCL_CANNY_H_

typedef struct
{
	char *data;
	cl_mem buf;
	unsigned int size;
	unsigned int rows;
	unsigned int cols;
	unsigned int steps;
	unsigned int offset;
}opencl_buf;
inline unsigned int divUp(unsigned int total, unsigned int grain);


void calcSobelRowPass_opencl(cl_context, cl_command_queue, cl_kernel, opencl_buf bufsource, opencl_buf bufDxbuf, opencl_buf bufDybuf,
                        UInt32 *inPtr[], UInt32 *outPtr[], UInt32 inPitch[],UInt32 outPitch[]);
void calcMagnitude_opencl(cl_context context, cl_command_queue queue, cl_kernel kernel, opencl_buf dxbuf, opencl_buf dybuf, opencl_buf dx, opencl_buf dy, opencl_buf mag,int rwos, int cols, char L2gradient);
void calcMap_opencl(cl_context context, cl_command_queue queue, cl_kernel kernel, opencl_buf dx, opencl_buf dy, opencl_buf mag,
                opencl_buf map, int rows, int cols, short low_thresh, short high_thresh,
                UInt32 *inPtr[],UInt32 *outPtr[],UInt32 inPitch[],UInt32 outPitch[],UInt32 numPlanes,UInt32 height,
                UInt32 wordWidth,opencl_buf trackBuf1, opencl_buf counter);

void edgesBlob_opencl(cl_context context, cl_command_queue queue, cl_kernel kernel, opencl_buf map,
                 opencl_buf trackBuf1, opencl_buf counter, int rows, int cols);

void getEdges_opencl(cl_context context, cl_command_queue queue, cl_kernel kernel, opencl_buf map, opencl_buf dst, int rows, int cols);
void add_ocl_logo(
             UInt32 *inPtr[],
             UInt32 *outPtr[],
             UInt32 inPitch[],
             UInt32 outPitch[]
             );
void set_chroma(
             UInt32 *inPtr[],
             UInt32 *outPtr[],
             UInt32 inPitch[],
             UInt32 outPitch[],
             UInt32 numPlanes,
             UInt32 height,
             UInt32 wordWidth
             );
#endif
