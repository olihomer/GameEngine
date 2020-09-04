#include "MeshManager.h"
#include "Mesh.h"
#include "Graphics2D.h"


MeshManager::MeshManager() :ResourceManager()
{
}

MeshManager::~MeshManager()
{
	OutputDebugString("CLEANUP MeshManager destructor\n");
}

MeshPtr MeshManager::createMeshFromFile(const wchar_t* file_path)
{
	return std::static_pointer_cast<Mesh>(createResourceFromFile(file_path));
}

MeshPtr MeshManager::createMeshFromBitmap(void* bitmap, int width, int height, float horiz_scale, float vert_scale,int sample_scale)
{
	return std::static_pointer_cast<Mesh>(createResourceFromBitmap(bitmap,width,height,horiz_scale,vert_scale,sample_scale));
}

Resource* MeshManager::createResourceFromFileConcrete(const wchar_t* file_path)
{
	Mesh* mesh = nullptr;
	try
	{
		mesh = new Mesh(file_path);
	}
	catch (...)
	{
		throw std::exception("createResourceFromFileConcrete Failed");
	}

	return mesh;

}

Resource* MeshManager::createResourceFromBitmapConcrete(void* bitmap, int width, int height, float horiz_scale, float vert_scale, int sample_scale)
{
	Mesh* mesh = nullptr;
	try
	{
		mesh = new Mesh((BGRA*)bitmap,width,height,horiz_scale,vert_scale, sample_scale);
	}
	catch (...)
	{
		throw std::exception("createResourceFromBitmapConcrete Failed");
	}

	return mesh;

}
