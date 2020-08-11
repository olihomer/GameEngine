#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
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
	RenderSystem();
	~RenderSystem();

	bool compileVertexShader(const wchar_t* file_name, const char* entry_point_name, void** shader_byte_code, size_t* byte_code_size);
	bool compilePixelShader(const wchar_t* file_name, const char* entry_point_name, void** shader_byte_code, size_t* byte_code_size);

	void releaseCompiledShader();

private:
	ID3D11Device* m_d3d_device;
	D3D_FEATURE_LEVEL m_feature_level;
	IDXGIDevice* m_dxgi_device;
	IDXGIAdapter* m_dxgi_adapter;
	IDXGIFactory* m_dxgi_factory;
	ID3D11SamplerState* m_sampler_state;
	DeviceContextPtr m_imm_device_context;
	ID3DBlob* m_blob;

	friend class SwapChain;
	friend class VertexBuffer;
	friend class ConstantBuffer;
	friend class VertexShader;
	friend class PixelShader;
	friend class IndexBuffer;
	friend class Texture;
};


