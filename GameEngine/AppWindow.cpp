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
};


void AppWindow::onCreate()
{
	OutputDebugString("Entering AppWindow::onCreate\n");
	Window::onCreate();

	RECT rc = this->getClientWindowRect();
	int width = rc.right - rc.left;
	int height = rc.bottom - rc.top;

	GraphicsEngine::get();
		
	// INIT INPUT SYSTEM

	InputSystem::get()->addListener(this);
	InputSystem::get()->hideCursor();
	InputSystem::get()->setCursorPosition(Point(width >> 1, height >> 1));

	//INIT TEXTURES

	m_wood = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"Assets\\Textures\\wood.jpg");

	//INIT MESHES

	m_mesh = GraphicsEngine::get()->getMeshManager()->createMeshFromFile(L"Assets\\Meshes\\statue.obj");

	//INIT SWAP CHAIN AND DEPTH BUFFER

	m_swap_chain = GraphicsEngine::get()->getRenderSystem()->createSwapChain(this->m_hwnd, width, height);
	//createDepthBuffer(m_swap_chain, )

	if (this->m_hwnd == nullptr)OutputDebugString("Warning: m_hwnd is null\n");

	// PREPARE VERTEX BUFFER, INDEX BUFFER, VERTEX DATA AND VERTEX SHADER
	
	Vector3D position_list[] =
	{
		Vector3D(-0.5F,-0.5F, -0.5F),
		Vector3D(-0.5F, 0.5F, -0.5F),
		Vector3D(0.5F,  0.5F, -0.5F),
		Vector3D(0.5F, -0.5F, -0.5F),
		Vector3D(0.5F, -0.5F,  0.5F),
		Vector3D(0.5F,  0.5F,  0.5F),
		Vector3D(-0.5F, 0.5F,  0.5F),
		Vector3D(-0.5F,-0.5F,  0.5F)
	};

	Vector2D texcoord_list[] =
	{
		Vector2D(0.0f,0.0f),
		Vector2D(0.0f,1.0f),
		Vector2D(1.0f,0.0f),
		Vector2D(1.0f,1.0f)
	};

	vertex vertex_list[] =
	{
		{position_list[0],texcoord_list[1]},
		{position_list[1],texcoord_list[0]},
		{position_list[2],texcoord_list[2]},
		{position_list[3],texcoord_list[3]},

		{position_list[4],texcoord_list[1]},
		{position_list[5],texcoord_list[0]},
		{position_list[6],texcoord_list[2]},
		{position_list[7],texcoord_list[3]},

		{position_list[1],texcoord_list[1]},
		{position_list[6],texcoord_list[0]},
		{position_list[5],texcoord_list[2]},
		{position_list[2],texcoord_list[3]},

		{position_list[7],texcoord_list[1]},
		{position_list[0],texcoord_list[0]},
		{position_list[3],texcoord_list[2]},
		{position_list[4],texcoord_list[3]},

		{position_list[3],texcoord_list[1]},
		{position_list[2],texcoord_list[0]},
		{position_list[5],texcoord_list[2]},
		{position_list[4],texcoord_list[3]},

		{position_list[7],texcoord_list[1]},
		{position_list[6],texcoord_list[0]},
		{position_list[1],texcoord_list[2]},
		{position_list[0],texcoord_list[3]}

	};
		
	UINT index_list[] =
	{
		0,1,2,
		2,3,0,

		4,5,6,
		6,7,4,

		8,9,10,
		10,11,8,
		
		12,13,14,
		14,15,12,

		16,17,18,
		18,19,16,

		20,21,22,
		22,23,20
	};

	void* shader_byte_code = nullptr;
	size_t size_shader = 0;

	GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"VertexShader.hlsl", "main", &shader_byte_code, &size_shader);
	m_vertex_shader = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	
	UINT size_list = ARRAYSIZE(vertex_list);
	UINT size_vertex = sizeof(vertex);

	//LOAD VERTEX BUFFER

	m_vertex_buffer = GraphicsEngine::get()->getRenderSystem()->createVertexBuffer(vertex_list, size_vertex, size_list, shader_byte_code, (UINT)size_shader);

	//LOAD INDEX BUFFER

	size_list = ARRAYSIZE(index_list);
	m_index_buffer = GraphicsEngine::get()->getRenderSystem()->createIndexBuffer(index_list, size_list);
	
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();
		
	// PREPARE PIXEL SHADER

	GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"PixelShader.hlsl", "main", &shader_byte_code, &size_shader);

	m_pixel_shader = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);

	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	// PREPARE AND LOAD CONSTANT BUFFER
	
	constant cc;
	
	m_constant_buffer = GraphicsEngine::get()->getRenderSystem()->createConstantBuffer(&cc, sizeof(constant));
	
	// CREATE CAMERA

	Vector3D pos = Vector3D(0, 0, -2);
	Vector3D lookat = Vector3D(0, 0, 0);
	Vector3D up= Vector3D(0, 1, 0);
	m_camera = new Camera(pos,lookat,up, 1.0f, 0.1f, 100.0f, 90);
	
}

void AppWindow::onUpdate()
{
	Window::onUpdate();
	InputSystem::get()->update();

	if (m_is_run) {

		//CLEAR RENDER TARGET, UPDATE VIEWPORT SIZE
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->clearRenderTargetColor(this->m_swap_chain, 0.5F, 0.1F, 0.8F, 1.0F);
		RECT rc = this->getClientWindowRect();
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setViewportSize(rc.right - rc.left, rc.bottom - rc.top);
				
		m_camera->setAspect (((float)(rc.right - rc.left)) / ((float)(rc.bottom - rc.top)));

		// UPDATE POSITIONS
		
		this->updateWorld();
				
		// UPDATE AND BIND CONSTANT BUFFER
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_pixel_shader, m_constant_buffer);
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_vertex_shader, m_constant_buffer);

		//BIND TEXTURE TO SHADERS
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setTexture(m_pixel_shader, m_wood);
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setTexture(m_vertex_shader, m_wood);
		
		// SET SHADERS AND VERTEX BUFFER AND INDEX BUFFER
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_vertex_shader);
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_pixel_shader);
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexBuffer(m_mesh->getVertexBuffer());
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setIndexBuffer(m_mesh->getIndexBuffer());

		// DRAW AND PRESENT
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->drawIndexedTriangles(m_mesh->getIndexBuffer()->getSizeIndexList(), 0, 0);
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
		m_camera->Translate(-3.0f * m_delta_time,m_camera->getRight());
	if (key == 'D')
		m_camera->Translate(3.0f * m_delta_time, m_camera->getRight());
	if (key == 'S')
		m_camera->Translate(-3.0f * m_delta_time, m_camera->getForward());
	if (key == 'W')
		m_camera->Translate(3.0f * m_delta_time, m_camera->getForward());

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
	
	Matrix4x4 m_light_rot_matrix;
	m_light_angle += m_delta_time;
	m_light_rot_matrix.setRotY(m_light_angle);
	cc.m_light_direction = m_light_rot_matrix.getForward();
	m_light_rot_matrix.debugPrint("Light:\n");
	
	//LOCAL TO WORLD TRANSFORM

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

	//WORLD TO CAMERA TRANSFORM

	cc.m_view = m_camera->getCameraTransform();
	
	//PERSPECTIVE TRANSFORM

	cc.m_proj = m_camera->getPerspectiveTransform();

	m_constant_buffer->update(GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext(), &cc);
}
