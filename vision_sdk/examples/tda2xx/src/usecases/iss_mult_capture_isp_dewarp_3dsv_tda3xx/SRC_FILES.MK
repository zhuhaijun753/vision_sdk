
SRCDIR += $(vision_sdk_PATH)/examples/tda2xx/src/usecases/iss_mult_capture_isp_dewarp_3dsv_tda3xx

SRCS_$(IPU_PRIMARY_CORE) += \
		chains_issMultCaptIspDeWarp3dSv_Display.c \
		chains_issMultCaptIspDeWarp3dSv_priv.c \
		chains_issMultCaptIspDeWarp3dSv_grpx_priv.c \
		chains_issMultCaptIspDeWarp3dSv_Media.c
