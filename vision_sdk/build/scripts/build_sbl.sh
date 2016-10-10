#!/bin/bash

CLEAN_BUILD=$1

cd ../../

if [ ! -z $CLEAN_BUILD  ]
then
    make -j -s VSDK_BOARD_TYPE=TDA2XX_EVM sbl_clean
    make -j -s VSDK_BOARD_TYPE=TDA2XX_MC  sbl_clean
    make -j -s VSDK_BOARD_TYPE=TDA2EX_EVM sbl_clean
    make -j -s VSDK_BOARD_TYPE=TDA3XX_EVM sbl_clean
fi

make -j -s VSDK_BOARD_TYPE=TDA2XX_EVM sbl_nor
make -j -s VSDK_BOARD_TYPE=TDA2XX_EVM sbl_sd
make -j -s VSDK_BOARD_TYPE=TDA2XX_EVM sbl_qspi

make -j -s VSDK_BOARD_TYPE=TDA2EX_EVM sbl_nor
make -j -s VSDK_BOARD_TYPE=TDA2EX_EVM sbl_qspi
make -j -s VSDK_BOARD_TYPE=TDA2EX_EVM sbl_sd

make -j -s VSDK_BOARD_TYPE=TDA2XX_MC  sbl_qspi
make -j -s VSDK_BOARD_TYPE=TDA2XX_MC  sbl_sd

make -j -s VSDK_BOARD_TYPE=TDA3XX_EVM sbl_qspi_sd
make -j -s VSDK_BOARD_TYPE=TDA3XX_EVM sbl_qspi

cd -

