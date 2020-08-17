#pragma once
#include <d3d11.h>
#include "Prerequisites.h"


class SwapChain
{
public:
	SwapChain(HWND window, UINT width, UINT height,RenderSystem* system);
	~SwapChain();
	bool present(bool vsync);
	ID3D11RenderTargetView* getRenderTargetView();
	ID3D11DepthStencilView* getDepthStencilView();
private:
	IDXGISwapChain* m_dxgi_swapchain;
	ID3D11RenderTargetView* m_render_target_view;
	RenderSystem* m_system = nullptr;
	ID3D11DepthStencilState* m_pDSState = nullptr;
	ID3D11DepthStencilView* m_pDSV = nullptr;
};

