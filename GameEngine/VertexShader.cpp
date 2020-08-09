#include "VertexShader.h"
#include "RenderSystem.h"

bool VertexShader::init(const void* shader_byte_code, size_t size_byte_code)
{
	OutputDebugString("Entering VertexShader::init\n");
	HRESULT hr= m_system->m_d3d_device->CreateVertexShader(shader_byte_code, size_byte_code, NULL, &m_vertex_shader);
	
	if (FAILED(hr))
	{
		OutputDebugString("Failed to CreateVertexShader\n");
		return false;
	}

	OutputDebugString("VertexShader::init ok\n");
	return true;
}

VertexShader::VertexShader(RenderSystem* system)
{
	m_system = system;
}

void VertexShader::release()
{
	OutputDebugString("CLEANUP VertexShader::release\n");
	m_vertex_shader->Release();
	delete this;
}


