#pragma once
#include <d3d11.h>
#include "Prerequisites.h"
#include "RenderSystem.h"
#include "TextureManager.h"

class GraphicsEngine
{
public:
	static GraphicsEngine* get();
	RenderSystem* getRenderSystem();
	TextureManager* getTextureManager();
	static void create();
	static void release();
private:
	RenderSystem* m_render_system = nullptr;
	TextureManager* m_tex_manager = nullptr;
	static GraphicsEngine* m_engine;
	GraphicsEngine();
	~GraphicsEngine();
	
};

