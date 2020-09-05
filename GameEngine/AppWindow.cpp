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
#include "HalfEdgeMesh.h"
#include "Quaternion.h"
#include "Perlin.h"
#include "dxgidebug.h"

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
	float m_time;
};


void AppWindow::onCreate()
{
	OutputDebugString("Entering AppWindow::onCreate\n");
	Window::onCreate();

	RECT rc = this->getClientWindowRect();
	int width = rc.right - rc.left;
	int height = rc.bottom - rc.top;
	
	GraphicsEngine* engine=GraphicsEngine::get();
	
	// INIT 2D PLOTTING BITMAP

	int land_width = 300;
	int land_height = 300;
	engine->getRenderSystem()->getGraphics2D()->createBitmap(land_width, land_height);

	// CREATE LANDSCAPE MESH

	CreateLandscape(land_width, land_height);
	m_landscape = engine->getMeshManager()->createMeshFromBitmap(engine->getRenderSystem()->getGraphics2D()->getBitmap(), land_width, land_height, 1.0f, 0.15f, 1);

	// INIT INPUT SYSTEM

	InputSystem::get()->addListener(this);
	InputSystem::get()->hideCursor();
	InputSystem::get()->setCursorPosition(Point(width >> 1, height >> 1));

	//INIT TEXTURES
	m_tex_array[0] = engine->getTextureManager()->createTextureFromFile(L"Assets\\Textures\\water.jpg");
	m_tex_array[1] = engine->getTextureManager()->createTextureFromFile(L"Assets\\Textures\\sand.jpg");
	m_tex_array[2] = engine->getTextureManager()->createTextureFromFile(L"Assets\\Textures\\grass.jpg");
	m_tex_array[3] = engine->getTextureManager()->createTextureFromFile(L"Assets\\Textures\\rock4.jpg");
	
	m_brick = engine->getTextureManager()->createTextureFromFile(L"Assets\\Textures\\brick.png");
	m_sky = engine->getTextureManager()->createTextureFromFile(L"Assets\\Textures\\sky.jpg");

	//INIT MESHES

	m_sky_mesh = engine->getMeshManager()->createMeshFromFile(L"Assets\\Meshes\\sphere.obj");

	//INIT SWAP CHAIN

	m_swap_chain = engine->getRenderSystem()->createSwapChain(this->m_hwnd, width, height);
	if (this->m_hwnd == nullptr)OutputDebugString("Warning: m_hwnd is null\n");

	// PREPARE SHADERS

	void* shader_byte_code = nullptr;
	size_t size_shader = 0;

	engine->getRenderSystem()->compileVertexShader(L"VertexShader.hlsl", "main", &shader_byte_code, &size_shader);
	m_vertex_shader = engine->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	engine->getRenderSystem()->releaseCompiledShader();
		
	engine->getRenderSystem()->compilePixelShader(L"PixelShader.hlsl", "main", &shader_byte_code, &size_shader);
	m_pixel_shader = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	engine->getRenderSystem()->releaseCompiledShader();

	engine->getRenderSystem()->compilePixelShader(L"SkyboxPS.hlsl", "main", &shader_byte_code, &size_shader);
	m_skybox_pixel_shader = engine->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	engine->getRenderSystem()->releaseCompiledShader();

	// PREPARE CONSTANT BUFFERS
	
	constant cc;
	m_constant_buffer = engine->getRenderSystem()->createConstantBuffer(&cc, sizeof(constant));
	m_skybox_constant_buffer = engine->getRenderSystem()->createConstantBuffer(&cc, sizeof(constant));
	
	// CREATE CAMERA AND LIGHTS

	Vector3D pos = Vector3D(100.0f,20.0f, -100.0f);
	//Vector3D pos = Vector3D(0.0f, 0.0f, -2.0f);
	Vector3D lookat = Vector3D(100.0f, 0.0f, 0.0f);
	Vector3D up= Vector3D(0, 1, 0);
	m_camera = new Camera(pos,lookat,up, 1.0f, 0.1f, 1000.0f, 90);

	m_light_position = Vector3D(0.2f, 1, 0.2f);
	
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
		drawMesh(m_landscape, m_vertex_shader, m_pixel_shader, m_constant_buffer, m_tex_array, 4,m_wireframe?CULL_WIREFRAME:CULL_BACK);
		drawMesh(m_sky_mesh, m_vertex_shader, m_skybox_pixel_shader, m_skybox_constant_buffer, &m_sky, 1,CULL_FRONT);

		//DRAW 2D GRAPHICS
		//draw2DGraphics(rc);

		//PRESENT
		m_swap_chain->present(false);

		//TIMING
		LARGE_INTEGER counter;
		::QueryPerformanceCounter(&counter);

		m_old_delta = m_new_delta;
		m_new_delta = counter.QuadPart;
		m_delta_time = (float)m_old_delta ? ((m_new_delta - m_old_delta) / 10000000.0f) : 0;
		m_time += m_delta_time;
		if (m_time > 100.0f)m_time = 0;
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
		m_camera->Translate(-30.0f * m_delta_time,m_camera->getRight());
	if (key == 'D')
		m_camera->Translate(30.0f * m_delta_time, m_camera->getRight());
	if (key == 'S')
		m_camera->Translate(-30.0f * m_delta_time, m_camera->getForward());
	if (key == 'W')
		m_camera->Translate(30.0f * m_delta_time, m_camera->getForward());

	if (key == VK_ESCAPE)this->onDestroy();

}

void AppWindow::onKeyUp(int key)
{
	if (key == VK_SPACE)m_wireframe = 1 - m_wireframe;
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
	
	//TIMING
	cc.m_time = m_time;

	//LIGHTING
	Quaternion::rotatePointAroundVector(m_light_position, m_delta_time * 1.0f, wUP);
	Vector3D dir = m_light_position;
	dir.normalize();	
	cc.m_light_direction = dir;

	// KEEP CAMERA ABOVE LANDSCAPE
	UpdateCamera();
				
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
	cc.m_time = 0;
	cc.m_world.setIdentity();
	cc.m_world.setScale(Vector3D(900.0f, 900.0f, 900.0f));
	cc.m_world.setTranslation(m_camera->getPosition());
	m_skybox_constant_buffer->update(&cc);

}

void AppWindow::UpdateCamera()
{

	float x = m_camera->getPosition().m_x;
	float y = m_camera->getPosition().m_y;
	float z = -m_camera->getPosition().m_z;

	float x_frac = x - (int)x;
	float z_frac = z - (int)z;

	Graphics2DPtr g2d = GraphicsEngine::get()->getRenderSystem()->getGraphics2D();

	BGRA* bitmap = g2d->getBitmap();
	Vector2D dim = g2d->getBitmapWidthHeight();

	float y1 = bitmap[(int)z * (int)dim.m_x + (int)x].a * 0.15f;
	float y2 = bitmap[(int)z * (int)dim.m_x + 1 + (int)x].a * 0.15f;
	float y3 = bitmap[(1 + (int)z) * (int)dim.m_x + (int)x].a * 0.15f;
	float y4 = bitmap[(1 + (int)z) * (int)dim.m_x + 1 + (int)x].a * 0.15f;

	float t1 = y1 + x_frac * (y2 - y1);
	float t2 = y3 + x_frac * (y4 - y3);
	float ly = t1 + z_frac * (t2 - t1);

	if (y < ly + 1)m_camera->setPosition(Vector3D(x, ly + 1, -z));
}


void AppWindow::drawMesh(const MeshPtr& mesh, const VertexShaderPtr& vs, const PixelShaderPtr& ps, const ConstantBufferPtr& cb, TexturePtr *tex, int num_textures, int cullmode)
{
	RenderSystem* system = GraphicsEngine::get()->getRenderSystem();
	DeviceContextPtr dc = system->getImmediateDeviceContext();

	system->setRasterizerState(cullmode);

	dc->setConstantBuffer(ps,cb);
	dc->setConstantBuffer(vs, cb);
	
	dc->setVertexShader(vs);
	dc->setPixelShader(ps);
		
	dc->setTexture(vs, tex, num_textures);
	dc->setTexture(ps, tex, num_textures);

	dc->setVertexBuffer(mesh->getVertexBuffer());
	dc->setIndexBuffer(mesh->getIndexBuffer());
	dc->drawIndexedTriangles(mesh->getIndexBuffer()->getSizeIndexList(), 0, 0);
}


void AppWindow::draw2DGraphics(RECT& rc)
{
	Graphics2DPtr g2D = GraphicsEngine::get()->getRenderSystem()->getGraphics2D();

	g2D->drawRectangle(rc.left + 100.0f, rc.top + 100.0f, rc.right - 100.0f, rc.bottom - 100.0f, D3DCOLORVALUE{ 0.8f, 0.0f, 0.0f,1.0f }, 1.0f, false);
	g2D->updateBitmap();
	g2D->drawBitmap(rc.left + 100.0f, rc.top + 100.0f, rc.left+750.0f, rc.top+750.0f);
	g2D->drawText(L"Hello World", rc.left + 100.0f, rc.top + 100.0f, rc.right - 100.0f, rc.bottom - 100.0f, D3DCOLORVALUE{ 0.8f, 0.0f, 0.0f,1.0f }, 24);

}

void AppWindow::CreateLandscape(int width, int height)
{
	Perlin p(true);
		
	float min = -1.2f;
	float max = 1.2f;
	float beach = 0.305f;
	float water = 0.3f;
	BYTE tex;

	unsigned int xplot = 0, yplot = 0;
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			float fi = i / 40.0f;
			float fj = j / 40.0f;
			float f = p.getNoise(fi, fj) + 0.5f * p.getNoise(i / 20.0f, j / 20.0f) + 0.25f * p.getNoise(i / 10.0f, j / 10.0f) + 0.1f * p.getNoise(i / 5.0f, j / 5.0f);
						
			f = (f - min) / (max - min); //map to [0,1]
			if (f > 1.0f)f = 1.0f;
			if (f < water)
				{
					f = water;
					tex = 0;
				}
			else
				{
				if (f < beach)
				{
					f = beach;
					tex = 0;
				}
					tex = 1;
				}
			
			BYTE h = (BYTE)(f * 255.0f); //map to [0,255]
			BGRA c = RGBA(tex, h, h, h);
			
			GraphicsEngine::get()->getRenderSystem()->getGraphics2D()->plotBitmap(xplot, yplot, c);
			xplot++;
		}
		yplot++;
		xplot = 0;
	}
	std::cout << "Fmin " << min << " Fmax " << max << std::endl;
}