#pragma once
#include "ResourceManager.h"
#include "Prerequisites.h"


class MeshManager :
    public ResourceManager
{
public:
    MeshManager();
    ~MeshManager();
    MeshPtr createMeshFromFile(const wchar_t* filepath);
    MeshPtr createMeshFromBitmap(void* bitmap, int width, int height, float horiz_scale, float vert_scale);
protected:
    virtual Resource* createResourceFromFileConcrete(const wchar_t* file_path);
    virtual Resource* createResourceFromBitmapConcrete(void* bitmap, int width, int height, float horiz_scale, float vert_scale);
};

