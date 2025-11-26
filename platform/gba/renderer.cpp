#include "platform/gba/renderer.h"

#include <algorithm>
#include <cstdio>

GBARenderer& GBARenderer::Instance()
{
        static GBARenderer sInstance;
        return sInstance;
}

void GBARenderer::BeginFrame()
{
        mPackets.clear();
}

void GBARenderer::SetBackgroundColor(u32 color)
{
        mBackgroundColor = color;
}

void GBARenderer::SetBlendMode(u32 blendMode)
{
        mBlendMode = blendMode;
}

void GBARenderer::SetDepthCompare(u32 depthMode)
{
        mDepthMode = depthMode;
}

void GBARenderer::SetDepthWrite(bool enabled)
{
        mDepthWrite = enabled;
}

void GBARenderer::SetFilterMode(u32 mode)
{
        mFilterMode = mode;
}

void GBARenderer::SetTexture(i32 textureId, u32 flags, u32 blendMode)
{
        mTextureId = textureId;
        mTextureFlags = flags;
        mBlendMode = blendMode;
}

void GBARenderer::QueueTile(const GBATileCommand& command)
{
        RenderPacket packet{};
        packet.depth = command.vertices[0].depth;
        packet.isTile = true;
        packet.tile = command;
        mPackets.push_back(packet);
}

void GBARenderer::QueueSprite(const GBASpriteCommand& command)
{
        RenderPacket packet{};
        packet.depth = command.depth;
        packet.isTile = false;
        packet.sprite = command;
        mPackets.push_back(packet);
}

void GBARenderer::emitBatches()
{
        // Placeholder for the GBA hardware submission. The intent is to mimic
        // the OpenLara GBA renderer: sort by depth, then blit tiles followed by
        // sprites (affine entries remain in the sprite list with the affine
        // flag set).
        std::stable_sort(mPackets.begin(), mPackets.end(), [](const RenderPacket& lhs, const RenderPacket& rhs) {
                return lhs.depth < rhs.depth;
        });

        for (const RenderPacket& packet : mPackets)
        {
                if (packet.isTile)
                {
                        const GBATileCommand& tile = packet.tile;
                        std::printf("[GBA] tile tex=%d depth=%d affine=%d\n", tile.textureId, static_cast<int>(tile.vertices[0].depth), tile.affine);
                }
                else
                {
                        const GBASpriteCommand& sprite = packet.sprite;
                        std::printf(
                                "[GBA] sprite (%d,%d) %dx%d depth=%d affine=%d angle=%d scale=%d\n",
                                sprite.x,
                                sprite.y,
                                sprite.width,
                                sprite.height,
                                static_cast<int>(sprite.depth),
                                sprite.affine,
                                sprite.angle,
                                sprite.scale);
                }
        }
}

void GBARenderer::Flush(bool flipBuffers)
{
        // Depth sort batches then emit them. The GBA renderer writes directly
        // into VRAM, so "flipping" simply resets the packet queue between
        // frames.
        emitBatches();
        mPackets.clear();

        if (flipBuffers)
        {
                // In a full port this would swap page-flipped buffers; here we
                // simply note the request for debugging parity.
                std::printf("[GBA] flip requested with bg=%08X blend=%u tex=%d flags=%u depth=%u write=%d filter=%u\n",
                            mBackgroundColor,
                            mBlendMode,
                            mTextureId,
                            mTextureFlags,
                            mDepthMode,
                            mDepthWrite,
                            mFilterMode);
        }
}

