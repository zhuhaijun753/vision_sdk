/*
 * Copyright (c) 2012-2013, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <linux/examples/tda2xx/include/chains.h>


Chains_Ctrl gChains_usecaseCfg;

/**
 *******************************************************************************
 * \brief Run Time Test Menu string.
 *******************************************************************************
 */
char gChains_testMenu[] = {
    "\r\n "
    "\r\n ========="
    "\r\n Test Menu"
    "\r\n ========="
    "\r\n "
    "\r\n Single Camera Use-cases,"
    "\r\n ------------------------"
    "\r\n 1: 1CH VIP capture + SGX Copy + DISPLAY"
#ifndef IPUMM_INCLUDE
    "\r\n 2: 1CH VIP capture + Encode + Decode + SGX Copy + DISPLAY"
#endif
    "\r\n 3: 1CH VIP capture + Alg FrameCopy (EVE1) + SGX Copy + DISPLAY - Supported only on TDA2xx"
#ifdef FPD_DISPLAY
    "\r\n 4: 1CH VIP capture + SGX Copy + DUAL Display(HDMI & FPD-Out)"
#endif
#ifndef IPUMM_INCLUDE
    "\r\n 5: NullSrc + Decode + Display (Only 1920x1080 H264/MJPEG Video Input Bit-Stream Supported)"
#endif
    "\r\n 6: 1CH VIP capture + Alg Frame Copy (A15) + SGX Copy + DISPLAY"
    "\r\n 7: 1CH VIP + Alg Frame Copy (A15) + Connetor Links (Dup, Merge, Select, Gate on A15) + SGX Copy + DISPLAY"
#ifdef ENABLE_OPENCV
    "\r\n 8: 1CH VIP capture + OpenCV Canny (A15) + SGX Copy + DISPLAY"
#endif
    "\r\n "
    "\r\n Multi-Camera LVDS Use-cases,"
    "\r\n ----------------------------"
    "\r\n a: 4CH VIP LVDS capture + SGX MOSAIC + DISPLAY"
    "\r\n b: 4CH VIP LVDS capture + 3D SRV (SGX/A15) + DISPLAY - Only HDMI 1080p display supported"
    "\r\n c: 4CH VIP LVDS capture + 3D SRV + 4CH SfM (3D perception demo - EVE1-4/DSP1&2) + DISPLAY - Only on TDA2xx with HDMI 1080p display"
    "\r\n d: 2CH OV490 2560x720 capture + Split + 3D SRV (SGX/A15) + DISPLAY - Only HDMI 1080p display supported"
    "\r\n e: Surround View Calibration"
    "\r\n "
    "\r\n AVB RX Use-cases, (TDA2x ONLY)"
    "\r\n ------------------------------"
    "\r\n f: 4CH AVB Capture + Decode + SGX MOSAIC + DISPLAY"
    "\r\n "
    "\r\n p: CPU Status"
    "\r\n i: Show IP Addr (IPU + NDK + AVB) "
    "\r\n "
    "\r\n s: System Settings "
    "\r\n "
    "\r\n x: Exit "
    "\r\n z: Exit - AND SHUTDOWN Remote CPUs"
    "\r\n "
    "\r\n Enter Choice: "
    "\r\n "
};


/**
 *******************************************************************************
 * \brief Menu for capture settings.
 *******************************************************************************
 */
char gChains_menuCaptureSrc[] = {
    "\r\n "
    "\r\n =============="
    "\r\n Capture Source"
    "\r\n =============="
    "\r\n "
    "\r\n 1: OV10635 Sensor 720P30"
    "\r\n 2: HDMI Capture 1080P60 "
    "\r\n "
    "\r\n x: Exit "
    "\r\n "
    "\r\n Enter Choice: "
    "\r\n "
};

/**
 *******************************************************************************
 * \brief Menu for system settings.
 *******************************************************************************
 */
 char gChains_menuSystemSettings[] = {
    "\r\n "
    "\r\n ==============="
    "\r\n System Settings"
    "\r\n ==============="
    "\r\n "
    "\r\n 1: Display Settings"
    "\r\n 2: Capture Settings"
    "\r\n "
    "\r\n x: Exit "
    "\r\n "
    "\r\n Enter Choice: "
    "\r\n "
};

/**
 *******************************************************************************
 * \brief Menu for display settings.
 *******************************************************************************
 */
char gChains_menuDisplaySettings[] = {
    "\r\n "
    "\r\n ================"
    "\r\n Display Settings"
    "\r\n ================"
    "\r\n "
#ifdef LCD_DISPLAY
    "\r\n 1: LCD  7-inch 800x480@60fps"
#endif
    "\r\n 2: HDMI 1080P60 "
#ifdef FPD_DISPLAY
    "\r\n 3: FPD Out "
#endif
    "\r\n "
    "\r\n x: Exit "
    "\r\n "
    "\r\n Enter Choice: "
    "\r\n "
};

/**
 *******************************************************************************
 * \brief Run Time Menu string.
 *******************************************************************************
 */
char gChains_runTimeMenu[] = {
    "\r\n "
    "\r\n ===================="
    "\r\n Chains Run-time Menu"
    "\r\n ===================="
    "\r\n "
    "\r\n 0: Stop Chain"
    "\r\n "
    "\r\n p: Print Performance Statistics "
    "\r\n "
    "\r\n Enter Choice: "
    "\r\n "
};

/**
 *******************************************************************************
 * \brief Menu for TDA2X 3D SRV Auto Calibration.
 *******************************************************************************
 */
 char gChains_menu3DSrvAutoCalibration[] = {
    "\r\n "
    "\r\n ======================="
    "\r\n 3D SRV Auto Calibration"
    "\r\n ======================="
    "\r\n "
    "\r\n a: Auto Calibration"
    "\r\n b: Calibration with USER Cal Mat"
    "\r\n c: Calibration with DEFAULT Cal Mat"
    "\r\n "
    "\r\n d: Dump YUV frames for all the cameras"
    "\r\n "
    "\r\n p: Print Performance Statistics "
    "\r\n "
    "\r\n x: Exit "
    "\r\n "
    "\r\n Enter Choice: "
    "\r\n "
};

/**
 *******************************************************************************
 *
 * \brief   Function to set display settings.
 *
 *******************************************************************************
*/
Void Chains_showDisplaySettingsMenu()
{
    char ch;
    Bool displaySelectDone;
    displaySelectDone = FALSE;

    do
    {
        Vps_printf(gChains_menuDisplaySettings);
        Vps_printf(" \r\n");
        ch = getchar();

        switch(ch)
        {
#ifdef LCD_DISPLAY
            case '1':
                gChains_usecaseCfg.displayType = CHAINS_DISPLAY_TYPE_LCD_7_INCH;
                displaySelectDone = TRUE;
                break;
#endif
            case '2':
                gChains_usecaseCfg.displayType = CHAINS_DISPLAY_TYPE_HDMI_1080P;
                displaySelectDone = TRUE;
                break;
#ifdef FPD_DISPLAY
            case '3':
                gChains_usecaseCfg.displayType = CHAINS_DISPLAY_TYPE_FPD;
                displaySelectDone = TRUE;
                break;
#endif
            case 'x':
            case 'X':
                displaySelectDone = TRUE;
                break;
            default:
                Vps_printf(" Unsupported option '%c'. Please try again\n", ch);
                break;
        }

    }while(displaySelectDone == FALSE);
}

/**
 *******************************************************************************
 *
 * \brief   Function to set capture settings.
 *
 *******************************************************************************
*/
Void Chains_showCaptureSettingsMenu()
{
    char ch;
    Bool captSrcSelectDone;
    captSrcSelectDone = FALSE;

    do
    {
        Vps_printf(gChains_menuCaptureSrc);
        Vps_printf(" \r\n");
        ch = Chains_readChar();
        switch(ch)
        {
            case '1':
                gChains_usecaseCfg.captureSrc = CHAINS_CAPTURE_SRC_OV10635;
                captSrcSelectDone = TRUE;
                break;
            case '2':
                gChains_usecaseCfg.captureSrc = CHAINS_CAPTURE_SRC_HDMI_1080P;
                captSrcSelectDone = TRUE;
                break;
            case 'x':
            case 'X':
                captSrcSelectDone = TRUE;
                break;
            default:
                Vps_printf(" Unsupported option '%c'. Please try again\n", ch);
                break;
        }

    }while(captSrcSelectDone == FALSE);
}
/**
 *******************************************************************************
 *
 * \brief   Run time Menu selection
 *
 *          This functions displays the run time options available
 *          And receives user input and calls corrosponding functions run time
 *          Instrumentation logs are printing routine is called in same function
 *
 *******************************************************************************
*/
char Chains_menuRunTime()
{
    Vps_printf(gChains_runTimeMenu);

    return Chains_readChar();
}

/**
 *******************************************************************************
 *
 * \brief   Function to select systems settings option.
 *
 *******************************************************************************
*/
Void Chains_showSystemSettingsMenu()
{
    char ch;
    Bool done;
    done = FALSE;

    do
    {
        Vps_printf(gChains_menuSystemSettings);
        Vps_printf(" \r\n");
        ch = Chains_readChar();

        switch(ch)
        {
            case '1':
                Chains_showDisplaySettingsMenu();
                done = TRUE;
                break;
            case '2':
                Chains_showCaptureSettingsMenu();
                done = TRUE;
                break;
            case 'x':
            case 'X':
                done = TRUE;
                break;
            default:
                Vps_printf(" Unsupported option '%c'. Please try again\n", ch);
                break;
        }
    }while(!done);
}

/**
 *******************************************************************************
 *
 * \brief   Function to select systems settings option.
 *
 *******************************************************************************
*/
Void Chains_3DSrvAutoCalibMenu()
{
    char ch;
    Bool done;
    done = FALSE;

    /* Create Auto Calibration use case */
    Chain_Common_SRV_3DAutoCalibrationCreate();

    do
    {
        Vps_printf(gChains_menu3DSrvAutoCalibration);
        Vps_printf(" \r\n");
        ch = Chains_readChar();

        switch(ch)
        {
            case 'a':
            case 'A':
                Chain_Common_SRV_3DAutoCalibrationProcess();
                break;
            case 'b':
            case 'B':
                Chain_Common_SRV_3DUserCalMatCalibrationProcess();
                break;
            case 'c':
            case 'C':
                Chain_Common_SRV_3DDefaultCalMatCalibrationProcess();
                break;
            case 'd':
            case 'D':
                Chain_Common_SRV_3DAutoCalibrationDumpframes();
                break;
            case 'p':
            case 'P':
                Chain_Common_SRV_3DAutoCalibrationPrintStats();
                break;

            case 'x':
            case 'X':
                done = TRUE;
                break;
            default:
                Vps_printf(" Unsupported option '%c'. Please try again\n", ch);
                break;
        }

    }while(!done);

    /* Delete Auto Calibration use case */
    Chain_Common_SRV_3DAutoCalibrationDelete();
}

/**
 *******************************************************************************
 *
 * \brief   Application's main - entry function
 *
 *******************************************************************************
*/
Int32 main (Int32 argc, Char ** argv)
{
    Char ch;
    Bool shutdownRemoteCores = FALSE;

    System_init();
    ChainsCommon_appCtrlCommonInit();
    Chains_Ctrl_Init(&gChains_usecaseCfg);

    /* allow remote prints to complete, before showing main menu */
    OSA_waitMsecs(500);

    while(1)
    {
        Vps_printf(gChains_testMenu);
        ch = getchar();

        if((ch=='x') || (ch=='X'))
            break;

        if((ch=='z') || (ch=='Z'))
        {
            shutdownRemoteCores = TRUE;
            break;
        }

        switch(ch)
        {
            case '1':
                chains_vipSingleCam_SgxDisplay(&gChains_usecaseCfg);
                break;
            case '2':
                chains_vipSingleCam_Enc_Dec_SgxDisplay(&gChains_usecaseCfg);
                break;
            case '3':
                gChains_usecaseCfg.algProcId = SYSTEM_PROC_EVE1;
                chains_vipSingleCamFrmcpyEve_SgxDisplay(&gChains_usecaseCfg);
                break;
#ifdef FPD_DISPLAY
            case '4':
                chains_vipSingleCam_SGX_IpuDualDisplay(&gChains_usecaseCfg);
                break;
#endif
            case '5':
                gChains_usecaseCfg.numLvdsCh = 1;
                Chains_nullSrc_DecodeDisplay(&gChains_usecaseCfg);
                break;
            case '6':
                chains_vipSingleCamFrameCopySgxDisplay(&gChains_usecaseCfg);
                break;
            case '7':
                chains_vipSingleCamConnectorLinksA15SgxDisplay(&gChains_usecaseCfg);
                break;
#ifdef ENABLE_OPENCV
            case '8':
                chains_vipSingleCamOpenCVCannySgxDisplay(&gChains_usecaseCfg);
                break;
#endif
            case 'a':
            case 'A':
                gChains_usecaseCfg.numLvdsCh = 4;
                gChains_usecaseCfg.captureSrc = CHAINS_CAPTURE_SRC_OV10635;
                chains_lvdsVipMultiCam_SgxDisplay(&gChains_usecaseCfg);
                break;
            case 'b':
            case 'B':
                gChains_usecaseCfg.numLvdsCh = 4;
                gChains_usecaseCfg.svOutputMode = ALGORITHM_LINK_SRV_OUTPUT_3D;
                gChains_usecaseCfg.enableCarOverlayInAlg = 0;
                gChains_usecaseCfg.captureSrc = CHAINS_CAPTURE_SRC_OV10635;
                gChains_usecaseCfg.displayType = CHAINS_DISPLAY_TYPE_HDMI_1080P;
                gChains_usecaseCfg.enableAutoCalib = 1;
                gChains_usecaseCfg.enablePersmatUpdate = 0;
                chains_lvdsVipMultiCam_Sgx3Dsrv(&gChains_usecaseCfg);
                break;
            case 'c':
            case 'C':
                gChains_usecaseCfg.numLvdsCh = 4;
                gChains_usecaseCfg.svOutputMode = ALGORITHM_LINK_SRV_OUTPUT_3D;
                gChains_usecaseCfg.captureSrc = CHAINS_CAPTURE_SRC_OV10635;
                gChains_usecaseCfg.displayType = CHAINS_DISPLAY_TYPE_HDMI_1080P;
                gChains_usecaseCfg.enableAutoCalib = 1;
                chains_lvdsVipMultiCam_3d_perception(&gChains_usecaseCfg);
                break;
            case 'd':
            case 'D':
                gChains_usecaseCfg.numLvdsCh = 4;
                gChains_usecaseCfg.svOutputMode = ALGORITHM_LINK_SRV_OUTPUT_3D;
                gChains_usecaseCfg.enableCarOverlayInAlg = 0;
                gChains_usecaseCfg.displayType = CHAINS_DISPLAY_TYPE_HDMI_1080P;
                gChains_usecaseCfg.enableAutoCalib = 1;
                gChains_usecaseCfg.enablePersmatUpdate = 0;
                chains_ov490VipMultiCam_Sgx3Dsrv(&gChains_usecaseCfg);
                break;
            case 'e':
            case 'E':
                Chains_3DSrvAutoCalibMenu();
                break;
#ifndef TDA2EX_BUILD
            case 'f':
            case 'F':
                gChains_usecaseCfg.numLvdsCh = 4;
                Chains_AvbRxDecodeSgxDisplay(&gChains_usecaseCfg);
                break;
#endif
            case 'p':
            case 'P':
                ChainsCommon_printCpuStatus();
                break;
            case 'i':
            case 'I':
                if(System_isProcEnabled(SYSTEM_IPU_PROC_PRIMARY))
                {
                    SystemCommon_IpAddr ipuIpAddr;
                    System_linkControl(
                        SYSTEM_LINK_ID_IPU1_0,
                        SYSTEM_COMMON_CMD_GET_IP_ADDR,
                        &ipuIpAddr,
                        sizeof(SystemCommon_IpAddr),
                        TRUE
                    );
                    Vps_printf(" Network IP address : %s\n", ipuIpAddr.ipAddr);
                }
                break;
            case 's':
            case 'S':
                    Chains_showSystemSettingsMenu();
                    break;
        }
    }
    ChainsCommon_appCtrlCommonDeInit();
    System_deInit(shutdownRemoteCores);

    return 0;
}

