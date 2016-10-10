SRCDIR += $(vision_sdk_PATH)/examples/tda2xx/src/opencv/usecases/vip_single_cam_opencvcanny

SRCS_$(IPU_PRIMARY_CORE) += \
		chains_vipSingleCamOpenCVCanny.c \
		chains_vipSingleCamOpenCVCanny_priv.c

