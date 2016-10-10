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
#include <src/utils_common/include/file_api.h>
#include <src/utils_common/include/utils_mem.h>
#include <boards/bsp_board.h>
#include <devices/bsp_device.h>

#if ( defined(FATFS_PROC_TO_USE_IPU1_0) && defined(BUILD_M4_0) ) || \
    ( defined(FATFS_PROC_TO_USE_IPU1_1) && defined(BUILD_M4_1) ) || \
    ( defined(FATFS_PROC_TO_USE_IPU2)   && defined(BUILD_M4_2) ) || \
    ( defined(FATFS_PROC_TO_USE_A15_0)  && defined(BUILD_A15) )

#include <src/utils_common/src/file_api_priv.h>

/*******************************************************************************
 *  Function prototype's
 *******************************************************************************
 */
static void File_lock(void);
static void File_unlock(void);
static Int32 File_allocObj();
static Void File_freeObj(Int32 fd);

FileSys_Obj gFileSys_obj;

Int32 File_init(void)
{
    UInt32 i;

    gFileSys_obj.lock = BspOsal_semCreate((Int32)0, (Bool)TRUE);
    UTILS_assert(gFileSys_obj.lock != NULL);

    gFileSys_obj.edmaDone = BspOsal_semCreate((Int32)0, (Bool)TRUE);
    UTILS_assert(gFileSys_obj.edmaDone != NULL);

    File_unlock();

    gFileSys_obj.hEdma = Utils_dmaGetEdma3Hndl(UTILS_DMA_SYSTEM_EDMA_INST_ID);

    for(i=0; i<UTILS_FILE_MAX; i++)
    {
        gFileSys_obj.isAlloc[i] = (Bool)FALSE;
    }

    #if defined(TDA2XX_FAMILY_BUILD)
    {
        void File_initPinMuxPowerMMCSD_tda2xx();

        /* MMC1 PAD / Power / Clock Configuration*/
        File_initPinMuxPowerMMCSD_tda2xx();

        /* Initialize the MMC driver data structures */
        HSMMCSDInit(MMC1_INST);
    }
    #elif defined(TDA3XX_FAMILY_BUILD)
    {
        void File_initPinMuxPowerMMCSD_tda3xx();

        /* MMC4 PAD / Power / Clock Configuration*/
        File_initPinMuxPowerMMCSD_tda3xx();

        /* Initialize the MMC driver data structures */
        HSMMCSDInit(MMC4_INST);
    }

    #endif



    return 0;
}

void File_deInit(void)
{
    BspOsal_semDelete(&gFileSys_obj.lock);
}

void File_mountFs(void)
{
    #if defined(TDA2XX_FAMILY_BUILD)
    HSMMCSDInit(MMC1_INST);
    #elif defined(TDA3XX_FAMILY_BUILD)
    HSMMCSDInit(MMC4_INST);
    #else
    UTILS_assert(0);
    #endif
}

void File_unmountFs(void)
{
    #if defined(TDA2XX_FAMILY_BUILD)
    HSMMCSDDeInit(MMC1_INST);
    #elif defined(TDA3XX_FAMILY_BUILD)
    HSMMCSDDeInit(MMC4_INST);
    #else
    UTILS_assert(0);
    #endif
}

static void File_lock(void)
{
    UTILS_assert(gFileSys_obj.lock != NULL);
    BspOsal_semWait(gFileSys_obj.lock, BSP_OSAL_WAIT_FOREVER);
}

static void File_unlock(void)
{
    UTILS_assert(gFileSys_obj.lock != NULL);
    BspOsal_semPost(gFileSys_obj.lock);
}

static Int32 File_isAlloc(Int32 fd)
{
    Bool isAlloc = FALSE;

    if( (fd >= 0) && (fd < UTILS_FILE_MAX) )
    {
        if(gFileSys_obj.isAlloc[fd]==(Bool)TRUE)
        {
            isAlloc = TRUE;
        }
    }

    return isAlloc;
}

static Int32 File_allocObj(void)
{
    UInt32 i;
    Int32 fd;

    for(i=0; i<UTILS_FILE_MAX; i++)
    {
        if(gFileSys_obj.isAlloc[i]==(Bool)FALSE)
        {
            break;
        }
    }

    if(i<UTILS_FILE_MAX)
    {
        gFileSys_obj.isAlloc[i]=(Bool)TRUE;
        fd = (Int32)i;
    }
    else
    {
        fd = -1;
    }

    return fd;
}

static Void File_freeObj(Int32 fd)
{
    if(File_isAlloc(fd)==(Bool)TRUE)
    {
        gFileSys_obj.isAlloc[fd]=(Bool)FALSE;
    }
}


Int32 File_read(Int32 fd, UInt8 *dataBuf, UInt32 bytesToRead, UInt32 *bytesRead)
{
    Int32 status = -1;
    UINT bytes_read = 0;
    FRESULT fresult;

    File_lock();

    if(File_isAlloc(fd)==(Bool)TRUE)
    {
        fresult = f_read(
                        &gFileSys_obj.fileObj[fd],
                        (void *) dataBuf,
                        bytesToRead,
                        (UINT *) &bytes_read
                    );
        if (fresult == FR_OK)
        {
            status = 0;
        }
    }

    *bytesRead = bytes_read;

    File_unlock();

    return status;
}


Int32 File_write(Int32 fd, UInt8 *dataBuf, UInt32 dataSize)
{
    Int32 status = -1;
    UINT bytes_written = 0;
    FRESULT fresult;

    File_lock();

    if(File_isAlloc(fd)==(Bool)TRUE)
    {
        fresult = f_write(
                        &gFileSys_obj.fileObj[fd],
                        (void *) dataBuf,
                        dataSize,
                        (UINT *) &bytes_written
                    );
        if( (fresult == FR_OK) && (dataSize == bytes_written) )
        {
            status = 0;
        }
    }

    File_unlock();

    return status;
}

Int32 File_seek(Int32 fd, UInt32 offset)
{
    Int32 status = -1;
    FRESULT fresult;

    File_lock();

    if(File_isAlloc(fd)==(Bool)TRUE)
    {
        fresult = f_lseek(
                        &gFileSys_obj.fileObj[fd],
                        (DWORD) offset
                    );
        if (fresult == FR_OK)
        {
            status = 0;
        }
    }

    File_unlock();

    return status;
}

Int32 File_chDir(char *dirName)
{
    Int32 status = -1;
    FRESULT fresult;

    UTILS_assert(strlen(dirName) <= 8);

    File_lock();

    fresult = f_chdir(dirName);
    if (fresult == FR_OK)
    {
        status = 0;
    }

    File_unlock();

    return status;
}

Int32 File_getCwd(char *dirName, UInt32 len)
{
    Int32 status = -1;
    FRESULT fresult;

    UTILS_assert(len <= 8);

    File_lock();

    fresult = f_getcwd(dirName, len);
    if (fresult == FR_OK)
    {
        status = 0;
    }

    File_unlock();

    return status;
}

Int32 File_open(char *filename, char *mode)
{
    Int32 fd;
    FRESULT fresult;

    File_lock();

    fd = File_allocObj();
    if(File_isAlloc(fd)==(Bool)TRUE)
    {
        if(mode[0]=='w')
        {
            fresult = f_open(&gFileSys_obj.fileObj[fd], filename, FA_WRITE);
            if (fresult != FR_OK)
            {
                /*If file does not exist. Create new one*/
                fresult = f_open(&gFileSys_obj.fileObj[fd], filename, FA_CREATE_NEW);
                if (fresult != FR_OK)
                {
                    File_freeObj(fd);
                    fd = -1;
                }
                else
                {
                    f_close(&gFileSys_obj.fileObj[fd]);
                    /*Open handle to the newly created file*/
                    fresult = f_open(&gFileSys_obj.fileObj[fd], filename, FA_WRITE);
                    if (fresult != FR_OK)
                    {
                        File_freeObj(fd);
                        fd = -1;
                    }
                }
            }
        }
        else
        if(mode[0]=='r')
        {
            fresult = f_open(&gFileSys_obj.fileObj[fd], filename, FA_READ);
            if (fresult != FR_OK)
            {
                File_freeObj(fd);
                fd = -1;
            }
        }
        else
        {
            File_freeObj(fd);
            fd = -1;
        }
    }

    File_unlock();

    return fd;
}

Int32 File_close(Int32 fd)
{
    File_lock();

    if(File_isAlloc(fd)==(Bool)TRUE)
    {
        f_close(&gFileSys_obj.fileObj[fd]);
        File_freeObj(fd);
    }

    File_unlock();

    return 0;
}

Int32 File_eof(Int32 fd)
{
    return(f_eof(&gFileSys_obj.fileObj[fd]));
}

Int32 File_lseek(Int32 fd, UInt32 ofs)
{
    FRESULT fresult;
    Int32 status = 0;

    File_lock();

    fresult = f_lseek(&gFileSys_obj.fileObj[fd], ofs);
    if (fresult != FR_OK)
    {
        status = -1;
    }

    File_unlock();

    return status;
}


#if defined(TDA2XX_FAMILY_BUILD)

void   MMCSD_LDO_PWR()
{
    /*CTRL_CORE_CONTROL_PBIAS*/

    uint32_t reg_val = 0;

    reg_val = HW_RD_REG32(
        SOC_CTRL_MODULE_CORE_CORE_PAD_REGISTERS_BASE + CTRL_CORE_CONTROL_PBIAS);

    reg_val &= ~CTRL_CORE_CONTROL_PBIAS_SDCARD_IO_PWRDNZ_MASK;
    HW_WR_REG32(SOC_CTRL_MODULE_CORE_CORE_PAD_REGISTERS_BASE +
                CTRL_CORE_CONTROL_PBIAS, reg_val);
    BspOsal_sleep(1);
    reg_val &= ~CTRL_CORE_CONTROL_PBIAS_SDCARD_BIAS_PWRDNZ_MASK;
    HW_WR_REG32(SOC_CTRL_MODULE_CORE_CORE_PAD_REGISTERS_BASE +
                CTRL_CORE_CONTROL_PBIAS, reg_val);

    /*Enable SDCARD_BIAS_VMODE*/
    reg_val |= CTRL_CORE_CONTROL_PBIAS_SDCARD_BIAS_VMODE_MASK; /* 3v */
    HW_WR_REG32(SOC_CTRL_MODULE_CORE_CORE_PAD_REGISTERS_BASE +
                CTRL_CORE_CONTROL_PBIAS, reg_val);

    reg_val = HW_RD_REG32(
        SOC_CTRL_MODULE_CORE_CORE_PAD_REGISTERS_BASE + CTRL_CORE_CONTROL_PBIAS);
    reg_val |= CTRL_CORE_CONTROL_PBIAS_SDCARD_BIAS_PWRDNZ_MASK;
    HW_WR_REG32(SOC_CTRL_MODULE_CORE_CORE_PAD_REGISTERS_BASE +
                CTRL_CORE_CONTROL_PBIAS, reg_val);
    BspOsal_sleep(1);

    reg_val |= CTRL_CORE_CONTROL_PBIAS_SDCARD_IO_PWRDNZ_MASK;
    HW_WR_REG32(SOC_CTRL_MODULE_CORE_CORE_PAD_REGISTERS_BASE +
                CTRL_CORE_CONTROL_PBIAS, reg_val);

    BspOsal_sleep(1);
}

void File_initPinMuxPowerMMCSD_tda2xx()
{
    uint32_t tmpRegVal;
    /*MMC1- CM_L3INIT_MMC1_CLKCTRL */
    tmpRegVal  = HW_RD_REG32(SOC_L3INIT_CM_CORE_BASE + CM_L3INIT_MMC1_CLKCTRL);
    tmpRegVal |= 0x03000000;
    HW_WR_REG32(SOC_L3INIT_CM_CORE_BASE + CM_L3INIT_MMC1_CLKCTRL, tmpRegVal);

    /*Enable the module - mmc1  */
    PMHALModuleModeSet(PMHAL_PRCM_MOD_MMC1, PMHAL_PRCM_MODULE_MODE_ENABLED,
                       PM_TIMEOUT_INFINITE);
    MMCSD_LDO_PWR();

    /*MMC1 PAD Configuration*/
    PlatformMMCSD1SetPinMux();
}
#endif

#if defined(TDA3XX_FAMILY_BUILD)

void File_initPinMuxPowerMMCSD_tda3xx()
{
    uint32_t tmpRegVal;
    /*MMC4- CM_L4INIT_MMC4_CLKCTRL */
    tmpRegVal  = HW_RD_REG32(SOC_L4PER_CM_CORE_BASE + CM_L4PER_MMC4_CLKCTRL);
    tmpRegVal |= 0x02000000;
    HW_WR_REG32(SOC_L4PER_CM_CORE_BASE + CM_L4PER_MMC4_CLKCTRL, tmpRegVal);

    /*MMC4 PRCM config*/
    HW_WR_REG32(SOC_L4PER_CM_CORE_BASE + CM_L4PER_MMC4_CLKCTRL, 0x2);

    /* MMC4 PAD Configuration */
    PlatformMMCSD4SetPinMux();

    /* IOExpander MUX Configuration */
    Bsp_boardSelectDevice(BSP_DRV_ID_MMC, BSP_DEVICE_MMC_INST_ID_4);
}
#endif

#else

/* Dummy APIs for FATFS_PROC = none */
Int32 File_init()
{
    return 0;
}

void File_deInit(void)
{

}

void File_mountFs(void)
{

}

void File_unmountFs(void)
{

}

Int32 File_open(char *name, char *mode)
{
    return -1;
}

Int32 File_read(Int32 fd, UInt8 *dataBuf, UInt32 bytesToRead, UInt32 *bytesRead)
{
    *bytesRead = 0;
    return -1;
}

Int32 File_write(Int32 fd, UInt8 *dataBuf, UInt32 dataSize)
{
    return -1;
}

Int32 File_seek(Int32 fd, UInt32 offset)
{
    return -1;
}

Int32 File_chDir(char *dirName)
{
    return -1;
}

Int32 File_getCwd(char *dirName, UInt32 len)
{
    return -1;
}

Int32 File_close(Int32 fd)
{
    return -1;
}

Int32 File_eof(Int32 fd)
{
    return -1;
}

Int32 File_lseek(Int32 fd, UInt32 ofs)
{
    return -1;
}

#endif

Int32 Utils_fileReadFile(char *fileName, UInt8 *addr, UInt32 readSize, Uint32 *actualReadSize)
{
    Int32 retVal = 0;
    UInt8  *curAddr;

    UInt32 readDataSize, fileSize, chunkSize=1024*100;

    Int32 hndlFile;

    Vps_printf(" FILE: Reading file [%s] ... \n", fileName);

    hndlFile = File_open(fileName, "rb");

    if(hndlFile < 0)
    {
        retVal = -1;
        *actualReadSize=0;
        Vps_printf(" FILE: ERROR: Could not open FILE !!! \n");
    }
    else
    {
        curAddr = addr;
        fileSize = 0;

        while(1)
        {
            if(chunkSize > readSize)
            {
                chunkSize = readSize;
            }

            File_read(hndlFile, curAddr, chunkSize, &readDataSize);
            fileSize += readDataSize;
            if(chunkSize != readDataSize
                ||
               readSize==fileSize
                )
            {
                break;
            }
            curAddr += chunkSize;
        }

        Vps_printf(" FILE: File read Done. [%d bytes] \r\n", fileSize);

        File_close(hndlFile);

        *actualReadSize = fileSize;
    }

    return retVal;
}

Int32 Utils_fileWriteFile(char *fileName, Uint8 *addr, Uint32 size)
{
    Int32 retVal = 0;
    UInt32 writeDataSize;
    Int32 hndlFile;

    Vps_printf(" FILE: Writing to file [%s] (%d bytes) ... ", fileName, size);

    hndlFile = File_open(fileName, "wb");

    if(hndlFile < 0)
    {
        retVal = -1;
        Vps_printf(" FILE: ERROR: Could not open FILE !!! \n");
    }
    else
    {
        /* write in units of chunkSize */
        Int32 fileSize, chunkSize = 96*1024;
        UInt8  *curAddr;

        fileSize = size;
        curAddr  = addr;

        while(fileSize>0)
        {
            if(fileSize<chunkSize)
            {
                chunkSize = fileSize;
            }

            retVal = File_write(hndlFile, curAddr, (UInt32)chunkSize);
            if(retVal!=0)
            {
                /* error in writing, abort */
                break;
            }

            curAddr += chunkSize;
            fileSize -= chunkSize;
        }
        writeDataSize = size - fileSize;

        Vps_printf(" FILE: File write done. [%d bytes] \r\n", writeDataSize);
        File_close(hndlFile);
    }

    return retVal;
}

#define UTILS_FILE_TEST_BUF_SIZE        (32*1024*1024)

void Utils_fileTestRun(void)
{
    Ptr pBuf;
    Int32 status;
    UInt32 fileSize;

    pBuf = Utils_memAlloc(
                UTILS_HEAPID_DDR_CACHED_SR,
                UTILS_FILE_TEST_BUF_SIZE,
                4);
    UTILS_assert(pBuf!=NULL);

    status = Utils_fileReadFile("AppImage", pBuf, UTILS_FILE_TEST_BUF_SIZE, &fileSize);
    if(status==0)
    {
        Utils_fileWriteFile("AppImage.OUT", pBuf, fileSize);
    }

    status = Utils_memFree(
        UTILS_HEAPID_DDR_CACHED_SR,
        pBuf,
        UTILS_FILE_TEST_BUF_SIZE
    );
    UTILS_assert(status==SYSTEM_LINK_STATUS_SOK);
}

