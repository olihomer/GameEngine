#include <d3d11.h>
#pragma once

class GraphicsEngine;
class DeviceContext;

class VertexShader
{
public:
	void release();
	friend class GraphicsEngine;
	friend class DeviceContext;
private:
	bool init(const void* shader_byte_code, size_t size__byte_code);
	ID3D11VertexShader* m_vertex_shader;

	
};

