#include "SwapChain.h"
#include "RenderSystem.h"
#include <exception>

SwapChain::SwapChain(HWND hwnd, UINT width, UINT height,RenderSystem* system)
{
	m_system = system;
	OutputDebugString("Entering SwapChain()\n");

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
	sc_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sc_desc.OutputWindow = hwnd;
	sc_desc.Windowed = true;

	HRESULT hr = m_system->m_dxgi_factory->CreateSwapChain(device, &sc_desc, &m_dxgi_swapchain);

	if (FAILED(hr))
	{
		throw std::exception("Failed to CreateSwapChain\n");
	}

	OutputDebugString("Created SwapChain\n");

	ID3D11Texture2D* back_buffer = nullptr;
	hr = m_dxgi_swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&back_buffer);

	if (FAILED(hr))
	{
		throw std::exception("Failed to GetBuffer\n");
	}

	hr = device->CreateRenderTargetView(back_buffer, NULL, &m_render_target_view);
	back_buffer->Release();

	if (FAILED(hr))
	{
		throw std::exception("Failed to CreateRenderTargetView\n");
	}
}

SwapChain::~SwapChain()
{
	OutputDebugString("CLEANUP SwapChain destructor\n");
	m_dxgi_swapchain->Release();
	m_render_target_view->Release();
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
