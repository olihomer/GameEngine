#pragma once
#include "Vector3D.h"
#include "Vector4D.h"
#include "Matrix4x4.h"
#include <windows.h>

constexpr auto DEG_RAD = 0.0174532925f;
constexpr bool rLocal=true;
constexpr bool rGlobal=false;

class Quaternion {
private:
	float m_w; //real part
	Vector3D m_v; //vector part
public:
	Quaternion() :m_w(0.0f), m_v() {}
	Quaternion(float w, Vector3D v) :m_w(w), m_v(v) {}
	Quaternion(float w, float x, float y, float z) :m_w(w), m_v(Vector3D(x,y,z)) {}

	Vector3D vectorPart(){return m_v;}
	float realPart(){return m_w;}

	Quaternion operator*(const Quaternion& q)
	{
		Quaternion t;
		
		Vector3D v = m_v * 1.0f;

		t.m_w = m_w * q.m_w - Vector3D::dot(m_v, q.m_v);
		t.m_v = (Vector3D(q.m_v) * m_w) + (Vector3D(m_v) * q.m_w) + Vector3D::cross(m_v, q.m_v);

		return t;
	}

	Quaternion operator+(const Quaternion& q)
	{
		Quaternion t;
		t.m_w = m_w + q.m_w;
		t.m_v = m_v + q.m_v;
		return t;
	}


	void setIdentity()
	{
		m_w = 1.0f;
		m_v = Vector3D(0, 0, 0);
	}

	float magnitude()
	{
		float vmag = m_v.magnitude();
		vmag *= vmag;
		vmag += m_w * m_w;
		return sqrtf(vmag);
	}


	void setRotationAroundAxis(float theta, const Vector3D& axis) //RADIANS
	{
		m_w = cosf(0.5f * theta);
		m_v.m_x = sinf(0.5f * theta) * axis.m_x;
		m_v.m_y = sinf(0.5f * theta) * axis.m_y;
		m_v.m_z = sinf(0.5f * theta) * axis.m_z;
	}

	static void rotatePointAroundVector(Vector3D& point, float theta, const Vector3D& axis) //RADIANS
	{
		Quaternion q,qbar;

		q.m_w = cosf(0.5f * theta);
		q.m_v.m_x = sinf(0.5f * theta) * axis.m_x;
		q.m_v.m_y = sinf(0.5f * theta) * axis.m_y;
		q.m_v.m_z = sinf(0.5f * theta) * axis.m_z;

		qbar = q.getCongugate();

		Quaternion v(0, point);

		v = q * v * qbar;

		point = v.vectorPart();
	}

	Quaternion getCongugate()
	{
		Quaternion q;
		q.m_w = m_w;
		q.m_v = m_v*-1.0f;
		return q;
	}

	void normalise()
	{
		float mag;
		mag = magnitude();
		m_w *= (1/mag);
		m_v = m_v*(1/mag);
	}

	void debugPrint(const char* txt)
	{
		char debug[100];
		sprintf_s(debug, "%s\n%f %f %f %f, mag = %f\n",txt, m_w,m_v.m_x, m_v.m_y, m_v.m_z,this->magnitude());
		OutputDebugString(debug);
	}

	Matrix4x4 toMatrix()
	{
		Matrix4x4 m;
		float qw = m_w;
		float qx = m_v.m_x;
		float qy = m_v.m_y;
		float qz = m_v.m_z;

		m.m_mat[0][0] = 1 - 2 * qy * qy - 2 * qz * qz;
		m.m_mat[0][1] = 2 * qx * qy + 2 * qz * qw;
		m.m_mat[0][2] = 2 * qx * qz - 2 * qy * qw;
		
		m.m_mat[1][0] = 2 * qx * qy - 2 * qz * qw;
		m.m_mat[1][1] = 1 - 2 * qx * qx - 2 * qz * qz;
		m.m_mat[1][2] = 2 * qy * qz + 2 * qx * qw;

		m.m_mat[2][0] = 2 * qx * qz + 2 * qy * qw;
		m.m_mat[2][1] = 2 * qy * qz - 2 * qx * qw;
		m.m_mat[2][2] = 1 - 2 * qx * qx - 2 * qy * qy;
		
		return m;
	}

	Quaternion(Matrix4x4 m)
	{
		float t;
	
		if (m.m_mat[2][2] < 0)
		{
			if (m.m_mat[0][0] > m.m_mat[1][1])
			{
				t = 1 + m.m_mat[0][0] - m.m_mat[1][1] - m.m_mat[2][2];
				m_w = m.m_mat[1][2] - m.m_mat[2][1];
				m_v = Vector3D(t, m.m_mat[0][1] + m.m_mat[1][0], m.m_mat[2][0] + m.m_mat[0][2]);
			}
			else
			{
				t = 1 - m.m_mat[0][0] + m.m_mat[1][1] - m.m_mat[2][2];
				m_w = m.m_mat[2][0] - m.m_mat[0][2];
				m_v = Vector3D(m.m_mat[0][1] + m.m_mat[1][0], t,m.m_mat[1][2] + m.m_mat[2][1]);
			}
		}
		else
		{
			if (m.m_mat[0][0] < -m.m_mat[1][1])
			{
				t = 1 - m.m_mat[0][0] - m.m_mat[1][1] + m.m_mat[2][2];
				m_w = m.m_mat[0][1] - m.m_mat[1][0];
				m_v = Vector3D(m.m_mat[2][0] + m.m_mat[0][2], m.m_mat[1][2] + m.m_mat[2][1],t);
			}
			else
			{
				t = 1 + m.m_mat[0][0] + m.m_mat[1][1] + m.m_mat[2][2];
				m_w = t;
				m_v = Vector3D(m.m_mat[1][2] - m.m_mat[2][1], m.m_mat[2][0] - m.m_mat[0][2], m.m_mat[0][1] - m.m_mat[1][0]);
			}
		}
		m_w *= (0.5f / sqrtf(t));
		m_v = m_v * (0.5f / sqrtf(t));
	}
};