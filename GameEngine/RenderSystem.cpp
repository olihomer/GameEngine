#include "RenderSystem.h"
#include "GraphicsEngine.h"
#include "SwapChain.h"
#include "VertexBuffer.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "ConstantBuffer.h"
#include "IndexBuffer.h"



bool RenderSystem::init()
{
	OutputDebugString("Entering RenderSystem::init\n");

	D3D_DRIVER_TYPE driver_types[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE
	};

	UINT num_driver_types = ARRAYSIZE(driver_types);

	D3D_FEATURE_LEVEL feature_levels[] = {
		D3D_FEATURE_LEVEL_11_0
	};

	UINT num_feature_levels = ARRAYSIZE(feature_levels);

	HRESULT hr;
	ID3D11DeviceContext* imm_device_context;

	for (UINT driver_type_index = 0; driver_type_index < num_driver_types;) {
		hr = D3D11CreateDevice(NULL, driver_types[driver_type_index], NULL, D3D11_CREATE_DEVICE_DEBUG, feature_levels, num_feature_levels, D3D11_SDK_VERSION, &m_d3d_device, &m_feature_level, &imm_device_context);

		if (SUCCEEDED(hr))
			break;

		++driver_type_index;

	}

	if (FAILED(hr))
	{
		OutputDebugString("Failed to Create D3D Device\n");
		return false;
	}

	m_d3d_device->QueryInterface(__uuidof(IDXGIDevice), (void**)&m_dxgi_device);
	m_dxgi_device->GetParent(__uuidof(IDXGIAdapter), (void**)&m_dxgi_adapter);
	m_dxgi_adapter->GetParent(__uuidof(IDXGIFactory), (void**)&m_dxgi_factory);

	OutputDebugString("D3D Device Created Ok\n");

	m_imm_device_context = new DeviceContext(imm_device_context,this);

	return true;
}

bool RenderSystem::release()
{
	OutputDebugString("CLEANUP RenderSystem::release\n");
	m_d3d_device->Release();
	m_dxgi_device->Release();
	m_dxgi_factory->Release();
	m_dxgi_adapter->Release();
	m_imm_device_context->release();

	return true;
}

RenderSystem* RenderSystem::get()
{
	static RenderSystem system;
	return &system;
}

SwapChain* RenderSystem::createSwapChain()
{
	return new SwapChain(this);
}

VertexBuffer* RenderSystem::createVertexBuffer()
{
	return new VertexBuffer(this);
}

ConstantBuffer* RenderSystem::createConstantBuffer()
{
	return new ConstantBuffer(this);
}

IndexBuffer* RenderSystem::createIndexBuffer()
{
	return new IndexBuffer(this);
}

VertexShader* RenderSystem::createVertexShader(const void* shader_byte_code, size_t size_byte_code)
{

	OutputDebugString("RenderSystem::createVertexShader\n");

	VertexShader* vs = new VertexShader(this);
	if (!vs->init(shader_byte_code, size_byte_code))
	{
		OutputDebugString("Failed to Init Vertex Shader\n");
		return nullptr;
	}
	return vs;
}

PixelShader* RenderSystem::createPixelShader(const void* shader_byte_code, size_t size_byte_code)
{
	OutputDebugString("RenderSystem::createPixelShader\n");

	PixelShader* ps = new PixelShader(this);
	if (!ps->init(shader_byte_code, size_byte_code))
	{
		OutputDebugString("Failed to Init Pixel Shader\n");
		return nullptr;
	}
	return ps;
}

DeviceContext* RenderSystem::getImmediateDeviceContext()
{
	return this->m_imm_device_context;
}

RenderSystem::RenderSystem()
{
	this->init();
}

RenderSystem::~RenderSystem()
{
	this->release();
}


bool RenderSystem::compileVertexShader(const wchar_t* file_name, const char* entry_point_name, void** shader_byte_code, size_t* byte_code_size)
{
	OutputDebugString("Entering RenderSystem::compileVertexShader\n");

	HRESULT hr = D3DCompileFromFile(file_name, nullptr, nullptr, entry_point_name, "vs_5_0", 0, 0, &m_blob, nullptr);
	if (FAILED(hr))
	{
		OutputDebugString("Failed to D3DCompileFromFile in compileVertexShader\n");
		return false;
	}

	*shader_byte_code = m_blob->GetBufferPointer();
	*byte_code_size = m_blob->GetBufferSize();

	OutputDebugString("compileVertexShader ok\n");
	return true;
}

bool RenderSystem::compilePixelShader(const wchar_t* file_name, const char* entry_point_name, void** shader_byte_code, size_t* byte_code_size)
{
	OutputDebugString("Entering RenderSystem::compilePixelShader\n");

	HRESULT hr = D3DCompileFromFile(file_name, nullptr, nullptr, entry_point_name, "ps_5_0", 0, 0, &m_blob, nullptr);
	if (FAILED(hr))
	{
		OutputDebugString("Failed to D3DCompileFromFile in compilePixelShader\n");
		return false;
	}

	*shader_byte_code = m_blob->GetBufferPointer();
	*byte_code_size = m_blob->GetBufferSize();

	OutputDebugString("compilePixelShader ok\n");
	return true;
}

void RenderSystem::releaseCompiledShader()
{
	if (m_blob)m_blob->Release();
}

