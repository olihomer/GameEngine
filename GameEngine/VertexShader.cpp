#include "VertexShader.h"
#include "GraphicsEngine.h"

bool VertexShader::init(const void* shader_byte_code, size_t size_byte_code)
{
	OutputDebugString("Entering VertexShader::init\n");
	HRESULT hr=GraphicsEngine::get()->m_d3d_device->CreateVertexShader(shader_byte_code, size_byte_code, NULL, &m_vertex_shader);
	
	if (FAILED(hr))
	{
		OutputDebugString("Failed to CreateVertexShader\n");
		return false;
	}

	OutputDebugString("VertexShader::init ok\n");
	return true;
}

void VertexShader::release()
{
	OutputDebugString("CLEANUP VertexShader::release\n");
	m_vertex_shader->Release();
	delete this;
}


