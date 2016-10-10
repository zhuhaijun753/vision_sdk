#!/bin/bash

# Device Id for TDA2Ex - 66
# Device ID & CPU ID should be in sync with SBL. Refer to SBL user guide for values
export Dev_ID=66
export MPU_CPU0_ID=0
export MPU_CPU1_ID=1
export IPU1_CPU0_ID=2
export IPU1_CPU1_ID=3
export IPU1_CPU_SMP_ID=4
export IPU2_CPU0_ID=5
export IPU2_CPU1_ID=6
export IPU2_CPU_SMP_ID=7
export DSP1_ID=8

export Out_Path=$PWD/binaries/vision_sdk/bin/tda2ex-evm/sbl_boot
export Tools_path=$PWD/../ti_components/drivers/starterware_01_06_00_16/bootloader/Tools

#Set this variable to "yes" if you want AppImages for HS devices
#Caution: "yes" is case-sensitive
#export HS_DEVICE=yes
export HS_DEVICE=

# Use profile passed from command line, else use default as release
export profile=$1
if [ -z $profile ]
then
    export profile=release
fi

if [ -d $Out_Path ]
then
    echo "$Out_Path exists"
else
    mkdir $Out_Path
fi


# Define Input file paths; To skip the core leave it blank
export App_MPU_CPU0=$PWD/binaries/vision_sdk/bin/tda2ex-evm/vision_sdk_a15_0_$profile.xa15fg
export App_MPU_CPU1=
export App_IPU1_CPU0=$PWD/binaries/vision_sdk/bin/tda2ex-evm/vision_sdk_ipu1_0_$profile.xem4
export App_IPU1_CPU1=$PWD/binaries/vision_sdk/bin/tda2ex-evm/vision_sdk_ipu1_1_$profile.xem4
# App_IPU1_CPU_SMP is to define the IPU SMP application
export App_IPU1_CPU_SMP=
# export App_IPU2_CPU0=$PWD/binaries/vision_sdk/bin/tda2ex-evm/vision_sdk_ipu2_$profile.xem4
# export App_IPU2_CPU1=
# App_IPU2_CPU_SMP is to define the IPU SMP application
# export App_IPU2_CPU_SMP=$PWD/binaries/vision_sdk/bin/tda2ex-evm/vision_sdk_ipu2_$profile.xem4
export App_DSP1=$PWD/binaries/vision_sdk/bin/tda2ex-evm/vision_sdk_c66xdsp_1_$profile.xe66


errorCheck() {
if [ $1 -ne 0 ]; then
    echo "Error occurred.. Check the script"
    exit
fi
}

if [ ! -z $App_MPU_CPU0  ]
then
    export MPU_CPU0=$MPU_CPU0_ID
    export image_gen=1
    export App_MPU_CPU0_RPRC=$App_MPU_CPU0.rprc
fi
if [ ! -z $App_MPU_CPU0  ]
then
    mono "$Tools_path/out2rprc/out2rprc.exe" $App_MPU_CPU0 $App_MPU_CPU0_RPRC
fi

if [ ! -z $App_MPU_CPU1  ]
then
    export MPU_CPU1=$MPU_CPU1_ID
    export image_gen=1
    export App_MPU_CPU1_RPRC=$App_MPU_CPU1.rprc
fi
if [ ! -z $App_MPU_CPU1  ]
then
    mono "$Tools_path/out2rprc/out2rprc.exe" $App_MPU_CPU1 $App_MPU_CPU1_RPRC
fi

if [ ! -z $App_IPU1_CPU0  ]
then
    export IPU1_CPU0=$IPU1_CPU0_ID
    export image_gen=1
    export App_IPU1_CPU0_RPRC=$App_IPU1_CPU0.rprc
fi
if [ ! -z $App_IPU1_CPU0  ]
then
    mono "$Tools_path/out2rprc/out2rprc.exe" $App_IPU1_CPU0 $App_IPU1_CPU0_RPRC
fi

if [ ! -z $App_IPU1_CPU1  ]
then
    export IPU1_CPU1=$IPU1_CPU1_ID
    export image_gen=1
    export App_IPU1_CPU1_RPRC=$App_IPU1_CPU1.rprc
fi
if [ ! -z $App_IPU1_CPU1  ]
then
    mono "$Tools_path/out2rprc/out2rprc.exe" $App_IPU1_CPU1 $App_IPU1_CPU1_RPRC
fi

if [ ! -z $App_IPU1_CPU_SMP  ]
then
    export IPU1_CPU_SMP=$IPU1_CPU_SMP_ID
    export image_gen=1
    export App_IPU1_CPU_SMP_RPRC=$App_IPU1_CPU_SMP.rprc
fi
if [ ! -z $App_IPU1_CPU_SMP  ]
then
    mono "$Tools_path/out2rprc/out2rprc.exe" $App_IPU1_CPU_SMP $App_IPU1_CPU_SMP_RPRC
fi

if [ ! -z $App_IPU2_CPU0  ]
then
    export IPU2_CPU0=$IPU2_CPU0_ID
    export image_gen=1
    export App_IPU2_CPU0_RPRC=$App_IPU2_CPU0.rprc
fi
if [ ! -z $App_IPU2_CPU0  ]
then
    mono "$Tools_path/out2rprc/out2rprc.exe" $App_IPU2_CPU0 $App_IPU2_CPU0_RPRC
fi

if [ ! -z $App_IPU2_CPU1  ]
then
    export IPU2_CPU1=$IPU2_CPU1_ID
    export image_gen=1
    export App_IPU2_CPU1_RPRC=$App_IPU2_CPU1.rprc
fi
if [ ! -z $App_IPU2_CPU1  ]
then
    mono "$Tools_path/out2rprc/out2rprc.exe" $App_IPU2_CPU1 $App_IPU2_CPU1_RPRC
fi

if [ ! -z $App_IPU2_CPU_SMP  ]
then
    export IPU2_CPU_SMP=$IPU2_CPU_SMP_ID
    export image_gen=1
    export App_IPU2_CPU_SMP_RPRC=$App_IPU2_CPU_SMP.rprc
fi
if [ ! -z $App_IPU2_CPU_SMP  ]
then
    mono "$Tools_path/out2rprc/out2rprc.exe" $App_IPU2_CPU_SMP $App_IPU2_CPU_SMP_RPRC
fi

if [ ! -z $App_DSP1  ]
then
    export DSP1_CPU=$DSP1_ID
    export image_gen=1
    export App_DSP1_RPRC=$App_DSP1.rprc
fi
if [ ! -z $App_DSP1  ]
then
    mono "$Tools_path/out2rprc/out2rprc.exe" $App_DSP1 $App_DSP1_RPRC
fi

#For HS Devices
if [ "$HS_DEVICE" == "yes" ]; then
cd ../*mshield*/scripts
echo ""
echo "HS AppImage and SBL Generation enabled!!!!"
echo ""
echo "Signing RPRC images..."
for var in $App_MPU_CPU0_RPRC $App_MPU_CPU1_RPRC $App_IPU1_CPU0_RPRC $App_IPU1_CPU1_RPRC $App_IPU1_CPU_SMP_RPRC $App_IPU2_CPU0_RPRC $App_IPU2_CPU1_RPRC $App_IPU2_CPU_SMP_RPRC $App_DSP1_RPRC $App_DSP2_RPRC $App_EVE1_RPRC $App_EVE2_RPRC $App_EVE3_RPRC $App_EVE4_RPRC ;
do
    if [ -f $var ]; then
        #echo $var
        #echo ./ift-image-sign.sh dra7xx $var $var.signed
        ./ift-image-sign.sh dra7xx $var $var.signed > /dev/null
        #echo mv $var.signed $var
        mv $var.signed $var
    fi
done
echo "Signing RPRC images...done"
#Build PPA
echo ""
echo "Ensure MSHIELD-DK is configured for TDA2XX before running this script !!!"
echo "Building PPA... (This can take a few minutes)... You can comment this step if PPA is already built"
./ift-ppa-gen.sh dra7xx 2>\&1 > /dev/null
errorCheck $?
echo "Building PPA...done"
echo ""
#Generate AppImages for QSPI and SD boot
echo ""
echo "!!!!!Ensure SBL was built before running this script using \"make -s sbl_sd sbl_qspi VSDK_BOARD_TYPE=TDA2EX_EVM\" !!!!!"
echo ""
echo "Generating SBL images..."
./create-boot-image.sh MLO ../../vision_sdk/build/scripts/mlo_tda2ex_hs/unsigned_MLO  ../../vision_sdk/build/scripts/mlo_tda2ex_hs/MLO > /dev/null
errorCheck $?
./create-boot-image.sh X-LOADER ../../vision_sdk/build/scripts/qspi_tda2ex_hs/unsigned_sbl_qspi_LE  ../../vision_sdk/build/scripts/qspi_tda2ex_hs/XLOADER > /dev/null
errorCheck $?
#Convert to BE as required by qspiflashwriter
xxd -p -c 4  ../../vision_sdk/build/scripts/qspi_tda2ex_hs/XLOADER  | sed "s/\(..\)\(..\)\(..\)\(..\)/\4\3\2\1/" | xxd -r -p >  ../../vision_sdk/build/scripts/qspi_tda2ex_hs/XLOADER_BE
errorCheck $?
echo "Generating SBL images...done"
cd - > /dev/null
fi

# ImageGen
echo ""
if [ ! -z $image_gen  ]; then
    # Generating MulticoreImage Gen
    "$Tools_path/multicore_image_generator/v2/MulticoreImageGen" LE $Dev_ID $Out_Path/AppImage_LE $MPU_CPU0 $App_MPU_CPU0_RPRC  $MPU_CPU1 $App_MPU_CPU1_RPRC $MPU_SMP $App_MPU_SMP_RPRC $IPU1_CPU0 $App_IPU1_CPU0_RPRC $IPU1_CPU1 $App_IPU1_CPU1_RPRC $IPU1_CPU_SMP $App_IPU1_CPU_SMP_RPRC $IPU2_CPU0 $App_IPU2_CPU0_RPRC $IPU2_CPU1 $App_IPU2_CPU1_RPRC $IPU2_CPU_SMP $App_IPU2_CPU_SMP_RPRC $DSP1_CPU $App_DSP1_RPRC $DSP2_CPU $App_DSP2_RPRC $EVE1_CPU $App_EVE1_RPRC $EVE2_CPU $App_EVE2_RPRC $EVE3_CPU $App_EVE3_RPRC $EVE4_CPU $App_EVE4_RPRC
    "$Tools_path/multicore_image_generator/v2/MulticoreImageGen" BE $Dev_ID $Out_Path/AppImage_BE $MPU_CPU0 $App_MPU_CPU0_RPRC  $MPU_CPU1 $App_MPU_CPU1_RPRC $MPU_SMP $App_MPU_SMP_RPRC $IPU1_CPU0 $App_IPU1_CPU0_RPRC $IPU1_CPU1 $App_IPU1_CPU1_RPRC $IPU1_CPU_SMP $App_IPU1_CPU_SMP_RPRC $IPU2_CPU0 $App_IPU2_CPU0_RPRC $IPU2_CPU1 $App_IPU2_CPU1_RPRC $IPU2_CPU_SMP $App_IPU2_CPU_SMP_RPRC $DSP1_CPU $App_DSP1_RPRC $DSP2_CPU $App_DSP2_RPRC $EVE1_CPU $App_EVE1_RPRC $EVE2_CPU $App_EVE2_RPRC $EVE3_CPU $App_EVE3_RPRC $EVE4_CPU $App_EVE4_RPRC
fi


cp $Out_Path/AppImage_LE $Out_Path/AppImage

# copy $Out_Path/AppImage F:

