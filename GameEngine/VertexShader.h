#pragma once

#include <d3d11.h>
#include "Prerequisites.h"

class VertexShader
{
public:
	VertexShader(const void* shader_byte_code, size_t size_byte_code, RenderSystem* system);
	~VertexShader();
	friend class RenderSystem;
	friend class DeviceContext;
private:
	bool init(const void* shader_byte_code, size_t size__byte_code);
	ID3D11VertexShader* m_vertex_shader;
	RenderSystem* m_system=nullptr;
};

