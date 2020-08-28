#include "ConstantBuffer.h"
#include <d3d11.h>
#include "RenderSystem.h"
#include "DeviceContext.h"
#include <exception>

ConstantBuffer::ConstantBuffer(void* buffer, UINT size, RenderSystem* system)
{
	m_system = system;

	OutputDebugString("Entering ConstantBuffer()\n");

	D3D11_BUFFER_DESC desc = {};
	D3D11_SUBRESOURCE_DATA data = {};

	desc.ByteWidth = size;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	data.pSysMem = buffer;

	HRESULT hr = m_system->m_d3d_device->CreateBuffer(&desc, &data, &m_constant_buffer);

	if (FAILED(hr))
	{
		throw std::exception("Failed to CreateBuffer in ConstantBuffer)");
	}

	OutputDebugString("Constant Buffer created ok\n");

}

ConstantBuffer::~ConstantBuffer()
{
	OutputDebugString("CLEANUP ConstantBuffer destructor\n");
	if (m_constant_buffer)m_constant_buffer->Release();

}

void ConstantBuffer::update(void* buffer)
{
	m_system->m_imm_device_context->UpdateSubresource(this->m_constant_buffer,NULL,NULL,buffer,NULL,NULL);
}
