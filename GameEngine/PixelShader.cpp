#include "PixelShader.h"
#include "RenderSystem.h"
#include <exception>

PixelShader::PixelShader(const void* shader_byte_code, size_t size_byte_code,RenderSystem* system)
{
	m_system = system;
	OutputDebugString("Entering PixelShader()\n");
	HRESULT hr = m_system->m_d3d_device->CreatePixelShader(shader_byte_code, size_byte_code, NULL, &m_pixel_shader);

	if (FAILED(hr))
	{
		throw std::exception("Failed to CreatePixelShader");
	}

	OutputDebugString("PixelShader::init ok\n");

}

PixelShader::~PixelShader()
{
	OutputDebugString("CLEANUP PixelShader destructor\n");
	m_pixel_shader->Release();

}


