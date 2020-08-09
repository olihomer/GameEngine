#include "GraphicsEngine.h"
#include "RenderSystem.h"


bool GraphicsEngine::init()
{
	return true;
}

bool GraphicsEngine::release()
{	
	return true;
}

GraphicsEngine* GraphicsEngine::get()
{
	static GraphicsEngine engine;
	return &engine;
}

GraphicsEngine::GraphicsEngine()
{
	OutputDebugString("Entering GraphicsEngine Constructor\n");
	m_render_system = new RenderSystem();
}

GraphicsEngine::~GraphicsEngine()
{
	OutputDebugString("CLEANUP Entering GraphicsEngine Destructor\n");
	delete m_render_system;
}

RenderSystem* GraphicsEngine::getRenderSystem()
{
	return m_render_system;
}

