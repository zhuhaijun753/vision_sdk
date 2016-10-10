
SRCDIR += $(vision_sdk_PATH)/examples/tda2xx/src/devices

SRCS_$(IPU_PRIMARY_CORE) += \
		video_sensor.c \
		lcd.c \
		hdmi_recvr.c \
		hdmi_tx.c
