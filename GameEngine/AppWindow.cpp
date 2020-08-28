#include <Windows.h>
#include <iostream>
#include "AppWindow.h"
#include "GraphicsEngine.h"
#include "RenderSystem.h"
#include "SwapChain.h"
#include "VertexShader.h"
#include "Vector3D.h"
#include "Vector2D.h"
#include "Matrix4x4.h"
#include "InputSystem.h"
#include "Mesh.h"
#include "Quaternion.h"

#define wUP Vector3D(0,1,0)
#define wX Vector3D(1,0,0)
#define wY Vector3D(0,1,0)
#define wZ Vector3D(0,0,1)

struct vertex
{
	Vector3D position;
	Vector2D texcoord;
};

__declspec(align(16))
struct constant
{
	Matrix4x4 m_world;
	Matrix4x4 m_view;
	Matrix4x4 m_proj;
	Vector4D m_light_direction;
	Vector4D m_camera_position;
};


void AppWindow::onCreate()
{
	OutputDebugString("Entering AppWindow::onCreate\n");
	Window::onCreate();

	RECT rc = this->getClientWindowRect();
	int width = rc.right - rc.left;
	int height = rc.bottom - rc.top;
	
	GraphicsEngine::get();
	
	// TESTING

	// INIT 2D PLOTTING BITMAP

	GraphicsEngine::get()->getRenderSystem()->getGraphics2D()->createBitmap(100, 100);

	// INIT INPUT SYSTEM

	InputSystem::get()->addListener(this);
	InputSystem::get()->hideCursor();
	InputSystem::get()->setCursorPosition(Point(width >> 1, height >> 1));

	//INIT TEXTURES

	m_wood = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"Assets\\Textures\\wood.jpg");
	m_sky = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"Assets\\Textures\\sky.jpg");

	//INIT MESHES

	m_mesh = GraphicsEngine::get()->getMeshManager()->createMeshFromFile(L"Assets\\Meshes\\statue.obj");
	m_sky_mesh = GraphicsEngine::get()->getMeshManager()->createMeshFromFile(L"Assets\\Meshes\\sphere.obj");

	//INIT SWAP CHAIN

	m_swap_chain = GraphicsEngine::get()->getRenderSystem()->createSwapChain(this->m_hwnd, width, height);
	if (this->m_hwnd == nullptr)OutputDebugString("Warning: m_hwnd is null\n");

	// PREPARE SHADERS

	void* shader_byte_code = nullptr;
	size_t size_shader = 0;

	GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"VertexShader.hlsl", "main", &shader_byte_code, &size_shader);
	m_vertex_shader = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();
		
	GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"PixelShader.hlsl", "main", &shader_byte_code, &size_shader);
	m_pixel_shader = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"SkyboxPS.hlsl", "main", &shader_byte_code, &size_shader);
	m_skybox_pixel_shader = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	// PREPARE CONSTANT BUFFERS
	
	constant cc;
	m_constant_buffer = GraphicsEngine::get()->getRenderSystem()->createConstantBuffer(&cc, sizeof(constant));
	m_skybox_constant_buffer = GraphicsEngine::get()->getRenderSystem()->createConstantBuffer(&cc, sizeof(constant));
	
	// CREATE CAMERA AND LIGHTS

	Vector3D pos = Vector3D(0, 0.5f, -2);
	Vector3D lookat = Vector3D(0, 0, 0);
	Vector3D up= Vector3D(0, 1, 0);
	m_camera = new Camera(pos,lookat,up, 1.0f, 0.1f, 100.0f, 90);

	m_light_position = Vector3D(1, 1, 1);
	
}

void AppWindow::onUpdate()
{
	Window::onUpdate();
	InputSystem::get()->update();

	if (m_is_run) {

		//CLEAR RENDER TARGET, UPDATE VIEWPORT SIZE
		DeviceContextPtr dc = GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext();
		dc->clearRenderTargetColor(this->m_swap_chain, 0.5F, 0.1F, 0.8F, 1.0F);
		RECT rc = this->getClientWindowRect();
		dc->setViewportSize(rc.right - rc.left, rc.bottom - rc.top);

		m_camera->setAspect(((float)(rc.right - rc.left)) / ((float)(rc.bottom - rc.top)));

		// UPDATE POSITIONS
		updateWorld();

		//DRAW 3D OBJECTS
		drawMesh(m_mesh, m_vertex_shader, m_pixel_shader, m_constant_buffer, m_wood,false);
		drawMesh(m_sky_mesh, m_vertex_shader, m_skybox_pixel_shader, m_skybox_constant_buffer, m_sky,true);

		//DRAW 2D GRAPHICS
		draw2DGraphics(rc);

		//PRESENT
		m_swap_chain->present(false);

		//TIMING
		LARGE_INTEGER counter;
		::QueryPerformanceCounter(&counter);

		m_old_delta = m_new_delta;
		m_new_delta = counter.QuadPart;
		m_delta_time = (float)m_old_delta ? ((m_new_delta - m_old_delta) / 10000000.0f) : 0;
	}
	else
	{
		InputSystem::get()->removeListener(this);
	}
}


void AppWindow::onDestroy()
{
	InputSystem::get()->showCursor();
	Window::onDestroy();
	delete m_camera;
}

void AppWindow::onKeyDown(int key)
{
	if (key == 'Z')
		m_trans_x -= 1.0f * m_delta_time;
	if (key == 'X')
		m_trans_x += 1.0f * m_delta_time;

	if (key == 'Q')
		m_camera->Roll(1.0f * m_delta_time);
	if (key == 'E')
		m_camera->Roll(-1.0f * m_delta_time);

	if (key == 'A')
		m_camera->Translate(-1.0f * m_delta_time,m_camera->getRight());
	if (key == 'D')
		m_camera->Translate(1.0f * m_delta_time, m_camera->getRight());
	if (key == 'S')
		m_camera->Translate(-2.0f * m_delta_time, m_camera->getForward());
	if (key == 'W')
		m_camera->Translate(2.0f * m_delta_time, m_camera->getForward());

	if (key == VK_ESCAPE)this->onDestroy();
}

void AppWindow::onKeyUp(int key)
{
	
}

void AppWindow::onMouseMove(const Point& point)
{
	RECT rc = this->getClientWindowRect();

	int width = rc.right - rc.left;
	int height = rc.bottom - rc.top;

	m_camera->Pitch(0.2f * (point.m_y-(height>>1)) * m_delta_time);
	m_camera->Yaw(0.2f * (point.m_x-(width>>1)) * m_delta_time);

	InputSystem::get()->setCursorPosition(Point(width>>1, height>>1));
}

void AppWindow::onLeftMouseDown(const Point& point)
{
	
}

void AppWindow::onRightMouseDown(const Point& point)
{
	m_camera->debugPrintState();
}

void AppWindow::onLeftMouseUp(const Point& point)
{
	
}

void AppWindow::onRightMouseUp(const Point& point)
{
	
}

void AppWindow::onFocus()
{
	InputSystem::get()->addListener(this);
}

void AppWindow::onKillFocus()
{
	InputSystem::get()->removeListener(this);
}


void AppWindow::updateWorld()
{
	constant cc;
	Matrix4x4 temp;
	
	//LIGHTING
	Quaternion::rotatePointAroundVector(m_light_position, m_delta_time * 1.0f, wUP);
	Vector3D dir = m_light_position;
	dir.normalize();	
	cc.m_light_direction = dir;
			
	//OBJECTS - LOCAL TO WORLD TRANSFORM
	cc.m_world.setIdentity();
	temp.setRotX(m_rot_x);
	cc.m_world *= temp;
	temp.setIdentity();
	temp.setRotY(m_rot_y);
	cc.m_world *= temp;
	temp.setIdentity();
	temp.setRotZ(0);
	cc.m_world *= temp;

	temp.setIdentity();
	temp.setTranslation(Vector3D(m_trans_x, 0, 0));
	cc.m_world *= temp;

	//LOAD CONSTANT BUFFER WITH CAMERA TRANSFORMS AND POSITION
	cc.m_view = m_camera->getCameraTransform();
	cc.m_camera_position = m_camera->getPosition();
	cc.m_proj = m_camera->getPerspectiveTransform();
	m_constant_buffer->update(&cc);

	//DIFFERENT CB FOR SKYBOX
	cc.m_world.setIdentity();
	cc.m_world.setScale(Vector3D(100.0f, 100.0f, 100.0f));
	cc.m_world.setTranslation(m_camera->getPosition());
	m_skybox_constant_buffer->update(&cc);

}

void AppWindow::drawMesh(const MeshPtr& mesh, const VertexShaderPtr& vs, const PixelShaderPtr& ps, const ConstantBufferPtr& cb, const TexturePtr& tex, bool cullfront)
{
	RenderSystem* system = GraphicsEngine::get()->getRenderSystem();
	DeviceContextPtr dc = system->getImmediateDeviceContext();

	system->setRasterizerState(cullfront);

	dc->setConstantBuffer(ps,cb);
	dc->setConstantBuffer(vs, cb);
	
	dc->setVertexShader(vs);
	dc->setPixelShader(ps);
		
	dc->setTexture(ps, tex);
	dc->setTexture(ps, tex);

	dc->setVertexBuffer(mesh->getVertexBuffer());
	dc->setIndexBuffer(mesh->getIndexBuffer());
	dc->drawIndexedTriangles(mesh->getIndexBuffer()->getSizeIndexList(), 0, 0);
}


void AppWindow::draw2DGraphics(RECT& rc)
{
	Graphics2DPtr g2D = GraphicsEngine::get()->getRenderSystem()->getGraphics2D();

	g2D->drawRectangle(rc.left + 100.0f, rc.top + 100.0f, rc.right - 100.0f, rc.bottom - 100.0f, D3DCOLORVALUE{ 0.8f, 0.0f, 0.0f,1.0f }, 1.0f, false);
	for (char i = 0; i < 100; i++)
		g2D->plotBitmap(i, i, RGB(0,i,i));
	g2D->updateBitmap();
	g2D->drawBitmap(rc.left + 100.0f, rc.top + 100.0f, rc.right - 100.0f, rc.bottom - 100.0f);
	g2D->drawText(L"Hello World", rc.left + 100.0f, rc.top + 100.0f, rc.right - 100.0f, rc.bottom - 100.0f, D3DCOLORVALUE{ 0.8f, 0.0f, 0.0f,1.0f }, 24);

}
