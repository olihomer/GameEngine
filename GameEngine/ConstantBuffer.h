#pragma once
#include <d3d11.h>
#include "Prerequisites.h"

class ConstantBuffer
{
public:
	ConstantBuffer(void* buffer, UINT size,RenderSystem* system);
	~ConstantBuffer();
	void update(DeviceContextPtr dc, void* buffer);
private:
	ID3D11Buffer* m_constant_buffer;
	friend class DeviceContext;
	RenderSystem* m_system = nullptr;
};

