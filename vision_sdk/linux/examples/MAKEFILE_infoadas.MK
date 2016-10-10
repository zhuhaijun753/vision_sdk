export fw_VERS=02_08_00_00
export fw_PATH=$(vision_sdk_PATH)/binaries/temp/visionsdk-fw

STRIP_M4  = $(CODEGEN_PATH_M4)/bin/armstrip -p
STRIP_DSP = $(CODEGEN_PATH_DSP)/bin/strip6x -p
STRIP_EVE = $(CODEGEN_PATH_EVE)/bin/strip-arp32 -p

demo:
	$(MAKE) -fMAKEFILE_infoadas.MK libs TARGET=depend
	$(MAKE) -fMAKEFILE_infoadas.MK libs TARGET=lib
	$(MAKE) -fMAKEFILE_infoadas.MK package

vision_sdk_linux_lib:
ifneq ($(TARGET),depend)
# Copy IPU1-0 generated memory map file to library include folder.
# If header file is not present then use previously generated file
	-cp $(vision_sdk_PATH)/binaries/obj/vision_sdk/$(PLATFORM)/$(IPU_PRIMARY_CORE)/$(PROFILE_$(IPU_PRIMARY_CORE))/vision_sdk_configuro/osa_mem_map.h $(vision_sdk_PATH)/linux/src/osa/include
endif
	$(MAKE) -fMAKEFILE.MK -C$(vision_sdk_PATH)/linux/src/links/ipcOut MODULE=vision_sdk_linux_lib $(TARGET)
	$(MAKE) -fMAKEFILE.MK -C$(vision_sdk_PATH)/linux/src/links/ipcIn  MODULE=vision_sdk_linux_lib $(TARGET)
	$(MAKE) -fMAKEFILE.MK -C$(vision_sdk_PATH)/linux/src/links/null  MODULE=vision_sdk_linux_lib $(TARGET)
	$(MAKE) -fMAKEFILE.MK -C$(vision_sdk_PATH)/linux/src/links/nullSrc  MODULE=vision_sdk_linux_lib $(TARGET)
	$(MAKE) -fMAKEFILE.MK -C$(vision_sdk_PATH)/linux/src/utils/multiproc MODULE=vision_sdk_linux_lib $(TARGET)
	$(MAKE) -fMAKEFILE_infoadas.MK -C$(vision_sdk_PATH)/linux/src/system MODULE=vision_sdk_linux_lib $(TARGET)
	$(MAKE) -fMAKEFILE.MK -C$(vision_sdk_PATH)/linux/src/system/sgxRenderUtils MODULE=vision_sdk_linux_lib $(TARGET)
	$(MAKE) -fMAKEFILE.MK -C$(vision_sdk_PATH)/linux/src/links/sgx3Dsrv MODULE=vision_sdk_linux_lib $(TARGET)
	$(MAKE) -fMAKEFILE.MK -C$(vision_sdk_PATH)/linux/src/osa/src MODULE=vision_sdk_linux_lib $(TARGET)
	$(MAKE) -fMAKEFILE.MK -C$(vision_sdk_PATH)/linux/examples/common MODULE=vision_sdk_linux_lib $(TARGET)
	$(MAKE) -fMAKEFILE.MK -C$(vision_sdk_PATH)/linux/examples/dra7xx/src/ti-plugins MODULE=vision_sdk_linux_lib $(TARGET)
	$(MAKE) -fMAKEFILE.MK -C$(vision_sdk_PATH)/linux/examples/dra7xx/src/common MODULE=vision_sdk_linux_lib $(TARGET)
	$(MAKE) -fMAKEFILE.MK -C$(vision_sdk_PATH)/linux/examples/dra7xx/src/links/ep MODULE=vision_sdk_linux_lib $(TARGET)
	$(MAKE) -fMAKEFILE.MK -C$(vision_sdk_PATH)/linux/examples/dra7xx/src/chains/lvds_vip_2d_srv MODULE=vision_sdk_linux_lib $(TARGET)
	$(MAKE) -fMAKEFILE.MK -C$(vision_sdk_PATH)/linux/examples/dra7xx/src/chains/lvds_vip_3d_srv MODULE=vision_sdk_linux_lib $(TARGET)
	$(MAKE) -fMAKEFILE.MK -C$(vision_sdk_PATH)/linux/examples/dra7xx/src/chains/lvds_vip_single_cam MODULE=vision_sdk_linux_lib $(TARGET)
	$(MAKE) -fMAKEFILE.MK -C$(vision_sdk_PATH)/linux/examples/dra7xx/src/chains/lvds_vip_multi_cam MODULE=vision_sdk_linux_lib $(TARGET)
#	$(MAKE) -fMAKEFILE.MK -C$(vision_sdk_PATH)/linux/examples/dra7xx/src/chains/buf_loop MODULE=vision_sdk_linux_lib $(TARGET)


libs: vision_sdk_linux_lib 


package:
	mkdir -p $(fw_PATH)
	mkdir -p $(fw_PATH)/lib
	mkdir -p $(fw_PATH)/include
	mkdir -p $(fw_PATH)/fw
	mkdir -p $(fw_PATH)/utils

	$(shell $(vision_sdk_PATH)/linux/examples/dra7xx/fw-version.sh)
	mv `pwd`/VERSION.txt $(fw_PATH)/

ifeq ($(PROC_IPU1_0_INCLUDE),yes)
	cp $(vision_sdk_PATH)/binaries/vision_sdk/bin/$(PLATFORM)/vision_sdk_ipu1_0_$(PROFILE_ipu1_0).xem4 $(fw_PATH)/fw/dra7-ipu1-fw.xem4
	$(STRIP_M4) $(fw_PATH)/fw/dra7-ipu1-fw.xem4
endif
ifeq ($(PROC_IPU2_INCLUDE),yes)
	cp $(vision_sdk_PATH)/binaries/vision_sdk/bin/$(PLATFORM)/vision_sdk_ipu2_$(PROFILE_ipu2).xem4 $(fw_PATH)/fw/dra7-ipu2-fw.xem4
	$(STRIP_M4) $(fw_PATH)/fw/dra7-ipu2-fw.xem4
endif
ifeq ($(PROC_DSP1_INCLUDE),yes)
	cp $(vision_sdk_PATH)/binaries/vision_sdk/bin/$(PLATFORM)/vision_sdk_c66xdsp_1_$(PROFILE_c66xdsp_1).xe66 $(fw_PATH)/fw/dra7-dsp1-fw.xe66
	$(STRIP_DSP) $(fw_PATH)/fw/dra7-dsp1-fw.xe66
endif
ifeq ($(PROC_DSP2_INCLUDE),yes)
	cp $(vision_sdk_PATH)/binaries/vision_sdk/bin/$(PLATFORM)/vision_sdk_c66xdsp_2_$(PROFILE_c66xdsp_2).xe66 $(fw_PATH)/fw/dra7-dsp2-fw.xe66
	$(STRIP_DSP) $(fw_PATH)/fw/dra7-dsp2-fw.xe66
endif
	#copy libadas.a
	cp $(vision_sdk_PATH)/binaries/lib/a15/debug/vision_sdk_linux_lib.a $(fw_PATH)/lib/libadas.a
	cp $(pvr_PATH)/Tools/OGLES2/Build/Linux_armv7hf/ReleaseNullWS/libogles2tools.a $(fw_PATH)/lib/libogles2tools.a
	cp $(vision_sdk_PATH)/linux/src/system/sgxRenderUtils/jeep2.bmp $(fw_PATH)/utils
	cp $(vision_sdk_PATH)/linux/src/system/sgxRenderUtils/car.pod $(fw_PATH)/utils
	cp $(vision_sdk_PATH)/linux/src/system/sgxRenderUtils/VertShader.vsh $(fw_PATH)/utils
	cp $(vision_sdk_PATH)/linux/src/system/sgxRenderUtils/FragShader.fsh $(fw_PATH)/utils

	#copy header files
	cp $(vision_sdk_PATH)/include/link_api/system_const.h $(fw_PATH)/include/
	cp $(vision_sdk_PATH)/include/link_api/system_link_info.h $(fw_PATH)/include/
	cp $(vision_sdk_PATH)/include/link_api/system_buffer.h $(fw_PATH)/include/
	cp $(vision_sdk_PATH)/linux/src/osa/include/osa_types.h $(fw_PATH)/include/
	cp $(vision_sdk_PATH)/linux/src/osa/include/osa_debug.h $(fw_PATH)/include/
	cp $(vision_sdk_PATH)/linux/src/osa/include/osa_mem_shared.h $(fw_PATH)/include/

	@echo \# Creating tar for firmwares !!!
	tar -czf $(vision_sdk_PATH)/binaries/visionsdk-fw-$(fw_VERS).tar.gz  -C $(fw_PATH)/.. .
	rm -Rf $(fw_PATH)/

clean:
	$(MAKE) -fMAKEFILE_infoadas.MK libs TARGET=clean

all: clean demo

.PHONY: libs clean demo all vision_sdk_linux_lib 

