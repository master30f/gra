#include "raylib.h"
#include "raymath.h"
#include "stdio.h"

#define CUBE_SIZE        1.0f
#define FONT_SIZE        20

#define SNAPSHOT_LENGTH  0.01f
#define PS               (SNAPSHOT_LENGTH)
#define PSS              (SNAPSHOT_LENGTH*SNAPSHOT_LENGTH)

#define CAR_ACCELERATION 5.0f*PSS

char stringBuffer[256] = { 0 };

int main(void)
{
    InitWindow(1000, 600, "Physics test");
    SetTargetFPS(60);
    DisableCursor();

    Model car = LoadModel("./assets/car.obj");
    BoundingBox bb = GetModelBoundingBox(car);
    printf("%f %f %f", bb.max.x-bb.min.x, bb.max.y-bb.min.y, bb.max.z-bb.min.z);

    Camera3D camera   = { 0 };
    camera.position   = (Vector3){ -2.0f, 1.0f, 0.0f };
    camera.target     = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera.up         = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.projection = CAMERA_PERSPECTIVE;
    camera.fovy       = 45.0f;

    Vector3 r         = (Vector3){ 0.0f, 0.0f, 0.0f };
    Vector3 v         = (Vector3){ 0.0f, 0.0f, 0.0f };
    Vector3 a         = (Vector3){ 0.0f, 0.0f, 0.0f };

    Vector3 rot       = (Vector3){ 0.0f, 0.0f, 0.0f };
    float engineV     = 0.0f;

    float queuedTime = 0.0f;

    while (!WindowShouldClose())
    {
        float h = GetRenderHeight();
        float dt = GetFrameTime();
        queuedTime += dt;

        while (queuedTime >= SNAPSHOT_LENGTH)
        {
            queuedTime -= SNAPSHOT_LENGTH;

            r = Vector3Add(r, v);

            v = Vector3Transform((Vector3){ engineV, 0.0f, 0.0f }, car.transform);
            if (IsKeyDown(KEY_I)) {
                engineV += CAR_ACCELERATION;
            }

            //a = Vector3Scale(Vector3Multiply(v, v), -0.153125f*PSS);

            if (IsKeyDown(KEY_J))
            {
                rot.y += 1*DEG2RAD;
            }
            if (IsKeyDown(KEY_L))
            {
                rot.y -= 1*DEG2RAD;
            }

            car.transform = MatrixRotateXYZ(rot);
        }

        if (IsKeyPressed(KEY_O))
        {
            v.y += 1.0f*PS;
        }
        if (IsKeyPressed(KEY_U))
        {
            v.y -= 1.0f*PS;
        }

        UpdateCamera(&camera, CAMERA_FREE);

        BeginDrawing();
            BeginMode3D(camera);
                ClearBackground(RAYWHITE);

                DrawGrid(50, 1.0f);
                DrawModel(car, r, 1.0f, WHITE);
            EndMode3D();

            sprintf(stringBuffer, "%d FPS", GetFPS());
            DrawText(stringBuffer, 10, 10, FONT_SIZE, BLACK);
            
            sprintf(stringBuffer, "r = %+.3f %+.3f %+.3f", r.x, r.y, r.z);
            DrawText(stringBuffer, 10, 20 + FONT_SIZE, FONT_SIZE, BLACK);
            sprintf(stringBuffer, "v = %+.3f %+.3f %+.3f", v.x/PS, v.y/PS, v.z/PS);
            DrawText(stringBuffer, 10, 25 + FONT_SIZE*2, FONT_SIZE, BLACK);
            sprintf(stringBuffer, "a = %+.3f %+.3f %+.3f", a.x/PSS, a.y/PSS, a.z/PSS);
            DrawText(stringBuffer, 10, 30 + FONT_SIZE*3, FONT_SIZE, BLACK);

            sprintf(stringBuffer, "%.0f km/h", Vector3Length(v)/PS*3.6f);
            DrawText(stringBuffer, 10, h - 10 - FONT_SIZE, FONT_SIZE, BLACK);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}