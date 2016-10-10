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
#include <include/link_api/system.h>
#include <examples/tda2xx/include/chains_main_srv_calibration.h>
#include <osa_file.h>
#include <osa_mem.h>
#include <osa_thr.h>
#include <linux/src/system/system_priv_ipc.h>
#include <linux/examples/common/chains_common.h>

/*
 *******************************************************************************
 *
 * Surround view calibration task parameters
 *
 *******************************************************************************
 */

#define CHAIN_COMMON_SRV_CALIBDATA_FILE_IO_TASK_SLEEP_DURATION (1000000)


/**
 *******************************************************************************
 *
 *  \brief  Data structure for the GA calibration
 *          The GA output LUT and perspestive matrix are generated while
 *          calibration ON and these tables are stored in QSPI flash.
 *          Successive run read these tables and input the same to SV GA
 *          Algorithm.
 *
 *******************************************************************************
 */
typedef struct {
    Chain_Common_SRV_GACalibrationType GACalibrationType;
    Chain_Common_SRV_GACalibrationType GACalibrationTypePrev;
    /**< This is a placeholder to remember the previous state
     *   of the SV alg GA calibration type.
     */
    UInt8 *gaLUTDDRPtr;
    UInt8 *persMatDDRPtr;
    UInt8 *featurePtDDRPtr;
    UInt8 *autoPersMatDDRPtr;
    AlgorithmLink_GAlignCalibrationMode calMode;
    Bool   TblWriteTaskExit;
    OSA_ThrHndl TblWriteTask;
    /**< Handle to task handing PersMat TBL write to QSPI flash */
    UInt8  svOutputMode;
} Chain_Common_SRV_GACalibrationInfo;

/**
 *******************************************************************************
 * \brief stack for PersMat table write task
 *******************************************************************************
 */

Chain_Common_SRV_GACalibrationInfo gChainCommon_SRVCalibInfo;

/**
 *******************************************************************************
 *
 * \brief   Task to write the GA persMat table to the QSPI flash
 *
 * \param   arg1    [IN]  Chain_Common_SRV_GACalibrationInfo
 * \param   arg2    [IN]  Chain_Common_SRV_GACalibrationInfo
 *
 *******************************************************************************
*/
Void *Chain_Common_SRV_PersMatTblWriteFun(Void *arg)
{

    Bool isPersMatTblUpdated;
    UInt32 persMatTblUpdateAfter1Min;
    UInt32 * ptr;
    Chain_Common_SRV_GACalibrationInfo *gaCalibInfo;
    UInt8 * persMatDDRPtr;

    gaCalibInfo = (Chain_Common_SRV_GACalibrationInfo *) arg;
    isPersMatTblUpdated = FALSE;
    persMatTblUpdateAfter1Min = 0;
    gaCalibInfo->TblWriteTaskExit = FALSE;

    // set persmat pointer to write into QSPI
    if (gaCalibInfo->svOutputMode == ALGORITHM_LINK_SRV_OUTPUT_3D)
    {
        persMatDDRPtr =  gaCalibInfo->autoPersMatDDRPtr;
    } else
    {
        persMatDDRPtr = gaCalibInfo->persMatDDRPtr;
    }

    while (!isPersMatTblUpdated && !gaCalibInfo->TblWriteTaskExit)
    {
        Task_sleep(CHAIN_COMMON_SRV_CALIBDATA_FILE_IO_TASK_SLEEP_DURATION);

        /* Both persMat & GA LUT are written back to QSPI during demo
         * STOP.  But if the demo ended due to battery drained condition
         * both these tables will not get updated in QSPI flash.  To
         * avoid this scenario, only persMat tbl alone is updated
         * after 5 minutes from the start of the demo.
         */
        persMatTblUpdateAfter1Min++;
        if ((persMatTblUpdateAfter1Min >= 60) && (!isPersMatTblUpdated ))
        {
            isPersMatTblUpdated = TRUE;
            if (gaCalibInfo->GACalibrationType != CHAIN_COMMON_SRV_GA_CALIBRATION_NO)
            {
               Vps_printf("Storing Persp Matrix ChainsCommon_SurroundView_PersMatTblWriteFun() \n");
               //ptr = (UInt32 *)OSA_memPhys2Virt((unsigned int)gaCalibInfo->persMatDDRPtr, OSA_MEM_REGION_TYPE_AUTO);
               ptr = (UInt32 *)OSA_memPhys2Virt((unsigned int)persMatDDRPtr, OSA_MEM_REGION_TYPE_AUTO);
               *ptr = GA_PERSPECTIVE_MATRIX_MAGIC_SEQUENCE;
               OSA_fileWriteFileOffset(
                  CALIBDATA_FILENAME,
                  //(UInt8 *)OSA_memPhys2Virt((unsigned int)gaCalibInfo->persMatDDRPtr, OSA_MEM_REGION_TYPE_AUTO),
                  (UInt8 *)OSA_memPhys2Virt((unsigned int)persMatDDRPtr, OSA_MEM_REGION_TYPE_AUTO),
                  GA_PERSPECTIVE_MATRIX_SIZE,
                  GA_PERSPECTIVE_MATRIX_FLASHMEM_OFFSET
                  );
               Vps_printf("Storing Persp Matrix ChainsCommon_SurroundView_PersMatTblWriteFun() done\n");
            }
        }
    }
    return NULL;
}

/**
 *******************************************************************************
 *
 * \brief   Function to allocate intermediate GA OUTPUT LUT and
 *          Perspective Matrix tables.  QSPI flash read/write
 *          is from/to this memory.
 *          Also create as task to update the peraMat table into
 *          QSPI flash after 5 min
 *
 * \param   gaCalibInfo    [IN]  Chain_Common_SRV_GACalibrationInfo
 *
 *******************************************************************************
*/
static Void Chain_Common_SRV_allocateGATbl(
            Chain_Common_SRV_GACalibrationInfo *gaCalibInfo)
{
    UInt32 * ptr;
    UInt32 actualReadSize;
    UInt8 * persMatDDRPtr;

    gaCalibInfo->gaLUTDDRPtr = (UInt8 *)System_ipcMemAlloc(OSA_HEAPID_DDR_CACHED_SR1, GA_OUTPUT_LUT_SIZE_MEM_ALLOC, 32);
    UTILS_assert(gaCalibInfo->gaLUTDDRPtr != NULL);

    gaCalibInfo->persMatDDRPtr = (UInt8 *)System_ipcMemAlloc(OSA_HEAPID_DDR_CACHED_SR1, GA_PERSPECTIVE_MATRIX_SIZE, 32);
    UTILS_assert(gaCalibInfo->persMatDDRPtr != NULL);

    gaCalibInfo->featurePtDDRPtr = (UInt8 *)System_ipcMemAlloc(OSA_HEAPID_DDR_CACHED_SR1, GA_FEATURE_PT_SIZE, 32);
    UTILS_assert(gaCalibInfo->featurePtDDRPtr != NULL);

    gaCalibInfo->autoPersMatDDRPtr = (UInt8 *)System_ipcMemAlloc(OSA_HEAPID_DDR_CACHED_SR1, GA_PERSPECTIVE_MATRIX_SIZE, 32);
    UTILS_assert(gaCalibInfo->autoPersMatDDRPtr != NULL);


    gaCalibInfo->calMode = ALGLINK_GALIGN_CALMODE_USERGALUT;


    // set persmat pointer to read from QSPI
    if (gaCalibInfo->svOutputMode == ALGORITHM_LINK_SRV_OUTPUT_3D)
    {
        persMatDDRPtr = gaCalibInfo->autoPersMatDDRPtr;
    } else
    {
        persMatDDRPtr = gaCalibInfo->persMatDDRPtr;
    }


    /* Read the PersMat tbl always from QSPI so that it can be
     * write back unconditionally to QSPI Flash */
    OSA_fileReadFileOffset(
       CALIBDATA_FILENAME,
       //(UInt8 *)OSA_memPhys2Virt((unsigned int)gaCalibInfo->persMatDDRPtr, OSA_MEM_REGION_TYPE_AUTO),
       (UInt8 *)OSA_memPhys2Virt((unsigned int)persMatDDRPtr, OSA_MEM_REGION_TYPE_AUTO),
       GA_PERSPECTIVE_MATRIX_SIZE,
       &actualReadSize,
       GA_PERSPECTIVE_MATRIX_FLASHMEM_OFFSET
       );

    switch (gaCalibInfo->GACalibrationType)
    {
        case CHAIN_COMMON_SRV_GA_CALIBRATION_NO:
            OSA_fileReadFileOffset(
               CALIBDATA_FILENAME,
               (UInt8 *)OSA_memPhys2Virt((unsigned int)gaCalibInfo->gaLUTDDRPtr, OSA_MEM_REGION_TYPE_AUTO),
               GA_MAGIC_PATTERN_SIZE_IN_BYTES,
               &actualReadSize,
               GA_OUTPUT_LUT_FLASHMEM_OFFSET
               );
            ptr = (UInt32 *)OSA_memPhys2Virt((unsigned int)gaCalibInfo->gaLUTDDRPtr, OSA_MEM_REGION_TYPE_AUTO);
            if (*ptr == GA_OUTPUT_LUT_MAGIC_SEQUENCE)
            {
                gaCalibInfo->calMode = ALGLINK_GALIGN_CALMODE_USERGALUT;
                OSA_fileReadFileOffset(
                  CALIBDATA_FILENAME,
                  (UInt8 *)OSA_memPhys2Virt((unsigned int)gaCalibInfo->gaLUTDDRPtr, OSA_MEM_REGION_TYPE_AUTO),
                  GA_OUTPUT_LUT_SIZE,
                  &actualReadSize,
                  GA_OUTPUT_LUT_FLASHMEM_OFFSET
                  );
                //TBD - cache write back
            }
            else
            {
                gaCalibInfo->calMode = ALGLINK_GALIGN_CALMODE_DEFAULT;
            }
            break;
        case CHAIN_COMMON_SRV_GA_CALIBRATION_FORCE:
           OSA_fileReadFileOffset(
             CALIBDATA_FILENAME,
             //(UInt8 *)OSA_memPhys2Virt((unsigned int)gaCalibInfo->persMatDDRPtr, OSA_MEM_REGION_TYPE_AUTO),
             (UInt8 *)OSA_memPhys2Virt((unsigned int)persMatDDRPtr, OSA_MEM_REGION_TYPE_AUTO),
             GA_MAGIC_PATTERN_SIZE_IN_BYTES,
             &actualReadSize,
             GA_PERSPECTIVE_MATRIX_FLASHMEM_OFFSET
             );
             //ptr = (UInt32 *)OSA_memPhys2Virt((unsigned int)gaCalibInfo->persMatDDRPtr, OSA_MEM_REGION_TYPE_AUTO);
             ptr = (UInt32 *)OSA_memPhys2Virt((unsigned int)persMatDDRPtr, OSA_MEM_REGION_TYPE_AUTO);
             if (*ptr == GA_PERSPECTIVE_MATRIX_MAGIC_SEQUENCE)
             {
                gaCalibInfo->calMode =
                                ALGLINK_GALIGN_CALMODE_FORCE_USERPERSMATRIX;
                OSA_fileReadFileOffset(
                  CALIBDATA_FILENAME,
                  //(UInt8 *)OSA_memPhys2Virt((unsigned int)gaCalibInfo->persMatDDRPtr, OSA_MEM_REGION_TYPE_AUTO),
                  (UInt8 *)OSA_memPhys2Virt((unsigned int)persMatDDRPtr, OSA_MEM_REGION_TYPE_AUTO),
                  GA_PERSPECTIVE_MATRIX_SIZE,
                  &actualReadSize,
                  GA_PERSPECTIVE_MATRIX_FLASHMEM_OFFSET
                  );

                //TBD - cache write back
            }
            else
            {
                gaCalibInfo->calMode =
                                ALGLINK_GALIGN_CALMODE_FORCE_DEFAULTPERSMATRIX;
            }
            break;
        case CHAIN_COMMON_SRV_GA_CALIBRATION_NO_USERSGXLUT:
            /*
             *  Read the pers mat
             */
            ptr = (UInt32 *)OSA_memPhys2Virt(
                                (unsigned int)persMatDDRPtr,
                                OSA_MEM_REGION_TYPE_AUTO);
            OSA_fileReadFileOffset(
                        CALIBDATA_FILENAME,
                        (UInt8 *)ptr,
                        GA_MAGIC_PATTERN_SIZE_IN_BYTES,
                        &actualReadSize,
                        GA_PERSPECTIVE_MATRIX_FLASHMEM_OFFSET);
            if (*ptr == GA_PERSPECTIVE_MATRIX_MAGIC_SEQUENCE)
            {
                Vps_printf (" Pers mat read ...\n");

                OSA_fileReadFileOffset(
                            CALIBDATA_FILENAME,
                            (UInt8 *)ptr,
                            GA_PERSPECTIVE_MATRIX_SIZE,
                            &actualReadSize,
                            GA_PERSPECTIVE_MATRIX_FLASHMEM_OFFSET);

                OSA_memCacheWb(
                        (UInt32)ptr,
                        (UInt32)ptr + GA_PERSPECTIVE_MATRIX_SIZE);

                Vps_printf (" Pers mat read (%d bytes) DONE\n",\
                                                GA_PERSPECTIVE_MATRIX_SIZE);

                /*
                 *  Read the GA SGX LUT
                 */
                ptr = (UInt32 *)OSA_memPhys2Virt(
                                    (unsigned int)gaCalibInfo->gaLUTDDRPtr,
                                    OSA_MEM_REGION_TYPE_AUTO);
                OSA_fileReadFileOffset(
                            CALIBDATA_FILENAME,
                            (UInt8 *)ptr,
                            GA_MAGIC_PATTERN_SIZE_IN_BYTES,
                            &actualReadSize,
                            GA_OUTPUT_LUT_FLASHMEM_OFFSET);
                if (*ptr == GA_OUTPUT_LUT_MAGIC_SEQUENCE)
                {
                    gaCalibInfo->calMode = ALGLINK_GALIGN3D_CALMODE_FORCE_USERGASGXLUT;

                    Vps_printf (" User GA SGX LUT read ...\n");

                    OSA_fileReadFileOffset(
                                CALIBDATA_FILENAME,
                                (UInt8 *)ptr,
                                GA_OUTPUT_LUT_SIZE,
                                &actualReadSize,
                                GA_OUTPUT_LUT_FLASHMEM_OFFSET);

                    OSA_memCacheWb(
                            (UInt32)ptr,
                            (UInt32)ptr + GA_OUTPUT_LUT_SIZE);

                    Vps_printf (" User GA SGX LUT read (%d bytes) DONE\n",\
                                                            GA_OUTPUT_LUT_SIZE);
                }
                else
                {
                    gaCalibInfo->calMode = ALGLINK_GALIGN3D_CALMODE_FORCE_DEFAULTPERSMATRIX;
                    Vps_printf (" User GA SGX LUT NOT found, using default LUT\n");
                }
            }
            else
            {
                gaCalibInfo->calMode = ALGLINK_GALIGN3D_CALMODE_FORCE_DEFAULTPERSMATRIX;
                Vps_printf (" Pers Mat NOT found, using default LUT\n");
            }

            break;
        default:
            UTILS_assert(0);
            break;
    }
}

/**
 *******************************************************************************
 *
 * \brief   Function to de-allocate intermediate GA OUTPUT LUT and
 *          Perspective Matrix tables.
 *          - Also perform QSPI flash write after the succesful calibration
 *          - Delete the task to update the peraMat table into QSPI flash
 *
 * \param   gaCalibInfo    [IN]  Chain_Common_SRV_GACalibrationInfo
 *
 *******************************************************************************
*/
static Void Chain_Common_SRV_freeGATbl(
            Chain_Common_SRV_GACalibrationInfo *gaCalibInfo)
{
    if (gaCalibInfo->GACalibrationType == CHAIN_COMMON_SRV_GA_CALIBRATION_FORCE)
    {
        gaCalibInfo->GACalibrationTypePrev = CHAIN_COMMON_SRV_GA_CALIBRATION_FORCE;
    }
    System_ipcMemFree(OSA_HEAPID_DDR_CACHED_SR1, (UInt32)gaCalibInfo->gaLUTDDRPtr, GA_OUTPUT_LUT_SIZE_MEM_ALLOC);
    System_ipcMemFree(OSA_HEAPID_DDR_CACHED_SR1, (UInt32)gaCalibInfo->persMatDDRPtr, GA_PERSPECTIVE_MATRIX_SIZE);
    System_ipcMemFree(OSA_HEAPID_DDR_CACHED_SR1, (UInt32)gaCalibInfo->featurePtDDRPtr, GA_FEATURE_PT_SIZE);
    System_ipcMemFree(OSA_HEAPID_DDR_CACHED_SR1, (UInt32)gaCalibInfo->autoPersMatDDRPtr, GA_PERSPECTIVE_MATRIX_SIZE);
}

/**
 *******************************************************************************
 *
 * \brief   Function implements different SRV calibration Types and Modes
 *
 * \param   gaCalibInfo    [IN]  Chain_Common_SRV_GACalibrationInfo
 *
 *******************************************************************************
*/
Void Chain_Common_SRV_Calibration(Chain_Common_SRV_CalibParams * calInfo)
{
    Chain_Common_SRV_GACalibrationInfo *gaCalibInfo;
    gaCalibInfo = &gChainCommon_SRVCalibInfo;
    gChainCommon_SRVCalibInfo.svOutputMode = calInfo->svOutputMode;


    switch (calInfo->calibState)
    {
      case CHAIN_COMMON_SRV_GA_CALIBRATION_STATE_CREATETIME:
      {
        if(calInfo->startWithCalibration)
        {
            gChainCommon_SRVCalibInfo.GACalibrationType
                = CHAIN_COMMON_SRV_GA_CALIBRATION_FORCE;
            gChainCommon_SRVCalibInfo.GACalibrationTypePrev
                = CHAIN_COMMON_SRV_GA_CALIBRATION_FORCE;
        }
        else
        {
            gChainCommon_SRVCalibInfo.GACalibrationType
                = CHAIN_COMMON_SRV_GA_CALIBRATION_NO;
            gChainCommon_SRVCalibInfo.GACalibrationTypePrev
                = CHAIN_COMMON_SRV_GA_CALIBRATION_NO;
        }

        if (calInfo->userGaSgxLut)
        {
            gChainCommon_SRVCalibInfo.GACalibrationType
                = CHAIN_COMMON_SRV_GA_CALIBRATION_NO_USERSGXLUT;
            gChainCommon_SRVCalibInfo.GACalibrationTypePrev
                = CHAIN_COMMON_SRV_GA_CALIBRATION_NO_USERSGXLUT;
        }

        Chain_Common_SRV_allocateGATbl(
            &gChainCommon_SRVCalibInfo
            );
        calInfo->calMode = gChainCommon_SRVCalibInfo.calMode;
        calInfo->gaLUTAddr = (UInt32) gChainCommon_SRVCalibInfo.gaLUTDDRPtr;
        calInfo->persMatAddr = (UInt32) gChainCommon_SRVCalibInfo.persMatDDRPtr;
        calInfo->featurePtsAddr = (UInt32) gChainCommon_SRVCalibInfo.featurePtDDRPtr;
        calInfo->autoPersMatAddr = (UInt32) gChainCommon_SRVCalibInfo.autoPersMatDDRPtr;
        break;
      }

      case CHAIN_COMMON_SRV_GA_CALIBRATION_STATE_RUNTIME:
      {
        switch(calInfo->calType)
        {
            case CHAIN_COMMON_SRV_GA_CALIBRATION_FORCE:
                gaCalibInfo->GACalibrationType = CHAIN_COMMON_SRV_GA_CALIBRATION_FORCE;
                break;
            case CHAIN_COMMON_SRV_GA_ERASE_ENTIRE_TABLE:
                OSA_fileCreateFile(CALIBDATA_FILENAME,
                  GA_PERSPECTIVE_MATRIX_FLASHMEM_OFFSET + GA_PERSPECTIVE_MATRIX_SIZE,
                  TRUE);
                gaCalibInfo->GACalibrationType = CHAIN_COMMON_SRV_GA_CALIBRATION_NO;
                gaCalibInfo->GACalibrationTypePrev = CHAIN_COMMON_SRV_GA_CALIBRATION_NO;
                break;
            case CHAIN_COMMON_SRV_GA_CALIBRATION_NO:
                gaCalibInfo->GACalibrationType = CHAIN_COMMON_SRV_GA_CALIBRATION_NO;
                break;
            default:
                Vps_printf("\nUnsupported option. Please try again\n");
                break;
        }
        break;
      }

      case CHAIN_COMMON_SRV_GA_CALIBRATION_STATE_DELETETIME:
      {
        Chain_Common_SRV_freeGATbl(
            &gChainCommon_SRVCalibInfo
            );
        break;
      }

      default:
          UTILS_assert(0);
          break;
    }
}

/**
 *******************************************************************************
 *
 * \brief   Function returns the calibration mode
 *
 * \param   gaCalibInfo    [IN]  Chain_Common_SRV_GACalibrationInfo
 *
 *******************************************************************************
*/
AlgorithmLink_GAlignCalibrationMode Chain_Common_SRV_getCalMode(Void)
{
    return (gChainCommon_SRVCalibInfo.calMode);
}

/*
 *  TDA2X 3D SRV Auto Calibration Functions
 */
/**
 *******************************************************************************
 *
 * \brief   TDA2X 3D SRV Auto Calibration Create
 *
 * \param   gaCalibInfo    [IN]  Chain_Common_SRV_GACalibrationInfo
 *
 *******************************************************************************
*/
Void Chain_Common_SRV_3DAutoCalibrationCreate()
{
    UInt32 i;
    CaptureLink_VipInstParams *pInstPrm;
    Chain_Common_SRV_3DAutoCalibCreateParams autoCalibCreatePrm;

    /* Mount the SD card */
    system("mount /dev/mmcblk0p1 /mnt\n");

    ChainsCommon_MultiCam_SetCapturePrms(&autoCalibCreatePrm.capturePrm, \
                                                                GA_NUM_CAMERA);

    for (i = 0; i < SYSTEM_CAPTURE_VIP_INST_MAX; i++)
    {
        pInstPrm = &autoCalibCreatePrm.capturePrm.vipInst[i];
        pInstPrm->numBufs = 6;
        pInstPrm->outParams[0].frameSkipMask = 0x00000000;
    }

    OSA_memCacheWb(
            (UInt32)&autoCalibCreatePrm,
            (UInt32)&autoCalibCreatePrm + \
                            sizeof (Chain_Common_SRV_3DAutoCalibCreateParams));

    /*
     *  Auto Calibration Use Case Creation on IPU1_0
     */
    System_linkControl(
            SYSTEM_LINK_ID_IPU1_0,
            TDA2X_SV_3D_AUTO_CALIB_CREATE,
            &autoCalibCreatePrm,
            sizeof (Chain_Common_SRV_3DAutoCalibCreateParams),
            TRUE);
}

/**
 *******************************************************************************
 *
 * \brief   Write Cal Mat into the MMC/SD card
 *
 * \param   gaCalibInfo    [IN]  Chain_Common_SRV_GACalibrationInfo
 *
 *******************************************************************************
*/
static Int32 writeCalMat(Int32 *calMatBuf, UInt32 calMatSize)
{
    FILE *fp;
    UInt32 cnt, writeVal;

    fp = fopen(GA_CAL_MAT_FILE, "wb");
    if (NULL != fp)
    {
        /* Num of cameras */
        writeVal = GA_NUM_CAMERA;
        fwrite(&writeVal, 1U, 4U, fp);

        /* Write size of each cal mat */
        for (cnt = 0U;cnt < GA_NUM_CAMERA;cnt ++)
        {
            writeVal = calMatSize;
            fwrite(&writeVal, 1U, 4U, fp);
        }

        fseek(fp, GA_MEDIA_SIZE_METADATA, SEEK_SET);

        OSA_memCacheInv(
                (UInt32)calMatBuf,
                calMatSize * GA_NUM_CAMERA);

        /* Write the cal mat */
        fwrite(calMatBuf, 1U, calMatSize * GA_NUM_CAMERA, fp);

        fclose(fp);

        return 0;
    }
    else
    {
        Vps_printf(" SRV: File open (%s) failed !!!\n",\
                                                GA_CAL_MAT_FILE);
        return -1;
    }
}

/**
 *******************************************************************************
 *
 * \brief   Read Cal Mat from the MMC/SD card
 *
 * \param   gaCalibInfo    [IN]  Chain_Common_SRV_GACalibrationInfo
 *
 *******************************************************************************
*/
static Int32 readCalMat(Int32 *calMatBuf, UInt32 *pCalMatSize)
{
    FILE *fp;
    UInt32 cnt, readVal;

    fp = fopen(GA_CAL_MAT_FILE,"rb");
    if (NULL != fp)
    {
        /* Num of cameras */
        fread(&readVal, 1U, 4U, fp);

        /* Write size of each cal mat */
        for (cnt = 0U;cnt < GA_NUM_CAMERA;cnt ++)
        {
            fread(&readVal, 1U, 4U, fp);
            *pCalMatSize = readVal;
        }

        fseek(fp, GA_MEDIA_SIZE_METADATA, SEEK_SET);

        /* Write the cal mat */
        fread(calMatBuf, 1U, (*pCalMatSize * GA_NUM_CAMERA), fp);

        fclose(fp);

        OSA_memCacheWb(
                (UInt32)calMatBuf,
                (UInt32)calMatBuf + \
                                (*pCalMatSize * GA_NUM_CAMERA));

        return 0;
    }
    else
    {
        Vps_printf(" SRV: File open (%s) failed !!!\n",\
                                                GA_CAL_MAT_FILE);
        return -1;
    }
}

/**
 *******************************************************************************
 *
 * \brief   TDA2X 3D SRV Write Calibration Output Buffers
 *
 * \param   gaCalibInfo    [IN]  Chain_Common_SRV_GACalibrationInfo
 *
 *******************************************************************************
*/
static Void writeBuffer(Chain_Common_SRV_3DCalibProcessParams *pCalibPrm)
{
    UInt8 *virtBufAddr;
    UInt32 magicSeq;

    /*
     *  Invalidate the GPU LUT buffer
     */
    virtBufAddr = (UInt8*)OSA_memPhys2Virt(
                            (unsigned int)pCalibPrm->gpuLutAddr,
                            OSA_MEM_REGION_TYPE_AUTO);
    /*
     *  Invalidate before reading the buffer
     */
    OSA_memCacheInv(
            (UInt32)virtBufAddr,
            pCalibPrm->gpuLutSize);

    /* Force create the calibration file */
    OSA_fileCreateFile(
            CALIBDATA_FILENAME,
            GA_PERSPECTIVE_MATRIX_FLASHMEM_OFFSET + \
                                        GA_PERSPECTIVE_MATRIX_SIZE,
            TRUE);

    /*
     *  Write the magic sequence
     */
    magicSeq = GA_OUTPUT_LUT_MAGIC_SEQUENCE;
    OSA_fileWriteFileOffset(
                        CALIBDATA_FILENAME,
                        (UInt8*)&magicSeq,
                        GA_MAGIC_PATTERN_SIZE_IN_BYTES,
                        GA_OUTPUT_LUT_FLASHMEM_OFFSET);

    /*
     *  Write the GA LUT
     */
    OSA_fileWriteFileOffset(
                        CALIBDATA_FILENAME,
                        (UInt8*)virtBufAddr,
                        pCalibPrm->gpuLutSize,
                        GA_OUTPUT_LUT_FLASHMEM_OFFSET + \
                                GA_MAGIC_PATTERN_SIZE_IN_BYTES);

    /*
     *  Write the cal mat into the file
     */
    virtBufAddr = (UInt8*)OSA_memPhys2Virt(
                        (unsigned int)pCalibPrm->calMatBufAddr,
                        OSA_MEM_REGION_TYPE_AUTO);
    OSA_memCacheInv(
            (UInt32)virtBufAddr,
            pCalibPrm->calMatSize * GA_NUM_CAMERA);

    /*
     *  Write the magic sequence
     */
    magicSeq = GA_PERSPECTIVE_MATRIX_MAGIC_SEQUENCE;
    OSA_fileWriteFileOffset(
                        CALIBDATA_FILENAME,
                        (UInt8*)&magicSeq,
                        GA_MAGIC_PATTERN_SIZE_IN_BYTES,
                        GA_PERSPECTIVE_MATRIX_FLASHMEM_OFFSET);

    /*
     *  Write 0x1 at the beginning of the cal mat to mark it as
     *  Auto caibrated cal mat
     */
    magicSeq = 1U;
    OSA_fileWriteFileOffset(
                        CALIBDATA_FILENAME,
                        (UInt8*)&magicSeq,
                        4U,
                        GA_PERSPECTIVE_MATRIX_FLASHMEM_OFFSET + \
                                    GA_MAGIC_PATTERN_SIZE_IN_BYTES);

    /*
     *  Write the GA LUT
     */
    OSA_fileWriteFileOffset(
                        CALIBDATA_FILENAME,
                        (UInt8*)virtBufAddr,
                        pCalibPrm->calMatSize * GA_NUM_CAMERA,
                        GA_PERSPECTIVE_MATRIX_FLASHMEM_OFFSET + \
                                GA_MAGIC_PATTERN_SIZE_IN_BYTES + 4U);

    /*
     *  Write the Cal mat into the MMC/SD card
     */
    writeCalMat((Int32*)virtBufAddr, pCalibPrm->calMatSize);
}

/**
 *******************************************************************************
 *
 * \brief   TDA2X 3D SRV Auto Calibration Process
 *
 * \param   gaCalibInfo    [IN]  Chain_Common_SRV_GACalibrationInfo
 *
 *******************************************************************************
*/
Void Chain_Common_SRV_3DAutoCalibrationProcess()
{
    Chain_Common_SRV_3DCalibProcessParams autoCalibPrm;
    Chain_Common_SRV_3DAutoCalibGetBufParams getBufPrm;
    FILE *hndlFile;
    UInt32 size;
    UInt8 *virtInChartBuf;

    hndlFile = fopen(GA_CHART_POSITION_FILE, "rb");
    if (NULL != hndlFile)
    {
        /*
         *  Get the buffer address and size
         */
        System_linkControl(
                SYSTEM_LINK_ID_IPU1_0,
                TDA2X_SV_3D_AUTO_CALIB_GET_BUF,
                &getBufPrm,
                sizeof (Chain_Common_SRV_3DAutoCalibGetBufParams),
                TRUE);

        OSA_memCacheInv(
                (UInt32)&getBufPrm,
                sizeof (Chain_Common_SRV_3DAutoCalibGetBufParams));

        virtInChartBuf = (UInt8*)OSA_memPhys2Virt(
                                (unsigned int)getBufPrm.inChartPosBuf,
                                OSA_MEM_REGION_TYPE_AUTO);

        /* Read the chart position file */
        size = fread (
                    virtInChartBuf,
                    1U,
                    getBufPrm.inChartPosBufSize,
                    hndlFile);
        if (0U <= size)
        {
            /* Write back the buffer */
            OSA_memCacheWb(
                    (UInt32)virtInChartBuf,
                    (UInt32)virtInChartBuf + \
                                    getBufPrm.inChartPosBufSize);

            /*
             *  Auto Calibration Process
             */
            System_linkControl(
                    SYSTEM_LINK_ID_IPU1_0,
                    TDA2X_SV_3D_AUTO_CALIB_PROCESS,
                    &autoCalibPrm,
                    sizeof (Chain_Common_SRV_3DCalibProcessParams),
                    TRUE);

            OSA_memCacheInv(
                    (UInt32)&autoCalibPrm,
                    sizeof (Chain_Common_SRV_3DCalibProcessParams));

            writeBuffer(&autoCalibPrm);
        }
        else
        {
            Vps_printf(" SRV: File read (%s) failed !!!\n",\
                                                    GA_CHART_POSITION_FILE);
        }

        fclose (hndlFile);
    }
    else
    {
        Vps_printf(" SRV: File open (%s) failed !!!\n",GA_CHART_POSITION_FILE);
    }
}

/**
 *******************************************************************************
 *
 * \brief   TDA2X 3D SRV User Cal Mat Calibration Process
 *
 * \param   gaCalibInfo    [IN]  Chain_Common_SRV_GACalibrationInfo
 *
 *******************************************************************************
*/
Void Chain_Common_SRV_3DUserCalMatCalibrationProcess()
{
    UInt8 *virtBufAddr;
    UInt32 calMatSize;
    Chain_Common_SRV_3DCalibProcessParams calMatCalibPrm;
    Chain_Common_SRV_3DAutoCalibGetBufParams getBufPrm;

    /*
     *  Get the buffer address and size
     */
    System_linkControl(
            SYSTEM_LINK_ID_IPU1_0,
            TDA2X_SV_3D_AUTO_CALIB_GET_BUF,
            &getBufPrm,
            sizeof (Chain_Common_SRV_3DAutoCalibGetBufParams),
            TRUE);

    OSA_memCacheInv(
            (UInt32)&getBufPrm,
            sizeof (Chain_Common_SRV_3DAutoCalibGetBufParams));

    virtBufAddr = (UInt8*)OSA_memPhys2Virt(
                        (unsigned int)getBufPrm.calMatBuf,
                        OSA_MEM_REGION_TYPE_AUTO);

    if (readCalMat((Int32*)virtBufAddr, &calMatSize) == 0)
    {
        System_linkControl(
                SYSTEM_LINK_ID_IPU1_0,
                TDA2X_SV_3D_CALMAT_CALIB_PROCESS,
                &calMatCalibPrm,
                sizeof (Chain_Common_SRV_3DCalibProcessParams),
                TRUE);

        OSA_memCacheInv(
                (UInt32)&calMatCalibPrm,
                sizeof (Chain_Common_SRV_3DCalibProcessParams));

        writeBuffer(&calMatCalibPrm);
    }
    else
    {
        Vps_printf(" SRV: CALMAT.BIN file is NOT present\n");
    }
}

/**
 *******************************************************************************
 *
 * \brief   TDA2X 3D SRV Default Cal Mat Calibration Process
 *
 * \param   gaCalibInfo    [IN]  Chain_Common_SRV_GACalibrationInfo
 *
 *******************************************************************************
*/
Void Chain_Common_SRV_3DDefaultCalMatCalibrationProcess()
{
    UInt8 *virtBufAddr;
    Chain_Common_SRV_3DCalibProcessParams calMatCalibPrm;
    Chain_Common_SRV_3DAutoCalibGetBufParams getBufPrm;
    Int32 defaultCalMat[] = {
        0x3fe3d380, 0xfc98e9df, 0x0183b1fc, 0xfc790aa9, 0xcb952369,
        0x248dc121, 0xff4c0220, 0xdb6cf86b, 0xcb7c34b0, 0xfffa7af1,
        0x000ba256, 0xfffbbeed, 0xfe34029e, 0xcbc583e6, 0x24f4787d,
        0xc005d487, 0x017bbde2, 0x0007019c, 0xff1f02ec, 0xdb0f17af,
        0xcbbf9c92, 0x0007df30, 0x000845a0, 0xfffe78dc, 0xc084b8cf,
        0x02ed3bdc, 0xf869ff11, 0x0667685c, 0x31f7d700, 0xd886ccb9,
        0x041e1dc6, 0xd81656d0, 0xce24d047, 0x00054fe5, 0xfffed221,
        0x0005fc0a, 0x00dde42f, 0x31660223, 0xd74f4579, 0x3ff9f9ed,
        0xfe7da513, 0xffbcd725, 0xfed685f9, 0xd75402bf, 0xce996899,
        0xfff8466c, 0xffff46c4, 0x0005b088};

    /*
     *  Get the buffer address and size
     */
    System_linkControl(
            SYSTEM_LINK_ID_IPU1_0,
            TDA2X_SV_3D_AUTO_CALIB_GET_BUF,
            &getBufPrm,
            sizeof (Chain_Common_SRV_3DAutoCalibGetBufParams),
            TRUE);

    OSA_memCacheInv(
            (UInt32)&getBufPrm,
            sizeof (Chain_Common_SRV_3DAutoCalibGetBufParams));

    virtBufAddr = (UInt8*)OSA_memPhys2Virt(
                        (unsigned int)getBufPrm.calMatBuf,
                        OSA_MEM_REGION_TYPE_AUTO);

    memcpy(
        virtBufAddr,
        defaultCalMat,
        (getBufPrm.calMatSize * GA_NUM_CAMERA));

    OSA_memCacheWb(
            (UInt32)virtBufAddr,
            (UInt32)virtBufAddr + \
                            (getBufPrm.calMatSize * GA_NUM_CAMERA));

    System_linkControl(
            SYSTEM_LINK_ID_IPU1_0,
            TDA2X_SV_3D_CALMAT_CALIB_PROCESS,
            &calMatCalibPrm,
            sizeof (Chain_Common_SRV_3DCalibProcessParams),
            TRUE);

    OSA_memCacheInv(
            (UInt32)&calMatCalibPrm,
            sizeof (Chain_Common_SRV_3DCalibProcessParams));

    writeBuffer(&calMatCalibPrm);
}

/**
 *******************************************************************************
 *
 * \brief   TDA2X 3D SRV Auto Calibration Dump frames
 *
 * \param   gaCalibInfo    [IN]  Chain_Common_SRV_GACalibrationInfo
 *
 *******************************************************************************
*/
Void Chain_Common_SRV_3DAutoCalibrationDumpframes()
{
    UInt32 cnt, size;
    UInt8 *virtBufAddr;
    Int8 fileName[64U];
    Chain_Common_SRV_3DAutoCalibDumpframesParams dumpframesPrm;
    FILE *hndlFile;

    System_linkControl(
            SYSTEM_LINK_ID_IPU1_0,
            TDA2X_SV_3D_AUTO_CALIB_DUMPFRAMES,
            &dumpframesPrm,
            sizeof (Chain_Common_SRV_3DAutoCalibDumpframesParams),
            TRUE);

    OSA_memCacheInv(
            (UInt32)&dumpframesPrm,
            sizeof (Chain_Common_SRV_3DAutoCalibDumpframesParams));

    UTILS_assert (0U != dumpframesPrm.frameSize);

    for (cnt = 0;cnt < GA_NUM_CAMERA;cnt ++)
    {
        UTILS_assert (NULL != (Ptr)dumpframesPrm.frameAddr[cnt]);

        virtBufAddr = (UInt8*)OSA_memPhys2Virt(
                            (unsigned int)dumpframesPrm.frameAddr[cnt],
                            OSA_MEM_REGION_TYPE_AUTO);

        OSA_memCacheInv(
                (UInt32)virtBufAddr,
                dumpframesPrm.frameSize);

        switch (cnt)
        {
            case 0U:
                strcpy(fileName,"/mnt/TDA2X/FRONT.YUV");
                break;
            case 1U:
                strcpy(fileName,"/mnt/TDA2X/RIGHT.YUV");
                break;
            case 2U:
                strcpy(fileName,"/mnt/TDA2X/BACK.YUV");
                break;
            case 3U:
                strcpy(fileName,"/mnt/TDA2X/LEFT.YUV");
                break;
        }

        hndlFile = fopen (fileName, "wb");
        if (NULL != hndlFile)
        {
            size = fwrite(
                        virtBufAddr,
                        1U,
                        dumpframesPrm.frameSize,
                        hndlFile);

            if (size != dumpframesPrm.frameSize)
            {
                Vps_printf(" SRV: File write (%s) failed !!!\n",fileName);
            }

            fclose (hndlFile);
        }
        else
        {
            Vps_printf(" SRV: File open (%s) failed !!!\n",fileName);
        }
    }
}

/**
 *******************************************************************************
 *
 * \brief   TDA2X 3D SRV Auto Calibration Print Statistics
 *
 * \param   gaCalibInfo    [IN]  Chain_Common_SRV_GACalibrationInfo
 *
 *******************************************************************************
*/
Void Chain_Common_SRV_3DAutoCalibrationPrintStats()
{
    System_linkControl(
            SYSTEM_LINK_ID_IPU1_0,
            TDA2X_SV_3D_AUTO_CALIB_PRINTSTAT,
            NULL,
            0U,
            TRUE);
}

/**
 *******************************************************************************
 *
 * \brief   TDA2X 3D SRV Auto Calibration Delete
 *
 * \param   gaCalibInfo    [IN]  Chain_Common_SRV_GACalibrationInfo
 *
 *******************************************************************************
*/
Void Chain_Common_SRV_3DAutoCalibrationDelete()
{
    /*
     *  Auto Calibration Use Case Deletion
     */
    System_linkControl(
            SYSTEM_LINK_ID_IPU1_0,
            TDA2X_SV_3D_AUTO_CALIB_DELETE,
            NULL,
            0U,
            TRUE);

    /* Unmount the SD card */
    system("umount /mnt\n");
}
