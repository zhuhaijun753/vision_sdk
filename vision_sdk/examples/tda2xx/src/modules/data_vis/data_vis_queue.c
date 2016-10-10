
#include "data_vis_priv.h"


Void DataVis_queueDataBwCreate()
{
    DataVis_CreatePrm *pCreatePrm;

    pCreatePrm = &gDataVis_Obj.createPrm;

    gDataVis_Obj.queue.readIdx = 0;
    gDataVis_Obj.queue.writeIdx = 0;
    gDataVis_Obj.queue.count = 0;

    gDataVis_Obj.queue.data =
        Utils_memAlloc(
            UTILS_HEAPID_DDR_CACHED_SR,
            sizeof(DataVis_DataBwElement)*pCreatePrm->maxNumberOfSamples, 32);
    UTILS_assert(gDataVis_Obj.pDrawDataBwElements != NULL);
}

Void DataVis_queueDataBwDelete()
{
    DataVis_CreatePrm *pCreatePrm;

    pCreatePrm = &gDataVis_Obj.createPrm;

    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR,
        gDataVis_Obj.queue.data,
        sizeof(DataVis_DataBwElement)*pCreatePrm->maxNumberOfSamples
        );
}

/*******************************************************************************
 *  \brief Write one element of data BW collected to shared memory
 *
 *         Called by the data collection thread.
 *
 *         API has no effect if DataVis_create() and DataVis_setDataBwPrm()
 *         are not called
 *
 *         If shared memory is full, the API internally reads one element to make
 *         space for new element
 *
 *******************************************************************************
 */
Int32 DataVis_writeDataBwData(DataVis_DataBwElement *pElem)
{
    Int32 retVal = SYSTEM_LINK_STATUS_EFAIL;
    DataVis_CreatePrm *pCreatePrm;

    if(gDataVis_Obj.isSetPrmDone==FALSE)
        return retVal;

    pCreatePrm = &gDataVis_Obj.createPrm;

    DataVis_lock();

    /* If we will overflow readPtr, increment readPtr */
    if(gDataVis_Obj.queue.count >= pCreatePrm->maxNumberOfSamples)
    {
        gDataVis_Obj.queue.readIdx = (gDataVis_Obj.queue.readIdx+1)%pCreatePrm->maxNumberOfSamples;
        gDataVis_Obj.queue.count--;
    }

    memcpy(&gDataVis_Obj.queue.data[gDataVis_Obj.queue.writeIdx],
           pElem,
           sizeof(DataVis_DataBwElement));

    gDataVis_Obj.queue.count++;
    gDataVis_Obj.queue.writeIdx = (gDataVis_Obj.queue.writeIdx+1)%pCreatePrm->maxNumberOfSamples;

    retVal = SYSTEM_LINK_STATUS_SOK;

    DataVis_unlock();

    return retVal;
}

/*******************************************************************************
 *  \brief Read element's of data BW collected in shared memory for drawing
 *         purpose
 *
 *         Called by the DataVis_drawDataBw() internally
 *
 *******************************************************************************
 */
Int32 DataVis_readDataBwData(DataVis_DataBwElement *pElem, UInt32 *numElementsRead, UInt32 maxElementsToRead)
{
    Int32 retVal = SYSTEM_LINK_STATUS_EFAIL;
    UInt32 elemToRead;
    DataVis_CreatePrm *pCreatePrm;

    if(gDataVis_Obj.isSetPrmDone==FALSE)
        return retVal;

    pCreatePrm = &gDataVis_Obj.createPrm;

    DataVis_lock();

    if((gDataVis_Obj.queue.count == 0) || (maxElementsToRead == 0))
    {
        /* Ignore if nothing has been written */
        *numElementsRead = 0;
        retVal = SYSTEM_LINK_STATUS_EFAIL;
    }
    else
    {
        if(gDataVis_Obj.queue.readIdx < gDataVis_Obj.queue.writeIdx)
        {
            /* Read till gDataVis_Obj.queue.writeIdx */
            elemToRead = gDataVis_Obj.queue.writeIdx - gDataVis_Obj.queue.readIdx;
            if(elemToRead > maxElementsToRead)
            {
                /* But don't exceed maxElementsToRead */
                elemToRead = maxElementsToRead;
            }
            memcpy(pElem,
                   &gDataVis_Obj.queue.data[gDataVis_Obj.queue.readIdx],
                   sizeof(DataVis_DataBwElement)*elemToRead);
            gDataVis_Obj.queue.readIdx = (gDataVis_Obj.queue.readIdx+elemToRead)%pCreatePrm->maxNumberOfSamples;
            gDataVis_Obj.queue.count -= elemToRead;

            *numElementsRead = elemToRead;
        }
        else
        {
            /* Read till end of buffer */
            elemToRead = pCreatePrm->maxNumberOfSamples - gDataVis_Obj.queue.readIdx;
            if(elemToRead > maxElementsToRead)
            {
                /* But don't exceed maxElementsToRead */
                elemToRead = maxElementsToRead;
            }
            memcpy(pElem,
                   &gDataVis_Obj.queue.data[gDataVis_Obj.queue.readIdx],
                   sizeof(DataVis_DataBwElement)*elemToRead);
            gDataVis_Obj.queue.readIdx = (gDataVis_Obj.queue.readIdx+elemToRead)%pCreatePrm->maxNumberOfSamples;
            gDataVis_Obj.queue.count -= elemToRead;
            *numElementsRead = elemToRead;

            /* If maxElementsToRead is not reached proceed */
            if(elemToRead < maxElementsToRead)
            {
                /* Update src and dest */
                pElem += elemToRead;

                /* Read till gDataVis_Obj.queue.writeIdx */
                if(gDataVis_Obj.queue.writeIdx > (maxElementsToRead - elemToRead))
                {
                    /* But don't exceed maxElementsToRead */
                    elemToRead = maxElementsToRead - elemToRead;
                }
                else
                {
                    elemToRead = gDataVis_Obj.queue.writeIdx;
                }
                memcpy(pElem,
                       &gDataVis_Obj.queue.data[gDataVis_Obj.queue.readIdx],
                       sizeof(DataVis_DataBwElement)*elemToRead);
                gDataVis_Obj.queue.readIdx = (gDataVis_Obj.queue.readIdx+elemToRead)%pCreatePrm->maxNumberOfSamples;
                gDataVis_Obj.queue.count -= elemToRead;
                *numElementsRead += elemToRead;
            }
        }

        retVal = SYSTEM_LINK_STATUS_SOK;
    }

    DataVis_unlock();

    return retVal;
}
