include $(vision_sdk_PATH)/linux/build/common_header_a15.mk

INCLUDE+= $(LINUX_COMMON_INC) -I$(ipc_PATH)/linux/include -I$(ipc_PATH)/hlos_common/include -I$(ipc_PATH)/packages -I$(kernel_PATH)/include/generated/uapi
INCLUDE+= $(LINK_API_INC) -I$(vision_sdk_PATH)/linux/src/osa/include
INCLUDE+= $(SGX_DRM_INC) -I./include/gbm -I$(pvr_PATH)/Tools -I$(pvr_PATH)/Tools/OGLES2 -I$(pvr_PATH)/Include
INCLUDE+= -I$(infoadas_PATH)/include -I$(infoadas_PATH)/ti-plugins/include -I$(infoadas_PATH)/ti-plugins/srv

include $(vision_sdk_PATH)/linux/build/common_footer_a15.mk
