
MODULE_NAME = vision_sdk_examples

vision_sdk_examples_PLATFORM_DEPENDENCY = yes
vision_sdk_examples_CORE_DEPENDENCY = yes

# List all the external components/interfaces, whose interface header files
#  need to be included for this component
INCLUDE_EXTERNAL_INTERFACES = bios xdc starterware bsp ipc vision_sdk fc xdais edma starterware_hal bsp_examples_utility  fat_lib utils_platform starterware_pm_hal starterware_pm_lib

ifeq ($(RADAR_INCLUDE),yes)
INCLUDE_INTERNAL_INTERFACES += radar
endif

MODULE_SRC_BASE_PATH := $(vision_sdk_PATH)/examples

# List all the components required by the application
COMP_LIST_$(CORE) = bios xdc ipc starterware bsp fc vision_sdk bsp_examples_utility

CFLAGS_LOCAL_$(CORE) = $(vision_sdk_CFLAGS)

# Common source files and CFLAGS across all platforms and cores
SRCS_COMMON =
SRCS_tda2xx =
CFLAGS_LOCAL_COMMON = $(vision_sdk_CFLAGS)

include $(MODULE_SRC_BASE_PATH)/tda2xx/src/usecases/common/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/tda2xx/src/usecases/vip_single_cam_view/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/tda2xx/src/usecases/vip_single_cam_dual_display/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/tda2xx/src/usecases/vip_single_cam_frame_copy/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/tda2xx/src/usecases/vip_single_cam_frame_copy_safety/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/tda2xx/src/usecases/vip_single_cam_edge_detection/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/tda2xx/src/usecases/vip_single_cam_dense_optical_flow/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/tda2xx/src/usecases/vip_single_cam_object_detection2/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/tda2xx/src/usecases/vip_single_cam_sparse_optical_flow/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/tda2xx/src/usecases/vip_single_cam_lane_detection/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/tda2xx/src/usecases/vip_single_cam_sfm/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/tda2xx/src/usecases/vip_single_cam_tlr/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/tda2xx/src/usecases/vip_single_cam_subframe_copy/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/tda2xx/src/usecases/lvds_vip_multi_cam_view/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/tda2xx/src/usecases/lvds_vip_multi_cam_view_tda3xx/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/tda2xx/src/usecases/lvds_vip_dual_cam_dual_display/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/tda2xx/src/usecases/lvds_vip_sv_standalone/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/tda2xx/src/usecases/lvds_vip_sv_tda3xx/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/tda2xx/src/usecases/lvds_vip_sv_analytics_us/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/tda2xx/src/usecases/avbrx_dec_display/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/tda2xx/src/usecases/avbrx_sv/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/tda2xx/src/usecases/iss_capture_isp_simcop_display/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/tda2xx/src/usecases/iss_monochrome_capt_isp_display/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/tda2xx/src/usecases/network_rx_tx/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/tda2xx/src/common/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/tda2xx/src/board/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/tda2xx/src/devices/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/tda2xx/src/modules/grpxSrc/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/tda2xx/src/modules/split/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/tda2xx/src/modules/ultrasonic_capture/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/tda2xx/src/modules/link_stats_monitor/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/tda2xx/src/modules/data_vis/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/tda2xx/src/modules/iss/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/tda2xx/src/usecases/vip_stereo_calibration/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/tda2xx/src/usecases/network_stereo_display/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/tda2xx/src/usecases/vip_stereo_only_display/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/tda2xx/src/usecases/vip_stereo_cam_analytics2/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/tda2xx/src/usecases/iss_mult_capture_isp_simcop_sv_tda3xx/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/tda2xx/src/usecases/iss_mult_capture_isp_dewarp_3dsv_tda3xx/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/tda2xx/src/usecases/iss_mult_capture_isp_2d_3d_sv_tda3x/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/tda2xx/src/usecases/iss_mult_capture_srv_calibration/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/tda2xx/src/usecases/ov490_vip_sv_standalone/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/tda2xx/src/usecases/vip_single_cam_view_dsswb/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/tda2xx/src/usecases/fast_boot_iss_capture_isp_simcop_pd_display/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/tda2xx/src/usecases/vip_single_cam_view_encdec/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/tda2xx/src/usecases/vip_single_cam_analytics2/SRC_FILES.MK
ifeq ($(RADAR_INCLUDE),yes)
include $(MODULE_SRC_BASE_PATH)/tda2xx/src/radar_fusion/links/radar/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/tda2xx/src/radar_fusion/usecases/radar_camera_overlay/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/tda2xx/src/radar_fusion/usecases/radar_detection_only/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/tda2xx/src/radar_fusion/usecases/radar_process_display/SRC_FILES.MK
endif
ifeq ($(ENABLE_OPENCV),yes)
include $(MODULE_SRC_BASE_PATH)/tda2xx/src/opencv/usecases/vip_single_cam_opencvcanny/SRC_FILES.MK
endif
ifeq ($(OPENCL_INCLUDE),yes)
include $(MODULE_SRC_BASE_PATH)/tda2xx/src/opencl/usecases/vip_single_cam_openclcanny_edge/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/tda2xx/src/opencl/usecases/vip_single_cam_openclframe_copy/SRC_FILES.MK
endif

# Core/SoC/platform specific source files and CFLAGS
# Example:
#   SRCS_<core/SoC/platform-name> =
#   CFLAGS_LOCAL_<core/SoC/platform-name> =

# Include common make files
include $(ROOTDIR)/build/makerules/common.mk

# OBJs and libraries are built by using rule defined in rules_<target>.mk
#     and need not be explicitly specified here

# Nothing beyond this point
