#include "SwapChain.h"
#include "RenderSystem.h"
#include <exception>

SwapChain::SwapChain(HWND hwnd, UINT width, UINT height,RenderSystem* system)
{
	m_system = system;
	OutputDebugString("Entering SwapChain()\n");

	//CREATE SWAP CHAIN

	ID3D11Device* device = m_system->m_d3d_device;
	DXGI_SWAP_CHAIN_DESC sc_desc;
	ZeroMemory(&sc_desc, sizeof(sc_desc));

	sc_desc.BufferCount = 1;
	sc_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sc_desc.SampleDesc.Count = 1;
	sc_desc.SampleDesc.Quality = 0;
	sc_desc.BufferDesc.Width = width;
	sc_desc.BufferDesc.Height = height;
	sc_desc.BufferDesc.RefreshRate.Numerator = 60;
	sc_desc.BufferDesc.RefreshRate.Denominator = 1;
	sc_desc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sc_desc.OutputWindow = hwnd;
	sc_desc.Windowed = true;

	HRESULT hr = m_system->m_dxgi_factory->CreateSwapChain(device, &sc_desc, &m_dxgi_swapchain);

	if (FAILED(hr))
	{
		throw std::exception("Failed to CreateSwapChain\n");
	}

	OutputDebugString("Created SwapChain\n");

	//GET BACK BUFFER

	ID3D11Texture2D* back_buffer = nullptr;
	hr = m_dxgi_swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&back_buffer);

	if (FAILED(hr))
	{
		throw std::exception("Failed to GetBuffer\n");
	}

	//CREATE 3D RENDER TARGET VIEW

	hr = device->CreateRenderTargetView(back_buffer, NULL, &m_render_target_view);
	back_buffer->Release();

	if (FAILED(hr))
	{
		throw std::exception("Failed to CreateRenderTargetView\n");
	}
	
	//CREATE 2D RENDER TARGET
		
	D2D1_BITMAP_PROPERTIES1 bitmapProperties = {};
	bitmapProperties.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET| D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
	bitmapProperties.pixelFormat = D2D1_PIXEL_FORMAT{ DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE };
	
	IDXGISurface* dxgiBackBuffer;
	hr=m_dxgi_swapchain->GetBuffer(0, __uuidof(IDXGISurface), (void**)&dxgiBackBuffer);

	if (FAILED(hr))
	{
		throw std::exception("Failed to DXGISurface GetBuffer\n");
	}
		
	hr=system->m_d2d_context->CreateBitmapFromDxgiSurface(dxgiBackBuffer, &bitmapProperties, &m_bitmap);

	if (FAILED(hr))
	{
		throw std::exception("Failed to CreateBitmapfromDXGISurface\n");
	}

	system->m_d2d_context->SetTarget(m_bitmap);

	dxgiBackBuffer->Release();
	
	OutputDebugString("Created 2D Render Target ok\n");

	//SET UP DEPTH-STENCIL BUFFER

	D3D11_TEXTURE2D_DESC descDepth;
	ID3D11Texture2D* buffer;

	descDepth.Width = sc_desc.BufferDesc.Width;
	descDepth.Height = sc_desc.BufferDesc.Height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	hr = device->CreateTexture2D(&descDepth, NULL, &buffer);

	if (FAILED(hr))
	{
		throw std::exception("Failed to CreateTexture2D Depth Buffer\n");
	}

	//SET UP DEPTH-STENCIL STATE

	D3D11_DEPTH_STENCIL_DESC dsDesc;

	// Depth test parameters
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

	// Stencil test parameters
	dsDesc.StencilEnable = true;
	dsDesc.StencilReadMask = 0xFF;
	dsDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create depth stencil state
	hr=device->CreateDepthStencilState(&dsDesc, &m_pDSState);

	if (FAILED(hr))
	{
		throw std::exception("Failed to CreateDepthStencilState\n");
	}

	// Bind depth stencil state
	system->m_imm_device_context->OMSetDepthStencilState(m_pDSState, 1);

	//Bind depth stencil to view

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	descDSV.Format = DXGI_FORMAT_UNKNOWN;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	descDSV.Flags = 0;

	// Create the depth stencil view
	
	hr = device->CreateDepthStencilView(buffer, // Depth stencil texture
		&descDSV, // Depth stencil desc
		&m_pDSV);  // [out] Depth stencil view

	if (FAILED(hr))
	{
		throw std::exception("Failed to CreateDepthStencilView\n");
	}
	buffer->Release();
}

SwapChain::~SwapChain()
{
	OutputDebugString("CLEANUP SwapChain destructor\n");
	m_pDSV->Release();
	m_pDSState->Release();
	m_dxgi_swapchain->Release();
	m_render_target_view->Release();
	m_bitmap->Release(); 
}

bool SwapChain::present(bool vsync)
{
	m_dxgi_swapchain->Present(vsync, NULL);
	return true;
}

ID3D11RenderTargetView* SwapChain::getRenderTargetView()
{
	return m_render_target_view;
}

ID3D11DepthStencilView* SwapChain::getDepthStencilView()
{
	return m_pDSV;
}
