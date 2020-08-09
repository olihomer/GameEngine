#include "IndexBuffer.h"
#include "RenderSystem.h"


IndexBuffer::IndexBuffer(RenderSystem* system)
{
	m_system = system;
}

bool IndexBuffer::load(void* list_indexes,UINT size_list)
{
	OutputDebugString("Entering IndexBuffer::load\n");

	if (m_index_buffer)m_index_buffer->Release();

	D3D11_BUFFER_DESC desc = {};
	D3D11_SUBRESOURCE_DATA data = {};

	desc.ByteWidth = size_list*sizeof(UINT);
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
		OutputDebugString("Failed to CreateBuffer\n");
		return false;
	}

	OutputDebugString("Index Buffer created ok\n");

	return true;
}

bool IndexBuffer::release()
{
	OutputDebugString("CLEANUP IndexBuffer::release\n");
	if (m_index_buffer)m_index_buffer->Release();
	delete this;
	return true;
}

UINT IndexBuffer::getSizeIndexList()
{
	return m_size_list;
}
