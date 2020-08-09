#include "PixelShader.h"
#include "RenderSystem.h"

PixelShader::PixelShader(RenderSystem* system)
{
	m_system = system;
}

void PixelShader::release()
{
	OutputDebugString("CLEANUP PixelShader::release\n");
	m_pixel_shader->Release();
	delete this;
}

bool PixelShader::init(const void* shader_byte_code, size_t size_byte_code)
{
	OutputDebugString("Entering PixelShader::init\n");
	HRESULT hr = m_system->m_d3d_device->CreatePixelShader(shader_byte_code, size_byte_code, NULL, &m_pixel_shader);

	if (FAILED(hr))
	{
		OutputDebugString("Failed to CreatePixelShader\n");
		return false;
	}

	OutputDebugString("PixelShader::init ok\n");
	return true;
}

