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
 * \file srv3DLink_drv.c
 *
 * \brief  This file has the implementation of Srv3D Link
 *
 *         Srv3D Link is used to feed video frames to SGX for
 *         rendering.
 *
 * \version 0.0 (Aug 2015) : [SS] First version
 *
 *******************************************************************************
*/

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */
#include "srv3dLink_priv.h"

/**
 *******************************************************************************
 *
 * \brief Srv3D link create function
 *
 * \param   pObj     [IN] Srv3D Link Instance handle
 * \param   pPrm     [IN] Srv3D link create parameters
 *                        This need to be configured by the application
 *
 * \return  SYSTEM_LINK_STATUS_SOK
 *
 *******************************************************************************
 */
Int32 srv3dLink_drvCreate(srv3dLink_Obj *pObj,
                             srv3dLink_CreateParams *pPrm)
{
    Int32 status = SYSTEM_LINK_STATUS_SOK;
    UInt32                  inQue;


#ifdef SRV3DLINK_DEBUG
    Vps_printf(" SRV3D Link: Create in progress !!!\n");
#endif

    memcpy(&pObj->createArgs, pPrm, sizeof(*pPrm));

    OSA_assert(pPrm->numInQue == SRV3D_LINK_NUM_IPQ);

    for (inQue = 0; inQue < pPrm->numInQue; inQue++)
    {
        status = System_linkGetInfo(pPrm->inQueParams[inQue].prevLinkId,
                                    &pObj->inTskInfo[inQue]);
        OSA_assert(status == SYSTEM_LINK_STATUS_SOK);
        OSA_assert(pPrm->inQueParams[inQue].prevLinkQueId <
                   pObj->inTskInfo[inQue].numQue);
    }

    pObj->sysBufferGALUT = NULL;
    pObj->sysBufferPALUT = NULL;
    pObj->sysBufferBlendLUT = NULL;

    /*We dont really need to plug in this info. Only to conform to practice*/
    pObj->tskInfo.numQue = 1;
    pObj->tskInfo.queInfo[0].numCh = 1;
    pObj->tskInfo.queInfo[0].chInfo[0].width = SRV3D_LINK_OUTPUT_FRAME_WIDTH;
    pObj->tskInfo.queInfo[0].chInfo[0].height = SRV3D_LINK_OUTPUT_FRAME_HEIGHT;
    pObj->tskInfo.queInfo[0].chInfo[0].pitch[0] = SRV3D_LINK_OUTPUT_FRAME_WIDTH * 4;
    pObj->tskInfo.queInfo[0].chInfo[0].flags = System_Link_Ch_Info_Set_Flag_Data_Format(pObj->tskInfo.queInfo[0].chInfo[0].flags, SYSTEM_DF_ARGB32_8888);

    status  = OSA_queCreate(&(pObj->eglEmptyBufQue), SRV3D_LINK_MAXNUM_EGL_BUFFERS);
    status  = OSA_queCreate(&(pObj->eglFullBufQue), SRV3D_LINK_MAXNUM_EGL_BUFFERS);

    pObj->numTexture = 0;

    OSA_assert(status == SYSTEM_LINK_STATUS_SOK);
#ifdef SRV3DLINK_DEBUG
    Vps_printf(" SRV3D Link: Create Done !!!\n");
#endif

    return SYSTEM_LINK_STATUS_SOK;
}


/**
 *******************************************************************************
 *
 * \brief This function prints the Link status
 *
 *        prints the Link status, such as
 *        - FPS
 *        - Callback Intervals
 *        - Input DropCount
 *        - etc
 *
 * \param   pObj        [IN] Srv3D Link Instance handle
 *
 * \return  SYSTEM_LINK_STATUS_SOK
 *
 *******************************************************************************
 */
Int32 srv3dLink_drvPrintStatistics(srv3dLink_Obj *pObj)
{
    return SYSTEM_LINK_STATUS_SOK;
}

/**
 *******************************************************************************
 *
 * \brief This function return the channel info to the next link
 *
 * \param  pTsk     [IN]  Task Handle
 * \param  pTsk     [OUT] channel info
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
 */
Int32 srv3dLink_drvGetLinkInfo(Void *pTsk,
                             System_LinkInfo *info)
{
    OSA_TskHndl * pTskHndl = (OSA_TskHndl *)pTsk;
    srv3dLink_Obj * pObj = (srv3dLink_Obj * )pTskHndl->appData;

    /* 'info' structure is set with valid values during 'create' phase
     * Simply pass on previous link info to next link
     */

    memcpy(info, &pObj->tskInfo, sizeof(*info));

    return SYSTEM_LINK_STATUS_SOK;
}

/**
 *******************************************************************************
 * \brief Function called by links connected to Srv3d link to get data from
 *    the output queue of the link
 *
 * \param  ptr      [IN]  Handle to task
 * \param  queId    [IN]  output queue Id
 * \param  pBufList [OUT] A List of buffers needed for the next link
 *
 * \return SYSTEM_LINK_STATUS_SOK on success
 *******************************************************************************
*/
Int32 srv3dLink_drvGetFullBuffers(Void * ptr, UInt16 queId,
                            System_BufferList * pBufList)
{
    System_Buffer *nextBuf;
    OSA_TskHndl * pTskHndl = (OSA_TskHndl *)ptr;
    srv3dLink_Obj * pObj = (srv3dLink_Obj * )pTskHndl->appData;
    Int32 status;

    pBufList->numBuf = 0;

    /*Get all the ready buffers and send them in one buflist*/
    do {
        status = OSA_queGet(&(pObj->eglFullBufQue), (Int32 *)&nextBuf, OSA_TIMEOUT_NONE);
	if(status == OSA_SOK) {
            pBufList->buffers[pBufList->numBuf] = nextBuf;
#ifdef SRV3DLINK_DEBUG
            Vps_printf("pixmap sending = %p\n", *((Int *)nextBuf->payload));
#endif
            pBufList->numBuf++;
	}
    } while (status == OSA_SOK);

    return SYSTEM_LINK_STATUS_SOK;
}

/**
 *******************************************************************************
 * \brief Function called by links connected to Srv3d link to put empty frames
 *    to the output queue of the link
 *
 * \param  ptr      [IN]  Handle to task
 * \param  queId    [IN]  output queue Id
 * \param  pBufList [OUT] A List of buffers needed for the next link
 *
 * \return SYSTEM_LINK_STATUS_SOK on success
 *******************************************************************************
*/
Int32 srv3dLink_drvPutEmptyBuffers(Void * ptr, UInt16 queId, System_BufferList * pBufList)
{
    System_Buffer *nextBuf;
    OSA_TskHndl * pTskHndl = (OSA_TskHndl *)ptr;
    srv3dLink_Obj * pObj = (srv3dLink_Obj * )pTskHndl->appData;
    Int32 status;
    Int32 count;

    /*Add them to the empty list. ProcessBUffers will fill them up*/
    for (count = 0; count < pBufList->numBuf; count++) {
	    nextBuf = pBufList->buffers[count];
#ifdef SRV3DLINK_DEBUG
            Vps_printf("pixmap returned = %p\n", *((Int *)nextBuf->payload));
#endif
	    status = OSA_quePut(&(pObj->eglEmptyBufQue), (Int32)nextBuf, OSA_TIMEOUT_FOREVER);
	    OSA_assert(status == SYSTEM_LINK_STATUS_SOK);
    }

    return SYSTEM_LINK_STATUS_SOK;
}

/**
 *******************************************************************************
 *
 * \brief This function start the simply driver
 *
 *        Primming of a few frames are required to start the Srv3D driver.
 *        Use blank buffers to prime and start the simply driver even
 *        before the actual frames are received by the srv3D link. This
 *        primming is done while srv3D link create. Start shall be called
 *        only after the link create function
 *
 * \param   pObj     [IN] Srv3D Link Instance handle
 *
 * \return  status
 *
 *******************************************************************************
 */
Int32 srv3dLink_drvStart(srv3dLink_Obj *pObj)
{
    Int32 status = SYSTEM_LINK_STATUS_SOK;

    return status;
}

/**
 *******************************************************************************
 *
 * \brief This function stop the simply driver
 *
 *        When ever the driver is stopped, enable the srv3D link periodic
 *        call back function. This will initiate to free-up the input frames
 *        in STOP state. The driver call back will be stopped when srv3D
 *        driver stop is done
 *
 * \param   pObj     [IN] Srv3D Link Instance handle
 *
 * \return  status
 *
 *******************************************************************************
 */
Int32 srv3dLink_drvStop(srv3dLink_Obj *pObj)
{
    Int32 status = SYSTEM_LINK_STATUS_SOK;

    return status;
}
/**
 *******************************************************************************
 *
 * \brief This function Provides the egl info to the simply driver
 *
 * \param   pObj     [IN] Srv3D Link Instance handle
 * \param   pPrm     [IN] eglInfo functions handle
 *
 * \return  status
 *
 *******************************************************************************
 */
Int32 srv3dLink_drvDoProcessEglInfo(srv3dLink_Obj *pObj, struct control_srv_egl_ctx *pPrm)
{
    Int32 count;
    System_Buffer *nextBuf;
    System_EglPixmapVideoFrameBuffer *eglBuf;
    EGLint major, minor, n;
    GLint ret;
    Int32 status = SYSTEM_LINK_STATUS_SOK;

    static const EGLint context_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };

    static const EGLint config_attribs[] = {
        EGL_SURFACE_TYPE, EGL_PIXMAP_BIT,
        EGL_RED_SIZE, 1,
        EGL_GREEN_SIZE, 1,
        EGL_BLUE_SIZE, 1,
        EGL_ALPHA_SIZE, 0,
        EGL_DEPTH_SIZE, 8,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_NONE
    };

    memcpy(&(pObj->eglInfo), pPrm, sizeof(pObj->eglInfo));

    pObj->nativeDisplay = pObj->eglInfo.get_egl_native_display();
    pObj->eglDisplay = eglGetDisplay(pObj->nativeDisplay);

    ret = eglInitialize(pObj->eglDisplay, &major, &minor);
    OSA_assert(ret != 0);

    ret = eglBindAPI(EGL_OPENGL_ES_API);
    OSA_assert(ret != 0);

    ret = eglChooseConfig(pObj->eglDisplay, config_attribs, &pObj->eglConfig, 1, &n);
    OSA_assert(ret != 0);
    OSA_assert(n == 1);

    pObj->eglContext = eglCreateContext(pObj->eglDisplay, pObj->eglConfig,
			EGL_NO_CONTEXT, context_attribs);
    OSA_assert(pObj->eglContext != NULL);

    eglMakeCurrent(pObj->eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, pObj->eglContext);

    status = SgxRender3DSRV_setup_offscreen(&pObj->render3DSRVObj);
    OSA_assert(status==SYSTEM_LINK_STATUS_SOK);

    for (count = 0; count < SRV3D_LINK_MAXNUM_EGL_BUFFERS; count++)
    {
        nextBuf = OSA_memAlloc(sizeof(System_Buffer));
	OSA_assert(nextBuf != NULL);

        nextBuf->payload = OSA_memAlloc(sizeof(System_EglPixmapVideoFrameBuffer));
	OSA_assert(nextBuf->payload != NULL);

        nextBuf->payloadSize = sizeof(System_EglPixmapVideoFrameBuffer);
        nextBuf->bufType = SYSTEM_BUFFER_TYPE_EGLPIXMAP_VIDEO_FRAME;
        nextBuf->chNum = 1;

        eglBuf = nextBuf->payload;
        eglBuf->eglPixmap = pObj->eglInfo.get_egl_native_buffer(SRV3D_LINK_OUTPUT_FRAME_WIDTH,
                                    SRV3D_LINK_OUTPUT_FRAME_HEIGHT);

	eglBuf->eglSurface = (UInt32)eglCreatePixmapSurface(pObj->eglDisplay, pObj->eglConfig, eglBuf->eglPixmap, NULL);
	OSA_assert(eglBuf->eglSurface != (UInt32)EGL_NO_SURFACE);

	eglBuf->flags = System_Bitstream_Buffer_Flag_Set_Bitstream_Format(eglBuf->flags, SYSTEM_DF_ARGB32_8888);
#ifdef SRV3DLINK_DEBUG
        Vps_printf("System Buffer created = %p, contains = %p (eglpixmap = %p)\n", nextBuf, eglBuf, eglBuf->eglPixmap);
#endif

	OSA_quePut(&(pObj->eglEmptyBufQue), (Int32)nextBuf, OSA_TIMEOUT_FOREVER);
	pObj->eglBuffers[count] = nextBuf;
    }

    pObj->eglCreateImageKHR = (PFNEGLCREATEIMAGEKHRPROC)eglGetProcAddress("eglCreateImageKHR");
    pObj->eglEGLImageTargetTexture2DOES = (PFNGLEGLIMAGETARGETTEXTURE2DOESPROC)eglGetProcAddress("glEGLImageTargetTexture2DOES");
    pObj->eglCreateSyncKHR = (PFNEGLCREATESYNCKHRPROC)eglGetProcAddress("eglCreateSyncKHR");
    pObj->eglDestroySyncKHR = (PFNEGLDESTROYSYNCKHRPROC)eglGetProcAddress("eglDestroySyncKHR");
    pObj->eglClientWaitSyncKHR = (PFNEGLCLIENTWAITSYNCKHRPROC)eglGetProcAddress("eglClientWaitSyncKHR");
    pObj->eglGetSyncAttribKHR = (PFNEGLGETSYNCATTRIBKHRPROC)eglGetProcAddress("eglGetSyncAttribKHR");

    return SYSTEM_LINK_STATUS_SOK;
}

/**
 *******************************************************************************
 *
 * \brief This function delete the srv3D link & driver
 *
 *        De-queue any frames which are held inside the driver, then
 *        - Delete the simply driver
 *        - delete the link periodic object
 *
 * \param   pObj     [IN] Sgx3Dsrv Link Instance handle
 *
 * \return  SYSTEM_LINK_STATUS_SOK
 *
 *******************************************************************************
 */
Int32 srv3dLink_drvDelete(srv3dLink_Obj *pObj)
{
    System_Buffer *nextBuf;
    System_EglPixmapVideoFrameBuffer *eglBuf;
    Int32 count;

#ifdef SRV3DLINK_DEBUG
    Vps_printf(" SRV3D Link: Delete in progress !!!\n");
#endif

    /*there might be pending EGL buffers. deque for full and empty queues*/
    eglMakeCurrent(pObj->eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    for (count = 0; count < SRV3D_LINK_MAXNUM_EGL_BUFFERS; count++)
    {
        nextBuf = pObj->eglBuffers[count];
        eglBuf = nextBuf->payload;
        eglDestroySurface(pObj->eglDisplay, (EGLSurface)eglBuf->eglSurface);
        pObj->eglInfo.destroy_egl_native_buffer((void *)eglBuf->eglPixmap);
        OSA_memFree(eglBuf);
        OSA_memFree(nextBuf);
    }
    eglDestroyContext(pObj->eglDisplay, pObj->eglContext);
    eglTerminate(pObj->eglDisplay);

    pObj->sysBufferGALUT = NULL;
    pObj->sysBufferPALUT = NULL;
    pObj->sysBufferBlendLUT = NULL;

    OSA_queDelete(&(pObj->eglEmptyBufQue));
    OSA_queDelete(&(pObj->eglFullBufQue));

#ifdef SRV3DLINK_DEBUG
    Vps_printf(" SRV3D Link: Delete Done !!!\n");
#endif

    return SYSTEM_LINK_STATUS_SOK;
}

Int32 srv3dLink_GetTexYuv(srv3dLink_Obj *pObj, Int32 width, Int32 height, Int32 *pitch, Int32 format, Void *bufAddr)
{
    GLuint textureId = 0;
    Int32 texFound = 0;
    Int32 i;
    EGLint attr[32];
    Int32 attrId;

    for(i = 0; i < pObj->numTexture; i++)
    {
        if(pObj->textureBufAddr[i]==bufAddr)
        {
            textureId = pObj->textureId[i];
            texFound = 1;
            break;
        }
    }

    if(texFound==0)
    {
        OSA_assert(i < SRV3DLINK_EGL_MAX_TEXTURES);

        /*Set the EGLImage attributes*/
        attrId = 0;
        attr[attrId++] = EGL_GL_VIDEO_FOURCC_TI;
        attr[attrId++] = FOURCC_STR("NV12");
        attr[attrId++] = EGL_GL_VIDEO_WIDTH_TI;
        attr[attrId++] = width;
        attr[attrId++] = EGL_GL_VIDEO_HEIGHT_TI;
        attr[attrId++] = height;
        attr[attrId++] = EGL_GL_VIDEO_BYTE_STRIDE_TI;
        attr[attrId++] = pitch[0];
        attr[attrId++] = EGL_GL_VIDEO_BYTE_SIZE_TI;
        attr[attrId++] = (pitch[0] * height * 3)/2;
        attr[attrId++] = EGL_GL_VIDEO_YUV_FLAGS_TI;
        attr[attrId++] = EGLIMAGE_FLAGS_YUV_CONFORMANT_RANGE | EGLIMAGE_FLAGS_YUV_BT601;
        attr[attrId++] = EGL_NONE;


        pObj->textureEglImage[i] = pObj->eglCreateImageKHR(
                                pObj->eglDisplay,
                                EGL_NO_CONTEXT,
                                EGL_RAW_VIDEO_TI,
                                bufAddr,
                                attr
                              );


        if (pObj->textureEglImage[i] == EGL_NO_IMAGE_KHR)
            Vps_printf(" EGL: ERROR: eglCreateImageKHR failed !!!\n");
        OSA_assert(pObj->textureEglImage[i] != EGL_NO_IMAGE_KHR);

        glGenTextures(1, &pObj->textureId[i]);

        glBindTexture(GL_TEXTURE_EXTERNAL_OES, pObj->textureId[i]);

        glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        pObj->eglEGLImageTargetTexture2DOES(GL_TEXTURE_EXTERNAL_OES, (GLeglImageOES)pObj->textureEglImage[i]);

        pObj->textureBufAddr[i] = bufAddr;
        pObj->numTexture++;

        textureId = pObj->textureId[i];

    }

    return textureId;
}

Int32 srv3dLink_drvDoProcessFrames(srv3dLink_Obj *pObj)
{
    srv3dLink_CreateParams     * pPrm;
    System_BufferList            inputBufList;
    System_BufferList            inputBufListReturn;
    System_Buffer              * pSysBufferInput;
    System_Buffer              * currCapture;
    System_VideoFrameCompositeBuffer *pVideoCompositeFrame;
    System_EglPixmapVideoFrameBuffer *eglBuf;
    System_MetaDataBuffer      * pPALUTBuffer;
    System_MetaDataBuffer      * pGALUTBuffer;
    System_MetaDataBuffer      * pBlendLUTBuffer;
    System_Buffer              * nextRenderBuf;
    Int32 bufId;
    GLuint                       texYuv[4] = {0};
    Int32 tex_width;
    Int32 tex_height;
    Int32 tex_pitch[3];
    Int32 tex_format;
    EGLSyncKHR sync;

    pPrm = &(pObj->createArgs);

    /*A new PALUT? replace the previous.*/
    System_getLinksFullBuffers(
        pPrm->inQueParams[SRV3D_LINK_IPQID_PALUT].prevLinkId,
        pPrm->inQueParams[SRV3D_LINK_IPQID_PALUT].prevLinkQueId,
        &inputBufList);

    inputBufListReturn.numBuf = 0;
    if(inputBufList.numBuf)
    {
#ifdef SRV3DLINK_DEBUG
        Vps_printf("PALUT obtained\n");
#endif
        for (bufId = 0; bufId < inputBufList.numBuf - 1; bufId++)
        {
            pSysBufferInput = inputBufList.buffers[bufId];
            inputBufListReturn.buffers[inputBufListReturn.numBuf] = pSysBufferInput;
            inputBufListReturn.numBuf++;
        }

	if(pObj->sysBufferPALUT) {
            inputBufListReturn.buffers[inputBufListReturn.numBuf] = pObj->sysBufferPALUT;
            inputBufListReturn.numBuf++;
	}

        if(inputBufListReturn.numBuf)
        {
            System_putLinksEmptyBuffers(pPrm->inQueParams[SRV3D_LINK_IPQID_PALUT].prevLinkId,
                                        pPrm->inQueParams[SRV3D_LINK_IPQID_PALUT].prevLinkQueId,
                                        &inputBufListReturn);
        }

        pObj->sysBufferPALUT = inputBufList.buffers[inputBufList.numBuf - 1];

	/*Also, it will be required to update PALUT array in openGL*/
    }

    /*A new GALUT? replace the previous.*/
    System_getLinksFullBuffers(
        pPrm->inQueParams[SRV3D_LINK_IPQID_GALUT].prevLinkId,
        pPrm->inQueParams[SRV3D_LINK_IPQID_GALUT].prevLinkQueId,
        &inputBufList);

    inputBufListReturn.numBuf = 0;
    if(inputBufList.numBuf)
    {
#ifdef SRV3DLINK_DEBUG
        Vps_printf("GALUT obtained\n");
#endif
        for (bufId = 0; bufId < inputBufList.numBuf - 1; bufId++)
        {
            pSysBufferInput = inputBufList.buffers[bufId];
            inputBufListReturn.buffers[inputBufListReturn.numBuf] = pSysBufferInput;
            inputBufListReturn.numBuf++;
        }

	if(pObj->sysBufferGALUT){
            inputBufListReturn.buffers[inputBufListReturn.numBuf] = pObj->sysBufferGALUT;
            inputBufListReturn.numBuf++;
	}

        if(inputBufListReturn.numBuf)
        {
            System_putLinksEmptyBuffers(pPrm->inQueParams[SRV3D_LINK_IPQID_GALUT].prevLinkId,
                                        pPrm->inQueParams[SRV3D_LINK_IPQID_GALUT].prevLinkQueId,
                                        &inputBufListReturn);
        }

        pObj->sysBufferGALUT = inputBufList.buffers[inputBufList.numBuf - 1];

	/*Also, it will be required to update GALUT array in openGL*/
    }

    /*A new BlendLUT? replace the previous.*/
    System_getLinksFullBuffers(
        pPrm->inQueParams[SRV3D_LINK_IPQID_BLENDLUT].prevLinkId,
        pPrm->inQueParams[SRV3D_LINK_IPQID_BLENDLUT].prevLinkQueId,
        &inputBufList);

    inputBufListReturn.numBuf = 0;
    if(inputBufList.numBuf)
    {
#ifdef SRV3DLINK_DEBUG
        Vps_printf("BLENDLUT obtained\n");
#endif
        for (bufId = 0; bufId < inputBufList.numBuf - 1; bufId++)
        {
            pSysBufferInput = inputBufList.buffers[bufId];
            inputBufListReturn.buffers[inputBufListReturn.numBuf] = pSysBufferInput;
            inputBufListReturn.numBuf++;
        }

	if(pObj->sysBufferBlendLUT) {
            inputBufListReturn.buffers[inputBufListReturn.numBuf] = pObj->sysBufferBlendLUT;
            inputBufListReturn.numBuf++;
	}

        if(inputBufListReturn.numBuf)
        {
            System_putLinksEmptyBuffers(pPrm->inQueParams[SRV3D_LINK_IPQID_BLENDLUT].prevLinkId,
                                        pPrm->inQueParams[SRV3D_LINK_IPQID_BLENDLUT].prevLinkQueId,
                                        &inputBufListReturn);
        }

        pObj->sysBufferBlendLUT = inputBufList.buffers[inputBufList.numBuf - 1];

	/*Also, it will be required to update BLENDLUT array in openGL*/
    }

    /*New Capture buffers? dequeue them all. Do 3D rendering for each of them.*/
    System_getLinksFullBuffers(
        pPrm->inQueParams[SRV3D_LINK_IPQID_MULTIVIEW].prevLinkId,
        pPrm->inQueParams[SRV3D_LINK_IPQID_MULTIVIEW].prevLinkQueId,
        &inputBufList);

    inputBufListReturn.numBuf = 0;
    if(inputBufList.numBuf)
    {
#ifdef SRV3DLINK_DEBUG
        Vps_printf("Multiframe obtained\n");
#endif
        for (bufId = 0; bufId < inputBufList.numBuf; bufId++)
        {
            pSysBufferInput = inputBufList.buffers[bufId];
            inputBufListReturn.buffers[inputBufListReturn.numBuf] = pSysBufferInput;
            inputBufListReturn.numBuf++;
        }

        currCapture = inputBufList.buffers[inputBufList.numBuf - 1];
        //Lets use this variable for flow flush. MISRA!!!!
        OSA_assert(currCapture != NULL);

	/*Check that GA, PA, BLEND are there*/
	if(!pObj->sysBufferBlendLUT)
		goto norender;
	if(!pObj->sysBufferGALUT)
		goto norender;
	if(!pObj->sysBufferPALUT)
		goto norender;

	/*Get an empty EGL buffer*/
	nextRenderBuf = NULL;
        OSA_queGet(&(pObj->eglEmptyBufQue), (Int32 *)&nextRenderBuf, OSA_TIMEOUT_NONE);
        if(!nextRenderBuf)
		goto norender;


        pPALUTBuffer = (System_MetaDataBuffer *)pObj->sysBufferPALUT->payload;
        OSA_assert (pPALUTBuffer != NULL);
        pObj->render3DSRVObj.PALUT3D = (void *) pPALUTBuffer->bufAddr[0];

        pGALUTBuffer = (System_MetaDataBuffer *)pObj->sysBufferGALUT->payload;
        OSA_assert (pGALUTBuffer != NULL);
        pObj->render3DSRVObj.LUT3D = (void *) pGALUTBuffer->bufAddr[0];

        pBlendLUTBuffer = (System_MetaDataBuffer *)pObj->sysBufferBlendLUT->payload;
        OSA_assert (pBlendLUTBuffer != NULL);
        pObj->render3DSRVObj.blendLUT3D = (void *) pBlendLUTBuffer->bufAddr[0];

        pVideoCompositeFrame = (System_VideoFrameCompositeBuffer *)(currCapture->payload);
        OSA_assert(pVideoCompositeFrame != NULL);

	eglBuf = nextRenderBuf->payload;

	eglMakeCurrent(pObj->eglDisplay,
			(EGLSurface)eglBuf->eglSurface,
			(EGLSurface)eglBuf->eglSurface,
			pObj->eglContext);

	/*Make textures from capture buffers*/
        tex_width      = SRV3D_LINK_INPUT_FRAME_WIDTH;
        tex_height     = SRV3D_LINK_INPUT_FRAME_HEIGHT;
        tex_pitch[0]   = SRV3D_LINK_INPUT_FRAME_WIDTH;
        tex_pitch[1]   = SRV3D_LINK_INPUT_FRAME_WIDTH;
        tex_format = SYSTEM_DF_YUV420SP_UV;
        texYuv[0] = srv3dLink_GetTexYuv(pObj, tex_width, tex_height, &tex_pitch[0], tex_format, pVideoCompositeFrame->bufAddr[0][0]);
        texYuv[1] = srv3dLink_GetTexYuv(pObj, tex_width, tex_height, &tex_pitch[0], tex_format, pVideoCompositeFrame->bufAddr[0][1]);
        texYuv[2] = srv3dLink_GetTexYuv(pObj, tex_width, tex_height, &tex_pitch[0], tex_format, pVideoCompositeFrame->bufAddr[0][2]);
        texYuv[3] = srv3dLink_GetTexYuv(pObj, tex_width, tex_height, &tex_pitch[0], tex_format, pVideoCompositeFrame->bufAddr[0][3]);

	/*render the frame*/
        render3dFrame_offscreen(&pObj->render3DSRVObj, texYuv);

	/*wait for the frame to finish*/
        if (pObj->eglCreateSyncKHR)
        {
            sync = pObj->eglCreateSyncKHR(pObj->eglDisplay, EGL_SYNC_FENCE_KHR, NULL);
            pObj->eglClientWaitSyncKHR(pObj->eglDisplay, sync, EGL_SYNC_FLUSH_COMMANDS_BIT_KHR, 1000 * 1000 * 32);
            pObj->eglDestroySyncKHR(pObj->eglDisplay, sync);
        } else {
            eglWaitGL();
        }

	/*Put it from full queue*/
	OSA_quePut(&(pObj->eglFullBufQue), (Int32)nextRenderBuf, OSA_TIMEOUT_FOREVER);
#ifdef SRV3DLINK_DEBUG
        Vps_printf("SystemBuf dequeued = %p, contains = %p (eglPixmap = %p)\n", nextRenderBuf, nextRenderBuf->payload, *((Int *)nextRenderBuf->payload));
#endif

	/*Tell next link, buffer is ready*/
        System_sendLinkCmd(pObj->createArgs.outQueParams.nextLink,
                           SYSTEM_CMD_NEW_DATA, NULL);

    }

norender:
    /*return the capture buffer. Not needed anymore*/
    if(inputBufListReturn.numBuf)
    {
        System_putLinksEmptyBuffers(pPrm->inQueParams[SRV3D_LINK_IPQID_MULTIVIEW].prevLinkId,
                                    pPrm->inQueParams[SRV3D_LINK_IPQID_MULTIVIEW].prevLinkQueId,
                                    &inputBufListReturn);
    }

    return SYSTEM_LINK_STATUS_SOK;

}

/* Nothing beyond this point */
