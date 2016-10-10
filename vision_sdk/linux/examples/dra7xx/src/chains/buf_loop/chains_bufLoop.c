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
 * \file chains_vipSingleCameraView.c
 *
 * \brief  Usecase file implementation of capture display usecase.
 *
 *         Usecase file for single camere view usecase.
 *
 *         EpSource --> EpSink Link(A15)
 *
 *         The data flow daigram is shown below
 *
 *              EpSource (A15)
 *                   |
 *               EpSink (A15)
 *
 *******************************************************************************
*/

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */

#include "chains_bufLoop_priv.h"
#include <linux/examples/common/chains_common.h>
#include <vivi_preview.h>
#include "../../ti-plugins/include/preview_chain.h"

/**
 *******************************************************************************
 *
 *  \brief  SingleCameraViewObject
 *
 *        This structure contains all the LinksId's and create Params.
 *        The same is passed to all create, start, stop functions.
 *
 *******************************************************************************
*/
typedef struct {
    chains_bufLoopObj ucObj;

    struct vivi_source source;
    struct vivi_source sink;

}chains_bufLoopAppObj;

chains_bufLoopAppObj gBufLoopObj;

/**
 *******************************************************************************
 *
 * \brief   Set link Parameters
 *
 *          It is called in Create function of the auto generated use-case file.
 *
 * \param pUcObj    [IN] Auto-generated usecase object
 * \param appObj    [IN] Application specific object
 *
 *******************************************************************************
*/
Void chains_bufLoop_SetAppPrms(chains_bufLoopObj *pUcObj,Void *appObj)
{
}

Void chains_bufLoop_InitSourceEpApp(struct vivi_sink *sink, struct vivi_source *source)
{
    chains_bufLoopAppObj *pObj
        = (chains_bufLoopAppObj *)&gBufLoopObj;

    chains_bufLoop_SetLinkId(&pObj->ucObj);

    /* Let's set the epLink ids now */
    sink->eplink_id[0] = pObj->ucObj.EpSinkLinkID;
    source->eplink_id[0] = pObj->ucObj.EpSourceLinkID;
    /* saving into the chain instance for future */
    pObj->sink.eplink_id[0] = sink->eplink_id[0];
    pObj->source.eplink_id[0] = source->eplink_id[0];
}

Int32 chains_bufLoop_CreateSourceEpApp(struct vivi_sink *sink, struct vivi_source *source,
                struct vivi_source_config *sourceCfg[])
{
    Int32 i;
    EpLink_CreateParams  *pEndp;
    chains_bufLoopObj *pUcObj;
    chains_bufLoopAppObj *pObj
        = (chains_bufLoopAppObj *)&gBufLoopObj;

    pUcObj = &pObj->ucObj;
    /* sink configuration */
    pEndp = &pUcObj->EpSinkPrm;
    pEndp->epType = EP_SINK;
    pEndp->chainId = PREVIEW_BUF_LOOP;
    strcpy(pEndp->plugName, PREVIEW_PLUGIN_NAME);
    /* source configuration */
    pEndp = &pUcObj->EpSourcePrm;
    pEndp->epType = EP_SOURCE;
    pEndp->chainId = PREVIEW_BUF_LOOP;
    strcpy(pEndp->plugName, PREVIEW_PLUGIN_NAME);

    for (i=0; i<PREVIEW_BUF_LOOP_NUM_EP_SOURCE; i++) {
        if (sourceCfg[i]->eplink_id == pObj->source.eplink_id[i]) {
            memcpy(&pEndp->srcConfig.queInfo, &sourceCfg[i]->config,
                            sizeof(System_LinkQueInfo));
        }
    }

    /* need to return it back to the application */
    sink->eplink_id[0] = pObj->sink.eplink_id[0];
    source->eplink_id[0] = pObj->source.eplink_id[0];

    return (chains_bufLoop_Create(&pObj->ucObj, pObj));
}

Int32 chains_bufLoop_StartApp(Void)
{
    chains_bufLoopAppObj *pObj
        = (chains_bufLoopAppObj *)&gBufLoopObj;

    return (chains_bufLoop_Start(&pObj->ucObj));
}

Int32 chains_bufLoop_StopApp(Void)
{
    chains_bufLoopAppObj *pObj
        = (chains_bufLoopAppObj *)&gBufLoopObj;

    return (chains_bufLoop_Stop(&pObj->ucObj));
}

Int32 chains_bufLoop_DeleteApp(Void)
{
    chains_bufLoopAppObj *pObj
        = (chains_bufLoopAppObj *)&gBufLoopObj;

    return (chains_bufLoop_Delete(&pObj->ucObj));
}
