
SRCDIR += $(vision_sdk_PATH)/examples/tda2xx/src/draw2d

SRCS_$(IPU_PRIMARY_CORE) += draw2d_font00.c \
        draw2d_font01.c \
        draw2d_font02.c \
        draw2d_font03.c \
        draw2d_font04.c \
        draw2d_font05.c \
        draw2d_font06.c \
        draw2d_font07.c \
        draw2d_font08.c \
        draw2d_font09.c \
        draw2d_font14.c \
        draw2d_ti_logo.c \
        draw2d_ti_logo_1.c \
        lut16x16x0_25_129x129_0.c \
        lut16x16x0_25_129x129_1.c \
        draw2d_frontcam_logo.c \
        draw2d_sv_logo.c \
        draw2d_sv_logo_small.c \
        draw2d_3d_perception_sfm_logo.c \
        draw2d_jeep_Image_220x330.c \
        draw2d_jeep_Image_228x432.c \
        draw2d_jeep_Image_288x432.c \
        draw2d_ti_logo_small.c \
        draw2d_colorbar_20x720.c \
        draw2d_colorbar_35x450.c \
        draw2d_tda2x.c \
        draw2c_tl_green.c \
        draw2c_tl_red.c

SRCS_a15_0 += draw2d_font00.c \
        draw2d_font01.c \
        draw2d_font02.c \
        draw2d_font03.c \
        draw2d_font04.c \
        draw2d_font05.c \
        draw2d_font06.c \
        draw2d_font07.c \
        draw2d_font08.c \
        draw2d_font09.c \
        draw2d_font14.c \
        draw2d_ti_logo.c \
        draw2d_ti_logo_1.c \
        lut16x16x0_25_129x129_0.c \
        lut16x16x0_25_129x129_1.c \
        draw2d_frontcam_logo.c \
        draw2d_sv_logo.c \
        draw2d_sv_logo_small.c \
        draw2d_jeep_Image_220x330.c \
        draw2d_jeep_Image_228x432.c \
        draw2d_jeep_Image_288x432.c \
        draw2d_ti_logo_small.c \
        draw2d_colorbar_20x720.c \
        draw2d_colorbar_35x450.c \
        draw2c_tl_green.c \
        draw2c_tl_red.c

SRCS_COMMON += draw2d.c \
    draw2d_font_bmp.c \
    draw2d_font10_yuv.c \
    draw2d_font11_yuv.c \
    draw2d_font12_yuv.c \
    draw2d_font13_yuv.c
