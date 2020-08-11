#pragma once
#include "ResourceManager.h"
#include "Texture.h"

class TextureManager : public ResourceManager
{
public:
	TextureManager();
	~TextureManager();
	TexturePtr createTextureFromFile(const wchar_t* filepath);
protected:
	virtual Resource* createResourceFromFileConcrete(const wchar_t* file_path);
};

