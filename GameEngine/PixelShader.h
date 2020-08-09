#pragma once

#include <d3d11.h>
#include "Prerequisites.h"

class PixelShader
{
public:
	PixelShader(const void* shader_byte_code, size_t size_byte_code, RenderSystem* system);
	~PixelShader();
	friend class RenderSystem;
	friend class DeviceContext;
private:
	ID3D11PixelShader* m_pixel_shader;
	RenderSystem* m_system=nullptr;


};

