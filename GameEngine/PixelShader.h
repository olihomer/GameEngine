#pragma once

#include <d3d11.h>
#include "Prerequisites.h"

class PixelShader
{
public:
	PixelShader(RenderSystem* system);
	void release();
	friend class GraphicsEngine;
	friend class RenderSystem;
	friend class DeviceContext;
private:
	bool init(const void* shader_byte_code, size_t size__byte_code);
	ID3D11PixelShader* m_pixel_shader;
	RenderSystem* m_system=nullptr;


};

