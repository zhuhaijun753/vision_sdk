/*
 *******************************************************************************
 *
 * Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */


/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */
#include <linux/src/osa/include/osa_mem.h>
#include <stdio.h>
#include <stdlib.h>
#include "system_pvrscope_if.h"


#ifdef BUILD_INFOADAS
#define PVRSCOPE_FIFO_DIR "/opt/infoadas"
#else
#define PVRSCOPE_FIFO_DIR "/opt/vision_sdk"
#endif

/**
 *******************************************************************************
 *
 * \brief To support SGX profiling. Create the Linux stack FIFO to read
 *        the GPU load dumped by pvr scope utility
 *
 *******************************************************************************
 */
void system_create_pvrscope_profile_data(System_PvrScope_Obj *pvrscope_obj)
{

   pvrscope_obj->fd = 0;
   pvrscope_obj->gpufifo = (char *) PVRSCOPE_FIFO_DIR "/gpufifo";
   mkfifo(pvrscope_obj->gpufifo, 0666);
   Vps_printf(" SYSTEM_PVRSCOPE: FIFO %s opened successfully \n",
                pvrscope_obj->gpufifo);

   pvrscope_obj->fd = open(pvrscope_obj->gpufifo, O_RDONLY|O_NONBLOCK);
   if (pvrscope_obj->fd==-1) 
   {
       Vps_printf (" SYSTEM_PVRSCOPE: FIFO open failed \n");
       exit(-1);
   }
}

/**
 *******************************************************************************
 *
 * \brief To support SGX profiling. Close the Linux stack FIFO
 *
 *******************************************************************************
 */
void system_delete_pvrscope_profile_data(System_PvrScope_Obj *pvrscope_obj)
{
   close(pvrscope_obj->fd);
}

/**
 *******************************************************************************
 *
 * \brief To support SGX profiling. Get the SGX load from the Linux stack FIFO,
 *        in to which the the GPU load is dumped by pvr scope utility
 *
 *******************************************************************************
 */
void system_get_pvrscope_profile_data(System_PvrScope_Obj *pvrscope_obj)
{
   char buf[MAX_BUF];
   int bytes; 

   bytes = read(pvrscope_obj->fd, buf, MAX_BUF);
   if (bytes>0)
   {
       char str[100];
       sscanf(buf, "%s %f %f", str, &pvrscope_obj->fragShLoad, &pvrscope_obj->vertShLoad);
       if(strcmp(str, "PVR:") == 0) 
       {
           pvrscope_obj->TotalShLoad =
               (pvrscope_obj->fragShLoad + pvrscope_obj->vertShLoad)/2;
           pvrscope_obj->loadStats.totalLoadParams.integerValue =
               (uint32_t) pvrscope_obj->TotalShLoad;
           pvrscope_obj->loadStats.totalLoadParams.fractionalValue =
               (uint32_t) ((pvrscope_obj->TotalShLoad -
                    pvrscope_obj->loadStats.totalLoadParams.integerValue) * 10);
#if 0  /* Enable to print on the console */
           Vps_printf(" SYSTEM_PVRSCOPE: Load of Fragment + Vertex shader %f\n",
               pvrscope_obj->TotalShLoad);
           Vps_printf(" SYSTEM_PVRSCOPE: Load of Fragment shader %f\n",
               pvrscope_obj->fragShLoad);
           Vps_printf(" SYSTEM_PVRSCOPE: Load of Vertex shader %f\n",
               pvrscope_obj->vertShLoad);
#endif
       }
       else
       {
           Vps_printf(" SYSTEM_PVRSCOPE: Received unexpected data from FIFO - \" %s \" \n", buf);
       }
       memset(buf, 0x0, sizeof(buf)); 
   }
   else
   {
      /* Do nothing */
   }
}

/* Nothing beyond this point */

