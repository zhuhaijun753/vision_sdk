
SRCDIR += $(vision_sdk_PATH)/examples/tda2xx/src/usecases/vip_stereo_cam_analytics2

SRCS_$(IPU_PRIMARY_CORE) += \
        chains_singleCameraAnalytics_tda2xx.c \
        chains_fileRxCameraAnalytics_tda2xx_priv.c \
        chains_fileRxCameraAnalytics_tda2xx.c
