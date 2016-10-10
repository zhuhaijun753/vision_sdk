/*
 *******************************************************************************
 *
 * Copyright (C) 2015 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */



#ifndef DATA_VIS_H_
#define DATA_VIS_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *  Include files
 *******************************************************************************
 */

#include <include/link_api/system.h>
#include <examples/tda2xx/include/draw2d.h>

/*******************************************************************************
 *  Defines's
 *******************************************************************************
 */

#define DATA_VIS_DATA_BW_INVALID    (UInt32)(0xFFFFFFFFu)

/**
 *******************************************************************************
 * \brief Max number of modules to group together in one graph
 *******************************************************************************
 */
#define DATA_VIS_DATA_BW_GROUP_MAX  (16)

/*******************************************************************************
 *  Enum's
 *******************************************************************************
 */
#ifdef TDA3XX_BUILD
typedef enum {
    DATA_VIS_DATA_BW_MOD_EMIF1_RD_WR,
    DATA_VIS_DATA_BW_MOD_IPU1,
    DATA_VIS_DATA_BW_MOD_DSP1_MDMA,
    DATA_VIS_DATA_BW_MOD_DSP2_MDMA,
    DATA_VIS_DATA_BW_MOD_DSS,
    DATA_VIS_DATA_BW_MOD_MMU,
    DATA_VIS_DATA_BW_MOD_EDMA_TC0_RD,
    DATA_VIS_DATA_BW_MOD_EDMA_TC0_WR,
    DATA_VIS_DATA_BW_MOD_VIP1_P1,
    DATA_VIS_DATA_BW_MOD_VIP1_P2,
    DATA_VIS_DATA_BW_MOD_ISS_RT,
    DATA_VIS_DATA_BW_MOD_ISS_NRT2,
    DATA_VIS_DATA_BW_MOD_EVE1_P1,
    DATA_VIS_DATA_BW_MOD_EVE1_P2,
    DATA_VIS_DATA_BW_MOD_GMAC_SW,
    DATA_VIS_DATA_BW_MOD_ISS_NRT1,
    DATA_VIS_DATA_BW_MOD_DSP2_CFG,
    DATA_VIS_DATA_BW_MOD_DSP2_EDMA,
    DATA_VIS_DATA_BW_MOD_OCMC_RAM,
    DATA_VIS_DATA_BW_MOD_DSP1_CFG,
    DATA_VIS_DATA_BW_MOD_DSP1_EDMA,
    DATA_VIS_DATA_BW_MOD_GPMC,
    DATA_VIS_DATA_BW_MOD_MCASP,
    DATA_VIS_DATA_BW_MOD_EDMA_TC1_RD,
    DATA_VIS_DATA_BW_MOD_EDMA_TC1_WR,
    DATA_VIS_DATA_BW_MOD_MAX
} DataVis_DataBwModule;
#elif defined(TDA2XX_BUILD)
typedef enum {

    DATA_VIS_DATA_BW_MOD_EMIF1_RD_WR,
    DATA_VIS_DATA_BW_MOD_EMIF2_RD_WR,
    DATA_VIS_DATA_BW_MOD_EMIF1_RD_ONLY,
    DATA_VIS_DATA_BW_MOD_EMIF1_WR_ONLY,
    DATA_VIS_DATA_BW_MOD_EMIF2_RD_ONLY,
    DATA_VIS_DATA_BW_MOD_EMIF2_WR_ONLY,
    DATA_VIS_DATA_BW_MOD_MA_MPU_P1,
    DATA_VIS_DATA_BW_MOD_MA_MPU_P2,
    DATA_VIS_DATA_BW_MOD_DSS,
    DATA_VIS_DATA_BW_MOD_IPU1,
    DATA_VIS_DATA_BW_MOD_VIP1_P1,
    DATA_VIS_DATA_BW_MOD_VIP1_P2,
    DATA_VIS_DATA_BW_MOD_VPE_P1,
    DATA_VIS_DATA_BW_MOD_VPE_P2,
    DATA_VIS_DATA_BW_MOD_DSP1_MDMA,
    DATA_VIS_DATA_BW_MOD_DSP1_EDMA,
    DATA_VIS_DATA_BW_MOD_DSP2_MDMA,
    DATA_VIS_DATA_BW_MOD_DSP2_EDMA,
    DATA_VIS_DATA_BW_MOD_EVE1_TC0,
    DATA_VIS_DATA_BW_MOD_EVE1_TC1,
    DATA_VIS_DATA_BW_MOD_EVE2_TC0,
    DATA_VIS_DATA_BW_MOD_EVE2_TC1,
    DATA_VIS_DATA_BW_MOD_EDMA_TC0_RD,
    DATA_VIS_DATA_BW_MOD_EDMA_TC0_WR,
    DATA_VIS_DATA_BW_MOD_EDMA_TC1_RD,
    DATA_VIS_DATA_BW_MOD_EDMA_TC1_WR,
    DATA_VIS_DATA_BW_MOD_VIP2_P1,
    DATA_VIS_DATA_BW_MOD_VIP2_P2,
    DATA_VIS_DATA_BW_MOD_VIP3_P1,
    DATA_VIS_DATA_BW_MOD_VIP3_P2,
    DATA_VIS_DATA_BW_MOD_EVE3_TC0,
    DATA_VIS_DATA_BW_MOD_EVE3_TC1,
    DATA_VIS_DATA_BW_MOD_EVE4_TC0,
    DATA_VIS_DATA_BW_MOD_EVE4_TC1,
    DATA_VIS_DATA_BW_MOD_IVA,
    DATA_VIS_DATA_BW_MOD_GPU_P1,
    DATA_VIS_DATA_BW_MOD_GPU_P2,
    DATA_VIS_DATA_BW_MOD_GMAC_SW,
    DATA_VIS_DATA_BW_MOD_MAX

} DataVis_DataBwModule;
#elif defined(TDA2EX_BUILD)
typedef enum {

    DATA_VIS_DATA_BW_MOD_EMIF1_RD_WR,
    DATA_VIS_DATA_BW_MOD_EMIF1_RD_ONLY,
    DATA_VIS_DATA_BW_MOD_EMIF1_WR_ONLY,
    DATA_VIS_DATA_BW_MOD_MA_MPU_P1,
    DATA_VIS_DATA_BW_MOD_MA_MPU_P2,
    DATA_VIS_DATA_BW_MOD_DSS,
    DATA_VIS_DATA_BW_MOD_IPU1,
    DATA_VIS_DATA_BW_MOD_VIP1_P1,
    DATA_VIS_DATA_BW_MOD_VIP1_P2,
    DATA_VIS_DATA_BW_MOD_VPE_P1,
    DATA_VIS_DATA_BW_MOD_VPE_P2,
    DATA_VIS_DATA_BW_MOD_DSP1_MDMA,
    DATA_VIS_DATA_BW_MOD_DSP1_EDMA,
    DATA_VIS_DATA_BW_MOD_EDMA_TC0_RD,
    DATA_VIS_DATA_BW_MOD_EDMA_TC0_WR,
    DATA_VIS_DATA_BW_MOD_EDMA_TC1_RD,
    DATA_VIS_DATA_BW_MOD_EDMA_TC1_WR,
    DATA_VIS_DATA_BW_MOD_CAL,
    DATA_VIS_DATA_BW_MOD_IVA,
    DATA_VIS_DATA_BW_MOD_GPU_P1,
    DATA_VIS_DATA_BW_MOD_GPU_P2,
    DATA_VIS_DATA_BW_MOD_GMAC_SW,
    DATA_VIS_DATA_BW_MOD_MAX

} DataVis_DataBwModule;
#endif

/*******************************************************************************
 *  Data structure's
 *******************************************************************************
 */

/*******************************************************************************
 *  \brief Represents one sample point of Data BW
 *
 *         Stores average Data BW in MB/s for a sample dureation
 *         specfied in DataVis_DataBwPrm
 *******************************************************************************
 */
typedef struct {

    UInt64 timestamp;
    /**< In units of usecs */

    UInt32 dataBw[DATA_VIS_DATA_BW_MOD_MAX];
    /**< if value is DATA_VIS_DATA_BW_INVALID, value should be ignored
      *  else value is units of MB/s
      */
} DataVis_DataBwElement;

/*******************************************************************************
 *  \brief Create time arguments
 *
 *         Specifies max limits which are used for resource allocation
 *         These parameters cannot be changed dynamically.
 *         When parameters are updated dynamically they should not exceed
 *         values set in this structure
 *******************************************************************************
 */
typedef struct {

    UInt32 maxWinWidth;
    /**< Max possible width of drawing window */

    UInt32 maxWinHeight;
    /**< Max possible height of drawing window */

    UInt32 maxNumberOfSamples;
    /**< Max possible number of samples */

} DataVis_CreatePrm;

/*******************************************************************************
 *  \brief Drawing properties to use for visualization of a data element
 *******************************************************************************
 */
typedef struct {

    UInt32 numModules;

    DataVis_DataBwModule moduleId[DATA_VIS_DATA_BW_GROUP_MAX];
    /**< ID of module */

    char dataBwModText[16];
    /**< Text to display for this data BW module */

    UInt32 color;
    /**< Color to use for drawing this data BW module graph
     *   Color format: RGB565
     */

} DataVis_DataBwModDrawProp;

/*******************************************************************************
 *  \brief Run-time arguments to control data visualization function
 *
 *         These parameters can be changed while data visualization is
 *         running
 *******************************************************************************
 */
typedef struct {

    UInt32 sampleDuration;
    /**< In units of msecs */
    UInt32 numOfSamples;
    /**<  Must be <= value specified in DataVis_CreatePrm */
    UInt32 drawRefreshRate;
    /**< In units of msecs */
    float yScaleMin;
    /**< just a number used to annotate the scale from min to max */
    float yScaleMax;
    /**< just a number used to annotate the scale from min to max */
    float yScaleMajorInc;
    /**< units by which to increment Scale */
    float yScaleMinorInc;
    /**< units by which to increment Scale */
    float xScaleMin;
    /**< just a number used to annotate the scale from min to max */
    float xScaleMax;
    /**< just a number used to annotate the scale from min to max */
    float xScaleMajorInc;
    /**< units by which to increment Scale */
    float xScaleMinorInc;
    /**< units by which to increment Scale */
    char  xScaleText[16];
    /**< Text to draw for scale in X-direction */
    char  yScaleText[16];
    /**< Text to draw for scale in Y-direction */
    UInt32 numDataBwMod;
    /**< Number of data BW modules that will be drawn */
    DataVis_DataBwModDrawProp dataBwModProp[DATA_VIS_DATA_BW_MOD_MAX];
    /**< Properties of data Bw module that would be used for drawing */
    UInt32 winWidth;
    /**< Must be <= value specified in DataVis_CreatePrm */
    UInt32 winHeight;
    /**< Must be <= value specified in DataVis_CreatePrm */
    UInt32 winPosX;
    /**< Position on window in display */
    UInt32 winPosY;
    /**< Position on window in display */
    Draw2D_Handle *pDraw2DHandle;
    /**< Handle of created and initialized drawing surface */
} DataVis_DataBwPrm;

/*******************************************************************************
 *  Function's
 *******************************************************************************
 */

/*******************************************************************************
 *  \brief Set default parameters
 *******************************************************************************
 */
Void DataVis_CreatePrm_Init(DataVis_CreatePrm *pPrm);

/*******************************************************************************
 *  \brief Set default parameters
 *******************************************************************************
 */
Void DataVis_DataBwPrm_Init(DataVis_DataBwPrm *pPrm);

/*******************************************************************************
 *  \brief Create resources for Data visualization
 *
 *         Called once during use-case startup
 *******************************************************************************
 */
Int32 DataVis_create(DataVis_CreatePrm *pPrm);

/*******************************************************************************
 *  \brief Free's resources allocated during create
 *
 *         Called once during use-case shutdown
 *******************************************************************************
 */
Int32 DataVis_delete();

/*******************************************************************************
 *  \brief Change drawing related parameters
 *
 *         Can be called multiple times while use-case is running
 *         Effect takes place, next time DataVis_drawDataBw() is called
 *         or next time when data collection is done
 *
 *         'resetDataCollector' is used to reset data collector thread
 *         'resetDraw' is used to reset drawing function
 *
 *******************************************************************************
 */
Int32 DataVis_setDataBwPrm(DataVis_DataBwPrm *pPrm, Bool resetDataCollector, Bool resetDraw);

/*******************************************************************************
 *  \brief Check if updated parameters should be used for drawing
 *
 *         Called DataVis_drawDataBw() to reset itself based on new parameters
 *         'pPrm' contains updated value only when return value is true
 *
 *******************************************************************************
 */
Bool DataVis_isPrmUpdatedForDataBwDraw(DataVis_DataBwPrm *pPrm);

/*******************************************************************************
 *  \brief Check if updated parameters should be used by data collection thread
 *
 *         Called data collection thread to reset itself based on new parameters
 *         'pPrm' contains updated value only when return value is true
 *
 *******************************************************************************
 */
Bool DataVis_isPrmUpdatedForDataBWCollector(DataVis_DataBwPrm *pPrm);

/*******************************************************************************
 *  \brief Draw Data BW visualization to screen
 *
 *         Should be called periodically, say once every 10ms
 *         by the drawing thread.
 *         Actual drawing takes a 'referesh rate' specified during
 *         DataVis_setDrawDataBwPrm()
 *
 *******************************************************************************
 */
Int32 DataVis_drawDataBw();

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
Int32 DataVis_writeDataBwData(DataVis_DataBwElement *pElem);

/*******************************************************************************
 *  \brief Read element's of data BW collected in shared memory for drawing
 *         purpose
 *
 *         Called by the DataVis_drawDataBw() internally
 *
 *******************************************************************************
 */
Int32 DataVis_readDataBwData(DataVis_DataBwElement *pElem, UInt32 *numElementsRead, UInt32 maxElementsToRead);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/* @} */


