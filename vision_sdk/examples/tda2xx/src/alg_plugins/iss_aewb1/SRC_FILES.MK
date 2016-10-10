
INCDIR += iss_aewb1/inc
SRCDIR += iss_aewb1

MODULE_SRC_BASE_PATH := $(vision_sdk_PATH)/examples/tda2xx/src/alg_plugins/

include $(MODULE_SRC_BASE_PATH)/iss_aewb1/src/SRC_FILES.MK

SRCS_$(IPU_PRIMARY_CORE) += issAewb1Link_algPlugin.c



