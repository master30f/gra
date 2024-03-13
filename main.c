#include <stdio.h>
#include "raylib.h"
#include <math.h>

typedef enum {
    SCENE_MAIN = 0,
    SCENE_GAME
} Scene;

char* text2 = "SKINS";
char* text = "PLAY";
int fontSize = 40;
Scene activeScene = SCENE_MAIN;

Texture2D car;
float carX = 0;
float carY = 0;
int carXDir = 1;
int carYDir = 1;
float carSpeed = 600;
float cameraTilt = 0.4;
float fov = 0.6;
int roadFragments = 25;
float roadPercentage = 0.7;

int button_state = -1;

void tickMainMenu(void) {
    int w = GetRenderWidth();
    int h = GetRenderHeight();

    if (IsKeyPressed(KEY_ENTER)) {
        activeScene = SCENE_GAME;
    }

    if (IsKeyPressed(KEY_SPACE)) {
        button_state = button_state*(-1);
    }
    
    
    
    
    int button_width = w/5;
    int button_hight = h/5;
    int textP_length = MeasureText(text2, fontSize)/2;
    int text_length = MeasureText(text, fontSize)/2;

    BeginDrawing();
        ClearBackground(BLACK);
        
        if (button_state==1){
            DrawRectangle(w/4 - text_length, h/2-fontSize, button_width, button_hight, GRAY);
        }
        else{
            DrawRectangle(3*w/4 - textP_length, h/2-fontSize, 3*w/4, h/5, GRAY);
        }
        
        DrawText(text, 3*(w/4) - text_length, h/2-fontSize/2, fontSize, WHITE);
        DrawText(text2, w/4 - textP_length, h/2-fontSize/2, fontSize, WHITE);
    EndDrawing();
}

void tickGame(void) {
    float w = GetRenderWidth();
    float h = GetRenderHeight();
    float dt = GetFrameTime();

    float roadWidth = w * roadPercentage;
    float roadFragmentHeight = ceilf(h * (1 - cameraTilt) / roadFragments);

    carX += carXDir * dt * carSpeed;
    carY += carYDir * dt * carSpeed;

    if (carY + car.height >= h || carY <= 0) carYDir *= -1;
    if (carX + car.width >+ w || carX <= 0)  carXDir *= -1;

    BeginDrawing();
        ClearBackground(BLUE);
        DrawRectangle(0, h * cameraTilt, w, h * (1 - cameraTilt), GREEN);

        for (int i = 0; i < roadFragments; i++) {
            float fragmentWidth = roadWidth * fov + roadWidth * (1 - fov) / roadFragments * i;
            DrawRectangle(w / 2 - fragmentWidth / 2 + (((float)roadFragments)/((float)i+2))*20, h * cameraTilt + i * roadFragmentHeight, fragmentWidth, roadFragmentHeight, BLACK);
        }

        DrawTexture(car, w / 2 - car.width / 2, h - car.height, WHITE);
    EndDrawing();
}

int main(void) {
    InitWindow(800, 600, "gra");
    SetWindowState(FLAG_WINDOW_RESIZABLE);

    car = LoadTexture("images/car.png");

    while (!WindowShouldClose()) {
        if (activeScene == SCENE_MAIN) tickMainMenu();
        else if (activeScene == SCENE_GAME) tickGame();
    }

    return 0;
}