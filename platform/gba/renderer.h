#pragma once

#include <vector>

#include "algebra.h"
#include "export.h"

struct GBAVertex
{
        i32 x;
        i32 y;
        alg_real depth;
        i32 u;
        i32 v;
};

struct GBATileCommand
{
        GBAVertex vertices[4];
        u32 color;
        i32 textureId;
        bool affine;
};

struct GBASpriteCommand
{
        i32 x;
        i32 y;
        i32 width;
        i32 height;
        alg_real depth;
        u32 color;
        bool affine;
        i32 angle;
        i32 scale; // fixed 8.8 style scale for GBA affine sprites
};

class GBARenderer
{
public:
        EXPORT static GBARenderer& Instance();

        EXPORT void BeginFrame();
        EXPORT void SetBackgroundColor(u32 color);
        EXPORT void SetBlendMode(u32 blendMode);
        EXPORT void SetDepthCompare(u32 depthMode);
        EXPORT void SetDepthWrite(bool enabled);
        EXPORT void SetFilterMode(u32 mode);
        EXPORT void SetTexture(i32 textureId, u32 flags, u32 blendMode);
        EXPORT void QueueTile(const GBATileCommand& command);
        EXPORT void QueueSprite(const GBASpriteCommand& command);
        EXPORT void Flush(bool flipBuffers);

        INLINE i32 ScreenWidth() const { return 240; }
        INLINE i32 ScreenHeight() const { return 160; }

private:
        GBARenderer() = default;

        struct RenderPacket
        {
                alg_real depth;
                bool isTile;
                GBATileCommand tile;
                GBASpriteCommand sprite;
        };

        void emitBatches();

        std::vector<RenderPacket> mPackets;
        u32 mBackgroundColor = 0;
        u32 mBlendMode = 0;
        i32 mTextureId = -1;
        u32 mTextureFlags = 0;
        u32 mDepthMode = 0;
        bool mDepthWrite = true;
        u32 mFilterMode = 0;
};

