
SRCDIR += $(vision_sdk_PATH)/examples/tda2xx/src/usecases/vip_single_cam_frame_copy

SRCS_$(IPU_PRIMARY_CORE) += \
		chains_vipSingleCameraFrameCopy.c \
		chains_vipSingleCameraFrameCopy_priv.c

