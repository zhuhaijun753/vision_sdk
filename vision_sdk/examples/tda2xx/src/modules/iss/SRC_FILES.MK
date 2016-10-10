
SRCDIR += $(vision_sdk_PATH)/examples/tda2xx/src/modules/iss
SRCDIR += $(vision_sdk_PATH)/examples/tda2xx/src/modules/iss/iss_sensors


SRCS_$(IPU_PRIMARY_CORE) += chains_iss_sensors.c \
               chains_common_iss.c \
               iss_sensor_ar0132.c \
               iss_sensor_ar0140.c \
               iss_sensor_ov10640.c \
               iss_sensor_imx224.c 

