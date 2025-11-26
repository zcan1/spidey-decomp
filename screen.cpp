#include "screen.h"
#include "algebra.h"
#include "platform/gba/renderer.h"

// Screen-space effects here (target arrows, circular fades) presume the PC
// renderer: pixel dimensions hard-code the 640px width used by DirectX
// back buffers, and screen_DrawCircularFade is expected to emit DirectX
// primitives. Portable targets need alternate draw helpers while preserving
// fade timing semantics.


EXPORT bool gScreenTarget;


EXPORT CVector gTargetRelated;
EXPORT u16 gTargetOne;
EXPORT u16 gTargetTwo;

EXPORT i32 gCircularFadeRelated;
EXPORT i32 gCircularFadeRelatedOne;
EXPORT i32 gCircularFadeRelatedTwo;

EXPORT u8 gCircularFadeRelatedThree;
EXPORT u8 gCircularFadeRelatedFour;


// @SMALLTODO
void Screen_DrawArrow(void)
{
    printf("Screen_DrawArrow(void)");
}

// @MEDIUMTODO
void Screen_DrawTarget(void)
{
    printf("Screen_DrawTarget(void)");
}

// @MEDIUMTODO
void Screen_SepiaFade(void)
{
    printf("Screen_SepiaFade(void)");
}

// @Ok
void Screen_SetTarget(
		CVector *a1,
		u16 a2,
		i16 a3)
{
	gTargetRelated = *a1;
	gTargetOne = a2;
	gTargetTwo = a3;
}

// @Ok
void Screen_StartCircularFadeIn(i32,i32 a2)
{
	gCircularFadeRelated = 32;
	gCircularFadeRelatedOne = 0;
	gCircularFadeRelatedTwo = a2 << 12;

	gCircularFadeRelatedThree = 1;
	gCircularFadeRelatedFour = 0;
}

// @Ok
// @Matching
void Screen_UpdateFades(void)
{
	if (gCircularFadeRelatedThree)
	{
		gCircularFadeRelatedOne += gCircularFadeRelatedTwo >> 12;
		gCircularFadeRelatedTwo += 3072;
		if ( gCircularFadeRelatedOne >= 640 )
		{
			gCircularFadeRelatedThree = 0;
			return;
		}
	}
	else
	{
		if (!gCircularFadeRelatedFour)
			return;
		gCircularFadeRelatedOne -= gCircularFadeRelatedTwo >> 12;
		if (gCircularFadeRelatedOne <= 0)
		{
			gCircularFadeRelatedFour = 0;
			return;
		}
	}

	screen_DrawCircularFade();
}

// @MEDIUMTODO
INLINE void screen_DrawCircularFade(void)
{
        GBASpriteCommand cmd{};
        cmd.x = GBARenderer::Instance().ScreenWidth() / 2;
        cmd.y = GBARenderer::Instance().ScreenHeight() / 2;
        cmd.width = GBARenderer::Instance().ScreenWidth();
        cmd.height = GBARenderer::Instance().ScreenHeight();
        cmd.color = 0xFF000000;
        cmd.affine = true;
        cmd.angle = 0;

        // Scale the affine sprite based on the fade accumulator. The result is
        // treated as 8.8 fixed point to mirror the GBA affine sprite hardware.
        cmd.scale = (gCircularFadeRelatedOne * 256) / 640;
        cmd.depth = Algebra_FixedFromFloat(0.0f);

        GBARenderer::Instance().QueueSprite(cmd);
}

// @Ok
// @Matching
void Screen_TargetOn(bool value)
{
	gScreenTarget = value;
}
