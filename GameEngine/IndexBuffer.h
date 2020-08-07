#pragma once
#include <d3d11.h>

class DeviceContext;

#pragma once
class IndexBuffer
{
public:
	bool load(void* list_indexes, UINT size_list);
	bool release();
	UINT getSizeIndexList();
private:
	ID3D11Buffer* m_index_buffer;
	UINT m_size_list;
	friend class DeviceContext;
};

