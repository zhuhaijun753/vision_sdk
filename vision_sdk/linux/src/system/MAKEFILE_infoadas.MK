
include $(vision_sdk_PATH)/linux/build/common_header_a15.mk


INCLUDE+= $(LINUX_COMMON_INC) -I$(ipc_PATH)/linux/include -I$(ipc_PATH)/hlos_common/include -I$(ipc_PATH)/packages -I$(kernel_PATH)/include/generated/uapi 
INCLUDE+= $(LINK_API_INC) -I$(vision_sdk_PATH)/linux/src/osa/include
INCLUDE+= $(SGX_DRM_INC) -I$(LINUX_TARGETFS)/usr/include/omap
ifeq ($(BUILD_INFOADAS), yes)
DEFINE += -DBUILD_INFOADAS
INCLUDE+=-I$(infoadas_PATH)/include
endif
include $(vision_sdk_PATH)/linux/build/common_footer_a15.mk


