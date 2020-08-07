#include <d3d11.h>

#pragma once
class SwapChain
{
public:
	bool init(HWND window, UINT width, UINT height);
	bool release();
	bool present(bool vsync);
	ID3D11RenderTargetView* getRenderTargetView();
private:
	IDXGISwapChain* m_dxgi_swapchain;
	ID3D11RenderTargetView* m_render_target_view;

};

