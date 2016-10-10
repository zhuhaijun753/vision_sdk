
SRCDIR += $(vision_sdk_PATH)/examples/tda2xx/src/usecases/vip_single_cam_sfm

SRCS_$(IPU_PRIMARY_CORE) += \
		chains_vipSingleCameraSfm_priv.c \
		chains_vipSingleCameraSfm.c

