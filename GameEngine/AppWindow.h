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
	void CreateLandscape(int width, int height);
	virtual void onUpdate() override;
	void draw2DGraphics(RECT& rc);
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
	void drawMesh(const MeshPtr& mesh, const VertexShaderPtr& vs, const PixelShaderPtr& ps, const ConstantBufferPtr& cb, TexturePtr *tex, int num_textures, int cullmode);

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
	float m_time=0;
	float m_light_angle;
	Vector3D m_light_position;
	float m_trans_x;
	float m_rot_x;
	float m_rot_y;
	float m_aspect_ratio;	
	TexturePtr m_brick;
	TexturePtr m_sky;
	TexturePtr m_tex_array[10];
	MeshPtr m_mesh;
	MeshPtr m_landscape;
	MeshPtr m_sky_mesh;	
	bool m_wireframe = false;
};

