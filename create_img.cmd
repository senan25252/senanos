@echo off
setlocal EnableExtensions

set "ROOT=%~dp0"
set "BUILD_DIR=%ROOT%build"
set "BUILD_ONLY=1"

call "%ROOT%boot.cmd"
if errorlevel 1 exit /b 1

powershell -NoProfile -Command "$source='%BUILD_DIR%\os_image.bin'; $destination='%BUILD_DIR%\os_usb.img'; [IO.File]::Copy($source,$destination,$true); $stream=[IO.File]::Open($destination,[IO.FileMode]::Open,[IO.FileAccess]::Write); try { $stream.SetLength(32MB) } finally { $stream.Dispose() }"
if errorlevel 1 exit /b 1

echo Created "%BUILD_DIR%\os_usb.img" (32 MiB raw disk image)
