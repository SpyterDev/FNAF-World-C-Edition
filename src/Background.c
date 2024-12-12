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

#include "Background.h"
#include <stdint.h>

void RenderBackgroundPro(Texture2D background, float rotation)
{
    uint8_t widthGreater = (float) GetScreenWidth() / GetScreenHeight() >= (float) background.width / background.height;
    float scaleFactor = widthGreater ? (float) GetScreenWidth() / background.width : (float) GetScreenHeight() / background.height;
    Vector2 location = (Vector2) {widthGreater ? 0 : GetScreenWidth() / 2 - background.width * scaleFactor / 2, 
                                        widthGreater ? GetScreenHeight() / 2 - background.height * scaleFactor / 2 : 0};
    DrawTexturePro( background, (Rectangle) {0,0, background.width, background.height}, 
                    (Rectangle) {location.x, location.y, background.width * scaleFactor, background.height * scaleFactor},
                    (Vector2) {background.width * scaleFactor * 1.5, background.height*scaleFactor * 1.5}, 
                    rotation, 
                    WHITE);
}
void RenderBackground(Texture2D background)
{
    RenderBackgroundPro(background, 0);
}