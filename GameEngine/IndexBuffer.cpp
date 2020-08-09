#include "IndexBuffer.h"
#include "RenderSystem.h"
#include <exception>

IndexBuffer::IndexBuffer(void* list_indexes, UINT size_list,RenderSystem* system)
{
	OutputDebugString("Entering IndexBuffer()\n");

	m_system = system;

	D3D11_BUFFER_DESC desc = {};
	D3D11_SUBRESOURCE_DATA data = {};

	desc.ByteWidth = size_list * sizeof(UINT);
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = sizeof(UINT);

	data.pSysMem = list_indexes;

	m_size_list = size_list;

	HRESULT hr = m_system->m_d3d_device->CreateBuffer(&desc, &data, &m_index_buffer);

	if (FAILED(hr))
	{
		throw std::exception("Failed to CreateBuffer");
	}

	OutputDebugString("Index Buffer created ok\n");

}

IndexBuffer::~IndexBuffer()
{
	OutputDebugString("CLEANUP IndexBuffer::release\n");
	if (m_index_buffer)m_index_buffer->Release();
}

UINT IndexBuffer::getSizeIndexList()
{
	return m_size_list;
}
