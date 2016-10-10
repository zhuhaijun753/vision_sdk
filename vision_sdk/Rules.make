#*******************************************************************************
#                                                                              *
# Copyright (c) 2013 Texas Instruments Incorporated - http://www.ti.com/       *
#                        ALL RIGHTS RESERVED                                   *
#                                                                              *
#*******************************************************************************

# file name: Rules.make
# set up the build environment

ifeq ($(vision_sdk_PATH), )

# Board type can be one of the following
#   1. TDA2XX_EVM
#   2. TDA3XX_EVM
#   3. TDA2XX_MC
#   4. TDA2EX_EVM
#
#e.g.
#VSDK_BOARD_TYPE := TDA3XX_EVM
#VSDK_BOARD_TYPE := TDA2EX_EVM
#VSDK_BOARD_TYPE := TDA2XX_MC
#VSDK_BOARD_TYPE := TDA3XX_RVP
# Default board typ is TDA2xx EVM
ifeq ($(VSDK_BOARD_TYPE), )
  VSDK_BOARD_TYPE := TDA2XX_EVM
endif

#
# LENS MODULE used for the camera:
# equisolid/sunex_dsl218/imi
#
#LENS_MODULE=equisolid
#LENS_MODULE=sunex_dsl218
LENS_MODULE=imi

#
# Set BUILD_OS to Windows_NT to compile from BUILD_OS prompt
#
#BUILD_OS := Windows_NT

# Default build environment, windows or linux
ifeq ($(BUILD_OS), )
  BUILD_OS := Linux
endif

#
# Set BUILD_MACHINE to 32BIT as required, this is needed only
# if A15_TARGET_OS is going to be Linux
#
# BUILD_MACHINE :=32BIT
#
ifeq ($(BUILD_MACHINE), )
  BUILD_MACHINE := 64BIT
endif

#
# Set A15_TARGET_OS, master core can be A15 or IPU1-C0
# A15 can run Linux or Bios
# IPU1 will always run Bios
#
#                 A15_TARGET_OS support
#   Platform        BIOS        Linux
#
#   TDA2XX_EVM      yes         yes
#   TDA3XX_EVM      yes         no
#   TDA2XX_MC       yes         no
#   TDA2EX_EVM      yes         yes
#

# Default run environment Bios or Linux
ifeq ($(A15_TARGET_OS), )
 A15_TARGET_OS := Bios
# A15_TARGET_OS := Linux
endif

# For TDA2xx & TDA2Ex - vision SDK can run on either IPU1 or IPU2 subsystem
# For TDA3xx - We have only IPU1 and hence IPU1 is the only option
# Select IPU primary core from the available IPU1 & IPU2 subsytems
IPU_PRIMARY_CORE=ipu1_0
IPU_SECONDARY_CORE = ipu2
ifeq ($(A15_TARGET_OS), Linux)
 IPU_PRIMARY_CORE=ipu2
 IPU_SECONDARY_CORE = ipu1_0
endif

# Default platform
# Supported values: tda2xx-evm, tda3xx-evm, tda2xx-mc, tda3xx-rvp
ifeq ($(PLATFORM), )
    ifeq ($(VSDK_BOARD_TYPE), TDA2XX_EVM)
        PLATFORM := tda2xx-evm
    endif
    ifeq ($(VSDK_BOARD_TYPE), TDA3XX_EVM)
        PLATFORM := tda3xx-evm
    endif
    ifeq ($(VSDK_BOARD_TYPE), TDA3XX_RVP)
        PLATFORM := tda3xx-rvp
    endif
    ifeq ($(VSDK_BOARD_TYPE), TDA2XX_MC)
        PLATFORM := tda2xx-mc
    endif
    ifeq ($(VSDK_BOARD_TYPE), TDA2EX_EVM)
        PLATFORM := tda2ex-evm
    endif
endif

ifeq ($(A15_TARGET_OS), Linux)
  BUILD_OS := Linux
endif

# Default build tda2xx only
ifeq ($(BUILD_INFOADAS), )
  BUILD_INFOADAS := no
endif

vision_sdk_RELPATH = vision_sdk

ifeq ($(BUILD_OS),Windows_NT)
  TI_SW_ROOT      := $(abspath ..)/ti_components
endif

ifeq ($(BUILD_OS),Linux)
  TI_SW_ROOT      := $(abspath ..)/ti_components
  # or /opt/ti if you follow the package installers
endif

vision_sdk_PATH  := $(abspath ..)/$(vision_sdk_RELPATH)
infoadas_PATH  ?=

#
# Code gen and config tools
#

ifeq ($(BUILD_OS),Windows_NT)
CODEGEN_PATH_DSP ?= $(TI_SW_ROOT)/cg_tools/windows/C6000_7.4.2
CODEGEN_PATH_EVE ?= $(TI_SW_ROOT)/cg_tools/windows/arp32_1.0.7
CODEGEN_PATH_A15 ?= $(TI_SW_ROOT)/cg_tools/windows/gcc-arm-none-eabi-4_9-2015q3
CODEGEN_PATH_M4  ?= $(TI_SW_ROOT)/cg_tools/windows/ti-cgt-arm_5.2.5
endif

ifeq ($(BUILD_OS),Linux)
CODEGEN_PATH_DSP ?= $(TI_SW_ROOT)/cg_tools/linux/C6000_7.4.2
CODEGEN_PATH_EVE ?= $(TI_SW_ROOT)/cg_tools/linux/arp32_1.0.7
CODEGEN_PATH_M4  ?= $(TI_SW_ROOT)/cg_tools/linux/ti-cgt-arm_5.2.5

ifeq ($(A15_TARGET_OS),Bios)
CODEGEN_PATH_A15 ?=$(TI_SW_ROOT)/cg_tools/linux/gcc-arm-none-eabi-4_9-2015q3
endif

ifeq ($(A15_TARGET_OS),Linux)
CODEGEN_PATH_A15     ?= $(TI_SW_ROOT)/os_tools/linux/linaro/gcc-linaro-arm-linux-gnueabihf-4.7-2013.03-20130313_linux
A15_TOOLCHAIN_PREFIX ?= $(CODEGEN_PATH_A15)/bin/arm-linux-gnueabihf-
#
# Path where linux uImage, uboot etc will be copied to after build
#
LINUX_BOOT_OUT_FILES =$(vision_sdk_PATH)/linux/boot
LINUX_TARGETFS ?=$(vision_sdk_PATH)/linux/targetfs
endif

endif

#
# BIOS, IPC and XDC, M4 Codegen
#

ifeq ($(PLATFORM),$(filter $(PLATFORM), tda2xx-evm tda2ex-evm tda2xx-mc tda3xx-evm tda3xx-rvp))
    bios_PATH ?= $(TI_SW_ROOT)/os_tools/bios_6_46_00_23
    ipc_PATH  ?= $(TI_SW_ROOT)/os_tools/ipc_3_42_00_02
    ifeq ($(BUILD_OS),Windows_NT)
        xdc_PATH  ?= $(TI_SW_ROOT)/os_tools/windows/xdctools_3_32_00_06_core
    endif
    ifeq ($(BUILD_OS),Linux)
        xdc_PATH  ?= $(TI_SW_ROOT)/os_tools/linux/xdctools_3_32_00_06_core
    endif
endif


ifeq ($(A15_TARGET_OS),Linux)
kernel_PATH ?= $(vision_sdk_PATH)/../ti_components/os_tools/linux/kernel/omap
kernel_addon_PATH ?= $(vision_sdk_PATH)/../ti_components/os_tools/linux/kernel/linux-kernel-addon
memcache_PATH ?= $(kernel_addon_PATH)/memcache
uboot_PATH  ?= $(vision_sdk_PATH)/../ti_components/os_tools/linux/u-boot/u-boot
sgx_PATH ?= $(vision_sdk_PATH)/../ti_components/os_tools/linux/sgx
pvr_PATH ?= $(vision_sdk_PATH)/../ti_components/os_tools/pvr_tools
mm_PATH ?= $(TI_SW_ROOT)/algorithms_codecs/ipumm
ce_PATH ?= $(TI_SW_ROOT)/algorithms_codecs/codec_engine_3_24_00_08
endif


#
# Low-level drivers
#
bsp_PATH         ?= $(TI_SW_ROOT)/drivers/bsp_01_06_00_11
edma_PATH        ?= $(TI_SW_ROOT)/drivers/edma3_lld_02_12_00_20
starterware_PATH ?= $(TI_SW_ROOT)/drivers/starterware_01_06_00_16

# Set the PACKAGE_SELECT option for BSP/STW to individually control VIP, VPE and DSS
# For Bios on A15  - Display will be controlled by M4 Bios, so set as "all" (including DSS)
# For Linux on A15 - If Display will be controlled by A15 Linux, then set as "vps-vip-vpe" (excluding DSS)
ifeq ($(A15_TARGET_OS),Bios)
BSP_STW_PACKAGE_SELECT := all
endif

# Vision SDK Linux supports display only on M4 (Bios)
ifeq ($(A15_TARGET_OS), Linux)
# For InfoADAS display is on A15 (Linux/DRM)
ifeq ($(BUILD_INFOADAS), yes)
BSP_STW_PACKAGE_SELECT := vps-vip-vpe
else
BSP_STW_PACKAGE_SELECT := all
endif
endif

#
# Networking related packages
#
ndk_PATH   ?= $(TI_SW_ROOT)/networking/ndk_2_24_02_31
nsp_PATH   ?= $(TI_SW_ROOT)/networking/nsp_gmacsw_4_14_00_00
avbtp_PATH ?= $(TI_SW_ROOT)/networking/avbtp_0_09_00_01

#
# Algorithm related packages
#
fc_PATH                       ?= $(TI_SW_ROOT)/algorithms_codecs/framework_components_3_40_01_04
xdais_PATH                    ?= $(TI_SW_ROOT)/algorithms_codecs/xdais_7_24_00_04
hdvicplib_PATH                ?= $(TI_SW_ROOT)/algorithms_codecs/ivahd_hdvicp20api_01_00_00_23_production
jpegvenc_PATH                 ?= $(TI_SW_ROOT)/algorithms_codecs/ivahd_jpegvenc_01_00_16_01_production
jpegvdec_PATH                 ?= $(TI_SW_ROOT)/algorithms_codecs/ivahd_jpegvdec_01_00_13_01_production
h264venc_PATH                 ?= $(TI_SW_ROOT)/algorithms_codecs/ivahd_h264enc_02_00_09_01_production
h264vdec_PATH                 ?= $(TI_SW_ROOT)/algorithms_codecs/ivahd_h264vdec_02_00_17_01_production
evealg_PATH                   ?= $(TI_SW_ROOT)/algorithms_codecs/eve_sw_01_12_00_00
vlib_PATH                     ?= $(TI_SW_ROOT)/algorithms_codecs/vlib_c66x_3_3_0_3
lane_detect_PATH              ?= $(TI_SW_ROOT)/algorithms_codecs/REL.200.V.LD.C66X.00.02.03.00/200.V.LD.C66X.00.02
object_detect_PATH            ?= $(TI_SW_ROOT)/algorithms_codecs/200.V.OD.C66X.00.05
object_classification_PATH    ?= $(TI_SW_ROOT)/algorithms_codecs/200.V.OC.C66X.00.01
hcf_PATH                      ?= $(TI_SW_ROOT)/algorithms_codecs/hcf/repos/
stereo_postprocess_PATH       ?= $(TI_SW_ROOT)/algorithms_codecs/REL.200.V.ST.C66X.00.02.02.00/200.V.ST.C66X.02.02
clr_PATH                      ?= $(TI_SW_ROOT)/algorithms_codecs/200.V.CLR.C66X.00.01
mathlib_PATH                  ?= $(TI_SW_ROOT)/algorithms_codecs/mathlib_c66x_3_1_0_0
scene_obstruction_detect_PATH ?= $(TI_SW_ROOT)/algorithms_codecs/REL.200.V.SOD.C66X.00.01.00.00/200.V.SOD.C66X.01.00
sfm_PATH                      ?= $(TI_SW_ROOT)/algorithms_codecs/REL.200.V.SFM.C66X.00.01.00.00/200.V.SFM.C66X.00.01
gpe_PATH                      ?= $(sfm_PATH)
fcw_PATH                      ?= $(sfm_PATH)

# Use below option to enable OpenCL
OPENCL_INCLUDE = no
ifeq ($(OPENCL),yes)
OpenCL_PATH ?= $(TI_SW_ROOT)/opencl
OPENCL_INCLUDE = yes
endif

# Use the ENABLE_OPENCV option to enable OpenCV
# (options: yes no)
ENABLE_OPENCV := no
ifeq ($(ENABLE_OPENCV),yes)
opencv_PATH=$(TI_SW_ROOT)/algorithms_codecs/opencv
endif

ROOTDIR := $(vision_sdk_PATH)

XDCPATH = $(bios_PATH)/packages;$(ipc_PATH)/packages;$(xdc_PATH)/packages;$(edma_PATH)/packages;$(bsp_PATH);$(starterware_PATH);$(fc_PATH)/packages;$(vision_sdk_PATH);$(xdais_PATH)/packages;$(hdvicplib_PATH)/packages;$(jpegvdec_PATH)/packages;$(jpegvenc_PATH)/packages;$(ndk_PATH)/packages;$(nsp_PATH)/packages;$(avbtp_PATH)/packages;$(mm_PATH);$(ce_PATH)/packages;$(mm_PATH)/extrel/ti/ivahd_codecs/packages;$(mm_PATH)/src;$(OpenCL_PATH)/packages
edma_DRV_PATH = $(edma_PATH)/packages/ti/sdo/edma3/drv
edma_RM_PATH  = $(edma_PATH)/packages/ti/sdo/edma3/rm

# postfixes we require
_CORES     := M4 DSP EVE A15
# Check for the existence of each compiler!
$(foreach path,$(_CORES),$(if $(realpath $(CODEGEN_PATH_$(path))),,$(error CODEGEN_PATH_$(path) does not exist! ($(CODEGEN_PATH_$(path))))))
# prefixes we require...
_REQ_PATHS := xdc bios ipc bsp edma starterware fc xdais evealg
_OPT_PATHS := ndk nsp avbtp hdvicplib jpegvdec jpegvenc vlib
# Check for the existence of each xxxxx_PATH variable! Error if it does not exist.
$(foreach path,$(_REQ_PATHS),$(if $(realpath $($(path)_PATH)),,$(error $(path)_PATH does not exist! ($($(path)_PATH)))))
$(foreach path,$(_OPT_PATHS),$(if $(realpath $($(path)_PATH)),,$(warning $(path)_PATH does not exist! ($($(path)_PATH)))))

ifeq ($(A15_TARGET_OS), Linux)
  DEFAULT_UBOOT_CONFIG  := dra7xx_evm_vision_config
  DEFAULT_KERNEL_CONFIG := omap2plus_defconfig
  ifeq ($(VSDK_BOARD_TYPE), TDA2EX_EVM)
    DEFAULT_DTB := dra72-evm-infoadas.dtb
  else
    DEFAULT_DTB := dra7-evm-infoadas.dtb
  endif
endif

###############################
# Set DDR_MEM config.         #
# Available/Supported configs #
#   For TDA2XX: 512MB         #
###############################

ifeq ($(PLATFORM), tda2xx-evm)
ifeq ($(DDR_MEM), )
  DDR_MEM := DDR_MEM_512M
endif
endif

ifeq ($(PLATFORM), tda2xx-mc)
ifeq ($(DDR_MEM), )
  DDR_MEM := DDR_MEM_512M
endif
endif

###############################
# Set DDR_MEM config.         #
# Available/Supported configs #
#   For TDA3XX: 512M, 64M;    #
###############################

ifeq ($(PLATFORM), tda3xx-evm)
ifeq ($(DDR_MEM), )
#  DDR_MEM := DDR_MEM_64M
  DDR_MEM := DDR_MEM_512M
endif
endif

ifeq ($(PLATFORM), tda3xx-rvp)
ifeq ($(DDR_MEM), )
#  DDR_MEM := DDR_MEM_64M
  DDR_MEM := DDR_MEM_512M
endif
endif

###############################
# Set DDR_MEM config.         #
# Available/Supported configs #
#   For TDA2EX: 512MB         #
###############################

ifeq ($(PLATFORM), tda2ex-evm)
ifeq ($(DDR_MEM), )
  DDR_MEM := DDR_MEM_512M
endif
endif

#######################################
# To Eanble Dual A15 in SMP BIOS mode #
# Set DUAL_A15_SMP_BIOS := yes        #
# This is supported only on TDA2xx    #
#######################################

# By default SMP BIOS is disabled. It can be enabled for TDA2xx platform below
DUAL_A15_SMP_BIOS := no
ifeq ($(PLATFORM),$(filter $(PLATFORM), tda2xx-evm tda2xx-mc))
ifeq ($(A15_TARGET_OS),Bios)
# Set below line as "DUAL_A15_SMP_BIOS := yes" for enabling Dual A15 with SMP Bios
  DUAL_A15_SMP_BIOS := no
endif
endif

# Default profile: release
# Supported profiles: release & debug
ifeq ($(PROFILE), )
# Enable debug profile for all cores
# PROFILE = debug
# Enable release profile for all cores
 PROFILE = release
endif

ifeq ($(PROFILE_ipu1_0), )
  PROFILE_ipu1_0 := $(PROFILE)
endif
ifeq ($(PROFILE_ipu1_1), )
  PROFILE_ipu1_1 := $(PROFILE)
endif

ifeq ($(PROFILE_ipu2), )
  PROFILE_ipu2 := $(PROFILE)
endif

ifeq ($(PROFILE_c66xdsp_1), )
  PROFILE_c66xdsp_1 := $(PROFILE)
endif
ifeq ($(PROFILE_c66xdsp_2), )
  PROFILE_c66xdsp_2 := $(PROFILE)
endif

ifeq ($(PROFILE_arp32_1), )
  PROFILE_arp32_1 := $(PROFILE)
endif
ifeq ($(PROFILE_arp32_2), )
  PROFILE_arp32_2 := $(PROFILE)
endif
ifeq ($(PROFILE_arp32_3), )
  PROFILE_arp32_3 := $(PROFILE)
endif
ifeq ($(PROFILE_arp32_4), )
  PROFILE_arp32_4 := $(PROFILE)
endif

# A15 use gcc tools (compiler, linker etc.)
ifeq ($(PROFILE_a15_0), )
  PROFILE_a15_0 := $(PROFILE)
endif

# Default klockwork build flag, yes or no
ifeq ($(KW_BUILD), )
  KW_BUILD := no
endif

# Default C++ build flag, yes or no
ifeq ($(CPLUSPLUS_BUILD), )
  CPLUSPLUS_BUILD := no
endif

#
# Change below to include or exclude certain core's
#
# EVE3, EVE4, IPU2 are kept disabled to reduce build time.
# Most use-cases not need these CPUs.
# Enable them in case you need it.
#
PROC_DSP1_INCLUDE=yes
PROC_DSP2_INCLUDE=yes
PROC_EVE1_INCLUDE=yes
PROC_EVE2_INCLUDE=yes
PROC_EVE3_INCLUDE=no
PROC_EVE4_INCLUDE=no
PROC_A15_0_INCLUDE=yes
PROC_IPU1_0_INCLUDE=yes
PROC_IPU1_1_INCLUDE=yes
PROC_IPU2_INCLUDE=no

#
# Change below to include or exclude certain HW module's
#
# Below are default's based on TDA2xx BIOS build, these are overridden
# later to TDA3xx or TDA2ex specfic one's or Linux on A15 specific one's
#

AVBRX_INCLUDE=yes
IVAHD_INCLUDE=yes
VPE_INCLUDE=yes
DSS_INCLUDE=yes
ISS_INCLUDE=no
ISS_ENABLE_DEBUG_TAPS=no
WDR_LDC_INCLUDE=no
ALG_CRC_INCLUDE=no
RTI_INCLUDE=no

#
# DCC is integrated in a way that requires ESM.
# Both modules are, therefore, clubbed together
# These are present only in TDA3XX
# Setting this to "no" only disables usage of these modules
# Source files are always enabled in build for TDA3XX
#
DCC_ESM_INCLUDE=no

#
# EMIF ECC and Freedom from Interference (FFI) are clubbed together as they
# involve consolidated memory map changes
# ECC_FFI_INCLUDE=yes is not supported for DDR_MEM_64M and TDA2XX_MC
# If ECC_FFI_INCLUDE is set to no - memory map will match older versions.
# Firewall configuration and DSP XMC configuration is enabled/disabled using this
# flag
# Vision SDK does not enable EMIF ECC but expects SBL to enable it.
# Vision SDK sets up ECC error handlers irrespective of value of ECC_FFI_INCLUDE.
#
ECC_FFI_INCLUDE?=no

#
# Enable this macro to enable Ultra sonic based initialization
# This is added as the power measurement and Ultra sonic use the same
# pad on the device. Kindly note the Ultrasonic and power measurement
# use the same GPIO6_14/15 pads with different functionalities. Ultrasonic configures
# UART10 on those pads and power measurement configures I2C3. Hence when this
# macro is yes the power measurement functionality is not supported.
#
ULTRASONIC_INCLUDE=no

#
# Enable below macro to enable Radar integration into Vision SDK
#
RADAR_INCLUDE=no


#
# Enable below macro to enable DCAN integration into Vision SDK.
#
DCAN_INCLUDE=no


#
# Enable below macro to enable linking of surround view algorithms
#
ALG_SV_INCLUDE=yes


#
# Enable below macro to view the bandwidth used by different IPs in the device
# as a graphical plot on the display screen.
#
DATA_VIS_INCLUDE=no

#
# CPU to use to run Networking stack. Valid values are
# ipu1_0 ipu1_1 a15_0 none
#
ifeq ($(NDK_PROC_TO_USE), )
NDK_PROC_TO_USE=a15_0
endif


#
# CPU to use to run FAT filesystem. Valid values are
# ipu1_0 none
#
ifeq ($(FATFS_PROC_TO_USE), )
FATFS_PROC_TO_USE=ipu1_0
endif

# Fast boot usecase is currently supported only for tda3x
FAST_BOOT_INCLUDE=no

# Fast Boot for 3D Surround View on TDA3XX
SRV_FAST_BOOT_INCLUDE=no

#
# Some CPU's and module's force disable if build is for TDA3xx
#
# Some modules and core's are disabled since they are not
# present in TDA3xx, like VPE, IVAHD, A15_0, EVE2/3/4
#
# Enable some module's if they present only in TDA3xx, like ISS
#
ifeq ($(PLATFORM),$(filter $(PLATFORM), tda3xx-evm tda3xx-rvp))

# Disabling or enabling below CPUs/modules for now until they are tested
# Currently, for 64MB DDR build for TDA3xx, DSP2 is not included
ifeq ($(DDR_MEM), DDR_MEM_64M)
PROC_DSP2_INCLUDE=no
ECC_FFI_INCLUDE=no
ALG_SV_INCLUDE=no
endif
ifeq ($(FAST_BOOT_INCLUDE), yes)
ALG_SV_INCLUDE=no
endif
ISS_INCLUDE=yes
ISS_ENABLE_DEBUG_TAPS=no
WDR_LDC_INCLUDE=yes
AVBRX_INCLUDE=no
NDK_PROC_TO_USE=none
# Below CPUs/modules are not present in TDA3xx
PROC_A15_0_INCLUDE=no
PROC_EVE2_INCLUDE=no
PROC_EVE3_INCLUDE=no
PROC_EVE4_INCLUDE=no
PROC_IPU2_INCLUDE=no
IVAHD_INCLUDE=no
VPE_INCLUDE=no
ALG_CRC_INCLUDE=yes
RTI_INCLUDE=yes
DCC_ESM_INCLUDE=yes

# For SRV fast boot DSP2, EVE and IPU1_1 are excluded
ifeq ($(SRV_FAST_BOOT_INCLUDE), yes)
PROC_DSP2_INCLUDE=no
PROC_EVE1_INCLUDE=no
PROC_IPU1_1_INCLUDE=no
endif
endif

#
# This variable is used to generate the 3D SRV View 2 world files using the
# static LUTs for TDA3XX.
# It needs in excess of 30 MB of IPU1_0 code size hence there are changes in
# the memory map and the DDR heap size is reduced accordingly to fit
# in bigger IPU1_0 code size.
# Keep this turned OFF for normal use cases.
#
TDA3X_3DSRV_V2W_GEN ?= no

#
# Some CPU's and module's force disable if build is for TDA2ex
#
# Some modules and core's are disabled since they are not
# present in TDA2ex, like DSP2, EVE1/2/3/4
#
ifeq ($(PLATFORM), tda2ex-evm)

# Disabling or enabling below CPUs/modules for now until they are tested
IVAHD_INCLUDE=yes
VPE_INCLUDE=yes
# Below CPUs/modules are not present in TDA2ex
PROC_DSP2_INCLUDE=no
PROC_EVE1_INCLUDE=no
PROC_EVE2_INCLUDE=no
PROC_EVE3_INCLUDE=no
PROC_EVE4_INCLUDE=no
ISS_INCLUDE=no
ISS_ENABLE_DEBUG_TAPS=no
WDR_LDC_INCLUDE=no
ALG_CRC_INCLUDE=no
RTI_INCLUDE=no
endif

#
# Additional override's for TDA2x-MC platform
#
ifeq ($(PLATFORM), tda2xx-mc)
#
# Some TDA2x-MC platform "out of box" use-cases need EVE3 and EVE4
# If EVE3 and EVE4 is not required for your use-cases on TDA2x-MC, this can be made as "no"
#
PROC_EVE3_INCLUDE=yes
PROC_EVE4_INCLUDE=yes
# ECC and FFI changes are not tested on TDA2XX_MC
ECC_FFI_INCLUDE=no
endif


#
# Some CPU's and module's force disable when linux run's on A15
#
ifeq ($(A15_TARGET_OS), Linux)
PROC_IPU1_1_INCLUDE=no
PROC_IPU2_INCLUDE=yes
ifeq ($(PLATFORM), tda2xx-evm)
PROC_EVE3_INCLUDE=yes
PROC_EVE4_INCLUDE=yes
endif
ifeq ($(BUILD_INFOADAS), yes)
DSS_INCLUDE=no
#Remove below lines onece infoAdas is tested along with VSDK on IPU2
PROC_IPU2_INCLUDE=no
IPU_PRIMARY_CORE=ipu1_0
IPU_SECONDARY_CORE = ipu2
endif
# TDA2Ex is not valiadted with IPU2 enabled when A15 OS is Linux
# So keeping IPU1 as primary core for TDA2Ex
ifeq ($(PLATFORM), tda2ex-evm)
PROC_IPU2_INCLUDE=no
IPU_PRIMARY_CORE=ipu1_0
IPU_SECONDARY_CORE = ipu2
endif
#Set NDK_PROC_TO_USE=ipu2 for TDA2X to enable AVB capture with A15-Linux OR
#Set NDK_PROC_TO_USE=ipu1_0 for TDA2EX to enable AVB capture with A15-Linux
NDK_PROC_TO_USE=none
FATFS_PROC_TO_USE=none
#FFI is not tested with Linux
ECC_FFI_INCLUDE=no
#
# Enable below macro to enable IPUMM to be included in image
# Please Note: VSDK ENC/DEC (IVA) UCs will not work if IPUMM_INCLUDE=yes
IPUMM_INCLUDE=no
ifeq ($(IPUMM_INCLUDE), yes)
IVAHD_INCLUDE=no
endif
endif


#
# To get a faster incremental build do below
# BUILD_DEPENDANCY_ALWAYS=no
#
# IMPORTANT: In this case make sure to do
# (g)make -s depend
# in case of any change in dependancies like
# - changes in BSP/Starterware/EDMA3LLD .c or .h file
# - changes in number of cores used for build (value of PROC_<CPU>_INCLUDE)
# - change in value of DDR_MEM
# - change in value of NDK_PROC_TO_USE
#
BUILD_DEPENDANCY_ALWAYS=no

#
# Used to control building of algorithm source.
# By default algorithm source not included in Vision SDK
#
BUILD_ALGORITHMS=no

TREAT_WARNINGS_AS_ERROR=yes

ifeq ($(OPENCL_INCLUDE),yes)
TREAT_WARNINGS_AS_ERROR=no
PROC_DSP2_INCLUDE=no
PROC_EVE2_INCLUDE=no
PROC_IPU1_1_INCLUDE=no
endif

#
# HCF is an experimental module, NOT to used by users of Vision SDK
# HCF wont be included in Vision SDK release
#
# Keep below as 'no' always
#
HCF_INCLUDE=no

ifeq ($(NDK_PROC_TO_USE),ipu1_1)
    ifeq ($(PROC_IPU1_1_INCLUDE),no)
        NDK_PROC_TO_USE=none
    endif
endif

ifeq ($(NDK_PROC_TO_USE),ipu1_0)
    ifeq ($(PROC_IPU1_0_INCLUDE),no)
        NDK_PROC_TO_USE=none
    endif
endif

ifeq ($(NDK_PROC_TO_USE),ipu2)
    ifeq ($(PROC_IPU2_INCLUDE),no)
        NDK_PROC_TO_USE=none
    endif
endif

ifeq ($(NDK_PROC_TO_USE),a15_0)
    ifeq ($(PROC_A15_0_INCLUDE),no)
        NDK_PROC_TO_USE=none
    endif
endif

ifeq ($(FATFS_PROC_TO_USE),ipu1_1)
    ifeq ($(PROC_IPU1_1_INCLUDE),no)
        FATFS_PROC_TO_USE=none
    endif
endif

ifeq ($(FATFS_PROC_TO_USE),ipu1_0)
    ifeq ($(PROC_IPU1_0_INCLUDE),no)
        FATFS_PROC_TO_USE=none
    endif
endif

ifeq ($(FATFS_PROC_TO_USE),ipu2)
    ifeq ($(PROC_IPU2_INCLUDE),no)
        FATFS_PROC_TO_USE=none
    endif
endif

ifeq ($(FATFS_PROC_TO_USE),a15_0)
    ifeq ($(PROC_A15_0_INCLUDE),no)
        FATFS_PROC_TO_USE=none
    endif
endif

#
# NDK/NSP on A15 with SMP BIOS has issues and hence disable networking
# support on A15, when A15 is running SMP Bios
#
ifeq ($(DUAL_A15_SMP_BIOS),yes)
    ifeq ($(NDK_PROC_TO_USE),a15_0)
        NDK_PROC_TO_USE=none
    endif
endif


#
# IPU1_EVELOADER_INCLUDE is used by IPU firmware to decide on
# EVE loading from IPU1. This is used only when A15_TARGET_OS
# is Linux.
#

IPU1_EVELOADER_INCLUDE=no

ifeq ($(A15_TARGET_OS), Linux)
    ifeq ($(PROC_EVE1_INCLUDE), yes)
        IPU1_EVELOADER_INCLUDE=yes
    endif
    ifeq ($(PROC_EVE2_INCLUDE), yes)
        IPU1_EVELOADER_INCLUDE=yes
    endif
    ifeq ($(PROC_EVE3_INCLUDE), yes)
        IPU1_EVELOADER_INCLUDE=yes
    endif
    ifeq ($(PROC_EVE4_INCLUDE), yes)
        IPU1_EVELOADER_INCLUDE=yes
    endif
endif

#
# Used to enable or disable CPU idle functionality in SDK
# By Default CPU idle is enabled
#
CPU_IDLE_ENABLED=yes

ifeq ($(PROFILE),debug)
CPU_IDLE_ENABLED=no
endif

PROC_EVE_INCLUDE = no
ifeq ($(PROC_EVE2_INCLUDE),yes)
	PROC_EVE_INCLUDE=yes
endif
ifeq ($(PROC_EVE3_INCLUDE),yes)
	PROC_EVE_INCLUDE=yes
endif
ifeq ($(PROC_EVE4_INCLUDE),yes)
	PROC_EVE_INCLUDE=yes
endif
ifeq ($(PROC_EVE1_INCLUDE),yes)
	PROC_EVE_INCLUDE=yes
endif

PROC_DSP_INCLUDE = no
ifeq ($(PROC_DSP1_INCLUDE),yes)
	PROC_DSP_INCLUDE=yes
endif
ifeq ($(PROC_DSP2_INCLUDE),yes)
	PROC_DSP_INCLUDE=yes
endif

#
# When NDK is enabled, FATFS cannot be used due to MMCSD conflict
#
ifeq ($(PLATFORM),$(filter $(PLATFORM), tda3xx-evm tda3xx-rvp))
	ifeq ($(NDK_PROC_TO_USE),ipu1_1)
		FATFS_PROC_TO_USE=none
	endif
	ifeq ($(NDK_PROC_TO_USE),ipu1_0)
		FATFS_PROC_TO_USE=none
	endif
	ifeq ($(NDK_PROC_TO_USE),a15_0)
		FATFS_PROC_TO_USE=none
	endif
endif

#
# When HCF is enabled some options cannot be used
# Below conditions overrides user set values to make it compatible to
# HCF requirements
#
ifeq ($(HCF_INCLUDE),yes)
	A15_TARGET_OS=Bios
	FAST_BOOT_INCLUDE=no
	ifeq ($(PLATFORM), tda2ex-evm)
		HCF_INCLUDE=no
	endif
	ifeq ($(PLATFORM),tda2xx-evm)
		DDR_MEM:=DDR_MEM_512M
	endif
	ifeq ($(PLATFORM),tda2xx-mc)
		DDR_MEM:=DDR_MEM_512M
	endif
	ifeq ($(PLATFORM),tda3xx-evm)
		DDR_MEM:=DDR_MEM_512M
	endif
	ifeq ($(PLATFORM),tda3xx-rvp)
		DDR_MEM:=DDR_MEM_512M
	endif
endif

PROC_SINGLE_CORE = no
ifeq ($(PROC_DSP_INCLUDE)$(PROC_EVE_INCLUDE)$(PROC_A15_0_INCLUDE)$(PROC_IPU1_0_INCLUDE)$(PROC_IPU1_1_INCLUDE)$(PROC_IPU2_INCLUDE), nononoyesnono)
    PROC_SINGLE_CORE = yes
endif


export TREAT_WARNINGS_AS_ERROR
export BUILD_OS
export BUILD_MACHINE
export PLATFORM
export CORE
export PROFILE_ipu1_0
export PROFILE_ipu1_1
export PROFILE_ipu2
export PROFILE_c66xdsp_1
export PROFILE_c66xdsp_2
export PROFILE_arp32_1
export PROFILE_arp32_2
export PROFILE_arp32_3
export PROFILE_arp32_4
export PROFILE_a15_0
export CODEGEN_PATH_M4
export CODEGEN_PATH_DSP
export CODEGEN_PATH_EVE
export CODEGEN_PATH_A15
export bios_PATH
export ipc_PATH
export mm_PATH
export ce_PATH
export kernel_PATH
export memcache_PATH
export kernel_addon_PATH
export uboot_PATH
export sgx_PATH
export pvr_PATH
export xdc_PATH
export starterware_PATH
export edma_PATH
export edma_DRV_PATH
export edma_RM_PATH
export bsp_PATH
export ndk_PATH
export nsp_PATH
export avbtp_PATH
export evealg_PATH
export fc_PATH
export xdais_PATH
export hdvicplib_PATH
export jpegvdec_PATH
export jpegvenc_PATH
export h264venc_PATH
export h264vdec_PATH
export vlib_PATH
export mathlib_PATH
export vision_sdk_RELPATH
export vision_sdk_PATH
export ROOTDIR
export XDCPATH
export KW_BUILD
export DDR_MEM
export DEST_ROOT
export PROC_DSP1_INCLUDE
export PROC_DSP2_INCLUDE
export PROC_DSP_INCLUDE
export PROC_EVE1_INCLUDE
export PROC_EVE2_INCLUDE
export PROC_EVE3_INCLUDE
export PROC_EVE4_INCLUDE
export PROC_EVE_INCLUDE
export PROC_A15_0_INCLUDE
export PROC_IPU1_0_INCLUDE
export PROC_IPU1_1_INCLUDE
export PROC_IPU2_INCLUDE
export NDK_PROC_TO_USE
export A15_TARGET_OS
export A15_TOOLCHAIN_PREFIX
export DEFAULT_UBOOT_CONFIG
export DEFAULT_KERNEL_CONFIG
export DEFAULT_DTB
export LINUX_BOOT_OUT_FILES
export LINUX_TARGETFS
export BSP_STW_PACKAGE_SELECT
export AVBRX_INCLUDE
export IVAHD_INCLUDE
export VPE_INCLUDE
export DSS_INCLUDE
export ISS_INCLUDE
export ISS_ENABLE_DEBUG_TAPS
export WDR_LDC_INCLUDE
export IPU1_EVELOADER_INCLUDE
export lane_detect_PATH
export object_detect_PATH
export object_classification_PATH
export hcf_PATH
export HCF_INCLUDE
export VSDK_BOARD_TYPE
export CPU_IDLE_ENABLED
export stereo_postprocess_PATH
export scene_obstruction_detect_PATH
export infoadas_PATH
export BUILD_INFOADAS
export DCAN_INCLUDE
export DUAL_A15_SMP_BIOS
export FAST_BOOT_INCLUDE
export ALG_CRC_INCLUDE
export RTI_INCLUDE
export BUILD_ALGORITHMS
export RADAR_INCLUDE
export FATFS_PROC_TO_USE
export PROC_SINGLE_CORE
export gpe_PATH
export sfm_PATH
export clr_PATH
export fcw_PATH
export ALG_SV_INCLUDE
export ULTRASONIC_INCLUDE
export ECC_FFI_INCLUDE
export DCC_ESM_INCLUDE
export DATA_VIS_INCLUDE
export ENABLE_OPENCV
export opencv_PATH
export TDA3X_3DSRV_V2W_GEN
export IPU_PRIMARY_CORE
export IPU_SECONDARY_CORE
export IPUMM_INCLUDE
export SRV_FAST_BOOT_INCLUDE
export OpenCL_PATH
export OPENCL_INCLUDE
export LENS_MODULE
endif

ifeq ($(MAKERULEDIR), )
  MAKERULEDIR := $(ROOTDIR)/build/makerules
  export MAKERULEDIR
endif

include $(MAKERULEDIR)/build_config.mk
include $(MAKERULEDIR)/platform.mk
include $(MAKERULEDIR)/env.mk
include $(MAKERULEDIR)/component.mk
