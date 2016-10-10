/*
*******************************************************************************
*
* Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/
* ALL RIGHTS RESERVED
*
*******************************************************************************
*/

/*
 *******************************************************************************
 *
 * \file chains_common_iss.h
 *
 * \brief This file contains common functions and definitions for the ISS
 *
 *******************************************************************************
 */


#ifndef _CHAINS_COMMON_ISS_H_
#define _CHAINS_COMMON_ISS_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *  Includes
 *******************************************************************************
 */

#include <examples/tda2xx/include/chains.h>

/*******************************************************************************
 *  Defines
 *******************************************************************************
 */

/*  \brief Command to save DCC file
 *         This command is used to DCC file in the QSPI. This command is sent
 *         by the network tool and processed by the chains common layer
 */
#define SYSTEM_LINK_CMD_SAVE_DCC_FILE           (0x8000)

/*  \brief Command to clear DCC QSPI mem for the give sensor
 *         This command is used to clear QSPI memory for the given sensor .
 *         This command is sent by the network tool and processed by
 *         the chains common layer
 */
#define SYSTEM_LINK_CMD_CLEAR_DCC_QSPI_MEM      (0x8001)

/*  \brief Command to read a sensor Register, used by the network utility
 *         for reading sensor register using video sensor layer
 */
#define SYSTEM_LINK_CMD_READ_SENSOR_REG         (0x8002)

/*  \brief Command to write to sensor register, used by network utility
 *         writes given value to given sensor register using video
 *         sensor layer
 */
#define SYSTEM_LINK_CMD_WRITE_SENSOR_REG        (0x8003)

/* Based QSPI offset where DCC binary files are stored */
#define CHAINS_COMMON_ISS_DCC_QSPI_OFFSET       (24U*1024U*1024U)

/* QSPI Offset for the AR0140 sensor */
#define CHAINS_COMMON_ISS_DCC_QSPI_OFFSET_AR0140 (                             \
                                            CHAINS_COMMON_ISS_DCC_QSPI_OFFSET)

/* QSPI Offset for the OV10640 sensor */
#define CHAINS_COMMON_ISS_DCC_QSPI_OFFSET_OV10640                              \
                                        (CHAINS_COMMON_ISS_DCC_QSPI_OFFSET +   \
                                         ALGORITHM_AEWB1_DCC_IN_BUF_SIZE)

/* QSPI Offset for the IMX224 sensor */
#define CHAINS_COMMON_ISS_DCC_QSPI_OFFSET_IMX224                               \
                                        (CHAINS_COMMON_ISS_DCC_QSPI_OFFSET +   \
                                         ALGORITHM_AEWB1_DCC_IN_BUF_SIZE * 2U)

/* QSPI Offset for the AR0132 sensor */
#define CHAINS_COMMON_ISS_DCC_QSPI_OFFSET_AR0132                               \
                                        (CHAINS_COMMON_ISS_DCC_QSPI_OFFSET +   \
                                         ALGORITHM_AEWB1_DCC_IN_BUF_SIZE * 3U)

/* Size of the header in DCC bin file */
#define CHAINS_COMMON_ISS_DCC_BIN_HEADER_SIZE           (16U)

/* offset of the qSPI address in dcc bin header */
#define CHAINS_COMMON_ISS_DCC_BIN_DCC_ID_OFFSET         (2U)

/* DCC Bin file Tag/magic number */
#define CHAINS_COMMON_ISS_DCC_BIN_FILE_TAG_ID           (0x00DCCBEEU)



/*******************************************************************************
 *  Structure declaration
 *******************************************************************************
 */

/* None */

/*******************************************************************************
 *  Function's
 *******************************************************************************
 */

Void ChainsCommon_SetIssCreatePrms(
    char sensorName[],
    IssCaptureLink_CreateParams *pCapturePrm,
    IssM2mIspLink_CreateParams *pIspPrm,
    IssM2mSimcopLink_CreateParams *pSimcopPrm,
    AlgorithmLink_IssAewbCreateParams *pAlgAewbPrm,
    AlgorithmLink_SceneObstructionDetectCreateParams *pAlgSceneObstructionDetectPrm,
    UInt32 dispWidth,
    UInt32 dispHeight,
    IssM2mSimcopLink_OperatingMode simcopOpMode,
    IssM2mIspLink_OperatingMode ispOpMode,
    IssM2mIspLink_WdrOffsetParams_t *wdrOffsetPrms);

Void ChainsCommon_SetIssCapturePrms(
    IssCaptureLink_CreateParams *pCapturePrm, ChainsIssSensor_Info *pSensorInfo);

/* Function to start Sensor Devices for the ISS Usecase */
Void ChainsCommon_IssGetExposureParams(IssAewbAlgOutParams *pAewbAlgOut);
Void ChainsCommon_IssUpdateAewbParams(IssAewbAlgOutParams *pAewbAlgOut);

Void ChainsCommon_IssStopAndDeleteCaptureDevice();

Void ChainsCommon_IssStartCaptureDevice();
Void ChainsCommon_IssUpdateFlipParams(UInt32 *flipAndMirror);


Void ChainsCommon_IssGetDefaultIspSimcopConfig(
        IssM2mIspLink_OperatingMode ispOpMode,
        IssIspConfigurationParameters *pIspConfig,
        IssM2mSimcopLink_ConfigParams *pSimcopCfg,
        UInt32 linkId);

Void ChainsCommon_SetIssLdcLutConfig(vpsissldcLutCfg_t *pLdcCfg, UInt32 tableIdx);

Void ChainsCommon_SetIssSimcopConfig(
        IssM2mSimcopLink_ConfigParams *pPrm,
        Bool bypssVtnf,
        Bool bypssLdc,
        UInt32 ldcTableIdx
);
Void ChainsCommon_SetIssSimcopLdcVtnfRtConfig(
        vpsissldcConfig_t *ldcCfg,
        vpsissvtnfConfig_t *vtnfCfg,
        Bool bypssVtnf,
        Bool bypssLdc);

Void ChainsCommon_MultipleCam_UpdateIssCapturePrms(
                        char sensorName[],
                        IssCaptureLink_CreateParams *pPrm,
                        IssM2mIspLink_OperatingMode ispOpMode,
                        UInt32 width, UInt32 height,
                        Bool sensorCfgBroadcast);

/*  \brief Function to enables detection of CRC error on CSI2 interface
 *
 *  params captSrc      Source from which CAL is used to capture.
 *  params issCaptureLinkId  Capture Link identifier. Required to configure
 *                          the link to detect errors.
 *  returns none.
 */
Void ChainsCommon_SetIssCaptureErrorHandlingCb(UInt32 issCaptureLinkId);

Void ChainsCommon_SetIssIspPrms(
                        IssM2mIspLink_CreateParams *pPrm,
                        UInt16 outWidthRszA,
                        UInt16 outHeightRszA,
                        UInt16 outWidthRszB,
                        UInt16 outHeightRszB,
                        IssM2mIspLink_OperatingMode ispOpMode,
                        IssM2mIspLink_WdrOffsetParams_t *wdrOffsetPrms);
Void ChainsCommon_SetIssSimcopPrms(
                        IssM2mSimcopLink_CreateParams *pPrm,
                        IssM2mSimcopLink_OperatingMode opMode);
Void ChainsCommon_SetIssAlgAewbPrms(
                        AlgorithmLink_IssAewbCreateParams *pPrm);
Void ChainsCommon_SetIssSensorFps(UInt32 sesnorFps);
Void ChainsCommon_SetSceneObstructionDetectPrm(
    AlgorithmLink_SceneObstructionDetectCreateParams *pPrm,
    IssM2mIspLink_OperatingMode ispOpMode,
    UInt32 paxelNumH,
    UInt32 paxelNumV);
uint32_t ChainsCommon_GetLdcLutIdx(char sensorName[]);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /* end of _CHAINS_COMMON_ISS_H_ */

/* @} */

/**
 *******************************************************************************
 *
 *   \defgroup EXAMPLES_API Example code implementation
 *
 *******************************************************************************
 */
