#include "Camera.h"
#include <Windows.h>
#include <iostream>

#define wUP Vector3D(0,1,0)
#define wX Vector3D(1,0,0)
#define wY Vector3D(0,1,0)
#define wZ Vector3D(0,0,1)

void Camera::setPosition(Vector3D pos)
{
	m_position = pos;
}

void Camera::setAspect(float aspect)
{
	m_aspect = aspect;
}

void Camera::lookAt(Vector3D target, Vector3D up)
{
	m_camera_local_matrix.setIdentity();

	//SET AXES
		
	Vector3D camera_z = (target - m_position);
	camera_z.normalize();
	Vector3D camera_y = up;
	camera_y.normalize();
	Vector3D camera_x;
	camera_x=Vector3D::cross(camera_y, camera_z);
	camera_x.normalize();
	camera_y=Vector3D::cross(camera_z, camera_x);
	camera_y.normalize();

	//COPY TO STATE MATRIX

	m_camera_local_matrix.m_mat[0][0] = camera_x.m_x;
	m_camera_local_matrix.m_mat[0][1] = camera_x.m_y;
	m_camera_local_matrix.m_mat[0][2] = camera_x.m_z;

	m_camera_local_matrix.m_mat[1][0] = camera_y.m_x;
	m_camera_local_matrix.m_mat[1][1] = camera_y.m_y;
	m_camera_local_matrix.m_mat[1][2] = camera_y.m_z;

	m_camera_local_matrix.m_mat[2][0] = camera_z.m_x;
	m_camera_local_matrix.m_mat[2][1] = camera_z.m_y;
	m_camera_local_matrix.m_mat[2][2] = camera_z.m_z;

	//COPY TO QUATERNION

	m_camera_quaternion = Quaternion(m_camera_local_matrix);

}


void Camera::Translate(float delta, Vector3D direction)
{
	m_position.m_x += delta * direction.m_x;
	m_position.m_y += delta * direction.m_y;
	m_position.m_z += delta * direction.m_z;

}

void Camera::Pitch(float theta)
{
	Quaternion q;
	q.setRotationAroundAxis(theta, getRight());
	m_camera_quaternion = q * m_camera_quaternion;
	m_camera_local_matrix = m_camera_quaternion.toMatrix();
}

void Camera::Yaw(float theta)
{
	Quaternion q;
	q.setRotationAroundAxis(theta, wUP);
	m_camera_quaternion = q * m_camera_quaternion;
	m_camera_local_matrix = m_camera_quaternion.toMatrix();
}

void Camera::Roll(float theta)
{
	Quaternion q;
	q.setRotationAroundAxis(theta, getForward());
	m_camera_quaternion = q * m_camera_quaternion;
	m_camera_local_matrix = m_camera_quaternion.toMatrix();

}

void Camera::debugPrintState()
{
	Matrix4x4 temp;
	temp = m_camera_local_matrix;
	temp.setTranslation(m_position);
	temp.debugPrint("Camera World Matrix");
	
	char debug[200];
	sprintf_s(debug, "MagUp: %f, MagRight %f, MagForward %f\n", this->getUp().magnitude(),  this->getRight().magnitude(), this->getForward().magnitude());
	OutputDebugString(debug);
}

Matrix4x4 Camera::getPerspectiveTransform()
{
	Matrix4x4 persp;
	float theta = (float)m_fov / 2.0f * 0.0174532f;

	persp.setIdentity();
	persp.m_mat[0][0] = 1 / tanf(theta);
	persp.m_mat[1][1] = m_aspect / tanf(theta);
	persp.m_mat[2][2] = m_far_plane / (m_far_plane - m_near_plane);
	persp.m_mat[3][2] = -(m_near_plane * m_far_plane) / (m_far_plane - m_near_plane);
	persp.m_mat[2][3] = 1.0f;
	persp.m_mat[3][3] = 0;
	return persp;
}

Matrix4x4 Camera::getCameraTransform()
{
	Matrix4x4 cam;
	
	cam = m_camera_local_matrix;
	cam.setTranslation(m_position);
	cam.invert();
		
	return cam;
}

Vector3D Camera::getPosition()
{
	return m_position;
}

Vector3D Camera::getUp()
{
	return m_camera_local_matrix.getUp();
}

Vector3D Camera::getRight()
{
	return m_camera_local_matrix.getRight();
}

Vector3D Camera::getForward()
{
	return m_camera_local_matrix.getForward();
}

Camera::Camera(Vector3D position, Vector3D lookat, Vector3D up, float aspect_ratio, float near_plane, float far_plane, int fov)
{
	Matrix4x4 temp;

	m_position = position;

	this->lookAt(lookat, up);
		
	m_aspect = aspect_ratio;
	m_near_plane = near_plane;
	m_far_plane = far_plane;
	m_fov = fov;

	this->debugPrintState();
}

