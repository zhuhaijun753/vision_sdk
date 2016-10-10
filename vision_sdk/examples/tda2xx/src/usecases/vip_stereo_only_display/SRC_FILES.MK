
SRCDIR += $(vision_sdk_PATH)/examples/tda2xx/src/usecases/vip_stereo_only_display

SRCS_$(IPU_PRIMARY_CORE) += \
		chains_vipStereoOnlyDisplay.c \
		chains_vipStereoOnlyDisplay_priv.c
