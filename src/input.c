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

#include "input.h"
#include <stdint.h>

#define MAX_INPUT_POINTS 11

Vector2 * GetInputMouseTouch(void) 
{
    static Vector2 Input_Points[MAX_INPUT_POINTS + 1] = {0};
    int iPoints = GetTouchPointCount() + IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    if (iPoints >= MAX_INPUT_POINTS) iPoints = MAX_INPUT_POINTS;
    for (uint8_t i = 0; i < iPoints; i++) Input_Points[i] = GetTouchPosition(i);
    Input_Points[iPoints] = (Vector2) {NAN, NAN};
    return Input_Points;
}

Vector2 * GetInputTap(void) 
{
    static Vector2 Input_Points[MAX_INPUT_POINTS + 1] = {0};
    int iPoints = GetTouchPointCount() + IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    if (iPoints >= MAX_INPUT_POINTS) iPoints = MAX_INPUT_POINTS;
    for (uint8_t i = 0; i < iPoints; i++) Input_Points[i] = GetTouchPosition(i);
    Input_Points[iPoints] = (Vector2) {NAN, NAN};
    return Input_Points;
}