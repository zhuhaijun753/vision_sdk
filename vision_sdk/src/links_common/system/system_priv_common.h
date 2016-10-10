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
 * \defgroup SYSTEM_IMPL   System framework implementation
 *
 * @{
 *
 *******************************************************************************
 */

/**
 *******************************************************************************
 * \file system_priv_common.h
 *
 * \brief  Header file for all system link internal APIs.
 *
 * \version 0.0 (Jun 2013) : [KC] First version taken from DVR RDK and
 *                                cleaned up for Vision_sdk
 * \version 0.1 (Jul 2013) : [HS] Commenting style update as per defined
 *                                format.
 *
 *******************************************************************************
 */

#ifndef SYSTEM_PRIV_COMMON_H_
#define SYSTEM_PRIV_COMMON_H_

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */
#include <src/utils_common/include/utils.h>
#include <src/utils_common/include/utils_prf.h>
#include <src/utils_common/include/utils_tsk.h>
#include <src/utils_common/include/utils_buf.h>
#include <src/utils_common/include/utils_mem.h>
#include <src/utils_common/include/utils_dma.h>

#include <include/link_api/system.h>
#include <include/link_api/system_common.h>
#include <include/link_api/systemLink_common.h>

#include <vps/vps_capture.h>
#include <vps/vps_displayCtrl.h>
#include <vps/vps_cfgDss.h>
#include <vps/vps_displayDss.h>
#include <platforms/bsp_platform.h>
#include <ti/xdais/xdas.h>
#include <ti/xdais/dm/xdm.h>
#include <ti/xdais/dm/ivideo.h>
#include <ti/xdais/dm/ividenc2.h>

#include <src/utils_common/src/utils_link_stats_collector.h>

/*******************************************************************************
 *  Defines
 *******************************************************************************
 */



/**
 * @def   SYSTEM_LINK_FRAMES_PER_CH
 * @brief COntrols the default number of buffers allocated per channel in each link
 */

/**
 *******************************************************************************
 *
 * \brief Macro defining default frames allocated per channel for link.
 *
 *
 *******************************************************************************
 */
#define SYSTEM_LINK_FRAMES_PER_CH   (4U)

/**
 *******************************************************************************
 *
 * \brief Macro defining max frames allocated per channel for link.
 *
 *        If user gives channels more than max it will be overridden by this
 *        number inside link.
 *
 *******************************************************************************
 */
#define SYSTEM_LINK_MAX_FRAMES_PER_CH   (16U)


/*


    Capture driver task inside HDVPSS driver is set to HIGHEST priority.

    No other task MUST be highest priority
*/

/**
 *******************************************************************************
 *
 * \brief Message Q task priority
 *
 *
 *******************************************************************************
 */
#define SYSTEM_MSGQ_TSK_PRI              (13U)

/**
 *******************************************************************************
 *
 * \brief IPC Link task priority
 *
 *
 *******************************************************************************
 */
#define SYSTEM_TSK_MULTI_MBX_TSK_PRI     (12U)

/**
 *******************************************************************************
 *
 * \brief IPC Link task priority
 *
 *
 *******************************************************************************
 */
#define IPC_LINK_TSK_PRI                 (12U)
/**
 *******************************************************************************
 *
 * \brief Dup link task priority
 *
 *
 *******************************************************************************
 */
#define DUP_LINK_TSK_PRI                 (12U)
/**
 *******************************************************************************
 *
 * \brief Gate link task priority
 *
 *
 *******************************************************************************
 */
#define GATE_LINK_TSK_PRI                (12U)
/**
 *******************************************************************************
 *
 * \brief Sync link task priority
 *
 *
 *******************************************************************************
 */
#define SYNC_LINK_TSK_PRI                (12U)
/**
 *******************************************************************************
 *
 * \brief Merge link task priority
 *
 *
 *******************************************************************************
 */

#define MERGE_LINK_TSK_PRI               (12U)
/**
 *******************************************************************************
 *
 * \brief Null link task priority
 *
 *
 *******************************************************************************
 */
#define NULL_LINK_TSK_PRI               (4U)

/**
 *******************************************************************************
 *
 * \brief Graphic Src link task priority
 *
 *
 *******************************************************************************
 */
#define GRPX_SRC_LINK_TSK_PRI               (1U)

/**
 *******************************************************************************
 *
 * \brief capture link task priority
 *
 *
 *******************************************************************************
 */

#define CAPTURE_LINK_TSK_PRI             (10U)

/**
 *******************************************************************************
 *
 * \brief Iss capture link task priority
 *
 *
 *******************************************************************************
 */

#define ISSCAPTURE_LINK_TSK_PRI             (10U)

/**
 *******************************************************************************
 *
 * \brief Iss m2m isp link task priority
 *
 *
 *******************************************************************************
 */

#define ISSM2MISP_LINK_TSK_PRI             (10U)

/**
 *******************************************************************************
 *
 * \brief Iss simcop isp link task priority
 *
 *
 *******************************************************************************
 */

#define ISSM2MSIMCOP_LINK_TSK_PRI             (10U)

/**
 *******************************************************************************
 *
 * \brief AvbRx link task priority
 *
 *
 *******************************************************************************
 */
#define AVB_RX_LINK_TSK_PRI             (10U)

/**
 *******************************************************************************
 *
 * \brief VPE link task priority
 *
 *
 *******************************************************************************
 */
#define VPE_LINK_TSK_PRI                 (8U)
/**
 *******************************************************************************
 *
 * \brief Software Mosaic link task priority
 *
 *
 *******************************************************************************
 */
#define SW_MS_LINK_TSK_PRI               (8U)
/**
 *******************************************************************************
 *
 * \brief Select link task priority
 *
 *
 *******************************************************************************
 */
#define SELECT_LINK_TSK_PRI              (12U)

/**
 *******************************************************************************
 *
 * \brief Null Source link task priority
 *
 *
 *******************************************************************************
 */
#define NULL_SRC_LINK_TSK_PRI               (10U)

/**
 *******************************************************************************
 *
 * \brief Display link task priority
 *
 *
 *******************************************************************************
 */
#define DISPLAY_LINK_TSK_PRI             (9U)
/**
 *******************************************************************************
 *
 * \brief Display controller link task priority
 *
 *
 *******************************************************************************
 */
#define DISPLAYCTRL_LINK_TSK_PRI         (4U)
/**
 *******************************************************************************
 *
 * \brief System task priority
 *
 *
 *******************************************************************************
 */
#ifdef SRV_FAST_BOOT_INCLUDE
    /*
     *  For SRV fast boot there is a thread created for doing the DSP load
        in parallel whose priority should be lower than the system task.
        And priority of 1 is the least.
     */
    #define SYSTEM_TSK_PRI                   (2U)
#else
    #define SYSTEM_TSK_PRI                   (1U)
#endif

/**
 *******************************************************************************
 *
 * \brief Algorithm link task priority
 *
 *
 *******************************************************************************
 */
#define ALGORITHM_LINK_TSK_PRI             (2U)

/**
 *******************************************************************************
 *
 * \brief Remote Debug client task priority
 *
 *
 *******************************************************************************
 */
#define REMOTE_LOG_CLIENT_TSK_PRI      (1U)

/**
 *******************************************************************************
 *
 * \brief DEC link task priority
 *
 *
 *******************************************************************************
 */
#define DEC_LINK_TSK_PRI                 (8U)

/**
 *******************************************************************************
 *
 * \brief ENC link task priority
 *
 *
 *******************************************************************************
 */
#define ENC_LINK_TSK_PRI                 (7U)

/**
 *******************************************************************************
 *
 * \brief Task size for links using large stack like capture and display links.
 *
 *******************************************************************************
 */
#define SYSTEM_TSK_STACK_SIZE_LARGE      (32U*KB)
/**
 *******************************************************************************
 *
 * \brief Task size for links using medium stack
 *
 *******************************************************************************
 */
#define SYSTEM_TSK_STACK_SIZE_MEDIUM     (16U*KB)
/**
 *******************************************************************************
 *
 * \brief Task size for links using small stack like select link
 *
 *******************************************************************************
 */
#define SYSTEM_TSK_STACK_SIZE_SMALL      ( 8U*KB)

/**
 *******************************************************************************
 *
 * \brief Max task size
 *
 *******************************************************************************
 */
#define SYSTEM_TSK_STACK_SIZE_MAX        (SYSTEM_TSK_STACK_SIZE_LARGE)

/**
 *******************************************************************************
 *
 * \brief Default task stack size
 *
 *******************************************************************************
 */
#define SYSTEM_DEFAULT_TSK_STACK_SIZE    (SYSTEM_TSK_STACK_SIZE_LARGE)

/**
 *******************************************************************************
 *
 * \brief Task size for system task
 *
 *******************************************************************************
 */
#define SYSTEM_TSK_STACK_SIZE            (SYSTEM_DEFAULT_TSK_STACK_SIZE)
/**
 *******************************************************************************
 *
 * \brief Task size for capture link task
 *
 *******************************************************************************
 */
#define CAPTURE_LINK_TSK_STACK_SIZE      (SYSTEM_DEFAULT_TSK_STACK_SIZE)

/**
 *******************************************************************************
 *
 * \brief Task stack size for ISS capture link task
 *
 *******************************************************************************
 */
#define ISSCAPTURE_LINK_TSK_STACK_SIZE      (SYSTEM_DEFAULT_TSK_STACK_SIZE)

/**
 *******************************************************************************
 *
 * \brief Task stack size for ISS M2M ISP link task
 *
 *******************************************************************************
 */
#define ISSM2MISP_LINK_TSK_STACK_SIZE      (SYSTEM_DEFAULT_TSK_STACK_SIZE)

/**
 *******************************************************************************
 *
 * \brief Task size for AvbRx link task
 *
 *******************************************************************************
 */
#define AVB_RX_LINK_TSK_STACK_SIZE      (SYSTEM_DEFAULT_TSK_STACK_SIZE)

/**
 *******************************************************************************
 *
 * \brief Task size for VPE link task
 *
 *******************************************************************************
 */
#define VPE_LINK_TSK_STACK_SIZE          (SYSTEM_DEFAULT_TSK_STACK_SIZE)
/**
 *******************************************************************************
 *
 * \brief Task size for Display link task
 *
 *******************************************************************************
 */
#define DISPLAY_LINK_TSK_STACK_SIZE      (SYSTEM_DEFAULT_TSK_STACK_SIZE)
/**
 *******************************************************************************
 *
 * \brief Task size for Display controller link task
 *
 *******************************************************************************
 */
#define DISPLAYCTRL_LINK_TSK_STACK_SIZE  (SYSTEM_DEFAULT_TSK_STACK_SIZE)
/**
 *******************************************************************************
 *
 * \brief Task size for Software mosaic link task
 *
 *******************************************************************************
 */
#define SW_MS_LINK_TSK_STACK_SIZE        (SYSTEM_DEFAULT_TSK_STACK_SIZE)

/**
 *******************************************************************************
 *
 * \brief Task size for Duplicate link task
 *
 *******************************************************************************
 */
#define DUP_LINK_TSK_STACK_SIZE          (SYSTEM_DEFAULT_TSK_STACK_SIZE)
/**
 *******************************************************************************
 *
 * \brief Task size for Gate link task
 *
 *******************************************************************************
 */
#define GATE_LINK_TSK_STACK_SIZE          (SYSTEM_DEFAULT_TSK_STACK_SIZE)
/**
 *******************************************************************************
 *
 * \brief Task size for Duplicate link task
 *
 *******************************************************************************
 */
#define SYNC_LINK_TSK_STACK_SIZE          (SYSTEM_DEFAULT_TSK_STACK_SIZE)
/**
 *******************************************************************************
 *
 * \brief Task size for merge link task
 *
 *******************************************************************************
 */
#define MERGE_LINK_TSK_STACK_SIZE        (SYSTEM_DEFAULT_TSK_STACK_SIZE)
/**
 *******************************************************************************
 *
 * \brief Task size for MessageQ link task
 *
 *******************************************************************************
 */
#define SYSTEM_MSGQ_TSK_STACK_SIZE       (SYSTEM_DEFAULT_TSK_STACK_SIZE)
/**
 *******************************************************************************
 *
 * \brief Task size for Select link task
 *
 *******************************************************************************
 */
#define SELECT_LINK_TSK_STACK_SIZE       (SYSTEM_TSK_STACK_SIZE_SMALL)
/**
 *******************************************************************************
 *
 * \brief Task size for Graphic link task
 *
 *******************************************************************************
 */
#define GRPX_SRC_LINK_TSK_STACK_SIZE         (SYSTEM_TSK_STACK_SIZE_SMALL)
/**
 *******************************************************************************
 *
 * \brief Task size for Null link task
 *
 *******************************************************************************
 */
#define NULL_LINK_TSK_STACK_SIZE         (SYSTEM_TSK_STACK_SIZE_SMALL)
/**
 * \brief Task size for remote debug client task
 *
 *******************************************************************************
 */
#define REMOTE_LOG_CLIENT_TSK_STACK_SIZE      (SYSTEM_DEFAULT_TSK_STACK_SIZE)
/**
 *******************************************************************************
 *
 * \brief Task size for Null Source link task
 *
 *******************************************************************************
 */
#define NULL_SRC_LINK_TSK_STACK_SIZE      (SYSTEM_DEFAULT_TSK_STACK_SIZE)
/**
 *******************************************************************************
 *
 * \brief Task size for Decode link task
 *
 *******************************************************************************
 */
#define DEC_LINK_TSK_STACK_SIZE      (SYSTEM_DEFAULT_TSK_STACK_SIZE)
/**
 *******************************************************************************
 *
 * \brief Task size for Encode link task
 *
 *******************************************************************************
 */
#define ENC_LINK_TSK_STACK_SIZE      (SYSTEM_DEFAULT_TSK_STACK_SIZE)


/**
 *******************************************************************************
 * \brief Link state: Idle, i.e not created state.
 *                    At system init or after 'delete'
 *******************************************************************************
 */
#define SYSTEM_LINK_STATE_IDLE          (0U)

/**
 *******************************************************************************
 * \brief Link state: After 'create' but before 'start'
 *******************************************************************************
 */
#define SYSTEM_LINK_STATE_CREATED       (1U)

/**
 *******************************************************************************
 * \brief Link state: After 'create' and 'start'
 *******************************************************************************
 */
#define SYSTEM_LINK_STATE_RUNNING       (2U)

/**
 *******************************************************************************
 * \brief Link state: After 'stop'
 *******************************************************************************
 */
#define SYSTEM_LINK_STATE_STOPPED       (3U)

/**
 *******************************************************************************
 * \brief Core state: Core is active and ready to recieve IPC messages
 *******************************************************************************
 */
#define SYSTEM_CORE_STATE_ACTIVE        (0U)

/**
 *******************************************************************************
 * \brief Core state: Core disabled due to an RTI Expiry
 *******************************************************************************
 */
#define SYSTEM_CORE_STATE_RTI_EXPIRED   (1U)

/**
 *******************************************************************************
 * \brief Core state: Core powered down for power management
 *******************************************************************************
 */
#define SYSTEM_CORE_STATE_POWER_DOWN    (2U)

/*******************************************************************************
 *  Data structure's
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 *  \brief  System link structure with information of all links
 *
 *******************************************************************************
*/
typedef struct {
    Utils_MbxHndl mbx;
    /**< Hanle to mail box */
    BspOsal_TaskHandle tsk;
    /**< Handle to system link task */
    Task_FuncPtr chainsMainFunc;
    /**< Function pointer for app start function */
    System_LinkObj linkObj[SYSTEM_LINK_ID_MAX];
    /**< Array of link objects for storing link information of all links */
    SystemLink_CmdHandler systemLinkCmdHandler[SYSTEM_COMMON_MAX_CMD_HANDLERS];
    /**< Handlers for system link command's */
    UInt32 coreState[SYSTEM_PROC_MAX];
    /**< Maintain status of all the cores in the system.
     * Framework only maintains the state. It does not detect it.
     * State is queried using System_getCoreState(procId)
     * State can be set using System_setCoreState(procId, status)
     */
} System_CommonObj;

/**
 *******************************************************************************
 * \brief System object extern declaration
 *******************************************************************************
 */
extern System_CommonObj gSystem_objCommon;




Int32 System_initCommon(void);
Int32 System_deInitCommon(void);

Void System_initLinks(void);
Void System_deInitLinks(void);

Utils_TskHndl *System_getLinkTskHndl(UInt32 linkId);

void System_memPrintHeapStatus(void);

void System_enumAssertCheck(void);

Int32 System_linkControl_local(UInt32 linkId, UInt32 cmd, Void * pPrm,
                               UInt32 prmSize, Bool waitAck);
Int32 System_sendLinkCmd_local(UInt32 linkId, UInt32 cmd, Void *payload);
Int32 System_linkGetInfo_local(UInt32 linkId, System_LinkInfo * info);

Int32 System_linkControl_remote(UInt32 linkId, UInt32 cmd, Void * pPrm,
                                UInt32 prmSize, Bool waitAck);

Int32 SystemLink_init(void);
Int32 SystemLink_deInit(void);
Void  SystemLink_userCmdHandler(UInt32 cmd, Void * pPrm);
Int32 System_start(Task_FuncPtr chainsMainFunc);

Void System_triggerAppInit(void);
Void System_triggerAppInitComplete(void);
Void System_triggerAppExit(void);
Void System_triggerAppExitComplete(void);
Void System_waitAppInitTrigger(void);
Void System_waitAppInitComplete(void);

UInt32 System_getCoreState(UInt32 procId);

/* Set status = SYSTEM_CORE_STATE_ACTIVE when active
 * Set status = SYSTEM_CORE_STATE_RTI_EXPIRED when the RTI timer expires
 * Set status = SYSTEM_CORE_STATE_POWER_DOWN when a request to power down
 *              the core occurs.
 * If any other value given then the status is set to default of
 * SYSTEM_CORE_STATE_POWER_DOWN.
 */
Void System_setCoreState(UInt32 procId, UInt32 status);

Utils_TskMultiMbxHndl *System_getTskMultiMbxHndl(void);


#endif

/* @} */
