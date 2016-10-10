

SRCDIR += iss_aewb1/src

SRCS_AWB1 = algTIaewb.c TIawbCalcData_default.c ae_ti.c dcc_ti.c

SRCS_$(IPU_PRIMARY_CORE) += $(SRCS_AWB1)
