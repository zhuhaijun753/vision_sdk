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
 *   \ingroup FRAMEWORK_MODULE_API
 *   \defgroup OPENCL_LINK_API OpenCL Link API
 *
 *   OpenCL link is used to connect to the OpenCL monitor is the DSP.
 *
 *   @{
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \file OpenclLink.h
 *
 * \brief OpenCL link API public header file.
 *
 * \version 0.0 (Feb 2016) : [NN] First version
 *
 *******************************************************************************
 */

#ifndef OPENCL_LINK_H_
#define OPENCL_LINK_H_

#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
    UInt32 tskId;
    /**< SystemLink Task Id */

    Task_Handle tsk;
    /**< SystemLink Task Handle */

} OpenCLLink_Obj;

/**
*******************************************************************************
 *
 * \brief OpenCL register and init
 *
 *    - Creates OpenCL task
 *    - Registers as a link with the system API
 *
 * \return  SYSTEM_LINK_STATUS_SOK
 *
 *******************************************************************************
 */

Int32 OpenCL_DSPInit(void);
int rtos_init_ocl_dsp_monitor(int argc, char* argv[]);

/**
*******************************************************************************
 *
 * \brief OpenCL de-register and de-init
 *
 *    - Deletes OpenCL task
 *    - De-registers as a link with the system API
 *
 * \return  SYSTEM_LINK_STATUS_SOK
 *
 *******************************************************************************
 */

Int32 OpenCL_DSPdeInit(void);
#ifdef __cplusplus
}
#endif /* __cplusplus */

/* @} */

#endif
