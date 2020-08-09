#include "VertexBuffer.h"
#include "RenderSystem.h"


bool VertexBuffer::load(void* list_vertices, UINT size_vertex, UINT size_list, void* shader_byte_code, UINT size_byte_shader)
{
	OutputDebugString("Entering VertexBuffer::load\n");
	
	if (m_vertex_buffer)m_vertex_buffer->Release();
	if (m_input_layout)m_input_layout->Release();

	D3D11_BUFFER_DESC desc = {};
	D3D11_SUBRESOURCE_DATA data = {};

	desc.ByteWidth = size_vertex*size_list;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = size_vertex;

	data.pSysMem = list_vertices;

	m_size_list = size_list;
	m_size_vertex = size_vertex;

	HRESULT hr= m_system->m_d3d_device->CreateBuffer(&desc, &data, &m_vertex_buffer);

	if (FAILED(hr))	
	{
		OutputDebugString("Failed to CreateBuffer\n");
		return false;
	}

	OutputDebugString("Vertex Buffer created ok\n");

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA,0},
		{"COLOR",0,DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA,0}
	};

	/*		//SEMANTIC NAME - SEMANTIC INDEX - FORMAT - INPUT SLOT - ALIGNED BYTE OFFSET - INPUT SLOT CLASS - INSTANCE DATA STEP RATE
		{"COLOR",0,DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA,0},
		{"COLOR",1,DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA,0}*/


	UINT size_layout = ARRAYSIZE(layout);

	hr = m_system->m_d3d_device->CreateInputLayout(layout,size_layout,shader_byte_code,size_byte_shader, &m_input_layout);

	if (FAILED(hr))
	{
		OutputDebugString("Failed to CreateInputLayout\n");
		return false;
	}

	OutputDebugString("Input Layout created ok\n");

	return true;
}

bool VertexBuffer::release()
{
	OutputDebugString("CLEANUP VertexBuffer::release\n");
	if(m_vertex_buffer)m_vertex_buffer->Release();
	m_input_layout->Release();
	delete this;
	return true;
}

UINT VertexBuffer::getSizeVertexList()
{
	return m_size_list;
}

VertexBuffer::VertexBuffer(RenderSystem* system) :m_input_layout(0), m_vertex_buffer(0)
{
	m_system = system;
}
