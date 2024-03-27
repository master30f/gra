#define  STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#include "raylib.h"
#include <stdio.h>

typedef enum {
    OBJECT_TREE
} ObjectType;

typedef struct {
    ObjectType type;
    Vector3 position;
    float scale;
    Model model;
} Object;

Object *levelObjects = NULL;

//char *buffer = NULL;

int main(void)
{
    /*arrput(levelObjects, ((Object){
        .type     = OBJECT_TREE,
        .model    = { 0 },
        .position = (Vector3){ 0, 0, 0 },
        .scale    = 0.01f
    }));
    arrput(levelObjects, ((Object){
        .type     = OBJECT_TREE,
        .model    = { 0 },
        .position = (Vector3){ 0, 0, 0 },
        .scale    = 0.01f
    }));

    int savedObjectSize = sizeof(ObjectType) + sizeof(Vector3) + sizeof(float);
    int bufferSize = arrlen(levelObjects)*savedObjectSize;

    arrsetlen(buffer, bufferSize);

    for (int i; i < arrlen(levelObjects); i++)
    {
        memcpy(buffer + i*savedObjectSize, &levelObjects[i], savedObjectSize);
    }

    if (!SaveFileData("./level.lvl", buffer, bufferSize)) return -1;

    arrfree(levelObjects);
    arrfree(buffer);*/

    int savedObjectSize = sizeof(ObjectType) + sizeof(Vector3) + sizeof(float);

    int bufferSize;
    char *buffer = LoadFileData("./level.lvl", &bufferSize);

    int objectsCount = bufferSize / savedObjectSize;
    arrsetcap(levelObjects, objectsCount);

    for (int i = 0; i < objectsCount; i++) {
        Object object;
        memcpy(&object, buffer + i*savedObjectSize, savedObjectSize);
        arrput(levelObjects, object);
    }

    printf("ondrovy objekty:\n");
    for (int i = 0; i < arrlen(levelObjects); i++)
    {
        printf("\ttype: %d, position: %f %f %f\n", levelObjects[i].type, levelObjects[i].position.x, levelObjects[i].position.y, levelObjects[i].position.z);
    }

    return 0;
}