#define  STB_DS_IMPLEMENTATION
#include "stb_ds.h"

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
#define CAR_ACCELERATION  18.0f/MPS2KMPH*PSS
#define CAR_DECELERAION   10.0f*PSS
#define CAR_MAX_SPEED     200.0f/MPS2KMPH*PS
#define CAR_TURN_FORCE    2*DEG2RAD

// camera
#define CAMERA_FOCUS      (Vector3){ 4.0f, 1.0f, 0.0f }
#define CAMERA_TARGET_POS (Vector3){ -5.0f, 2.0f, 0.0f }


char stringBuffer[256] = { 0 };

typedef enum {
    OBJECT_GROUND
} ObjectType;

typedef struct {
    ObjectType type;
    Vector3 position;
    float scale;
} SavedObject;

typedef struct {
    SavedObject data;
    Model model;
} Object;

Object *levelObjects = NULL;

void loadLevel(const char *filePath, Object *objects)
{
    int bufferSize;
    char *buffer = LoadFileData(filePath, &bufferSize);

    int objectsCount = bufferSize / sizeof(SavedObject);
    arrsetlen(objects, objectsCount);

    for (int i = 0; i < objectsCount; i++) {
        Object object;
        memcpy(&object + offsetof(Object, data), buffer + i*sizeof(SavedObject), sizeof(SavedObject));
        objects[i] = object;

        switch (object.data.type)
        {
            case OBJECT_GROUND: {
                object.model = LoadModelFromMesh(GenMeshPlane(10.0f, 10.0f, 1, 1));
                break;
            }
            default: {
                printf("TODO: assert\n");
            }
        }
    }

    return 0;
}

int main(void)
{
    InitWindow(1000, 600, "GRA");
    SetTargetFPS(60);
    DisableCursor();

    Model car = LoadModel("./assets/car.obj");

    loadLevel("./level.lvl", levelObjects);

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

    Model ground = LoadModelFromMesh(GenMeshPlane(200, 200, 1, 1));
    ground.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = LoadTexture("./assets/track.png");

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

            float inputFactor = 1*IsKeyDown(KEY_W) + -1*IsKeyDown(KEY_S);


            if(IsKeyDown(KEY_SPACE))
            {
                if (engineV >= 0)
                {
                    engineV = fmaxf(0, engineV - CAR_DECELERAION - 5*engineV/((float)CAR_MAX_SPEED));
                }
                else
                {
                    engineV = fminf(0, engineV + CAR_DECELERAION - 5*engineV/((float)CAR_MAX_SPEED));
                }

                engineV = fmaxf(0, engineV - CAR_DECELERAION - 10*engineV/CAR_MAX_SPEED);
            }
            else
            {
                engineV += (CAR_MAX_SPEED - engineV)/((float)CAR_MAX_SPEED)*CAR_ACCELERATION*inputFactor;
            }
            v = Vector3Transform((Vector3){ engineV, 0.0f, 0.0f }, car.transform);

            r = Vector3Add(r, v);

            if (IsKeyDown(KEY_A)) rot.y += CAR_TURN_FORCE*engineV/((float)CAR_MAX_SPEED);
            if (IsKeyDown(KEY_D)) rot.y -= CAR_TURN_FORCE*engineV/((float)CAR_MAX_SPEED);
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

                for (int i = 0; i < arrlen(levelObjects); i++) {
                    Object object = levelObjects[i];
                    DrawModel(object.model, object.data.position, object.data.scale, GREEN);
                }

                DrawModel(ground, (Vector3){0.0f, 0.0f, 0.0f}, 1.0f, WHITE);
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