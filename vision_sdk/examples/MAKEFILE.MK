
include $(ROOTDIR)/build/makerules/platform.mk

APP_LIBS_$(IPU_PRIMARY_CORE)   =$(DEST_ROOT)/lib/$(PLATFORM)/$(IPU_PRIMARY_CORE)/$(PROFILE_$(IPU_PRIMARY_CORE))/vision_sdk_examples.aem4 \
		   $(DEST_ROOT)/lib/$(PLATFORM)/m4/$(PROFILE_$(IPU_PRIMARY_CORE))/vision_sdk_alg_plugins.aem4

ifeq ($(ISS_INCLUDE),yes)
	APP_LIBS_$(IPU_PRIMARY_CORE) += $(vision_sdk_PATH)/examples/tda2xx/src/alg_plugins/iss_aewb1/lib/aewb_ipu1_0_$(PROFILE_ipu1_0).aem4
	APP_LIBS_$(IPU_PRIMARY_CORE) += $(vision_sdk_PATH)/examples/tda2xx/src/alg_plugins/iss_aewb1/lib/iss_dcc_ipu1_0_$(PROFILE_ipu1_0).aem4
endif

APP_LIBS_ipu1_1   =$(DEST_ROOT)/lib/$(PLATFORM)/m4/$(PROFILE_ipu1_1)/vision_sdk_alg_plugins.aem4
APP_LIBS_$(IPU_SECONDARY_CORE)     =$(DEST_ROOT)/lib/$(PLATFORM)/m4/$(PROFILE_$(IPU_SECONDARY_CORE))/vision_sdk_alg_plugins.aem4
APP_LIBS_c66xdsp_1=$(DEST_ROOT)/lib/$(PLATFORM)/66/$(PROFILE_c66xdsp_1)/vision_sdk_alg_plugins.ae66
APP_LIBS_c66xdsp_2=$(DEST_ROOT)/lib/$(PLATFORM)/66/$(PROFILE_c66xdsp_2)/vision_sdk_alg_plugins.ae66
APP_LIBS_arp32_1  =$(DEST_ROOT)/lib/$(PLATFORM)/arp32/$(PROFILE_arp32_1)/vision_sdk_alg_plugins.aearp32F
APP_LIBS_arp32_2  =$(DEST_ROOT)/lib/$(PLATFORM)/arp32/$(PROFILE_arp32_2)/vision_sdk_alg_plugins.aearp32F
APP_LIBS_arp32_3  =$(DEST_ROOT)/lib/$(PLATFORM)/arp32/$(PROFILE_arp32_3)/vision_sdk_alg_plugins.aearp32F
APP_LIBS_arp32_4  =$(DEST_ROOT)/lib/$(PLATFORM)/arp32/$(PROFILE_arp32_4)/vision_sdk_alg_plugins.aearp32F
APP_LIBS_a15_0    =$(DEST_ROOT)/lib/$(PLATFORM)/a15/$(PROFILE_a15_0)/vision_sdk_alg_plugins.aa15fg

# Algorithm libraries to be linked
APP_LIBS_c66xdsp_1 +=$(vision_sdk_PATH)/examples/tda2xx/src/alg_plugins/commonutils/lib/commonutils_c66xdsp_$(PROFILE_c66xdsp_1).ae66
APP_LIBS_c66xdsp_2 +=$(vision_sdk_PATH)/examples/tda2xx/src/alg_plugins/commonutils/lib/commonutils_c66xdsp_$(PROFILE_c66xdsp_2).ae66

APP_LIBS_c66xdsp_1 +=$(vision_sdk_PATH)/examples/tda2xx/src/alg_plugins/structurefrommotion/lib/structurefrommotion_c66xdsp_$(PROFILE_c66xdsp_1).ae66
APP_LIBS_c66xdsp_2 +=$(vision_sdk_PATH)/examples/tda2xx/src/alg_plugins/structurefrommotion/lib/structurefrommotion_c66xdsp_$(PROFILE_c66xdsp_2).ae66

APP_LIBS_c66xdsp_1 +=$(vision_sdk_PATH)/examples/tda2xx/src/alg_plugins/surroundview/lib/surroundview_c66xdsp_$(PROFILE_c66xdsp_1).ae66
APP_LIBS_c66xdsp_2 +=$(vision_sdk_PATH)/examples/tda2xx/src/alg_plugins/surroundview/lib/surroundview_c66xdsp_$(PROFILE_c66xdsp_2).ae66

APP_LIBS_c66xdsp_1 +=$(vision_sdk_PATH)/examples/tda2xx/src/alg_plugins/autocalibration/lib/autocalibration_c66xdsp_$(PROFILE_c66xdsp_1).ae66
APP_LIBS_c66xdsp_2 +=$(vision_sdk_PATH)/examples/tda2xx/src/alg_plugins/autocalibration/lib/autocalibration_c66xdsp_$(PROFILE_c66xdsp_2).ae66

APP_LIBS_arp32_1  +=$(vision_sdk_PATH)/examples/tda2xx/src/alg_plugins/edgedetection/lib/edgedetection_$(PROFILE_arp32_1).aearp32F
APP_LIBS_arp32_1  +=$(vision_sdk_PATH)/examples/tda2xx/src/alg_plugins/denseopticalflow/lib/libeveapps_opticalflow.eve.lib

APP_LIBS_arp32_2  +=$(vision_sdk_PATH)/examples/tda2xx/src/alg_plugins/edgedetection/lib/edgedetection_$(PROFILE_arp32_1).aearp32F
APP_LIBS_arp32_2  +=$(vision_sdk_PATH)/examples/tda2xx/src/alg_plugins/denseopticalflow/lib/libeveapps_opticalflow.eve.lib

APP_LIBS_arp32_3  +=$(vision_sdk_PATH)/examples/tda2xx/src/alg_plugins/edgedetection/lib/edgedetection_$(PROFILE_arp32_1).aearp32F
APP_LIBS_arp32_3  +=$(vision_sdk_PATH)/examples/tda2xx/src/alg_plugins/denseopticalflow/lib/libeveapps_opticalflow.eve.lib

APP_LIBS_arp32_4  +=$(vision_sdk_PATH)/examples/tda2xx/src/alg_plugins/edgedetection/lib/edgedetection_$(PROFILE_arp32_1).aearp32F
APP_LIBS_arp32_4  +=$(vision_sdk_PATH)/examples/tda2xx/src/alg_plugins/denseopticalflow/lib/libeveapps_opticalflow.eve.lib

ifeq ($(RADAR_INCLUDE),yes)
APP_LIBS_c66xdsp_1 +=$(vision_sdk_PATH)/examples/tda2xx/src/radar_fusion/alg_plugins/radarprocessing/lib/radar_dsp_c66xdsp_$(PROFILE_c66xdsp_1).ae66
APP_LIBS_c66xdsp_2 +=$(vision_sdk_PATH)/examples/tda2xx/src/radar_fusion/alg_plugins/radarprocessing/lib/radar_dsp_c66xdsp_$(PROFILE_c66xdsp_2).ae66
endif
ifeq ($(ENABLE_OPENCV),yes)
OPENCVAPP_LIBS_a15_0 =
OPENCVAPP_LIBS_a15_0 +=$(opencv_PATH)/opencv-3.1.0/libs_bios/libopencv_imgproc.a
OPENCVAPP_LIBS_a15_0 +=$(opencv_PATH)/opencv-3.1.0/libs_bios/libopencv_core.a
endif


export APP_LIBS_ipu1_0
export APP_LIBS_ipu1_1
export APP_LIBS_ipu2
export APP_LIBS_c66xdsp_1
export APP_LIBS_c66xdsp_2
export APP_LIBS_arp32_1
export APP_LIBS_arp32_2
export APP_LIBS_arp32_3
export APP_LIBS_arp32_4
export APP_LIBS_a15_0
export OPENCVAPP_LIBS_a15_0
export OPENCLAPP_LIBS_a15_0

examples:

ifeq ($(IPU_PRIMARY_CORE),ipu1_0)
ifeq ($(PROC_IPU1_0_INCLUDE),yes)
	$(MAKE) -C $(vision_sdk_PATH)/examples CORE=$(IPU_PRIMARY_CORE) $(TARGET)
endif
endif

ifeq ($(IPU_PRIMARY_CORE),ipu2)
ifeq ($(PROC_IPU2_INCLUDE),yes)
	$(MAKE) -C $(vision_sdk_PATH)/examples CORE=$(IPU_PRIMARY_CORE) $(TARGET)
endif
endif


apps:
	$(MAKE) -fMAKEFILE.MK -C $(vision_sdk_PATH)/src apps

clean:
	$(MAKE) -fMAKEFILE.MK examples TARGET=clean

all: clean examples


