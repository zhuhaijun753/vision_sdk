
SRCDIR += $(vision_sdk_PATH)/examples/tda2xx/src/usecases/vip_single_cam_frame_copy_safety

SRCS_$(IPU_PRIMARY_CORE) += \
		chains_vipSingleCameraSafeFrameCopy.c \
		chains_vipSingleCameraSafeFrameCopy_priv.c

