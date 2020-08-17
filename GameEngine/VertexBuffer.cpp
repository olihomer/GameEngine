#include "VertexBuffer.h"
#include "RenderSystem.h"
#include <exception>


VertexBuffer::~VertexBuffer()
{
	OutputDebugString("CLEANUP VertexBuffer destructor\n");
	if(m_vertex_buffer)m_vertex_buffer->Release();
	m_input_layout->Release();
}

UINT VertexBuffer::getSizeVertexList()
{
	return m_size_list;
}

VertexBuffer::VertexBuffer(void* list_vertices, UINT size_vertex, UINT size_list, void* shader_byte_code, UINT size_byte_shader, RenderSystem* system) :m_input_layout(0), m_vertex_buffer(0)
{
	m_system = system;
	OutputDebugString("Entering VertexBuffer()\n");

	D3D11_BUFFER_DESC desc = {};
	D3D11_SUBRESOURCE_DATA data = {};

	desc.ByteWidth = size_vertex * size_list;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = size_vertex;

	data.pSysMem = list_vertices;

	m_size_list = size_list;
	m_size_vertex = size_vertex;

	HRESULT hr = m_system->m_d3d_device->CreateBuffer(&desc, &data, &m_vertex_buffer);

	if (FAILED(hr))
	{
		throw std::exception("Failed to CreateBuffer");
	}

	OutputDebugString("Vertex Buffer created ok\n");

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA,0},
		{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA,0},
		{"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA,0}
	};

	UINT size_layout = ARRAYSIZE(layout);

	hr = m_system->m_d3d_device->CreateInputLayout(layout, size_layout, shader_byte_code, size_byte_shader, &m_input_layout);

	if (FAILED(hr))
	{
		throw std::exception("Failed to CreateInputLayout");
	}

	OutputDebugString("Input Layout created ok\n");


}
