@echo off

echo Starting OpenOCD...
start "" openocd -f interface/stlink.cfg -f target/stm32f4x.cfg

echo Waiting for OpenOCD to start...

:loop
timeout /t 1 >nul
netstat -an | find "3333" >nul
if errorlevel 1 goto loop

echo OpenOCD Ready!

arm-none-eabi-gdb -x .gdbinit output.elf