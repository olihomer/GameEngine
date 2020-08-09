#include <Windows.h>
#include <iostream>
#include "AppWindow.h"
#include "GraphicsEngine.h"
#include "RenderSystem.h"
#include "SwapChain.h"
#include "VertexShader.h"
#include "Vector3D.h"
#include "Matrix4x4.h"
#include "InputSystem.h"

using namespace std;

struct vertex
{
	Vector3D position;
	Vector3D color0;
	//Vector3D color1;
};

__declspec(align(16))
struct constant
{
	Matrix4x4 m_world;
	Matrix4x4 m_view;
	Matrix4x4 m_proj;
	unsigned int m_time=0;
};


void AppWindow::onCreate()
{
	OutputDebugString("Entering AppWindow::onCreate\n");
	Window::onCreate();

	RECT rc = this->getClientWindowRect();
	int width = rc.right - rc.left;
	int height = rc.bottom - rc.top;

	GraphicsEngine::get()->init();
		
	// INIT INPUT SYSTEM

	InputSystem::get()->addListener(this);
	InputSystem::get()->hideCursor();
	InputSystem::get()->setCursorPosition(Point(width >> 1, height >> 1));

	//INIT SWAP CHAIN

	m_swap_chain = GraphicsEngine::get()->getRenderSystem()->createSwapChain(this->m_hwnd, width, height);

	if (this->m_hwnd == nullptr)OutputDebugString("Warning: m_hwnd is null\n");

	// PREPARE VERTEX BUFFER, INDEX BUFFER, VERTEX DATA AND VERTEX SHADER
	
	vertex vertex_list[] =
	{
		{Vector3D(-0.5F, -0.5F, -0.5F), Vector3D(1,0,0)},
		{Vector3D(-0.5F, 0.5F,  -0.5F), Vector3D(1,1,0)},  
		{Vector3D(0.5F,0.5F,  -0.5F) , Vector3D(1, 0, 1)}, 
		{Vector3D(0.5F,-0.5F,  -0.5F) , Vector3D(0, 1, 1)},
		{Vector3D(0.5F, -0.5F, 0.5F), Vector3D(1,0,0.5)},
		{Vector3D(0.5F, 0.5F,  0.5F), Vector3D(1,1,0.5)},
		{Vector3D(-0.5F,0.5F,  0.5F) , Vector3D(1, 0.5, 1)},
		{Vector3D(-0.5F,-0.5F,  0.5F) , Vector3D(0.5, 1, 1)}
	};
		
	UINT index_list[] =
	{
		0,1,2,
		2,3,0,
		4,5,6,
		6,7,4,
		1,6,5,
		5,2,1,
		7,0,3,
		3,4,7,
		3,2,5,
		5,4,3,
		7,6,1,
		1,0,7
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
	cc.m_time = 0;

	m_constant_buffer = GraphicsEngine::get()->getRenderSystem()->createConstantBuffer(&cc, sizeof(constant));
	
	// CREATE CAMERA

	Vector3D pos = Vector3D(0, 0, -4);
	Vector3D lookat = Vector3D(0, 0, 0);
	Vector3D up= Vector3D(0, 1, 0);
	m_camera = new Camera(pos,lookat,up, 1.0f, 0.1f, 100.0f, 90);
	
}

void AppWindow::onUpdate()
{
	Window::onUpdate();
	InputSystem::get()->update();
	
	if (isRun()) {

		//CLEAR RENDER TARGET, UPDATE VIEWPORT SIZE
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->clearRenderTargetColor(this->m_swap_chain, 0.5F, 0.1F, 0.8F, 1.0F);
		RECT rc = this->getClientWindowRect();
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setViewportSize(rc.right - rc.left, rc.bottom - rc.top);
				
		m_camera->setAspect (((float)(rc.right - rc.left)) / ((float)(rc.bottom - rc.top)));

		// UPDATE POSITIONS
		
		this->updateQuadPosition();
				
		// UPDATE AND BIND CONSTANT BUFFER
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_pixel_shader, m_constant_buffer);
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_vertex_shader, m_constant_buffer);
		
		// SET SHADERS AND VERTEX BUFFER AND INDEX BUFFER
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_vertex_shader);
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_pixel_shader);
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexBuffer(m_vertex_buffer);
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setIndexBuffer(m_index_buffer);
		
		// DRAW AND PRESENT
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->drawIndexedTriangles(m_index_buffer->getSizeIndexList(), 0, 0);
		//GraphicsEngine::get()->getImmediateDeviceContext()->drawTriangleList(m_vertex_buffer->getSizeVertexList(), 0);
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
	GraphicsEngine::get()->release();
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

	m_camera->Pitch(0.5f * (point.m_y-(height>>1)) * m_delta_time);
	m_camera->Yaw(0.5f * (point.m_x-(width>>1)) * m_delta_time);

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


void AppWindow::updateQuadPosition()
{
	constant cc;
	cc.m_time = (int)m_new_delta;

	m_delta_pos += m_delta_time/4.0f;
	m_delta_scale += m_delta_time * 3.0f;
	if (m_delta_pos > 1.0f)m_delta_pos = 0;

	RECT rc = this->getClientWindowRect();
	
	Matrix4x4 temp;
	
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
