#pragma once
#include <memory>

class VertexBuffer;
class VertexShader;
class PixelShader;
class SwapChain;
class ConstantBuffer;
class IndexBuffer;
class DeviceContext;
class GraphicsEngine;
class RenderSystem;

typedef std::shared_ptr<VertexBuffer> VertexBufferPtr;
typedef std::shared_ptr<VertexShader> VertexShaderPtr;
typedef std::shared_ptr<PixelShader> PixelShaderPtr;
typedef std::shared_ptr<SwapChain> SwapChainPtr;
typedef std::shared_ptr<ConstantBuffer> ConstantBufferPtr;
typedef std::shared_ptr<IndexBuffer> IndexBufferPtr;
typedef std::shared_ptr<DeviceContext> DeviceContextPtr;
