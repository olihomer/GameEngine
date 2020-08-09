#pragma once
#include <d3d11.h>
#include "Prerequisites.h"

#pragma once
class IndexBuffer
{
public:
	IndexBuffer(RenderSystem* system);
	bool load(void* list_indexes, UINT size_list);
	bool release();
	UINT getSizeIndexList();
private:
	ID3D11Buffer* m_index_buffer;
	UINT m_size_list;
	friend class DeviceContext;
	RenderSystem* m_system = nullptr;
};

