@echo off
setlocal EnableExtensions EnableDelayedExpansion

set "ROOT=%~dp0"
set "BUILD_DIR=%ROOT%build"

if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
if errorlevel 1 exit /b 1

if not exist "%ROOT%scripts\c\*.c" (
    echo No C source files found in "%ROOT%scripts\c".
    exit /b 1
)

set "OBJECTS="
for %%F in ("%ROOT%scripts\c\*.c") do (
    if not exist "%%~fF" (
        echo No C source files found in "%ROOT%scripts\c".
        exit /b 1
    )
    gcc -m64 -ffreestanding -fno-stack-protector -fno-pic -mno-red-zone -fno-asynchronous-unwind-tables -fno-unwind-tables -c "%%~fF" -o "%BUILD_DIR%\%%~nF.o"
    if errorlevel 1 exit /b 1
    set "OBJECTS=!OBJECTS! "%BUILD_DIR%\%%~nF.o""
)

ld -m i386pep --image-base 0 -T "%ROOT%linker.ld" !OBJECTS! -o "%BUILD_DIR%\kernel.pe"
if errorlevel 1 exit /b 1

objcopy -O binary "%BUILD_DIR%\kernel.pe" "%BUILD_DIR%\kernel.bin"
if errorlevel 1 exit /b 1

for %%I in ("%BUILD_DIR%\kernel.bin") do set "KERNEL_BYTES=%%~zI"
set /a KERNEL_SECTORS=(KERNEL_BYTES + 511) / 512
if %KERNEL_SECTORS% LSS 1 (
    echo Kernel image is empty.
    exit /b 1
)
if %KERNEL_SECTORS% GTR 17 (
    echo Kernel exceeds the bootloader's 17-sector CHS read limit.
    exit /b 1
)

nasm -f bin -D KERNEL_SECTORS=%KERNEL_SECTORS% "%ROOT%scripts\asm\boot_sect.asm" -o "%BUILD_DIR%\boot.bin"
if errorlevel 1 exit /b 1

powershell -NoProfile -Command "$path='%BUILD_DIR%\kernel.bin'; $stream=[IO.File]::Open($path,[IO.FileMode]::Open,[IO.FileAccess]::Write); $length=[int64]([math]::Ceiling($stream.Length/512.0)*512); try { $stream.SetLength($length) } finally { $stream.Dispose() }"
if errorlevel 1 exit /b 1

copy /b "%BUILD_DIR%\boot.bin"+"%BUILD_DIR%\kernel.bin" "%BUILD_DIR%\os_image.bin" >nul
if errorlevel 1 exit /b 1

echo Built "%BUILD_DIR%\os_image.bin"

if /i "%BUILD_ONLY%"=="1" exit /b 0

qemu-system-x86_64 -drive "format=raw,file=%BUILD_DIR%\os_image.bin" -boot order=c
