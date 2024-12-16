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

#include "Title_Screen.h"
#include "Game_State.h"
#include "Particle.h"
#include "UI.h"
#include <stdint.h>
#include <stdio.h>
#include <time.h>

#define TITLE_SECONDS_TO_CENTRE 3
#define PARTY_SECONDS_TO_CENTRE 3
#define START_SECONDS_TO_CENTRE 2


Font TitleScreenFont = {0};
UIElement UIBackground = {UItexture, 0};
UIElement UIParty = {UItexture, 0};
UIElement UITitleA = {UIanimation, 0};
UIElement UITitleT = {UItexture, 0};

Music Theme = {0};

void FreeTitleScreen(void)
{
    FreeUIElement(&UIBackground);
    FreeUIElement(&UIParty);
    FreeUIElement(&UITitleA);
    FreeUIElement(&UITitleT);
}
void ButtonPlayPressed(UIButton * button)
{
    printf("Play Button Pressed!\n");
    SwapGameState(World);
}

UIButton UIPlay = {(UIElement){UIanimation,0}, ButtonPlayPressed, 0};
anim_t StartTime = 0;
uint8_t ParticleStars = 0;

void InitTitleScreen(void) 
{
    UITitleA.visual.animation = CreateAnimation("Assets/Menu/Title_Screen/Animations/Title/", 10);
    UITitleA.scale = 1.3;

    Theme = LoadMusicStream("Assets/Themes/theme.wav");
    Theme.looping = 1;

    PlayMusicStream(Theme);

    UIBackground.visual.texture = LoadTexture("Assets/Menu/Title_Screen/Background.png");
    SetTextureFilter(UIBackground.visual.texture, TEXTURE_FILTER_BILINEAR);
    UIParty.visual.texture = LoadTexture("Assets/Menu/Title_Screen/Party.png");
    SetTextureFilter(UIParty.visual.texture, TEXTURE_FILTER_BILINEAR);
    UIParty.scale = 1.5;

    UITitleT.visual.texture = LoadTexture("Assets/Menu/Title_Screen/Title.png");
    SetTextureFilter(UITitleT.visual.texture, TEXTURE_FILTER_BILINEAR);
    UITitleT.scale = 1.3;

    UIPlay.graphic.visual.animation = CreateAnimation("Assets/Menu/Title_Screen/Buttons/Start/Animations/", 30);
    UIPlay.graphic.scale = 1.5;

    ParticleStars = CreateParticleIndexA_V2("Assets/Particles/titlestar.png", 20,8, (Vector2) {12, 12}, 1.5);
    TitleScreenFont = LoadFont("Assets/Menu/Title_Screen/font.ttf");
}

void UninitTitleScreen(void)
{
    FreeUIElement(&UITitleA);
    FreeUIElement(&UITitleT);
    FreeUIElement(&UIBackground);
    FreeUIElement(&UIParty);
    FreeUIElement(&UIPlay.graphic);
    FlushParticles();
    RemoveParticleIndex(ParticleStars);
    UnloadMusicStream(Theme);
    //UnloadFont(TitleScreenFont);
}

void ResetTitleScreen(void) 
{
    InitTitleScreen();

    UITitleA.visual.animation.Clock = StartTime;

    UITitleA.x = GetOutsideWindowX(UITitleA.visual.animation.Frames[0]);
    UITitleA.y = -0.775;

    UITitleT.x = 0;
    UITitleT.y = -0.775;

    UIParty.x = 0;
    UIParty.y = GetOutsideWindowY(UIParty.visual.texture);

    UIPlay.graphic.x = 0;
    UIPlay.graphic.y = 0.8125;
    PlayMusicStream(Theme);
    StartTime = clock();
}

void RenderTitle(void) 
{
    register float timeSinceStart = (float)(clock() - StartTime) / CLOCKS_PER_SEC;
    if (clock() - StartTime < 3 * CLOCKS_PER_SEC) 
    {
        UITitleA.x = -GetOutsideWindowX(UITitleA.visual.animation.Frames[0]) + timeSinceStart*(GetOutsideWindowX(UITitleA.visual.animation.Frames[0]) / TITLE_SECONDS_TO_CENTRE);
        RenderUIElement(&UITitleA);
    }
    else 
    {
        RenderUIElement(&UITitleT);
    }
    
}

void UpdateParty(void) 
{
    register float timeSinceStart = (float)(clock() - StartTime) / CLOCKS_PER_SEC;
    UIParty.y = GetOutsideWindowY(UIParty.visual.texture) + timeSinceStart * -(GetOutsideWindowY(UIParty.visual.texture) / PARTY_SECONDS_TO_CENTRE);
    UIParty.y = UIParty.y <= 0.26 ? 0.26 : UIParty.y;
}

void UpdatePlay(void) 
{
    register float timeSinceStart = (float)(clock() - StartTime) / CLOCKS_PER_SEC;
    UIPlay.graphic.x = GetOutsideWindowX(UIPlay.graphic.visual.texture) + timeSinceStart * -(GetOutsideWindowX(UIPlay.graphic.visual.texture) / START_SECONDS_TO_CENTRE);
    UIPlay.graphic.x = UIPlay.graphic.x < 0 ? 0: UIPlay.graphic.x;
}

void CreateTitlestars(void)
{
    static clock_t timeSinceLastParticle = 0;
    if (clock() - timeSinceLastParticle > 50 && UIParty.y <= 0.26) 
    {
        float degrees = GetRandomValue(0, 200 *PI)/100.;
        CreateParticle(ParticleStars, 0, 0, cosf(degrees), sinf(degrees));
        timeSinceLastParticle = clock();
    }
}

void PutTitleScreen(void) 
{
    RenderBackground(UIBackground.visual.texture);
    UpdateMusicStream(Theme);
    RenderTitle();
    UpdateParty();
    CreateTitlestars();
    PutUIParticles();
    UpdatePlay();
    RenderUIElement(&UIParty);
    PutUIButton(&UIPlay);
    RenderUIText("Version: 0.1", -0.95, 0.925,0.03, LEFTMOST, TitleScreenFont, WHITE);
    RenderUIText("Demo: 1", -0.95, 0.85,0.05, LEFTMOST, TitleScreenFont, WHITE);
    RenderUIText("Original Game By: Scott Cawthon", 0.9, 0.8,0.04, RIGHTMOST,TitleScreenFont, WHITE);
    RenderUIText("Remake By: SpyterDev", 0.9, 0.9,0.04, RIGHTMOST, TitleScreenFont, WHITE);
    
}