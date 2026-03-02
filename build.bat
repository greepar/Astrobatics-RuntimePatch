@echo off
setlocal

set CLANG=C:\Program Files\LLVM\bin\clang.exe
set SRC=winmm_patch_min.c
set DEF=winmm_patch_min.def
set OUT=winmm.dll

if not exist "%CLANG%" (
  echo [ERROR] clang not found: %CLANG%
  exit /b 1
)

"%CLANG%" --target=i686-pc-windows-msvc -O2 -shared "%SRC%" -o "%OUT%" -Wl,/DEF:%DEF% -Wl,/MACHINE:X86
if errorlevel 1 (
  echo [ERROR] build failed
  exit /b 1
)

echo [OK] built %OUT%
