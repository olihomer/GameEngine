#include "Camera.h"
#include <Windows.h>


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
	Vector3D camera_z = (target - this->getPosition());
	camera_z.normalize();
	Vector3D camera_y = up;
	camera_y.normalize();
	Vector3D camera_x;
	camera_x.cross(camera_y, camera_z);

	m_camera_local_matrix.m_mat[0][0] = camera_x.m_x;
	m_camera_local_matrix.m_mat[0][1] = camera_x.m_y;
	m_camera_local_matrix.m_mat[0][2] = camera_x.m_z;

	m_camera_local_matrix.m_mat[1][0] = camera_y.m_x;
	m_camera_local_matrix.m_mat[1][1] = camera_y.m_y;
	m_camera_local_matrix.m_mat[1][2] = camera_y.m_z;

	m_camera_local_matrix.m_mat[2][0] = camera_z.m_x;
	m_camera_local_matrix.m_mat[2][1] = camera_z.m_y;
	m_camera_local_matrix.m_mat[2][2] = camera_z.m_z;

	m_pitch = atanf(camera_z.m_y / camera_z.m_z); //PITCH ZERO => FORWARD = +VE Z WORLD AXIS. +VE PITCH = UPWARDS
	m_roll = atanf(camera_y.m_x / camera_y.m_y); //ROLL ZERO => UP = +VE Y AXIS. +VE ROLL = CLOCKWISE
	m_yaw = atanf(camera_z.m_x / camera_z.m_z); //YAW ZERO => FORWARD = +VE Z WORLD AXIS. +VE YAW = RIGHTWARDS

}


void Camera::Pitch(float theta)
{
	Vector3D camera_x, camera_y, camera_z;

	m_pitch += theta;
	
	camera_x = getRight();
	camera_z.m_x = sinf(m_yaw);
	camera_z.m_y = sinf(m_pitch);
	camera_z.m_z = cosf(m_pitch);
	camera_x.normalize();
	camera_z.normalize();
	camera_y.cross(camera_z, camera_x);
	camera_y.normalize();

	m_camera_local_matrix.m_mat[0][0] = camera_x.m_x;
	m_camera_local_matrix.m_mat[0][1] = camera_x.m_y;
	m_camera_local_matrix.m_mat[0][2] = camera_x.m_z;

	m_camera_local_matrix.m_mat[1][0] = camera_y.m_x;
	m_camera_local_matrix.m_mat[1][1] = camera_y.m_y;
	m_camera_local_matrix.m_mat[1][2] = camera_y.m_z;

	m_camera_local_matrix.m_mat[2][0] = camera_z.m_x;
	m_camera_local_matrix.m_mat[2][1] = camera_z.m_y;
	m_camera_local_matrix.m_mat[2][2] = camera_z.m_z;
	
}

void Camera::Yaw(float theta)
{
	Vector3D camera_x, camera_y, camera_z;

	m_yaw += theta;

	camera_y = getUp();
	camera_z.m_x = sinf(m_yaw);
	camera_z.m_y = sinf(m_pitch);
	camera_z.m_z = cosf(m_yaw);
	camera_y.normalize();
	camera_z.normalize();
	camera_x.cross(camera_y, camera_z);
	camera_x.normalize();

	m_camera_local_matrix.m_mat[0][0] = camera_x.m_x;
	m_camera_local_matrix.m_mat[0][1] = camera_x.m_y;
	m_camera_local_matrix.m_mat[0][2] = camera_x.m_z;

	m_camera_local_matrix.m_mat[1][0] = camera_y.m_x;
	m_camera_local_matrix.m_mat[1][1] = camera_y.m_y;
	m_camera_local_matrix.m_mat[1][2] = camera_y.m_z;

	m_camera_local_matrix.m_mat[2][0] = camera_z.m_x;
	m_camera_local_matrix.m_mat[2][1] = camera_z.m_y;
	m_camera_local_matrix.m_mat[2][2] = camera_z.m_z;

	float a = camera_x.magnitude();
	float b = camera_y.magnitude();
	float c = camera_z.magnitude();
	char debug[200];
	sprintf_s(debug, "%f %f %f\n", a, b, c);
	OutputDebugString(debug);


}

void Camera::Roll(float theta)
{
	//Matrix4x4 temp;
	//temp.setIdentity();
	//temp.setRotZ(theta);
	//m_camera_local_matrix *= temp;
}

void Camera::debugPrintState()
{
	//m_camera_local_matrix.debugPrint("Camera Local Matrix");
	Matrix4x4 temp;
	temp = m_camera_local_matrix;
	temp.setTranslation(m_position);
	temp.debugPrint("Camera World Matrix");
	char debug[200];
	sprintf_s(debug, "Pitch: %f, Roll: %f, Yaw %f\n", m_pitch / DEG_RAD, m_roll / DEG_RAD, m_yaw / DEG_RAD);
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
	Matrix4x4 temp;
	temp = m_camera_local_matrix;
	temp.setTranslation(m_position);

	temp.invert();
	
	//temp.debugPrint("Created Inverse Matrix:");
		
	return temp;
	
}

Vector3D Camera::getPosition()
{
	return m_position;
}

Vector3D Camera::getUp()
{
	return Vector3D(m_camera_local_matrix.m_mat[1][0], m_camera_local_matrix.m_mat[1][1], m_camera_local_matrix.m_mat[1][2]);
}

Vector3D Camera::getRight()
{
	return Vector3D(m_camera_local_matrix.m_mat[0][0], m_camera_local_matrix.m_mat[0][1], m_camera_local_matrix.m_mat[0][2]);
}

Vector3D Camera::getForward()
{
	return Vector3D(m_camera_local_matrix.m_mat[2][0], m_camera_local_matrix.m_mat[2][1], m_camera_local_matrix.m_mat[2][2]);
}

Camera::Camera(Vector3D position, Vector3D lookat, Vector3D up, float aspect_ratio, float near_plane, float far_plane, int fov)
{
	Matrix4x4 temp;

	m_position = position;
	m_camera_local_matrix.setIdentity();
		
	Vector3D camera_z = (lookat - position);
	camera_z.normalize();
	Vector3D camera_y = up;
	camera_y.normalize();
	Vector3D camera_x;
	camera_x.cross(camera_y, camera_z);

	m_camera_local_matrix.m_mat[0][0] = camera_x.m_x;
	m_camera_local_matrix.m_mat[0][1] = camera_x.m_y;
	m_camera_local_matrix.m_mat[0][2] = camera_x.m_z;

	m_camera_local_matrix.m_mat[1][0] = camera_y.m_x;
	m_camera_local_matrix.m_mat[1][1] = camera_y.m_y;
	m_camera_local_matrix.m_mat[1][2] = camera_y.m_z;

	m_camera_local_matrix.m_mat[2][0] = camera_z.m_x;
	m_camera_local_matrix.m_mat[2][1] = camera_z.m_y;
	m_camera_local_matrix.m_mat[2][2] = camera_z.m_z;

	m_pitch = atanf(camera_z.m_y / camera_z.m_z); //PITCH ZERO => FORWARD = +VE Z WORLD AXIS. +VE PITCH = UPWARDS
	m_roll = atanf(camera_y.m_x / camera_y.m_y); //ROLL ZERO => UP = +VE Y AXIS. +VE ROLL = CLOCKWISE
	m_yaw = atanf(camera_z.m_x / camera_z.m_z); //YAW ZERO => FORWARD = +VE Z WORLD AXIS. +VE YAW = RIGHTWARDS

	m_aspect = aspect_ratio;
	m_near_plane = near_plane;
	m_far_plane = far_plane;
	m_fov = fov;

	this->debugPrintState();

}

