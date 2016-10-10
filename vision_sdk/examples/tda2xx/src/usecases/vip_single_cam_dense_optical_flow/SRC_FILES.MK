
SRCDIR += $(vision_sdk_PATH)/examples/tda2xx/src/usecases/vip_single_cam_dense_optical_flow

SRCS_$(IPU_PRIMARY_CORE) += \
		chains_vipSingleCameraDenseOpticalFlow_4eve_priv.c \
		chains_vipSingleCameraDenseOpticalFlow_1eve_priv.c \
		chains_vipSingleCameraDenseOpticalFlow.c

