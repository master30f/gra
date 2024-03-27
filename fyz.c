#include "raylib.h"
#include "raymath.h"
#include "stdio.h"


// misc
#define FONT_SIZE         20

// physics
#define SNAPSHOT_LENGTH   0.01f
#define PS                (SNAPSHOT_LENGTH)
#define PSS               (SNAPSHOT_LENGTH*SNAPSHOT_LENGTH)
#define MPS2KMPH          3.6f

// car
#define CAR_ACCELERATION  5.0f*PSS
#define CAR_DECELERAION   100.0f*PSS
#define CAR_MAX_SPEED     200.0f/MPS2KMPH*PS

// camera
#define CAMERA_FOCUS      (Vector3){ 4.0f, 1.0f, 0.0f }
#define CAMERA_TARGET_POS (Vector3){ -5.0f, 2.0f, 0.0f }


char stringBuffer[256] = { 0 };


int main(void)
{
    InitWindow(1000, 600, "Physics test");
    SetTargetFPS(60);
    DisableCursor();

    Model car = LoadModel("./assets/car.obj");

    Camera3D camera   = { 0 };
    camera.up         = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.projection = CAMERA_PERSPECTIVE;
    camera.fovy       = 45.0f;
    Vector3 cameraT   = (Vector3){ 0.0f, 0.0f, 0.0f };
    Vector3 cameraV   = (Vector3){ 0.0f, 0.0f, 0.0f };

    Vector3 r         = (Vector3){ 0.0f, 0.0f, 0.0f };
    Vector3 v         = (Vector3){ 0.0f, 0.0f, 0.0f };
    Vector3 rot       = (Vector3){ 0.0f, 0.0f, 0.0f };
    float engineV     = 0.0f;

    float queuedTime = 0.0f;

    while (!WindowShouldClose())
    {
        // update

        float h = GetRenderHeight();
        float dt = GetFrameTime();
        queuedTime += dt;

        // physics

        while (queuedTime >= SNAPSHOT_LENGTH)
        {
            queuedTime -= SNAPSHOT_LENGTH;

            if (IsKeyDown(KEY_W))
            {
                engineV += (CAR_MAX_SPEED - engineV)/((float)CAR_MAX_SPEED)*CAR_ACCELERATION;
            }
            if (IsKeyDown(KEY_S))
            {
                engineV -= (CAR_MAX_SPEED + engineV)/((float)CAR_MAX_SPEED)*CAR_ACCELERATION;
            }
            if(IsKeyDown(KEY_SPACE))
            {
                engineV = fmaxf(0, engineV - CAR_DECELERAION);
            }
            v = Vector3Transform((Vector3){ engineV, 0.0f, 0.0f }, car.transform);

            r = Vector3Add(r, v);

            if (IsKeyDown(KEY_A)) rot.y += 1*DEG2RAD;
            if (IsKeyDown(KEY_D)) rot.y -= 1*DEG2RAD;
            car.transform = MatrixRotateXYZ(rot);

            camera.target   = Vector3Add(r, Vector3Transform(CAMERA_FOCUS, car.transform));
            cameraT         = Vector3Add(r, Vector3Transform(CAMERA_TARGET_POS, car.transform));
            cameraV         = Vector3Scale(Vector3Subtract(cameraT, camera.position), 0.1f);
            camera.position = Vector3Add(camera.position, cameraV);
        }

        // render

        BeginDrawing();
            BeginMode3D(camera);
                ClearBackground(RAYWHITE);

                DrawGrid(500, 1.0f);
                DrawModel(car, r, 1.0f, WHITE);
            EndMode3D();

            sprintf(stringBuffer, "%d FPS", GetFPS());
            DrawText(stringBuffer, 10, 10, FONT_SIZE, BLACK);
            
            sprintf(stringBuffer, "r = %+.3f %+.3f %+.3f", r.x, r.y, r.z);
            DrawText(stringBuffer, 10, 20 + FONT_SIZE, FONT_SIZE, BLACK);
            sprintf(stringBuffer, "v = %+.3f %+.3f %+.3f", v.x/PS, v.y/PS, v.z/PS);
            DrawText(stringBuffer, 10, 25 + FONT_SIZE*2, FONT_SIZE, BLACK);

            sprintf(stringBuffer, "%.0f km/h", Vector3Length(v)/PS*MPS2KMPH);
            DrawText(stringBuffer, 10, h - 10 - FONT_SIZE, FONT_SIZE, BLACK);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}