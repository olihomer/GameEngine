#include "VertexShader.h"
#include "RenderSystem.h"
#include <exception>

VertexShader::VertexShader(const void* shader_byte_code, size_t size_byte_code,RenderSystem* system)
{
	m_system = system;
	OutputDebugString("Entering VertexShader()\n");
	HRESULT hr = m_system->m_d3d_device->CreateVertexShader(shader_byte_code, size_byte_code, NULL, &m_vertex_shader);

	if (FAILED(hr))
	{
		throw std::exception("Failed to CreateVertexShader\n");
	}

	OutputDebugString("VertexShader() ok\n");
}

VertexShader::~VertexShader()
{
	OutputDebugString("CLEANUP VertexShader destructor\n");
	m_vertex_shader->Release();
}


