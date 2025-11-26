#pragma once

#include "export.h"

EXPORT void GBAINPUT_Initialize(void);
EXPORT void GBAINPUT_Release(void);
EXPORT u8 GBAINPUT_GetControllerButtonState(u8 button);
EXPORT u8 GBAINPUT_GetKeyState(u8 key);
EXPORT i32 GBAINPUT_GetNumControllerButtons(void);
EXPORT i32 GBAINPUT_PollController(i32* outX, i32* outY, i32* outAxes);
EXPORT i32 GBAINPUT_PollKeyboard(void);
EXPORT void GBAINPUT_SetKeyState(u8 key, u8 state);
