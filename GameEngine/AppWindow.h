#pragma once
#include "Window.h"
#include "SwapChain.h"
#include "VertexBuffer.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "ConstantBuffer.h"
#include "IndexBuffer.h"
#include "InputListener.h"
#include "Camera.h"



class AppWindow: public Window, public InputListener
{
public:

	// Inherited via Window
	
	virtual void onCreate() override;
	virtual void onUpdate() override;
	void Draw2DGraphics(RECT& rc);
	virtual void onDestroy() override;

	//Listener call backs for Input

	virtual void onKeyDown(int key) override;
	virtual void onKeyUp(int key) override;
	virtual void onMouseMove(const Point& point) override;
	virtual void onLeftMouseDown(const Point& point) override;
	virtual void onRightMouseDown(const Point& point) override;
	virtual void onLeftMouseUp(const Point& point) override;
	virtual void onRightMouseUp(const Point& point) override;
	virtual void onFocus() override;
	virtual void onKillFocus() override;

	void updateWorld();
	void drawMesh(const MeshPtr& mesh, const VertexShaderPtr& vs, const PixelShaderPtr& ps, const ConstantBufferPtr& cb, const TexturePtr& tex, bool cullfront);

private:
	SwapChainPtr m_swap_chain;
	VertexBufferPtr m_vertex_buffer;
	VertexShaderPtr m_vertex_shader;
	PixelShaderPtr m_pixel_shader;
	PixelShaderPtr m_skybox_pixel_shader;
	ConstantBufferPtr m_constant_buffer;
	ConstantBufferPtr m_skybox_constant_buffer;
	IndexBufferPtr m_index_buffer;
	Camera *m_camera;
	INT64 m_old_delta;
	INT64 m_new_delta;
	float m_delta_time;
	float m_light_angle;
	Vector3D m_light_position;
	float m_trans_x;
	float m_rot_x;
	float m_rot_y;
	float m_aspect_ratio;	
	TexturePtr m_wood;
	TexturePtr m_sky;
	MeshPtr m_mesh;
	MeshPtr m_sky_mesh;
};

