#pragma once
#include <unordered_map>
#include "Prerequisites.h"
#include <string>
#include <Windows.h>

class ResourceManager
{
public:
	ResourceManager() {};
	virtual ~ResourceManager();
	ResourcePtr createResourceFromFile(const wchar_t* file_path);
	ResourcePtr createResourceFromBitmap(void* bitmap, int width, int height, float horiz_scale, float vert_scale, int sample_scale);

protected:
	virtual Resource* createResourceFromFileConcrete(const wchar_t* file_path) = 0;
	virtual Resource* createResourceFromBitmapConcrete(void* bitmap, int width, int height, float horiz_scale, float vert_scale, int sample_scale) = 0;
private:
	std::unordered_map<std::wstring, ResourcePtr> m_map_resources;

};

