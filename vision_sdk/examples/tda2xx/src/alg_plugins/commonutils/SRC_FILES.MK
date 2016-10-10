

SRCDIR += commonutils

ifeq ($(SOCFAMILY),tda3xx)
    SRCS_c66xdsp_1 += ldc_lut_tda3x.c
    SRCS_c66xdsp_1 += ldc_lut_equisolid_2D.c

    SRCS_c66xdsp_2 += ldc_lut_tda3x.c
    SRCS_c66xdsp_2 += ldc_lut_equisolid_2D.c
else
    SRCS_c66xdsp_1 += ldc_lut_tda2x.c
    SRCS_c66xdsp_1 += ldc_lut_equisolid_2D.c

    SRCS_c66xdsp_2 += ldc_lut_tda2x.c
    SRCS_c66xdsp_2 += ldc_lut_equisolid_2D.c
endif

SRCS_c66xdsp_1 += calmat.c

SRCS_c66xdsp_2 += calmat.c


