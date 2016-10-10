
SRCDIR += $(vision_sdk_PATH)/examples/tda2xx/src/usecases/vip_single_cam_analytics2

SRCS_$(IPU_PRIMARY_CORE) += \
		chains_vipSingleCameraAnalytics2_priv.c \
		chains_vipSingleCameraAnalytics2.c
