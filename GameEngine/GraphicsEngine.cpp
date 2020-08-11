#include "GraphicsEngine.h"

GraphicsEngine* GraphicsEngine::m_engine=nullptr;

GraphicsEngine::GraphicsEngine()
{
	OutputDebugString("Entering GraphicsEngine Constructor\n");
	try
	{
		m_render_system = new RenderSystem();
		m_tex_manager = new TextureManager();
	}
	catch (...)
	{
		throw std::exception("GraphicsEngine failed to create system objects");
	}
}

GraphicsEngine::~GraphicsEngine()
{
	OutputDebugString("CLEANUP Entering GraphicsEngine Destructor\n");
	GraphicsEngine::m_engine = nullptr;
	delete m_tex_manager;
	delete m_render_system;
	
}

RenderSystem* GraphicsEngine::getRenderSystem()
{
	return m_render_system;
}

TextureManager* GraphicsEngine::getTextureManager()
{
	return m_tex_manager;
}


void GraphicsEngine::create()
{
	if (GraphicsEngine::m_engine) throw std::exception("GraphicsEngine already created");
	GraphicsEngine::m_engine = new GraphicsEngine();
}

void GraphicsEngine::release()
{
	if (!GraphicsEngine::m_engine) return;
	delete GraphicsEngine::m_engine;
}

GraphicsEngine* GraphicsEngine::get()
{
	return m_engine;
}
