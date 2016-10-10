

#ifndef _OSA_MEM_SHARED_H_
#define _OSA_MEM_SHARED_H_

#include <osa_types.h>


typedef enum
{
    OSA_HEAPID_DDR_NON_CACHED_SR0,
    OSA_HEAPID_DDR_CACHED_SR1,
    OSA_HEAPID_MAXNUMHEAPS
} Osa_HeapId;

Void* OSA_memAllocSR(UInt32 heapId, UInt32 size, UInt32 align);
Int32 OSA_memFreeSR(UInt32 heapId, Ptr addr, UInt32 size);

#endif /* _OSA_MEM_SHARED_H_ */

