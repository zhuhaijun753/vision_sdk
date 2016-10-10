REM Use profile passed from command line, else use default as release
set profile=%1
IF %1.==. (
set profile=release
)

set TICOMPONENTSBASE=%CD%\..\..\..\

set STARTERWAREDIRNAME=%TICOMPONENTSBASE%\ti_components\drivers\starterware_01_06_00_16

set Gcc_Tools_Path=%TICOMPONENTSBASE%\ti_components\cg_tools\windows\gcc-arm-none-eabi-4_9-2015q3\bin

REM <blank> or _opp_od or _opp_high
set OPPMODE=

set Sbl_Tools_Path=%STARTERWAREDIRNAME%\bootloader\Tools\tiimage
set Sbl_Elf_Path=%STARTERWAREDIRNAME%\binary\sbl_sd%OPPMODE%\bin\tda2ex
set Sbl_Out_Path=%CD%\mlo_tda2ex
set Sbl_Out_Path_hs=%CD%\mlo_tda2ex_hs

IF NOT EXIST %Sbl_Out_Path%\ mkdir %Sbl_Out_Path%
IF NOT EXIST %Sbl_Out_Path_hs%\ mkdir %Sbl_Out_Path_hs%

del %Sbl_Out_Path%\MLO

"%Gcc_Tools_Path%\arm-none-eabi-objcopy.exe" --gap-fill=0xff -O binary %Sbl_Elf_Path%\sbl_sd%OPPMODE%_a15host_%profile%.xa15fg %Sbl_Out_Path%\SBL.bin

"%Sbl_Tools_Path%\tiimage.exe" 0x40300000 LE %Sbl_Out_Path%\SBL.bin %Sbl_Out_Path%\SBL.tiimage
"%Sbl_Tools_Path%\tiimage.exe" 0xFFFFFFFF LE %Sbl_Out_Path%\SBL.bin %Sbl_Out_Path%\SBL.tiimage

ren %Sbl_Out_Path%\SBL.tiimage MLO
ren %Sbl_Out_Path_hs%\SBL.tiimage unsigned_MLO

del %Sbl_Out_Path%\SBL.bin

pause
