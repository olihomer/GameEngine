#pragma once
#include <d3d11.h>
#include "Prerequisites.h"

class ConstantBuffer
{
public:
	ConstantBuffer(RenderSystem* system);
	bool load(void* buffer, UINT size);
	void update(DeviceContext* dc, void* buffer);
	bool release();
private:
	ID3D11Buffer* m_constant_buffer;
	friend class DeviceContext;
	RenderSystem* m_system = nullptr;
};

