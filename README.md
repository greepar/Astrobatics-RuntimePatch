# Astrobatics WinMM Runtime Patch

This project builds a `winmm.dll` proxy patch for `Astrobatics.exe` (32-bit).

## Files

- `winmm_patch_min.c`: runtime patch + forwarding implementation
- `winmm_patch_min.def`: exported WinMM functions needed by game/runtime (`dinput` etc.)
- `build.bat`: one-command build script (LLVM clang)

## What It Does

At process attach (`DllMain`), the proxy:

1. Loads real WinMM from `C:\Windows\SysWOW64\winmm.dll`
2. Applies in-memory byte patches to the game module
3. Forwards WinMM calls used by game/DirectInput

### Patch goals

- bypass legacy DirectX check branches on modern Windows
- suppress old 3D-sound warning popup branch
- avoid startup crash at the known null-read site (`0x4850DA`)

## Build

### Requirements

- Windows x64
- LLVM clang installed at `C:\Program Files\LLVM\bin\clang.exe`

### Command

```bat
build.bat
```

Expected output:

- `winmm.dll`
- `winmm.lib`
- `winmm.exp`

## Deploy

1. Put generated `winmm.dll` in the same folder as `Astrobatics.exe`
2. Keep filename exactly `winmm.dll`
3. Launch `Astrobatics.exe`

## Notes

- This patch is for 32-bit game binary layout used in this workspace.
- If executable build differs, offsets in `kPatches` may need update.
- Do not rename proxy DLL unless you also rewrite EXE import table.

