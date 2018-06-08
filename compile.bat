@echo off

set name="BlockDude"

set path=%path%;..\bin\

set CC65_HOME=C:\Users\mitch3a\Downloads\cc65-snapshot-win32\bin

C:\Users\mitch3a\Downloads\cc65-snapshot-win32\bin\cc65 -Oi %name%.c --add-source
C:\Users\mitch3a\Downloads\cc65-snapshot-win32\bin\ca65 reset.s
C:\Users\mitch3a\Downloads\cc65-snapshot-win32\bin\ca65 %name%.s
C:\Users\mitch3a\Downloads\cc65-snapshot-win32\bin\ca65 asm4c.s

C:\Users\mitch3a\Downloads\cc65-snapshot-win32\bin\ld65 -C nes.cfg -o %name%.nes reset.o %name%.o asm4c.o "C:\Users\mitch3a\Downloads\cc65-snapshot-win32\lib\nes.lib"

del *.o

pause


