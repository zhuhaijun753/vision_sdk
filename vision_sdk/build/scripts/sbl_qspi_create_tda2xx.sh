#!/bin/bash

export profile=$1
if [ -z $profile ]
then
    export profile=release
fi

export TICOMPONENTSBASE=$PWD/../../../

export STARTERWAREDIRNAME=$PWD/../../../ti_components/drivers/starterware_01_06_00_16

export Gcc_Tools_Path=$TICOMPONENTSBASE/ti_components/cg_tools/linux/gcc-arm-none-eabi-4_9-2015q3/bin

#REM <blank> or _opp_od or _opp_high
export OPPMODE=

export Sbl_Tools_Path=$STARTERWAREDIRNAME/bootloader/Tools/tiimage
export Sbl_Elf_Path=$STARTERWAREDIRNAME/binary/sbl_qspi$OPPMODE/bin/tda2xx
export Sbl_Elf_Path_hs=$STARTERWAREDIRNAME/binary/sbl_qspi$OPPMODE\_hs/bin/tda2xx
export Sbl_Out_Path=$PWD/qspi_tda2xx
export Sbl_Out_Path_hs=$PWD/qspi_tda2xx_hs

if [ -d $Sbl_Out_Path ]
then
    echo "$Sbl_Out_Path exists"
else
    mkdir $Sbl_Out_Path
fi
if [ -d $Sbl_Out_Path_hs ]
then
    echo "$Sbl_Out_Path_hs exists"
else
    mkdir $Sbl_Out_Path_hs
fi

rm -rf $Sbl_Out_Path/sbl_qspi

"$Gcc_Tools_Path/arm-none-eabi-objcopy" --gap-fill=0xff -O binary $Sbl_Elf_Path/sbl_qspi$OPPMODE\_a15host_$profile.xa15fg $Sbl_Out_Path/SBL.bin
"$Gcc_Tools_Path/arm-none-eabi-objcopy" --gap-fill=0xff -O binary $Sbl_Elf_Path_hs/sbl_qspi$OPPMODE\_hs_a15host_$profile.xa15fg $Sbl_Out_Path_hs/SBL.bin

"$Sbl_Tools_Path/tiimage" 0x40300000 BE $Sbl_Out_Path/SBL.bin $Sbl_Out_Path/SBL.tiimage
"$Sbl_Tools_Path/tiimage" 0xFFFFFFFF LE $Sbl_Out_Path_hs/SBL.bin $Sbl_Out_Path_hs/SBL.tiimage.LE

mv $Sbl_Out_Path/SBL.tiimage $Sbl_Out_Path/sbl_qspi
mv $Sbl_Out_Path_hs/SBL.tiimage.LE $Sbl_Out_Path_hs/unsigned_sbl_qspi_LE

rm $Sbl_Out_Path/SBL.bin
rm $Sbl_Out_Path_hs/SBL.bin

cp $STARTERWAREDIRNAME/binary/qspiFlashWriter/bin/tda2xx/qspiFlashWriter_m4_$profile.xem4 $Sbl_Out_Path/
