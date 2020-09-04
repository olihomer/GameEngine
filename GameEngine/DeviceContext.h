#pragma once
#include <d3d11.h>
#include "SwapChain.h"
#include "Prerequisites.h"



class DeviceContext
{
public:
	DeviceContext(ID3D11DeviceContext* device_context, RenderSystem* system);
	~DeviceContext();
	void clearRenderTargetColor(const SwapChainPtr& swap_chain, float red, float green, float blue, float alpha);
	
	void setVertexBuffer(const VertexBufferPtr& vertex_buffer);
	void setIndexBuffer(const IndexBufferPtr& index_buffer);
	
	void setConstantBuffer(const VertexShaderPtr& vertex_shader, const ConstantBufferPtr &constant_buffer);
	void setConstantBuffer(const PixelShaderPtr& pixel_shader, const ConstantBufferPtr &constant_buffer);
	
	void drawTriangleList(UINT vertex_count, UINT start_vertex_index);
	void drawTriangleStrip(UINT vertex_count, UINT start_vertex_index);
	void drawIndexedTriangles(UINT index_count, UINT start_vertex_index, UINT start_index_location);
	
	void setViewportSize(UINT width, UINT height);
	
	void setVertexShader(const VertexShaderPtr& vertex_shader);
	void setPixelShader(const PixelShaderPtr& pixel_shader);
	void setSampler(ID3D11SamplerState* sampler);
	
	void setTexture(const VertexShaderPtr& vertex_shader, TexturePtr* texture,int num_textures);
	void setTexture(const PixelShaderPtr& pixel_shader, TexturePtr* texture,int num_textures);

private:
	ID3D11DeviceContext* m_device_context;
	RenderSystem* m_system = nullptr;
	friend class ConstantBuffer;
};

