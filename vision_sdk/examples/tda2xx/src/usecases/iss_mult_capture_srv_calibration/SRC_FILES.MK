
SRCDIR += $(vision_sdk_PATH)/examples/tda2xx/src/usecases/iss_mult_capture_srv_calibration

SRCS_$(IPU_PRIMARY_CORE) += \
        chains_srvCalibration_Display_tda2x_priv.c \
        chains_srvCalibration_Display_tda3x_priv.c \
        chains_srvCalibration_Display.c


