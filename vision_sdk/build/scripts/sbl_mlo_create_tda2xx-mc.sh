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
export Sbl_Elf_Path=$STARTERWAREDIRNAME/binary/sbl_sd$OPPMODE/bin/tda2xx-mc
export Sbl_Elf_Path_hs=$STARTERWAREDIRNAME/binary/sbl_sd$OPPMODE\_hs/bin/tda2xx-mc
export Sbl_Out_Path=$PWD/mlo_tda2xx-mc
export Sbl_Out_Path_hs=$PWD/mlo_tda2xx-mc_hs

if [ -d $Sbl_Out_Path ]
then
    echo "$Sbl_Out_Path exists"
else
    mkdir -p $Sbl_Out_Path
fi
if [ -d $Sbl_Out_Path_hs ]
then
    echo "$Sbl_Out_Path_hs exists"
else
    mkdir -p $Sbl_Out_Path_hs
fi

rm -rf $Sbl_Out_Path/MLO

"$Gcc_Tools_Path/arm-none-eabi-objcopy" --gap-fill=0xff -O binary $Sbl_Elf_Path/sbl_sd$OPPMODE\_a15host_$profile.xa15fg $Sbl_Out_Path/SBL.bin
"$Gcc_Tools_Path/arm-none-eabi-objcopy" --gap-fill=0xff -O binary $Sbl_Elf_Path_hs/sbl_sd$OPPMODE\_hs_a15host_$profile.xa15fg $Sbl_Out_Path_hs/SBL.bin

"$Sbl_Tools_Path/tiimage" 0x40300000 LE $Sbl_Out_Path/SBL.bin $Sbl_Out_Path/SBL.tiimage
"$Sbl_Tools_Path/tiimage" 0xFFFFFFFF LE $Sbl_Out_Path_hs/SBL.bin $Sbl_Out_Path_hs/SBL.tiimage

mv $Sbl_Out_Path/SBL.tiimage $Sbl_Out_Path/MLO
mv $Sbl_Out_Path_hs/SBL.tiimage $Sbl_Out_Path_hs/unsigned_MLO

rm -f $Sbl_Out_Path/SBL.bin
rm -f $Sbl_Out_Path_hs/SBL.bin


