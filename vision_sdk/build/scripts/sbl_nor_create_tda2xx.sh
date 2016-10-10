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
export Sbl_Elf_Path=$STARTERWAREDIRNAME/binary/sbl_nor$OPPMODE/bin/tda2xx
export Sbl_Out_Path=$PWD/nor_tda2xx

if [ -d $Sbl_Out_Path ]
then
    echo "$Sbl_Out_Path exists"
else
    mkdir $Sbl_Out_Path
fi

rm -rf $Sbl_Out_Path/sbl_nor

"$Gcc_Tools_Path/arm-none-eabi-objcopy" --gap-fill=0xff -O binary $Sbl_Elf_Path/sbl_nor$OPPMODE\_a15host_$profile.xa15fg $Sbl_Out_Path/SBL.bin

mv $Sbl_Out_Path/SBL.bin $Sbl_Out_Path/sbl_nor

cp $STARTERWAREDIRNAME/binary/nor_flash_writer/bin/tda2xx/nor_flash_writer_m4_$profile.xem4 $Sbl_Out_Path/

