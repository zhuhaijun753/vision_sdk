

#ifndef _OSA_MEM_H_
#define _OSA_MEM_H_

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>


#include <osa_mutex.h>
#include <osa_mem_map.h>
#include <osa_mem_shared.h>

#define  OSA_MEM_MAXHEAPS                                                 (8U)

typedef enum
{
  OSA_MEM_REGION_TYPE_SR0,
  OSA_MEM_REGION_TYPE_SR1,
  OSA_MEM_REGION_TYPE_REMOTE_LOG,
  OSA_MEM_REGION_TYPE_SYSTEM_IPC,
  OSA_MEM_REGION_TYPE_AUTO,
  OSA_MEM_REGION_TYPE_INVALID,
  OSA_MEM_REGION_TYPE_MAX

} OSA_MemRegionType;

typedef struct
{
  unsigned int physAddr;
  unsigned int virtAddr;
  unsigned int size;
} OSA_MemRegion;

int OSA_memInit(void);
int OSA_memDeInit(void);

unsigned int OSA_memMap(unsigned int physAddr, unsigned int size);
int          OSA_memUnMap(unsigned int virtAddr, unsigned int size);

unsigned int OSA_memPhys2Virt(unsigned int physAddr, OSA_MemRegionType type);
unsigned int OSA_memVirt2Phys(unsigned int virtAddr, OSA_MemRegionType type);

unsigned int OSA_memOffset2Virt(unsigned int offset, OSA_MemRegionType type);
unsigned int OSA_memVirt2Offset(unsigned int virtAddr, OSA_MemRegionType type);

unsigned int OSA_memPhys2Offset(unsigned int physAddr, OSA_MemRegionType type);
unsigned int OSA_memOffset2Phys(unsigned int offset, OSA_MemRegionType type);

unsigned int OSA_memMapHeapIdToRegionId(Osa_HeapId heapId);

int OSA_memCacheWb(unsigned int virtAddrStart, unsigned int virtAddrEnd);
int OSA_memCacheInv(unsigned int virtAddr, unsigned int length);

Void* OSA_memAllocSR(UInt32 heapId, UInt32 size, UInt32 align);
Int32 OSA_memFreeSR(UInt32 heapId, Ptr addr, UInt32 size);

#endif /* _OSA_MEM_H_ */

