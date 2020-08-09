#pragma once
#include <d3d11.h>
#include "Prerequisites.h"

#pragma once
class IndexBuffer
{
public:
	IndexBuffer(void* list_indexes, UINT size_list, RenderSystem* system);
	~IndexBuffer();
	UINT getSizeIndexList();
private:
	ID3D11Buffer* m_index_buffer;
	UINT m_size_list;
	friend class DeviceContext;
	RenderSystem* m_system = nullptr;
};

