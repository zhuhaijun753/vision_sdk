
SRCDIR += $(vision_sdk_PATH)/examples/tda2xx/src/usecases/common

SRCS_$(IPU_PRIMARY_CORE) += \
        chains_common.c \
        chains_common_surround_view.c \
        chains_common_stereo.c \
        chains_common_sfm.c \
        chains_common_fc_analytics2.c

