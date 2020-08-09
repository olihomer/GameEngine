#pragma once
#include <d3d11.h>
#include "Prerequisites.h"

class GraphicsEngine
{
public:
	static GraphicsEngine* get();
	RenderSystem* getRenderSystem();
	static void create();
	static void release();
private:
	RenderSystem* m_render_system = nullptr;
	GraphicsEngine();
	~GraphicsEngine();
	static GraphicsEngine* m_engine;
};

