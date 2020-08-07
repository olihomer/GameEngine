#include "ConstantBuffer.h"
#include <d3d11.h>
#include "GraphicsEngine.h"
#include "DeviceContext.h"

bool ConstantBuffer::load(void* buffer, UINT size)
{
	OutputDebugString("Entering ConstantBuffer::load\n");

	if (m_constant_buffer)m_constant_buffer->Release();

	D3D11_BUFFER_DESC desc = {};
	D3D11_SUBRESOURCE_DATA data = {};

	desc.ByteWidth = size;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	
	data.pSysMem = buffer;

	HRESULT hr = GraphicsEngine::get()->m_d3d_device->CreateBuffer(&desc, &data, &m_constant_buffer);
	
	if (FAILED(hr))
	{
		OutputDebugString("Failed to CreateBuffer in ConstantBuffer)\n");
		return false;
	}

	OutputDebugString("Constant Buffer created ok\n");

    return true;
}

void ConstantBuffer::update(DeviceContext* dc, void* buffer)
{
	dc->m_device_context->UpdateSubresource(this->m_constant_buffer,NULL,NULL,buffer,NULL,NULL);
}

bool ConstantBuffer::release()
{
	OutputDebugString("CLEANUP ConstantBuffer::release\n");
	if(m_constant_buffer)m_constant_buffer->Release();
	delete this;
	return true;
}
