#include "RenderSystem.h"
#include "GraphicsEngine.h"
#include "SwapChain.h"
#include "VertexBuffer.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "ConstantBuffer.h"
#include "IndexBuffer.h"
#include <exception>


RenderSystem* RenderSystem::get()
{
	static RenderSystem system;
	return &system;
}

SwapChainPtr RenderSystem::createSwapChain(HWND window, UINT width, UINT height)
{
	SwapChainPtr sc = nullptr;
	try {
		sc= std::make_shared<SwapChain>(window, width, height, this);
	}
	catch (...)
	{

	}
	return sc;
}

VertexBufferPtr RenderSystem::createVertexBuffer(void* list_vertices, UINT size_vertex, UINT size_list, void* shader_byte_code, UINT size_byte_shader)
{
	VertexBufferPtr vb = nullptr;
	try
	{
		vb= std::make_shared<VertexBuffer>(list_vertices, size_vertex, size_list, shader_byte_code, size_byte_shader, this);
	}
	catch (...)
	{

	}
	return vb;
}

ConstantBufferPtr RenderSystem::createConstantBuffer(void* buffer, UINT size)
{
	ConstantBufferPtr cb = nullptr;
	try
	{
		cb = std::make_shared <ConstantBuffer>(buffer, size, this);
	}
	catch (...)
	{

	}
	return cb;
}

IndexBufferPtr RenderSystem::createIndexBuffer(void* list_indexes, UINT size_list)
{
	IndexBufferPtr ib = nullptr;
	try
	{
		ib = std::make_shared <IndexBuffer>(list_indexes, size_list, this);
	}
	catch (...)
	{

	}
	return ib;
}

VertexShaderPtr RenderSystem::createVertexShader(const void* shader_byte_code, size_t size_byte_code)
{

	OutputDebugString("RenderSystem::createVertexShader\n");

	VertexShaderPtr vs = nullptr;

	try
	{
		vs = std::make_shared<VertexShader>(shader_byte_code, size_byte_code, this);
	}
	catch(...)
	{
		OutputDebugString("Failed to Init Vertex Shader\n");
	}
	return vs;
}

PixelShaderPtr RenderSystem::createPixelShader(const void* shader_byte_code, size_t size_byte_code)
{
	OutputDebugString("RenderSystem::createPixelShader\n");
	PixelShaderPtr ps = nullptr;
	try
	{
		ps = std::make_shared<PixelShader>(shader_byte_code, size_byte_code, this);
	}
	catch(...)
	{
		OutputDebugString("Failed to Init Pixel Shader\n");
	}
	return ps;
}

DeviceContextPtr RenderSystem::getImmediateDeviceContext()
{
	return this->m_imm_device_context;
}

ID2D1DeviceContext* RenderSystem::get2DDeviceContext()
{
	return m_d2d_context;
}

void RenderSystem::getVertexMeshLayoutShaderByteCodeAndSize(void** byte_code, size_t* size)
{
	*byte_code = m_mesh_layout_byte_code;
	*size = m_mesh_layout_size;
}

RenderSystem::RenderSystem()
{
	OutputDebugString("Entering RenderSystem()\n");

	//CREATE D3D11DEVICE

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
		hr = D3D11CreateDevice(NULL, driver_types[driver_type_index], NULL, D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_BGRA_SUPPORT, feature_levels, num_feature_levels, D3D11_SDK_VERSION, &m_d3d_device, &m_feature_level, &imm_device_context);

		if (SUCCEEDED(hr))
			break;

		++driver_type_index;
	}

	if (FAILED(hr))
	{
		throw std::exception("Failed to Create D3D Device");
	}

	// GET FACTORY AND DEVICE CONTEXT

	m_d3d_device->QueryInterface(__uuidof(IDXGIDevice), (void**)&m_dxgi_device);
	m_dxgi_device->GetParent(__uuidof(IDXGIAdapter), (void**)&m_dxgi_adapter);
	m_dxgi_adapter->GetParent(__uuidof(IDXGIFactory), (void**)&m_dxgi_factory);

	OutputDebugString("D3D Device Created ok\n");

	m_imm_device_context = std::make_shared<DeviceContext>(imm_device_context, this);

	// CREATE DIRECT2D DEVICES

	D2D1_FACTORY_OPTIONS options;
	options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
	hr=D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory), &options ,(void**)&m_d2d_factory);

	if (FAILED(hr))
	{
		throw std::exception("Failed to Create D2D Factory");
	}

	OutputDebugString("D2D Factory Created ok\n");

	hr=m_d2d_factory->CreateDevice(m_dxgi_device, &m_d2d_device);

	if (FAILED(hr))
	{
		throw std::exception("Failed to Create D2D Device");
	}

	OutputDebugString("D2D Device Created ok\n");

	hr = m_d2d_device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &m_d2d_context);

	if (FAILED(hr))
	{
		throw std::exception("Failed to Create D2D DeviceContext");
	}

	OutputDebugString("D2D DeviceContext Created ok\n");


	//SET UP SHADER SAMPLER

	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD = -FLT_MAX;
	samplerDesc.MaxLOD = FLT_MAX;

	hr = m_d3d_device->CreateSamplerState(&samplerDesc, &m_sampler_state);
	if (FAILED(hr))
	{
		throw std::exception("Failed to create sampler");
	}

	m_imm_device_context->setSampler(m_sampler_state);

	//SET UP VERTEX MESH LAYOUT SHADER

	void* byte_code = nullptr;
	compileVertexShader(L"VertexMeshLayoutShader.hlsl", "main", &byte_code, &m_mesh_layout_size);
	memcpy(m_mesh_layout_byte_code, byte_code, m_mesh_layout_size);
	releaseCompiledShader();

	//INITIALISE RASTERIZER

	initRasterizerState();
}

RenderSystem::~RenderSystem()
{
	OutputDebugString("CLEANUP RenderSystem destructor\n");
	m_d3d_device->Release();
	m_dxgi_device->Release();
	m_dxgi_factory->Release();
	m_dxgi_adapter->Release();
	m_sampler_state->Release();
	m_cull_front_state->Release();
	m_cull_back_state->Release();
	m_d2d_factory->Release();
	m_d2d_device->Release();
	m_d2d_context->Release();
}


bool RenderSystem::compileVertexShader(const wchar_t* file_name, const char* entry_point_name, void** shader_byte_code, size_t* byte_code_size)
{
	OutputDebugString("Entering RenderSystem::compileVertexShader\n");

	HRESULT hr = D3DCompileFromFile(file_name, nullptr, nullptr, entry_point_name, "vs_5_0", 0, 0, &m_blob, nullptr);
	if (FAILED(hr))
	{
		throw std::exception("Failed to D3DCompileFromFile in compileVertexShader\n");
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
		throw std::exception("Failed to D3DCompileFromFile in compilePixelShader\n");
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

void RenderSystem::setRasterizerState(bool cull_front)
{
	if (cull_front)
	{
		m_imm_device_context->m_device_context->RSSetState(m_cull_front_state);
	}
	else
	{
		m_imm_device_context->m_device_context->RSSetState(m_cull_back_state);
	}
}

void RenderSystem::initRasterizerState()
{
	D3D11_RASTERIZER_DESC desc = {};
	desc.CullMode = D3D11_CULL_FRONT;
	desc.DepthClipEnable = true;
	desc.FillMode = D3D11_FILL_SOLID;

	HRESULT hr = m_d3d_device->CreateRasterizerState(&desc, &m_cull_front_state);
	if (FAILED(hr))
	{
		throw std::exception("Failed to CreateRasterizerState\n");
	}

	desc = {};
	desc.CullMode = D3D11_CULL_BACK;
	desc.DepthClipEnable = true;
	desc.FillMode = D3D11_FILL_SOLID;

	hr = m_d3d_device->CreateRasterizerState(&desc, &m_cull_back_state);
	if (FAILED(hr))
	{
		throw std::exception("Failed to CreateRasterizerState\n");
	}

	OutputDebugString("initRasterizerState ok\n");
}

