#include <d3d11.h>
#include "Prerequisites.h"

#pragma once
class VertexBuffer
{
public:
	bool load(void* list_vertices, UINT size_vertex, UINT size_list,void* shader_byte_code, UINT size_byte_shader);
	bool release();
	UINT getSizeVertexList();
	VertexBuffer(RenderSystem* system);
private:
	ID3D11Buffer* m_vertex_buffer;
	ID3D11InputLayout* m_input_layout;
	RenderSystem* m_system = nullptr;
	UINT m_size_vertex;
	UINT m_size_list;
	friend class DeviceContext;
};

