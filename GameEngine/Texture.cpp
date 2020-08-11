#include "Texture.h"
#include <DirectXTex.h>
#include "GraphicsEngine.h"
#include "RenderSystem.h"
#include <stdexcept>

Texture::Texture(const wchar_t* full_path):Resource(full_path)
{
	OutputDebugString("Entering Texture()\n");
	DirectX::ScratchImage image_data;
	
	HRESULT hr = DirectX::LoadFromWICFile(full_path, DirectX::WIC_FLAGS_NONE, nullptr, image_data);

	if (SUCCEEDED(hr))
	{
		hr = DirectX::CreateTexture(GraphicsEngine::get()->getRenderSystem()->m_d3d_device, image_data.GetImages(), image_data.GetImageCount(), image_data.GetMetadata(), &m_texture);

		if (FAILED(hr))
			throw std::exception("Creature Texture failed");

		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		desc.Format = image_data.GetMetadata().format;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipLevels = (UINT)image_data.GetMetadata().mipLevels;
		desc.Texture2D.MostDetailedMip = 0;

		hr= GraphicsEngine::get()->getRenderSystem()->m_d3d_device->CreateShaderResourceView(m_texture,&desc, &m_shader_res_view);

		if (FAILED(hr))
			throw std::exception("Creature Shader Resource failed");


	}
	else
	{
		throw std::exception("LoadFromWICFile failed");
	}
	
	
	OutputDebugString("Texture created ok()\n");
}

Texture::~Texture()
{
	OutputDebugString("CLEANUP Texture destructor\n");
	m_texture->Release();
	m_shader_res_view->Release();
}
