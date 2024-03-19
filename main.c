#include "raylib.h"
#include "raymath.h"

#define KEY_SELECT KEY_ENTER
#define KEY_NEXT   KEY_SPACE

typedef enum {
    SCENE_MAIN,
    SCENE_GAME
} Scene;

Scene activeScene = SCENE_MAIN;

Vector3 carPosition = {0};
Vector3 carAcceleration = {0};
Camera3D camera = {0};

void drawButton(int x, int y, int w, int h, const char* text, int fontSize) {
    DrawRectangle(x, y, w, h, SKYBLUE);
    DrawText(text, x + w / 2 - MeasureText(text, fontSize) / 2, y + h / 2 - fontSize / 2, fontSize, BLACK);
}

void updateCamera(void) {
    UpdateCamera(&camera, CAMERA_FREE);
}

void startGame(void) {
    activeScene = SCENE_GAME;

    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    updateCamera();
    //carPosition = (Vector3){ 0.0f, 1.0f, 0.0f };

    camera.position = Vector3Add(carPosition, (Vector3){ 0.0f, 0.0f, 10.0f });
    camera.target = Vector3Add(carPosition, (Vector3){ 0.0f, 0.0f, 0.0f });

}

int main(void) {
    // init window
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(60);
    InitWindow(1000, 600, "gra");
    DisableCursor();

    // load assets
    Model car = LoadModel("./assets/car.obj");

    // game loop
    while (!WindowShouldClose()) {
        switch (activeScene) {
            // main menu
            case SCENE_MAIN: {
                float w = GetRenderWidth();

                if (IsKeyPressed(KEY_SELECT)) {
                    startGame();
                }

                BeginDrawing();
                    ClearBackground(RAYWHITE);

                    drawButton(w / 2 - 100, 100, 200, 50, "play", w / 10);
                EndDrawing();
            } break;

            // game
            case SCENE_GAME: {
                float dt = GetFrameTime();
                BoundingBox carBoundingBox = GetModelBoundingBox(car);
                carBoundingBox.min = Vector3Add(carBoundingBox.min, carPosition);
                carBoundingBox.max = Vector3Add(carBoundingBox.max, carPosition);

                //carPosition.x += 10*dt;
                updateCamera();

                BeginDrawing();

                    ClearBackground(SKYBLUE);

                    BeginMode3D(camera);

                        DrawPlane((Vector3){0, 0, 0}, (Vector2){100, 100}, GREEN);
                        DrawGrid(10, 1.0f);
                        DrawModel(car, carPosition, 1, WHITE);
                        DrawBoundingBox(carBoundingBox, GREEN);

                    EndMode3D();

                    DrawFPS(10, 10);

                EndDrawing();
            } break;
        }
    }

    // clean up
    CloseWindow();

    return 0;
}