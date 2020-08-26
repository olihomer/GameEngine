#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d2d1_1.h>
#include "DeviceContext.h"
#include "Prerequisites.h"

class RenderSystem
{
public:
	bool init();
	bool release();
	static RenderSystem* get();
	SwapChainPtr createSwapChain(HWND window, UINT width, UINT height);
	VertexBufferPtr createVertexBuffer(void* list_vertices, UINT size_vertex, UINT size_list, void* shader_byte_code, UINT size_byte_shader);
	ConstantBufferPtr createConstantBuffer(void* buffer, UINT size);
	IndexBufferPtr createIndexBuffer(void* list_indexes, UINT size_list);
	VertexShaderPtr createVertexShader(const void* shader_byte_code, size_t size_byte_code);
	PixelShaderPtr createPixelShader(const void* shader_byte_code, size_t size_byte_code);
	DeviceContextPtr getImmediateDeviceContext();
	ID2D1DeviceContext* get2DDeviceContext();
	void getVertexMeshLayoutShaderByteCodeAndSize(void** byte_code, size_t* size);
	RenderSystem();
	~RenderSystem();

	bool compileVertexShader(const wchar_t* file_name, const char* entry_point_name, void** shader_byte_code, size_t* byte_code_size);
	bool compilePixelShader(const wchar_t* file_name, const char* entry_point_name, void** shader_byte_code, size_t* byte_code_size);

	void releaseCompiledShader();
	void setRasterizerState(bool cull_front);

private:
	ID2D1Factory1* m_d2d_factory;
	ID2D1Device* m_d2d_device;
	ID2D1DeviceContext* m_d2d_context;
	ID3D11RasterizerState* m_cull_front_state;
	ID3D11RasterizerState* m_cull_back_state;
	D3D_FEATURE_LEVEL m_feature_level;
	ID3D11Device* m_d3d_device;
	ID3D11DeviceContext* m_imm_device_context;
	IDXGIDevice* m_dxgi_device;
	IDXGIAdapter* m_dxgi_adapter;
	IDXGIFactory* m_dxgi_factory;
	ID3D11SamplerState* m_sampler_state;
	DeviceContextPtr m_device_context_wrapper;
	ID3DBlob* m_blob;
	unsigned char m_mesh_layout_byte_code[1024];
	size_t m_mesh_layout_size;

	void initRasterizerState();

	friend class SwapChain;
	friend class VertexBuffer;
	friend class ConstantBuffer;
	friend class VertexShader;
	friend class PixelShader;
	friend class IndexBuffer;
	friend class Texture;
};


