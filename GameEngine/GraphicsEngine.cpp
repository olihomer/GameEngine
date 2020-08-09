#include "GraphicsEngine.h"
#include "RenderSystem.h"

GraphicsEngine* GraphicsEngine::m_engine=nullptr;

GraphicsEngine::GraphicsEngine()
{
	OutputDebugString("Entering GraphicsEngine Constructor\n");
	try
	{
		m_render_system = new RenderSystem();
	}
	catch (...)
	{
		throw std::exception("GraphicsEngine failed to create Render System");
	}
}

GraphicsEngine::~GraphicsEngine()
{
	OutputDebugString("CLEANUP Entering GraphicsEngine Destructor\n");
	GraphicsEngine::m_engine = nullptr;
	delete m_render_system;
}

RenderSystem* GraphicsEngine::getRenderSystem()
{
	return m_render_system;
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
