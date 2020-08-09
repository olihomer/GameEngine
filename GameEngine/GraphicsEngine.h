#pragma once
#include <d3d11.h>
#include "Prerequisites.h"

class GraphicsEngine
{
public:
	bool init();
	bool release();
	static GraphicsEngine* get();
	GraphicsEngine();
	~GraphicsEngine();
	RenderSystem* getRenderSystem();
private:
	RenderSystem* m_render_system = nullptr;
};

