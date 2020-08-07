#pragma once
#include <d3d11.h>

class DeviceContext;

class ConstantBuffer
{
public:
	bool load(void* buffer, UINT size);
	void update(DeviceContext* dc, void* buffer);
	bool release();
private:
	ID3D11Buffer* m_constant_buffer;
	friend class DeviceContext;
};

