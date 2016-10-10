
SRCDIR += $(vision_sdk_PATH)/examples/tda2xx/src/usecases/vip_stereo_calibration

SRCS_$(IPU_PRIMARY_CORE) += \
		chains_vipStereoCalibration.c \
		chains_vipStereoCalibration_priv.c
