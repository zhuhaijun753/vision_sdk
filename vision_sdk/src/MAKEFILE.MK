# Disable XDC package build by default
ENABLE_XDC_PKG_BUILD ?= false

XDC_BUILD_PKG_LIST_COMMON +=
XDC_BUILD_PKG_LIST_ipu1_0 += $(XDC_BUILD_PKG_LIST_COMMON)
XDC_BUILD_PKG_LIST_ipu1_0 += $(fc_PATH)/packages/ti.sdo.fc.ires.hdvicp
XDC_BUILD_PKG_LIST_ipu1_0 += $(fc_PATH)/packages/ti.sdo.fc.dskt2
XDC_BUILD_PKG_LIST_ipu1_1 += $(XDC_BUILD_PKG_LIST_COMMON)
XDC_BUILD_PKG_LIST_ipu2   += $(XDC_BUILD_PKG_LIST_COMMON)
XDC_BUILD_PKG_LIST_ipu2   += $(fc_PATH)/packages/ti.sdo.fc.ires.hdvicp
XDC_BUILD_PKG_LIST_ipu2   += $(fc_PATH)/packages/ti.sdo.fc.dskt2
XDC_BUILD_PKG_LIST_c66xdsp_1 += $(XDC_BUILD_PKG_LIST_COMMON)
XDC_BUILD_PKG_LIST_c66xdsp_2 += $(XDC_BUILD_PKG_LIST_COMMON)
XDC_BUILD_PKG_LIST_arp32_1 += $(XDC_BUILD_PKG_LIST_COMMON)
XDC_BUILD_PKG_LIST_arp32_2 += $(XDC_BUILD_PKG_LIST_COMMON)
XDC_BUILD_PKG_LIST_arp32_3 += $(XDC_BUILD_PKG_LIST_COMMON)
XDC_BUILD_PKG_LIST_arp32_4 += $(XDC_BUILD_PKG_LIST_COMMON)
XDC_BUILD_PKG_LIST_a15_0 += $(XDC_BUILD_PKG_LIST_COMMON)


include $(ROOTDIR)/build/makerules/platform.mk
include $(ROOTDIR)/build/makerules/env.mk

.PHONY: apps_ipu1_eveloader apps_ipu1_0 apps_ipu2
# Force EVELOADER to run sequentially
# For IPU1_0 & IPU2 to run only after apps_ipu1_eveloader - separate lines to allow "-j" to work correctly
apps_parallel: apps_eve1 apps_eve2 apps_eve3 apps_eve4 apps_dsp1 apps_dsp2 apps_ipu1_1 apps_a15

apps:
ifeq ($(BUILD_OS), Linux)
	$(MAKE) -fMAKEFILE.MK apps_parallel
else
	$(MAKE) -fMAKEFILE.MK apps_eve1
	$(MAKE) -fMAKEFILE.MK apps_eve2
	$(MAKE) -fMAKEFILE.MK apps_eve3
	$(MAKE) -fMAKEFILE.MK apps_eve4
	$(MAKE) -fMAKEFILE.MK apps_dsp1
	$(MAKE) -fMAKEFILE.MK apps_dsp2
	$(MAKE) -fMAKEFILE.MK apps_ipu1_1
	$(MAKE) -fMAKEFILE.MK apps_a15
endif
	$(MAKE) -fMAKEFILE.MK apps_ipu1_eveloader
ifeq ($(BUILD_OS), Linux)
	$(MAKE) -fMAKEFILE.MK apps_ipu1_0 apps_ipu2
else
	$(MAKE) -fMAKEFILE.MK apps_ipu1_0
	$(MAKE) -fMAKEFILE.MK apps_ipu2
endif

apps_eve1:
ifeq ($(PROC_EVE1_INCLUDE),yes)
ifeq ($(ENABLE_XDC_PKG_BUILD),true)
	$(xdc_PATH)/xdc XDCBUILDCFG=$(vision_sdk_PATH)/build/$(SOC)/config_arp32.bld XDCARGS="profile=$(PROFILE_arp32_1)" -PD $(subst .,/,$(XDC_BUILD_PKG_LIST_arp32_1))
endif
	$(MAKE) -C $(vision_sdk_PATH)/src/main_app/$(SOC)/eve1 xdc_configuro CORE=arp32_1
	$(MAKE) -C $(vision_sdk_PATH)/src/main_app/$(SOC)/eve1 CORE=arp32_1
endif

apps_eve2:
ifeq ($(PROC_EVE2_INCLUDE),yes)
ifeq ($(ENABLE_XDC_PKG_BUILD),true)
	$(xdc_PATH)/xdc XDCBUILDCFG=$(vision_sdk_PATH)/build/$(SOC)/config_arp32.bld XDCARGS="profile=$(PROFILE_arp32_2)" -PD $(subst .,/,$(XDC_BUILD_PKG_LIST_arp32_2))
endif
	$(MAKE) -C $(vision_sdk_PATH)/src/main_app/$(SOC)/eve2 xdc_configuro CORE=arp32_2
	$(MAKE) -C $(vision_sdk_PATH)/src/main_app/$(SOC)/eve2 CORE=arp32_2
endif

apps_eve3:
ifeq ($(PROC_EVE3_INCLUDE),yes)
ifeq ($(ENABLE_XDC_PKG_BUILD),true)
	$(xdc_PATH)/xdc XDCBUILDCFG=$(vision_sdk_PATH)/build/$(SOC)/config_arp32.bld XDCARGS="profile=$(PROFILE_arp32_3)" -PD $(subst .,/,$(XDC_BUILD_PKG_LIST_arp32_3))
endif
	$(MAKE) -C $(vision_sdk_PATH)/src/main_app/$(SOC)/eve3 xdc_configuro CORE=arp32_3
	$(MAKE) -C $(vision_sdk_PATH)/src/main_app/$(SOC)/eve3 CORE=arp32_3
endif

apps_eve4:
ifeq ($(PROC_EVE4_INCLUDE),yes)
ifeq ($(ENABLE_XDC_PKG_BUILD),true)
	$(xdc_PATH)/xdc XDCBUILDCFG=$(vision_sdk_PATH)/build/$(SOC)/config_arp32.bld XDCARGS="profile=$(PROFILE_arp32_4)" -PD $(subst .,/,$(XDC_BUILD_PKG_LIST_arp32_4))
endif
	$(MAKE) -C $(vision_sdk_PATH)/src/main_app/$(SOC)/eve4 xdc_configuro CORE=arp32_4
	$(MAKE) -C $(vision_sdk_PATH)/src/main_app/$(SOC)/eve4 CORE=arp32_4
endif

apps_ipu1_eveloader:
ifeq ($(IPU1_EVELOADER_INCLUDE), yes)
	$(MAKE) -f $(vision_sdk_PATH)/build/makerules/build_appImageHeader.mk build_AppImageHeader
endif

apps_ipu1_1:
ifeq ($(PROC_IPU1_1_INCLUDE),yes)
ifeq ($(ENABLE_XDC_PKG_BUILD),true)
	$(xdc_PATH)/xdc XDCBUILDCFG=$(vision_sdk_PATH)/build/$(SOC)/config_m4.bld XDCARGS="profile=$(PROFILE_ipu1_1)" -PD $(subst .,/,$(XDC_BUILD_PKG_LIST_ipu1_1))
endif
	$(MAKE) -C $(vision_sdk_PATH)/src/main_app/$(SOC)/ipu1_1 xdc_configuro CORE=ipu1_1
	$(MAKE) -C $(vision_sdk_PATH)/src/main_app/$(SOC)/ipu1_1 CORE=ipu1_1
endif

apps_dsp1:
ifeq ($(PROC_DSP1_INCLUDE),yes)
ifeq ($(ENABLE_XDC_PKG_BUILD),true)
	$(xdc_PATH)/xdc XDCBUILDCFG=$(vision_sdk_PATH)/build/$(SOC)/config_c66.bld XDCARGS="profile=$(PROFILE_c66xdsp_1)" -PD $(subst .,/,$(XDC_BUILD_PKG_LIST_c66xdsp_1))
endif
	$(MAKE) -C $(vision_sdk_PATH)/src/main_app/$(SOC)/dsp1 xdc_configuro CORE=c66xdsp_1
	$(MAKE) -C $(vision_sdk_PATH)/src/main_app/$(SOC)/dsp1 CORE=c66xdsp_1
endif

apps_dsp2:
ifeq ($(PROC_DSP2_INCLUDE),yes)
ifeq ($(ENABLE_XDC_PKG_BUILD),true)
	$(xdc_PATH)/xdc XDCBUILDCFG=$(vision_sdk_PATH)/build/$(SOC)/config_c66.bld XDCARGS="profile=$(PROFILE_c66xdsp_2)" -PD $(subst .,/,$(XDC_BUILD_PKG_LIST_c66xdsp_2))
endif
	$(MAKE) -C $(vision_sdk_PATH)/src/main_app/$(SOC)/dsp2 xdc_configuro CORE=c66xdsp_2
	$(MAKE) -C $(vision_sdk_PATH)/src/main_app/$(SOC)/dsp2 CORE=c66xdsp_2
endif

apps_a15:
ifeq ($(PROC_A15_0_INCLUDE),yes)
ifeq ($(A15_TARGET_OS),Bios)
ifeq ($(ENABLE_XDC_PKG_BUILD),true)
	$(xdc_PATH)/xdc XDCBUILDCFG=$(vision_sdk_PATH)/build/$(SOC)/config_a15.bld XDCARGS="profile=$(PROFILE_a15_0)" -PD $(subst .,/,$(XDC_BUILD_PKG_LIST_a15_0))
endif
	$(MAKE) -C $(vision_sdk_PATH)/src/main_app/$(SOC)/a15_0 xdc_configuro CORE=a15_0
	$(MAKE) -C $(vision_sdk_PATH)/src/main_app/$(SOC)/a15_0 CORE=a15_0
endif
endif

apps_ipu1_0:
ifeq ($(PROC_IPU1_0_INCLUDE),yes)
ifeq ($(ENABLE_XDC_PKG_BUILD),true)
	$(xdc_PATH)/xdc XDCBUILDCFG=$(vision_sdk_PATH)/build/$(SOC)/config_m4.bld XDCARGS="profile=$(PROFILE_ipu1_0)" -PD $(subst .,/,$(XDC_BUILD_PKG_LIST_ipu1_0))
endif
	$(MAKE) -C $(vision_sdk_PATH)/src/main_app/$(SOC)/ipu1_0 xdc_configuro CORE=ipu1_0
	$(MAKE) -C $(vision_sdk_PATH)/src/main_app/$(SOC)/ipu1_0 CORE=ipu1_0
endif

apps_ipu2:
ifeq ($(PROC_IPU2_INCLUDE),yes)
ifeq ($(ENABLE_XDC_PKG_BUILD),true)
	$(xdc_PATH)/xdc XDCBUILDCFG=$(vision_sdk_PATH)/build/$(SOC)/config_m4.bld XDCARGS="profile=$(PROFILE_ipu2)" -PD $(subst .,/,$(XDC_BUILD_PKG_LIST_ipu2))
endif
	$(MAKE) -C $(vision_sdk_PATH)/src/main_app/$(SOC)/ipu2 xdc_configuro CORE=ipu2
	$(MAKE) -C $(vision_sdk_PATH)/src/main_app/$(SOC)/ipu2 CORE=ipu2
endif

libs:
ifeq ($(PROC_EVE1_INCLUDE),yes)
	$(MAKE) -C $(vision_sdk_PATH)/src CORE=arp32_1
endif
ifeq ($(PROC_EVE2_INCLUDE),yes)
	$(MAKE) -C $(vision_sdk_PATH)/src CORE=arp32_2
endif
ifeq ($(PROC_EVE3_INCLUDE),yes)
	$(MAKE) -C $(vision_sdk_PATH)/src CORE=arp32_3
endif
ifeq ($(PROC_EVE4_INCLUDE),yes)
	$(MAKE) -C $(vision_sdk_PATH)/src CORE=arp32_4
endif
ifeq ($(PROC_IPU1_0_INCLUDE),yes)
	$(MAKE) -C $(vision_sdk_PATH)/src CORE=ipu1_0
endif
ifeq ($(PROC_IPU1_1_INCLUDE),yes)
	$(MAKE) -C $(vision_sdk_PATH)/src CORE=ipu1_1
endif
ifeq ($(PROC_IPU2_INCLUDE),yes)
	$(MAKE) -C $(vision_sdk_PATH)/src CORE=ipu2
endif
ifeq ($(PROC_DSP1_INCLUDE),yes)
	$(MAKE) -C $(vision_sdk_PATH)/src CORE=c66xdsp_1
endif
ifeq ($(PROC_DSP2_INCLUDE),yes)
	$(MAKE) -C $(vision_sdk_PATH)/src CORE=c66xdsp_2
endif
ifeq ($(PROC_A15_0_INCLUDE),yes)
ifeq ($(A15_TARGET_OS),Bios)
	$(MAKE) -C $(vision_sdk_PATH)/src CORE=a15_0
endif
endif

clean:
ifeq ($(PROC_EVE1_INCLUDE),yes)
	$(MAKE) -C $(vision_sdk_PATH)/src clean CORE=arp32_1
	$(MAKE) -C $(vision_sdk_PATH)/src/main_app/$(SOC)/eve1 clean CORE=arp32_1
endif
ifeq ($(PROC_EVE2_INCLUDE),yes)
	$(MAKE) -C $(vision_sdk_PATH)/src clean CORE=arp32_2
	$(MAKE) -C $(vision_sdk_PATH)/src/main_app/$(SOC)/eve2 clean CORE=arp32_2
endif
ifeq ($(PROC_EVE3_INCLUDE),yes)
	$(MAKE) -C $(vision_sdk_PATH)/src clean CORE=arp32_3
	$(MAKE) -C $(vision_sdk_PATH)/src/main_app/$(SOC)/eve3 clean CORE=arp32_3
endif
ifeq ($(PROC_EVE4_INCLUDE),yes)
	$(MAKE) -C $(vision_sdk_PATH)/src clean CORE=arp32_4
	$(MAKE) -C $(vision_sdk_PATH)/src/main_app/$(SOC)/eve4 clean CORE=arp32_4
endif
ifeq ($(PROC_IPU1_0_INCLUDE),yes)
	$(MAKE) -C $(vision_sdk_PATH)/src clean CORE=ipu1_0
	$(MAKE) -C $(vision_sdk_PATH)/src/main_app/$(SOC)/ipu1_0 clean CORE=ipu1_0
endif
ifeq ($(PROC_IPU1_1_INCLUDE),yes)
	$(MAKE) -C $(vision_sdk_PATH)/src clean CORE=ipu1_1
	$(MAKE) -C $(vision_sdk_PATH)/src/main_app/$(SOC)/ipu1_1 clean CORE=ipu1_1
endif
ifeq ($(PROC_IPU2_INCLUDE),yes)
	$(MAKE) -C $(vision_sdk_PATH)/src clean CORE=ipu2
	$(MAKE) -C $(vision_sdk_PATH)/src/main_app/$(SOC)/ipu2 clean CORE=ipu2
endif
	$(MAKE) -C $(vision_sdk_PATH)/src clean CORE=c66xdsp_1
	$(MAKE) -C $(vision_sdk_PATH)/src/main_app/$(SOC)/dsp1 clean CORE=c66xdsp_1
ifeq ($(PROC_DSP2_INCLUDE),yes)
	$(MAKE) -C $(vision_sdk_PATH)/src clean CORE=c66xdsp_2
	$(MAKE) -C $(vision_sdk_PATH)/src/main_app/$(SOC)/dsp2 clean CORE=c66xdsp_2
endif
ifeq ($(PROC_A15_0_INCLUDE),yes)
	$(MAKE) -C $(vision_sdk_PATH)/src clean CORE=a15_0
	$(MAKE) -C $(vision_sdk_PATH)/src/main_app/$(SOC)/a15_0 clean CORE=a15_0
endif

ifeq ($(ENABLE_XDC_PKG_BUILD),true)
	$(xdc_PATH)/xdc clean XDCBUILDCFG=$(vision_sdk_PATH)/build/$(SOC)/config_m4.bld -PD $(subst .,/,$(XDC_BUILD_PKG_LIST_ipu1_0))
endif

ifeq ($(ENABLE_XDC_PKG_BUILD),true)
	$(xdc_PATH)/xdc clean XDCBUILDCFG=$(vision_sdk_PATH)/build/$(SOC)/config_m4.bld -PD $(subst .,/,$(XDC_BUILD_PKG_LIST_ipu1_1))
endif

ifeq ($(ENABLE_XDC_PKG_BUILD),true)
	$(xdc_PATH)/xdc clean XDCBUILDCFG=$(vision_sdk_PATH)/build/$(SOC)/config_m4.bld -PD $(subst .,/,$(XDC_BUILD_PKG_LIST_ipu2))
endif

ifeq ($(ENABLE_XDC_PKG_BUILD),true)
	$(xdc_PATH)/xdc clean XDCBUILDCFG=$(vision_sdk_PATH)/build/$(SOC)/config_c66.bld -PD $(subst .,/,$(XDC_BUILD_PKG_LIST_c66xdsp_1))
endif

ifeq ($(ENABLE_XDC_PKG_BUILD),true)
	$(xdc_PATH)/xdc clean XDCBUILDCFG=$(vision_sdk_PATH)/build/$(SOC)/config_c66.bld -PD $(subst .,/,$(XDC_BUILD_PKG_LIST_c66xdsp_2))
endif

ifeq ($(ENABLE_XDC_PKG_BUILD),true)
	$(xdc_PATH)/xdc clean XDCBUILDCFG=$(vision_sdk_PATH)/build/$(SOC)/config_arp32.bld -PD $(subst .,/,$(XDC_BUILD_PKG_LIST_arp32_1))
endif

ifeq ($(ENABLE_XDC_PKG_BUILD),true)
	$(xdc_PATH)/xdc clean XDCBUILDCFG=$(vision_sdk_PATH)/build/$(SOC)/config_arp32.bld -PD $(subst .,/,$(XDC_BUILD_PKG_LIST_arp32_2))
endif

ifeq ($(ENABLE_XDC_PKG_BUILD),true)
	$(xdc_PATH)/xdc clean XDCBUILDCFG=$(vision_sdk_PATH)/build/$(SOC)/config_arp32.bld -PD $(subst .,/,$(XDC_BUILD_PKG_LIST_arp32_3))
endif

ifeq ($(ENABLE_XDC_PKG_BUILD),true)
	$(xdc_PATH)/xdc clean XDCBUILDCFG=$(vision_sdk_PATH)/build/$(SOC)/config_arp32.bld -PD $(subst .,/,$(XDC_BUILD_PKG_LIST_arp32_4))
endif

ifeq ($(ENABLE_XDC_PKG_BUILD),true)
	$(xdc_PATH)/xdc clean XDCBUILDCFG=$(vision_sdk_PATH)/build/$(SOC)/config_a15.bld -PD $(subst .,/,$(XDC_BUILD_PKG_LIST_a15_0))
endif

all: clean
	$(MAKE) -fMAKEFILE.MK apps
