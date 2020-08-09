#pragma once
#include <d3d11.h>
#include "Prerequisites.h"


class SwapChain
{
public:
	SwapChain(RenderSystem* system);
	bool init(HWND window, UINT width, UINT height);
	bool release();
	bool present(bool vsync);
	ID3D11RenderTargetView* getRenderTargetView();
private:
	IDXGISwapChain* m_dxgi_swapchain;
	ID3D11RenderTargetView* m_render_target_view;
	RenderSystem* m_system = nullptr;

};

