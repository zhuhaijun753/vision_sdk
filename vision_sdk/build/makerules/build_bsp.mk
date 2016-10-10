#*******************************************************************************
#                                                                              *
# Copyright (c) 2013 Texas Instruments Incorporated - http://www.ti.com/       *
#                        ALL RIGHTS RESERVED                                   *
#                                                                              *
#*******************************************************************************

# File name: build_bsp.mk
#            This file builds the BSP libs.

bsp:
	$(MAKE) -C $(bsp_PATH)/ bsp 	INCLUDE_DUMMY_OSAL=no CORE=m4vpss  PROFILE_m4vpss=$(PROFILE_$(IPU_PRIMARY_CORE))
	$(MAKE) -C $(bsp_PATH)/src/osal INCLUDE_DUMMY_OSAL=no CORE=c66x    PROFILE_c66x=$(PROFILE_c66xdsp_1)
ifeq ($(PLATFORM), $(filter $(PLATFORM), tda2xx-evm tda2xx-mc tda3xx-evm tda3xx-rvp))
	$(MAKE) -C $(bsp_PATH)/src/osal INCLUDE_DUMMY_OSAL=no CORE=arp32_1 PROFILE_arp32_1=$(PROFILE_arp32_1)
endif
ifeq ($(PROC_A15_0_INCLUDE),yes)
ifeq ($(A15_TARGET_OS),Bios)
	$(MAKE) -C $(bsp_PATH)/src/osal INCLUDE_DUMMY_OSAL=no CORE=a15_0   PROFILE_a15_0=$(PROFILE_a15_0)
endif
endif

bsp_clean:
	$(MAKE) -C $(bsp_PATH)/ clean 	INCLUDE_DUMMY_OSAL=no CORE=m4vpss  PROFILE_m4vpss=$(PROFILE_$(IPU_PRIMARY_CORE))
	$(MAKE) -C $(bsp_PATH)/src/osal clean INCLUDE_DUMMY_OSAL=no CORE=c66x    PROFILE_c66x=$(PROFILE_c66xdsp_1)
ifeq ($(PLATFORM), $(filter $(PLATFORM), tda2xx-evm tda2xx-mc tda3xx-evm tda3xx-rvp))
	$(MAKE) -C $(bsp_PATH)/src/osal clean INCLUDE_DUMMY_OSAL=no CORE=arp32_1 PROFILE_arp32_1=$(PROFILE_arp32_1)
endif
ifeq ($(PROC_A15_0_INCLUDE),yes)
ifeq ($(A15_TARGET_OS),Bios)
	$(MAKE) -C $(bsp_PATH)/src/osal clean INCLUDE_DUMMY_OSAL=no CORE=a15_0   PROFILE_a15_0=$(PROFILE_a15_0)
endif
endif

