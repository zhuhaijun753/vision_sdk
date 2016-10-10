
SRCDIR += $(vision_sdk_PATH)/examples/tda2xx/src/usecases/avbrx_sv

SRCS_$(IPU_PRIMARY_CORE) += \
		chains_avbRxSurroundView_priv.c \
		chains_avbRxSurroundView.c
