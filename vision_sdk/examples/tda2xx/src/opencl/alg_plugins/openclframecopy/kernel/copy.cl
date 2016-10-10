/*
 *******************************************************************************
 *
 * Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */
/*__attribute__((reqd_work_group_size(1,1,1))) */
kernel
void Copy(
           global int4* inPtr,
           global int4* outPtr,
           int rows,
           int cols,
           int inPitch,
           int outPitch
)
{

	int gidx = get_global_id(0);
	int gidy = get_global_id(1);

   // if(gidx < cols && gidy < rows)
    {

    outPtr[gidx+gidy*outPitch] = inPtr[gidx+gidy*inPitch];

    }

}



