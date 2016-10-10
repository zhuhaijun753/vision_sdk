#!/bin/bash

export profile=$1
if [ -z $profile ]
then
    export profile=release
fi

export TICOMPONENTSBASE=$PWD/../../../

export STARTERWAREDIRNAME=$PWD/../../../ti_components/drivers/starterware_01_06_00_16

export Gcc_Tools_Path=$TICOMPONENTSBASE/ti_components/cg_tools/linux/gcc-arm-none-eabi-4_7-2013q3/bin

export Sbl_Tools_Path=$STARTERWAREDIRNAME/bootloader/Tools/tiimage
export Sbl_Elf_Path=$STARTERWAREDIRNAME/binary/sbl_qspi/bin/tda3xx-evm
export Sbl_Out_Path=$PWD/qspi_tda3xx

if [ -d $Sbl_Out_Path ]
then
    echo "$Sbl_Out_Path exists"
else
    mkdir $Sbl_Out_Path
fi

rm -rf $Sbl_Out_Path/sbl_qspi

"$Gcc_Tools_Path/arm-none-eabi-objcopy" --gap-fill=0xff -O binary $Sbl_Elf_Path/sbl_qspi_m4_$profile.xem4 $Sbl_Out_Path/SBL.bin

"$Sbl_Tools_Path/tiimage" 0x00300000 BE $Sbl_Out_Path/SBL.bin $Sbl_Out_Path/SBL.tiimage
"$Sbl_Tools_Path/tiimage" 0x00300000 LE $Sbl_Out_Path/SBL.bin $Sbl_Out_Path/SBL.tiimage.LE

mv $Sbl_Out_Path/SBL.tiimage $Sbl_Out_Path/sbl_qspi
mv $Sbl_Out_Path/SBL.tiimage.LE $Sbl_Out_Path/sbl_qspi_LE

rm $Sbl_Out_Path/SBL.bin

cp $STARTERWAREDIRNAME/binary/qspiFlashWriter/bin/tda3xx-evm/qspiFlashWriter_m4_$profile.xem4 $Sbl_Out_Path/

