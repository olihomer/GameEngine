#pragma once

#include "Vector3D.h"
#include "Vector4D.h"
#include "Matrix4x4.h"
#include "Quaternion.h"

#define wUP Vector3D(0,1,0)
#define wX Vector3D(1,0,0)
#define wY Vector3D(0,1,0)
#define wZ Vector3D(0,0,1)

class Camera
{
public:
	void setPosition(Vector3D pos);
	void setAspect(float aspect);
	void lookAt(Vector3D target, Vector3D up);
	void Roll(float theta);
	void Yaw(float theta);
	void Pitch(float theta);
	void Translate(float delta, Vector3D direction);
	void debugPrintState();

	Matrix4x4 getPerspectiveTransform();
	Matrix4x4 getCameraTransform();
	Vector3D getPosition();
	Vector3D getUp();
	Vector3D getRight();
	Vector3D getForward();
	
	Camera(Vector3D position = Vector3D(0, 0, 0), Vector3D lookat = Vector3D(0, 0, 1), Vector3D up = Vector3D(0, 1, 0), float aspect_ratio = 1.0f, float near_plane = 0.1f, float far_plane = 100.0f, int fov = 90);

private:
	Matrix4x4 m_camera_local_matrix;
	Quaternion m_camera_quaternion;
	Vector3D m_position;
	float m_near_plane;
	float m_far_plane;
	int m_fov;
	float m_aspect;
};
