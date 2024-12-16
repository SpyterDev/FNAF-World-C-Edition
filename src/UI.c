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

#include "UI.h"
#include "Animation.h"
#include <memory.h>
#include <stdlib.h>
#include "input.h"
#include <math.h>
#include <string.h>
#include <stdio.h>
#include "Particle.h"
#include <stdint.h>

// Gets the screen ratio
float GetScreenRatio(void)
{
    uint8_t widthGreater = (float) GetScreenWidth() / GetScreenHeight() >= 4/3.;
    return widthGreater ? (float) GetScreenWidth() / GetScreenHeight() : (float) GetScreenHeight() / GetScreenWidth();
}

// Gets the screen scale relative to the screen height
float GetScreenScale(void)
{
    return GetScreenHeight()/720.;
}

// Gets the screen scale relative to the screen width
float GetScreenScaleW(void)
{
    return GetScreenWidth()/1280.;
}

// Gets the position x needed for a texture to not be visible in UI space
float GetOutsideWindowX_u16(uint16_t width) 
{
    return 1 + (float)(width) / (GetScreenWidth());
}

// Gets the position y needed for a texture to not be visible in UI space
float GetOutsideWindowY_u16(uint16_t height) 
{
    return 1 + (float)(height) / (GetScreenWidth());
}

// Gets the position x needed for a texture to not be visible in UI space
float GetOutsideWindowX(Texture2D texture) 
{
    return 1 + (float)(texture.width) / (GetScreenWidth());
}

// Gets the position y needed for a texture to not be visible in UI space
float GetOutsideWindowY(Texture2D texture) 
{
    return 1 + (float)(texture.height) / (GetScreenHeight());
}

void FreeUIElement(UIElement * element)
{
    switch (element -> visual.type) 
    {
        case UItexture:
            UnloadTexture(element -> visual.texture);
            break;
        case UIanimation:
            FreeAnimation(&element -> visual.animation);
            break;
        case UIanimationV2:
            FreeAnimation_V2(&element -> visual.animation_V2);
            break;
        default:
            break;
        memset(element, 0, sizeof(UIElement));
    }
}

// Debugging DrawTextureEx wrapper which also prints position
void DrawDebugTexture(Texture2D texture, Vector2 position, float rotation, float scale, Color tint)
{
    printf("%f, %f\n", position.x, position.y);
    DrawTextureEx(texture, position, rotation, scale, tint);
}

// Scales and Renders a UITexture in UI Space
void RenderUITexture(UITexture texture, float x, float y, float scale)
{
    DrawTextureEx(  texture, 
                    (Vector2) { (float) SCREEN_POSITION_TO_PIXEL_X(x, texture.width, scale), 
                                 (float) SCREEN_POSITION_TO_PIXEL_Y(y, texture.height, scale) },
                    0, 
                    scale, 
                    WHITE);
}

// Copies a UIVisual to the heap and returns the address
UIVisual * UIVisual_Heap(UIVisual visual)
{
    UIVisual * visual_heap = malloc(sizeof(UIVisual));
    if (!visual_heap) exit(EXIT_FAILURE);
    *visual_heap = visual;
    return visual_heap;
}

// Creates a UIVisual of type UItexture via UItexture
UIVisual CreateUIVisual_UITexture(UITexture texture, Color tint)
{
    UIVisual temp = {0};
    temp.type = UItexture;
    temp.texture = texture;
    temp.tint = tint;
    return temp;
}

// Creates a UIVisual of type UItexture via path
UIVisual CreateUIVisual_UITexture_P(const char * path, Color tint)
{
    UIVisual temp = {0};
    temp.type = UItexture;
    temp.texture = LoadTexture(path);
    temp.tint = tint;
    return temp;
}

// Creates a UIVisual of type UIanimation via path
UIVisual CreateUIVisual_UIAnimation(const char * path, const uint8_t targetFPS, const Color tint)
{
    UIVisual temp = {0};
    temp.type = UIanimation;
    temp.animation = CreateAnimation(path, targetFPS);
    temp.tint = tint;
    return temp;
}

// Creates a UIVisual of type UIanimationV2 via path
UIVisual CreateUIVisual_UIAnimation_V2(const char * path, const uint8_t targetFPS, const uint8_t amount, Vector2 tileSize, Color tint)
{
    UIVisual temp = {0};
    temp.type = UIanimationV2;
    temp.animation_V2 = CreateAnimation_V2(path, targetFPS, amount, tileSize.x, tileSize.y);
    temp.tint = tint;
    return temp;
}

UIElement CreateUIElement(UIVisual visual, float x, float y, float scale)
{
    UIElement temp = {0};
    temp.visual = visual;
    temp.scale = scale;
    temp.x = x;
    temp.y = y;
    return temp;
}

// Scales and Renders a UIElement
void RenderUIElement(const UIElement * element) 
{
    register float scale = element -> scale * GetScreenHeight() / 720.;
    switch (element -> visual.type) {

        case UIanimation:
            RenderAnimation(&element -> visual.animation, 
                            element -> x, 
                            element -> y, 
                            scale, 0);
            return;
        case UItexture:
            RenderUITexture(element -> visual.texture, 
                            element -> x, 
                            element -> y, 
                            scale);
            return;
        case UIanimationV2:
            RenderAnimation_V2(&element -> visual.animation_V2, 
                                element -> x, element -> y, 
                                element -> scale, 0);
            return;
        default:
            return;
    }
}

// Scales and Renders a UIButton
void RenderUIButton(const UIButton * button)
{
    RenderUIElement(&button -> graphic);
}

// Scales and Renders a UIVisual at X, Y scaled
void RenderUIVisual(float x, float y, UIVisual * visual, float scale)
{
    UIElement temp = (UIElement) {visual -> type, visual -> texture, x, y, scale};
    RenderUIElement(&temp);
}

// Scales and Renders text in UI space
void RenderUIText(const char * text, float x, float y, float fontSize, enum UITextAlignment allignment, Font font, Color color)
{
    fontSize *= GetScreenHeight();
    Vector2 position = (Vector2) {SCREEN_POSITION_TO_PIXEL_X(x, 0, 1), SCREEN_POSITION_TO_PIXEL_Y(y, fontSize, 1)};
    Vector2 origin = {0}; 
    if (!font.baseSize) font = GetFontDefault();
    Vector2 size = MeasureTextEx(font, text, fontSize, 1);
    switch (allignment) 
    {
        case LEFTMOST:
            origin = (Vector2){0, size.y/2};
            break;
        case CENTRE:
            origin = (Vector2){size.x/2, size.y/2};
            break;
        case RIGHTMOST:
            origin = (Vector2){size.x, size.y/2};
            break;
    }
    DrawTextPro(font, text, position, origin, 0, fontSize, 1, color);
}

// Checks and updates a button if it has been pressed
void UpdateUIButton(const UIButton * button) 
{
    register uint16_t inputX = 0;
    register uint16_t inputY = 0;

    register float scale = button -> graphic.scale * GetScreenHeight() / 720.;

    UITexture buttonTexture = {0};

    switch (button -> graphic.visual.type)
    {
        case UIanimation: 
            uint16_t currentFrame = GetCurrentAnimationFrame(&button -> graphic.visual.animation);
            buttonTexture = button -> graphic.visual.animation.Frames[currentFrame];
            break;
        case UItexture:
            buttonTexture = button -> graphic.visual.texture;
            break;
        default:
        return;
    }

    register uint16_t buttonX = SCREEN_POSITION_TO_PIXEL_X(button -> graphic.x, buttonTexture.width, scale);
    register uint16_t buttonY = SCREEN_POSITION_TO_PIXEL_Y(button -> graphic.y, buttonTexture.height, scale);

    register uint16_t buttonW = buttonTexture.width * scale;
    register uint16_t buttonH = buttonTexture.height * scale;
    
    Vector2 * inputs = GetInputMouseTouch();
    for (uint8_t i = 0; inputs[i].x != NAN && inputs[i].y != NAN && i < MAX_INPUT_POINTS; i++) 
    {
        inputX = (uint16_t) inputs[i].x;
        inputY = (uint16_t) inputs[i].y;

        if ((inputX > buttonX &&  inputX <= buttonX + buttonW) &&
            (inputY > buttonY && inputY <= buttonY + buttonH) )
        {
            if (button -> press) button -> press(button);
        }
    }
}

// Updates and Renders a UIButton
void PutUIButton(const UIButton * button)
{
    UpdateUIButton(button);
    RenderUIButton(button);
}

// Draws a Sprite from a UITexture Spritesheet
void DrawUITextureSpritesheet(Texture2D atlas, int16_t x, int16_t y, uint16_t index, uint16_t tileSize)
{
    DrawUITextureSpritesheetEx(atlas, x, y, index, (Vector2) {tileSize, tileSize}, 1, WHITE);
}

// Draws a Sprite from a UITexture Spritesheet with addition parameters
void DrawUITextureSpritesheetEx(Texture2D atlas, int16_t x, int16_t y, uint16_t index, Vector2 tileSize, float scale, Color tint)
{
    uint16_t adjustedWidth = atlas.width / (uint16_t) tileSize.x;
    adjustedWidth *= (uint16_t) tileSize.x; 
    Rectangle source = {    (index * (uint16_t) tileSize.x) % adjustedWidth, 
                            (uint16_t)((float) (index * (uint16_t) tileSize.x) / adjustedWidth) * tileSize.y,
                            tileSize.x,
                            tileSize.y   };
    Rectangle dest = { x, y, tileSize.x * scale, tileSize.y * scale};
    DrawTexturePro(atlas, source, dest, (Vector2) {0, 0}, 0, tint);
}

// Scales and Renders a Sprite from a UITexture Spritesheet in UI space
void RenderUITextureSpritesheet(Texture2D atlas, float x, float y, uint16_t index, uint16_t tileSize)
{
    DrawUITextureSpritesheetEx(   atlas, 
                                SCREEN_POSITION_TO_PIXEL_X(x, tileSize, GetScreenScale()),
                                SCREEN_POSITION_TO_PIXEL_Y(y, tileSize, GetScreenScale()),
                                index, 
                                (Vector2) {tileSize, tileSize}, 
                                GetScreenScale(), 
                                WHITE);
}

// Scales and Renders a Sprite from a UITexture Spritesheet in UI space with addition parameters
void RenderUITextureSpritesheetEx(Texture2D atlas, float x, float y, uint16_t index, Vector2 tileSize, float scale, Color tint)
{
    scale *= GetScreenScale();
    DrawUITextureSpritesheetEx(   atlas, 
                                SCREEN_POSITION_TO_PIXEL_X(x, tileSize.x, scale),
                                SCREEN_POSITION_TO_PIXEL_Y(y, tileSize.y, scale), 
                                index, 
                                tileSize, 
                                scale, 
                                WHITE);
}