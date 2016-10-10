
SRCDIR += $(vision_sdk_PATH)/examples/tda2xx/src/modules/ultrasonic_capture

SRCS_$(IPU_PRIMARY_CORE) += \
		ultrasonicCaptureLink_tsk.c \
		ultrasonicCaptureLink_hw.c \

