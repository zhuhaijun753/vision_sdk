/*
 *******************************************************************************
 *
 * Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */

#ifndef _SYSTEM_PVRSCOPE_IF_H_
#define _SYSTEM_PVRSCOPE_IF_H_

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

/*******************************************************************************
 *  Defines
 *******************************************************************************
 */
#define MAX_BUF 256

/*******************************************************************************
 *  Enums
 *******************************************************************************
 */

/*******************************************************************************
 *  Data structure's
 *******************************************************************************
 */

typedef struct {

    struct TotalLoad{

        uint32_t integerValue;
        uint32_t fractionalValue;

    } totalLoadParams;

} Utils_SystemLoadStats;

typedef struct
{
   int fd;
   char *gpufifo;
   float fragShLoad;
   float vertShLoad;
   double TotalShLoad;
   Utils_SystemLoadStats loadStats;

}System_PvrScope_Obj;

/**
 *******************************************************************************
 * \brief Function's
 *******************************************************************************
 */
void system_create_pvrscope_profile_data(System_PvrScope_Obj *pvrscope_obj);
void system_delete_pvrscope_profile_data(System_PvrScope_Obj *pvrscope_obj);
void system_get_pvrscope_profile_data(System_PvrScope_Obj *pvrscope_obj);

#endif

/* @} */
