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
set Sbl_Elf_Path=%STARTERWAREDIRNAME%\binary\sbl_nor%OPPMODE%\bin\tda2xx
set Sbl_Out_Path=%CD%\nor_tda2xx

IF NOT EXIST %Sbl_Out_Path%\ mkdir %Sbl_Out_Path%

del %Sbl_Out_Path%\sbl_nor

"%Gcc_Tools_Path%\arm-none-eabi-objcopy.exe" --gap-fill=0xff -O binary %Sbl_Elf_Path%\sbl_nor%OPPMODE%_a15host_%profile%.xa15fg %Sbl_Out_Path%\SBL.bin

ren %Sbl_Out_Path%\SBL.bin sbl_nor

copy %STARTERWAREDIRNAME%\binary\nor_flash_writer\bin\tda2xx\nor_flash_writer_m4_%profile%.xem4 %Sbl_Out_Path%\

pause