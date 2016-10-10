
SRCDIR += $(vision_sdk_PATH)/examples/tda2xx/src/usecases/vip_single_cam_edge_detection

SRCS_$(IPU_PRIMARY_CORE) += \
		chains_vipSingleCameraEdgeDetection.c \
		chains_vipSingleCameraEdgeDetection_priv.c

