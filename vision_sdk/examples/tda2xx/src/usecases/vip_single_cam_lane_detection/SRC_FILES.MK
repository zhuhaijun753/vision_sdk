
SRCDIR += $(vision_sdk_PATH)/examples/tda2xx/src/usecases/vip_single_cam_lane_detection

SRCS_$(IPU_PRIMARY_CORE) += \
		chains_vipSingleCameraLaneDetect_priv.c \
		chains_vipSingleCameraLaneDetect.c

