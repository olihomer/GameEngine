#include "Camera.h"
#include <Windows.h>
#include <iostream>

constexpr float TWO_PI = 6.28318530718f;

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
	camera_x.cross(camera_y, camera_z);
	camera_x.normalize();
	camera_y.cross(camera_z, camera_x);
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

	//GET EULER ANGLES

	float theta, psi, phi;

	if (camera_z.m_y != 1 && camera_z.m_y != -1)
	{
		psi = -asinf(camera_z.m_y);
		theta = atan2f(camera_z.m_x / cosf(psi), camera_z.m_z / cosf(psi));
		phi = atan2f(camera_x.m_y / cosf(psi), camera_y.m_y / cosf(psi));
	}
	else
	{
		phi = 0;
		if (camera_z.m_y == -1)
		{
			psi = TWO_PI / 4;
			theta = phi + atan2(camera_y.m_x, camera_x.m_x);
		}
		else
		{
			psi = -TWO_PI / 4;
			theta = -phi - atan2(camera_y.m_x, camera_x.m_x);
		}

	}

	m_pitch = psi;
	m_yaw = theta;
	m_roll = phi;

	if (m_pitch != m_pitch)m_pitch = 0;
	if (m_yaw != m_yaw)m_yaw = 0;
	if (m_roll != m_roll)m_roll = 0;

}


void Camera::Pitch(float theta)
{
	m_pitch += theta;
	if (m_pitch > TWO_PI)m_pitch -= TWO_PI;
	if (m_pitch < -TWO_PI)m_pitch += TWO_PI;
}

void Camera::Translate(float delta, Vector3D direction)
{
	m_position.m_x += delta * direction.m_x;
	m_position.m_y += delta * direction.m_y;
	m_position.m_z += delta * direction.m_z;

}

void Camera::Yaw(float theta)
{
	m_yaw += theta;
	if (m_yaw > TWO_PI)m_yaw -= TWO_PI;
	if (m_yaw < -TWO_PI)m_yaw += TWO_PI;
}

void Camera::Roll(float theta)
{
	m_roll += theta;
	if (m_roll > TWO_PI)m_roll -= TWO_PI;
	if (m_roll < -TWO_PI)m_roll += TWO_PI;
}

void Camera::debugPrintState()
{
	Matrix4x4 temp;
	temp = m_camera_local_matrix;
	temp.setTranslation(m_position);
	temp.debugPrint("Camera World Matrix");
	
	char debug[200];
	sprintf_s(debug, "Pitch: %f, Roll: %f, Yaw %f\nMagUp: %f, MagRight %f, MagForward %f\n", m_pitch / DEG_RAD, m_roll / DEG_RAD, m_yaw / DEG_RAD,this->getUp().magnitude(),  this->getRight().magnitude(), this->getForward().magnitude());
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
	Matrix4x4 cam,temp;
	cam.setIdentity(); //CORRESPONDS TO CAMERA LOOKING +VE Z, UP = +VE Y, RIGHT = +VE X
	
	temp.setRotX(m_pitch); //APPLY PITCH
	cam *= temp;

	temp.setIdentity();
	temp.setRotY(m_yaw); //APPLY YAW
	cam *= temp;

	temp.setIdentity();
	temp.setRotZ(m_roll); //APPLY ROLL
	temp *= cam;

	cam = temp;

	m_camera_local_matrix = cam;

	cam.setTranslation(m_position);
	cam.invert();
		
	//cout << "Pitch: "<< m_pitch << " Roll: " << m_roll << " Yaw: "<< m_yaw << endl;
	//cout << "Forward: " << this->getForward() << "Up: " << this->getUp() << " Right: " << this->getRight()<<endl;

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

