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
 * \file file_api.h Wrapper API's to use FAT filesystem with MMC/SD
 *
 *******************************************************************************
 */

#ifndef FILE_API_H_
#define FILE_API_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *  Include files
 *******************************************************************************
 */

#include <include/link_api/system.h>

/*******************************************************************************
 *  Function's
 *******************************************************************************
 */

/*
 *******************************************************************************
 * \brief Init Filesystem and MMCSD
 *
 *        Called during system init. User application should not call this API.
 *
 * \return 0 on success, else failure
 *******************************************************************************
 */
Int32 File_init(void);

/*
 *******************************************************************************
 * \brief De-Init Filesystem
 *
 *        Called during system deinit. User application should not call this API.
 *
 *******************************************************************************
 */
void  File_deInit(void);

/*
 *******************************************************************************
 * \brief Mount Filesystem
 *
 *        Called during system deinit. User application should not call this API.
 *
 *******************************************************************************
 */
void File_mountFs(void);

/*
 *******************************************************************************
 * \brief Un-Mount Filesystem
 *
 *        Called during system deinit. User application should not call this API.
 *
 *******************************************************************************
 */
void File_unmountFs(void);

/*
 *******************************************************************************
 * \brief Open a file in read or write mode
 *
 * \param filename [IN] MUST be in 8.3 format, long filename's not supported
 * \param mode     [IN] MUST be "rb" or "wb"
 *
 * \return -1 if file could not be opened, else file handle
 *******************************************************************************
 */
Int32 File_open(char *filename, char *mode);

/*
 *******************************************************************************
 * \brief Read from a file which is previously opened
 *
 * \param fd            [IN] File handle
 * \param dataBuf       [IN] buffer in which data will be read
 * \param bytesToRead   [IN] Number of bytes to read
 * \param bytesRead     [OUT] Number of bytes actually read
 *
 * \return 0 if success, else failure
 *******************************************************************************
 */
Int32 File_read(Int32 fd, UInt8 *dataBuf, UInt32 bytesToRead, UInt32 *bytesRead);

/*
 *******************************************************************************
 * \brief Write to a file which is previously opened
 *
 * \param fd        [IN] File handle
 * \param dataBuf   [IN] buffer from which data will be written
 * \param dataSize  [IN] Number of bytes to write
 *
 * \return 0 if success, else failure
 *******************************************************************************
 */
Int32 File_write(Int32 fd, UInt8 *dataBuf, UInt32 dataSize);

/*
 *******************************************************************************
 * \brief Write to a file which is previously opened
 *
 * \param fd        [IN] File handle
 * \param dataBuf   [IN] buffer from which data will be written
 * \param dataSize  [IN] Number of bytes to write
 *
 * \return 0 if success, else failure
 *******************************************************************************
 */
Int32 File_seek(Int32 fd,UInt32 offset);

/*
 *******************************************************************************
 * \brief Write to a file which is previously opened
 *
 * \param fd        [IN] File handle
 * \param dataBuf   [IN] buffer from which data will be written
 * \param dataSize  [IN] Number of bytes to write
 *
 * \return 0 if success, else failure
 *******************************************************************************
 */
Int32 File_chDir(char *dirName);

/*
 *******************************************************************************
 * \brief Write to a file which is previously opened
 *
 * \param fd        [IN] File handle
 * \param dataBuf   [IN] buffer from which data will be written
 * \param dataSize  [IN] Number of bytes to write
 *
 * \return 0 if success, else failure
 *******************************************************************************
 */
Int32 File_getCwd(char *dirName, UInt32 len);

/*
 *******************************************************************************
 * \brief Close a previously opened file
 *
 * \param fd [IN] File handle
 *
 * \return 0 if success, else failure
 *******************************************************************************
 */
Int32 File_close(Int32 fd);

/*
 *******************************************************************************
 * \brief Checks end of file
 *
 * \param fd [IN] File handle
 *
 * \return 0 if success, else failure
 *******************************************************************************
 */

Int32 File_eof(Int32 fd);

/*
 *******************************************************************************
 * \brief Checks end of file
 *
 * \param fd [IN] File handle
 *
 * \return 0 if success, else failure
 *******************************************************************************
 */
Int32 File_lseek(Int32 fd, UInt32 ofs);



/*
 * Utility wrapper APIs to read/write complete file's
 */
Int32 Utils_fileReadFile(char *fileName, UInt8 *addr, UInt32 readSize, Uint32 *actualReadSize);
Int32 Utils_fileWriteFile(char *fileName, Uint8 *addr, Uint32 size);

/*
 * Utility APIs to run basic file IO test
 */
void Utils_fileTestRun(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/* @} */


