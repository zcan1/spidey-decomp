# Porting notes: PC dependencies and gameplay systems

## PC/DirectX subsystems to replace
- **`main.cpp`** coordinates startup through DirectX and Win32 helpers (`DXINIT`, `DXsound`, `PCGfx`, `PCInput`) and invokes PC FMV playback before handing control to gameplay logic. Any handheld port needs alternate init, render, input, and movie hooks.
- **`SpideyDX.cpp`** owns the Windows message pump, HWND creation, timer shutdown, and DirectInput pad bridge; MMX/3D accelerator checks also assume x86. Replace these with platform-native window/input bootstrap code while preserving game loop timing.
- **`PCGfx.cpp`** builds scenes through the DXPOLY pipeline, uses DirectSound mixer callbacks, and drives fog/z-layer math in floating point tailored for the PC renderer. Substitute a GBA renderer that mirrors the same submission points and texture management semantics.
- **`screen.cpp`** screen effects assume a 640px back buffer and issue DirectX-friendly primitives via `screen_DrawCircularFade`. Portable implementations need equivalent overlay/fade routines.

## Core gameplay systems that must remain intact
- **Physics**: `physics.cpp` currently stubs gravity setup, but all gameplay movement depends on the vector math stack (`CVector`, `VectorNormal`) once filled in. Expect floating-point-heavy gravity and acceleration logic that will need fixed-point replacements on GBA.
- **AI**: `ai.cpp` holds the state machine for NPC procedures (`CAIProc_*`). Movement helpers (e.g., `CAIProc_MoveTo`) rely on normalized vectors and scaled velocities, implying eventual floating-point calculations that should be converted to fixed-point for GBA while keeping behavior identical.
- **Level streaming**: `spool.cpp` manages PSX region loading, texture hash tables, and environment swaps. Keep its sequencing intact and replace DirectX file/texture dependencies with GBA-friendly streaming while honoring the same region lifecycle.
- **Cutscenes/FMVs**: `main.cpp` calls `GameFMV_PlayMovie` for intro playback, and movie/shutdown flow assumes PC CD-ROM access. A GBA port should stub these while preserving the timing/flag transitions that gate gameplay startup.
