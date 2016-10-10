
SRCDIR += $(vision_sdk_PATH)/examples/tda2xx/src/common

ifeq ($(A15_TARGET_OS), Bios)
SRCS_$(IPU_PRIMARY_CORE) += \
		chains_main_bios.c 
endif

ifeq ($(A15_TARGET_OS), Linux)
SRCS_$(IPU_PRIMARY_CORE) += \
		chains_main_linux.c \
		chains_main_srv_calibration.c
endif
