
SRCDIR += $(vision_sdk_PATH)/examples/tda2xx/src/usecases/vip_single_cam_tlr

SRCS_$(IPU_PRIMARY_CORE) += \
        chains_vipSingleCameraTlr_priv.c \
        chains_vipSingleCameraTlr.c

