#include "platform/gba/input.h"

#include <cstring>

namespace
{
constexpr u16 GBA_KEY_A = 1 << 0;
constexpr u16 GBA_KEY_B = 1 << 1;
constexpr u16 GBA_KEY_SELECT = 1 << 2;
constexpr u16 GBA_KEY_START = 1 << 3;
constexpr u16 GBA_KEY_RIGHT = 1 << 4;
constexpr u16 GBA_KEY_LEFT = 1 << 5;
constexpr u16 GBA_KEY_UP = 1 << 6;
constexpr u16 GBA_KEY_DOWN = 1 << 7;
constexpr u16 GBA_KEY_R = 1 << 8;
constexpr u16 GBA_KEY_L = 1 << 9;

struct KeyMapping
{
        u16 keyMask;
        u8 scanCode;
};

struct ButtonMapping
{
        u16 keyMask;
        u8 buttonIndex;
};

constexpr KeyMapping kKeyboardMappings[] = {
        {GBA_KEY_UP, 0xC8},
        {GBA_KEY_DOWN, 0xD0},
        {GBA_KEY_LEFT, 0xCB},
        {GBA_KEY_RIGHT, 0xCD},
        {GBA_KEY_R, 0x50},
        {GBA_KEY_L, 0x4B},
        {GBA_KEY_B, 0x4D},
        {GBA_KEY_SELECT, 0x51},
        {GBA_KEY_START, 0x0F},
        {GBA_KEY_A, 0x1C},
};

constexpr ButtonMapping kControllerMappings[] = {
        {GBA_KEY_A, 0},
        {GBA_KEY_B, 1},
        {GBA_KEY_R, 3},
        {GBA_KEY_L, 4},
        {GBA_KEY_START, 6},
        {GBA_KEY_SELECT, 9},
        {GBA_KEY_B, 7},
        {GBA_KEY_A, 11},
};

u8 sKeyState[256];
u8 sControllerButtonState[32];
u16 sPreviousKeypad = 0;
u16 sCurrentKeypad = 0;

INLINE u16 readKeypad()
{
        volatile u16* keypad = reinterpret_cast<volatile u16*>(0x04000130);
        return static_cast<u16>(~(*keypad)) & 0x03FF;
}

INLINE void updateState(u8* storage, u8 index, bool wasDown, bool isDown)
{
        if (isDown)
        {
                storage[index] = wasDown ? 0x7F : 0xFF;
        }
        else
        {
                storage[index] = wasDown ? 0x80 : 0x00;
        }
}

INLINE void refreshStates(u16 newKeypad)
{
        u16 previous = sCurrentKeypad;
        sCurrentKeypad = newKeypad;

        for (const KeyMapping& mapping : kKeyboardMappings)
        {
                bool wasDown = (previous & mapping.keyMask) != 0;
                bool isDown = (sCurrentKeypad & mapping.keyMask) != 0;
                updateState(sKeyState, mapping.scanCode, wasDown, isDown);
        }

        for (const ButtonMapping& mapping : kControllerMappings)
        {
                bool wasDown = (previous & mapping.keyMask) != 0;
                bool isDown = (sCurrentKeypad & mapping.keyMask) != 0;
                updateState(sControllerButtonState, mapping.buttonIndex, wasDown, isDown);
        }

        sPreviousKeypad = previous;
}
} // namespace

void GBAINPUT_Initialize(void)
{
        std::memset(sKeyState, 0, sizeof(sKeyState));
        std::memset(sControllerButtonState, 0, sizeof(sControllerButtonState));
        sPreviousKeypad = 0;
        sCurrentKeypad = 0;
}

i32 GBAINPUT_GetNumControllerButtons(void)
{
        return 12;
}

i32 GBAINPUT_PollKeyboard(void)
{
        u16 keypad = readKeypad();
        refreshStates(keypad);
        return keypad != sPreviousKeypad;
}

i32 GBAINPUT_PollController(i32* outX, i32* outY, i32* outAxes)
{
        GBAINPUT_PollKeyboard();

        if (outX)
        {
                *outX = 0;
                if (sCurrentKeypad & GBA_KEY_LEFT)
                        *outX -= 512;
                if (sCurrentKeypad & GBA_KEY_RIGHT)
                        *outX += 512;
        }

        if (outY)
        {
                *outY = 0;
                if (sCurrentKeypad & GBA_KEY_UP)
                        *outY -= 512;
                if (sCurrentKeypad & GBA_KEY_DOWN)
                        *outY += 512;
        }

        if (outAxes)
        {
                *outAxes = -1;
        }

        return 1;
}

u8 GBAINPUT_GetKeyState(u8 key)
{
        return sKeyState[key];
}

u8 GBAINPUT_GetControllerButtonState(u8 button)
{
        return sControllerButtonState[button];
}

void GBAINPUT_SetKeyState(u8 key, u8 state)
{
        sKeyState[key] = state;
}

void GBAINPUT_Release(void)
{
        std::memset(sKeyState, 0, sizeof(sKeyState));
        std::memset(sControllerButtonState, 0, sizeof(sControllerButtonState));
        sPreviousKeypad = 0;
        sCurrentKeypad = 0;
}
