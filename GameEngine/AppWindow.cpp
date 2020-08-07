#include <Windows.h>
#include "AppWindow.h"
#include "GraphicsEngine.h"
#include "SwapChain.h"
#include "VertexShader.h"
#include "Vector3D.h"
#include "Matrix4x4.h"
#include "InputSystem.h"


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
	unsigned int m_time;
};


void AppWindow::onCreate()
{
	OutputDebugString("Entering AppWindow::onCreate\n");
	Window::onCreate();
	GraphicsEngine::get()->init();
	InputSystem::get()->addListener(this);

	//INIT SWAP CHAIN

	m_swap_chain = GraphicsEngine::get()->createSwapChain();
	RECT rc = this->getClientWindowRect();

	if (this->m_hwnd == nullptr)OutputDebugString("Warning: m_hwnd is null\n");
	m_swap_chain->init(this->m_hwnd, rc.right - rc.left, rc.bottom - rc.top);

	// PREPARE VERTEX BUFFER, INDEX BUFFER, VERTEX DATA AND VERTEX SHADER

	m_vertex_buffer = GraphicsEngine::get()->createVertexBuffer();
	m_index_buffer = GraphicsEngine::get()->createIndexBuffer();

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

	GraphicsEngine::get()->compileVertexShader(L"VertexShader.hlsl", "main", &shader_byte_code, &size_shader);

	m_vertex_shader = GraphicsEngine::get()->createVertexShader(shader_byte_code, size_shader);
	
	UINT size_list = ARRAYSIZE(vertex_list);
	UINT size_vertex = sizeof(vertex);

	//LOAD VERTEX BUFFER

	m_vertex_buffer->load(vertex_list, size_vertex, size_list, shader_byte_code, size_shader);

	//LOAD INDEX BUFFER

	size_list = ARRAYSIZE(index_list);
	m_index_buffer->load(index_list, size_list);
	
	GraphicsEngine::get()->releaseCompiledShader();
		
	// PREPARE PIXEL SHADER

	GraphicsEngine::get()->compilePixelShader(L"PixelShader.hlsl", "main", &shader_byte_code, &size_shader);

	m_pixel_shader = GraphicsEngine::get()->createPixelShader(shader_byte_code, size_shader);

	GraphicsEngine::get()->releaseCompiledShader();

	// PREPARE AND LOAD CONSTANT BUFFER
	
	constant cc;
	cc.m_time = 0;

	m_constant_buffer = GraphicsEngine::get()->createConstantBuffer();
	m_constant_buffer->load(&cc, sizeof(constant));

	// CREATE CAMERA

	Vector3D pos = Vector3D(0, 0, -4);
	Vector3D lookat = Vector3D(0, 0, 1);
	Vector3D up= Vector3D(0, 1, 0);
	m_camera = new Camera(pos,lookat,up, 1.0f, 0.1f, 100.0f, 90);
	
}

void AppWindow::onUpdate()
{
	Window::onUpdate();
	InputSystem::get()->update();
	if (isRun()) {

		//CLEAR RENDER TARGET, UPDATE VIEWPORT SIZE
		GraphicsEngine::get()->getImmediateDeviceContext()->clearRenderTargetColor(this->m_swap_chain, 0.5F, 0.1F, 0.8F, 1.0F);
		RECT rc = this->getClientWindowRect();
		GraphicsEngine::get()->getImmediateDeviceContext()->setViewportSize(rc.right - rc.left, rc.bottom - rc.top);
				
		m_camera->setAspect (((float)(rc.right - rc.left)) / ((float)(rc.bottom - rc.top)));

		// UPDATE POSITIONS
		this->updateQuadPosition();
				
		// UPDATE AND BIND CONSTANT BUFFER
		GraphicsEngine::get()->getImmediateDeviceContext()->setConstantBuffer(m_pixel_shader, m_constant_buffer);
		GraphicsEngine::get()->getImmediateDeviceContext()->setConstantBuffer(m_vertex_shader, m_constant_buffer);
		
		// SET SHADERS AND VERTEX BUFFER AND INDEX BUFFER
		GraphicsEngine::get()->getImmediateDeviceContext()->setVertexShader(m_vertex_shader);
		GraphicsEngine::get()->getImmediateDeviceContext()->setPixelShader(m_pixel_shader);
		GraphicsEngine::get()->getImmediateDeviceContext()->setVertexBuffer(m_vertex_buffer);
		GraphicsEngine::get()->getImmediateDeviceContext()->setIndexBuffer(m_index_buffer);
		
		// DRAW AND PRESENT
		GraphicsEngine::get()->getImmediateDeviceContext()->drawIndexedTriangles(m_index_buffer->getSizeIndexList(), 0, 0);
		//GraphicsEngine::get()->getImmediateDeviceContext()->drawTriangleList(m_vertex_buffer->getSizeVertexList(), 0);
		m_swap_chain->present(false);	

		//TIMING
		LARGE_INTEGER counter;
		::QueryPerformanceCounter(&counter);
		
		m_old_delta = m_new_delta;
		m_new_delta = (float)counter.LowPart;  
		m_delta_time = m_old_delta ? ((m_new_delta - m_old_delta) / 10000000.0f) : 0;

		}
	}
	

void AppWindow::onDestroy()
{
	Window::onDestroy();
	m_swap_chain->release();
	m_vertex_buffer->release();
	m_pixel_shader->release();
	m_vertex_shader->release();
	m_constant_buffer->release();
	m_index_buffer->release();
	delete m_camera;
	GraphicsEngine::get()->release();
	InputSystem::get()->removeListener(this);
}

void AppWindow::onKeyDown(int key)
{
	if (key == 'Z')
		m_trans_x -= 1.0f * m_delta_time;
	if (key == 'X')
		m_trans_x += 1.0f * m_delta_time;
}

void AppWindow::onKeyUp(int key)
{
	
}

void AppWindow::onMouseMove(Point point)
{
	m_camera->Pitch(0.1f * point.m_y * m_delta_time);
	m_camera->Yaw(0.1f * point.m_x * m_delta_time);
	//m_camera->debugPrintState();
}

void AppWindow::onLeftMouseDown(Point point)
{
	
}

void AppWindow::onRightMouseDown(Point point)
{
	m_camera->debugPrintState();
}

void AppWindow::onLeftMouseUp(Point point)
{
	
}

void AppWindow::onRightMouseUp(Point point)
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

	temp = m_camera->getCameraTransform();
	cc.m_view = temp;
	
	//PERSPECTIVE TRANSFORM
	temp = m_camera->getPerspectiveTransform();
	cc.m_proj = temp;

	m_constant_buffer->update(GraphicsEngine::get()->getImmediateDeviceContext(), &cc);
}
