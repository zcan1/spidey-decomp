# OpenLara GBA renderer/audio/input patterns vs. Spidey extension points

## What the OpenLara GBA backend does
- **Display setup & page flipping.** The GBA entry point boots into mode 4, enables BG2, and toggles the framebuffer by XOR-ing the VRAM base every frame while swapping the DCNT_PAGE bit. This keeps two 8-bit pages in VRAM and renders after a `VBlankIntrWait` when vsync is enabled.【F:OpenLara-master/src/platform/gba/main.cpp†L585-L617】
- **VBlank-driven audio DMA.** An interrupt increments `frameIndex` and refills audio. `soundInit` wires timer 0 to the sample rate, points DMA1 at the FIFO A register, and `soundFill` pushes alternating halves of a double buffer before the next VBlank.【F:OpenLara-master/src/platform/gba/main.cpp†L487-L514】
- **Palette-backed frame writes.** Rendering targets an 8-bit indexed framebuffer; background and UI copies use `dmaCopy` to move whole frames, expecting palette entries in `MEM_PAL_BG`.【F:OpenLara-master/src/platform/gba/main.cpp†L319-L322】【F:OpenLara-master/src/platform/gba/render.iwram.cpp†L1187-L1199】
- **Tile/lightmap usage.** The renderer tracks `gTile` and lightmap lookups to shade palette indices, packing working geometry in EWRAM/IWRAM to fit bandwidth limits. VRAM writes occur as 16-bit-aligned stores (see rasterizer assembly) to satisfy GBA bus rules.【F:OpenLara-master/src/platform/gba/render.iwram.cpp†L63-L75】【F:OpenLara-master/src/platform/gba/rasterizer.h†L1-L60】
- **Input and rumble.** Buttons are polled each frame and mapped to internal `keys` bits; cart rumble is toggled via GPIO and cleared after a fixed tick budget.【F:OpenLara-master/src/platform/gba/main.cpp†L440-L483】

## Where to plug similar behaviors into Spidey
- **Video mode/back-buffer control.** Spidey’s DirectX bootstrap (`DXINIT_DirectX8`) already centralizes resolution, back-buffer creation, and pixel format selection. That’s the natural hook for a GBA-like “mode 4” path that forces 8-bit surfaces and explicit buffer swaps rather than relying on the current 16/32-bit defaults.【F:DXinit.cpp†L20-L120】
- **Palette/CLUT management.** Palette slots and CLUT packing are already abstracted in `pal.cpp` via `Pal_Init`, `GetFree16Slot`, and related helpers. A GBA-style 256-color pipeline can reuse these allocators to mirror `MEM_PAL_BG` updates and keep the renderer emitting indices instead of true color.【F:pal.cpp†L1-L120】
- **Audio streaming and haptics.** DirectSound buffer setup and controller force-feedback handles live in `DXsound.cpp` (`g_pDSBuffer`, `gDxSoundBuffers`, `gForceFeedbackRelated`). A hardware FIFO/DMA model would map to continually refilling these buffers on a timer callback akin to the VBlank ISR while reusing the existing force-feedback entry points for rumble parity.【F:DXsound.cpp†L1-L110】
- **Controller mapping.** `PCInput.cpp` owns the translation from device state to game actions (`PCINPUT_GetControllerDirections`, mapping tables). Hooking a GBA key matrix reader here lets the rest of the game consume inputs without change, and the rumble bridge can forward to `DXsound`’s force-feedback hooks.【F:PCInput.cpp†L122-L200】【F:DXsound.cpp†L60-L110】
- **Frame timing.** GBA relies on VBlank interrupts for pacing. Spidey’s `PCTIMER_Init` and `TimerCallback` plumbing establish a similar periodic tick; integrating a “VBlank-equivalent” callback here would give a spot to queue audio refills and buffer flips before the next frame submit.【F:PCTimer.cpp†L7-L120】

## Implementation notes
- Preserve the double-buffered, palette-indexed path end-to-end: swap buffers only after vsync/timer callbacks and translate palette writes through the existing `pal.cpp` allocator.
- Model DMA pushes as timed buffer updates (audio/video) rather than ad hoc copies so they stay synchronized with the timer loop.
- Route rumble through the existing force-feedback handles to mirror GBA GPIO toggles without changing gameplay code.
