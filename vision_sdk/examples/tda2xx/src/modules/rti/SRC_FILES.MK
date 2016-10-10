
SRCDIR += $(vision_sdk_PATH)/examples/tda2xx/src/modules/rti/src

SRCS_$(IPU_PRIMARY_CORE)     += rtiLink_tsk.c
SRCS_ipu1_1     += rtiLink_tsk.c
SRCS_$(IPU_SECONDARY_CORE)       += rtiLink_tsk.c
SRCS_c66xdsp_1  += rtiLink_tsk.c
SRCS_c66xdsp_2  += rtiLink_tsk.c
SRCS_arp32_1    += rtiLink_tsk.c
