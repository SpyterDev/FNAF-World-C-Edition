/*
    MIT License

    Copyright (c) 2024 SpyterDev

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#pragma once

#include "UI.h"
#include <stdint.h>

enum WORLDZONES 
{
    FAZBEARHILLS=1
};

typedef struct WORLDTileIndex
{
    Texture2D texture;
} WORLDTileIndex;

typedef uint16_t WORLDTile;

typedef struct WORLDEntity 
{
    Vector2 position;
    Vector2 size;
    Vector2 velocity;
    UITexture texture;
    float scale;
    uint16_t collisionTargets;
    void (*customCollision)(void);
} WORLDEntity;

#define LAYER_COLLIDABLE 1
#define LAYER_INVISIBLE 2
#define LAYER_SPAWN 4

typedef struct tilemap_layer 
{
    // Transformation Variables

    uint16_t offsetX;
    uint16_t offsetY;
    uint16_t sizeX;
    uint16_t sizeY;

    void * tiles; // WORLDTile 2D Array Pointer

    // Flags

    uint8_t FLAGS;
} tilemap_layer;

#define TILES_POINTER_TYPE(layer_pointer) WORLDTile (*)[layer_pointer->sizeY][layer_pointer->sizeX]

typedef struct WORLDTilemap
{
    tilemap_layer * layers;
    uint16_t amount;
} WORLDTilemap;

extern void LoadWorldTilemap(void);

#define ACCESS_TILEMAP(x, y, tilemap) 