#pragma once
#include <math.h>
#include <iostream>

	class Vector3D {
	public:

		float m_x;
		float m_y;
		float m_z;

		Vector3D() :m_x(0), m_y(0), m_z(0)
		{}
		Vector3D(float x, float y, float z) :m_x(x), m_y(y), m_z(z)
		{}
		Vector3D(const Vector3D& vector) :m_x(vector.m_x), m_y(vector.m_y), m_z(vector.m_z)
		{}

		static float dot(const Vector3D& u, const Vector3D& v)
		{
			return u.m_x * v.m_x + u.m_y * v.m_y + u.m_z * v.m_z;
		}

		static Vector3D cross(const Vector3D& v1, const Vector3D& v2)
		{
			Vector3D temp;
			temp.m_x = v1.m_y * v2.m_z - v1.m_z * v2.m_y;
			temp.m_y = v1.m_z * v2.m_x - v1.m_x * v2.m_z;
			temp.m_z = v1.m_x * v2.m_y - v1.m_y * v2.m_x;
			return temp;
		}

		Vector3D operator+(const Vector3D& vector)
		{
			Vector3D sum;
			sum.m_x = vector.m_x + m_x;
			sum.m_y = vector.m_y + m_y;
			sum.m_z = vector.m_z + m_z;
			return sum;
		}

		Vector3D operator-(const Vector3D& vector)
		{
			Vector3D diff;
			diff.m_x = m_x - vector.m_x;
			diff.m_y = m_y - vector.m_y;
			diff.m_z = m_z - vector.m_z;
			return diff;
		}

		Vector3D operator*(const float& f)
		{
			Vector3D mult;
			mult.m_x = m_x * f;
			mult.m_y = m_y * f;
			mult.m_z = m_z * f;
			return mult;
		}

		friend std::ostream& operator<< (std::ostream& os, const Vector3D& vector)
		{
			os << "(" << vector.m_x << "," << vector.m_y << "," << vector.m_z << ")";
			return os;
		}

		static Vector3D lerp(const Vector3D& start, const Vector3D& end, float delta)
		{
			Vector3D v;
			v.m_x = start.m_x * (1.0f - delta) + end.m_x * delta;
			v.m_y = start.m_y * (1.0f - delta) + end.m_y * delta;
			v.m_z = start.m_z * (1.0f - delta) + end.m_z * delta;

			return v;
		}

		float magnitude()
		{
			return sqrtf(m_x * m_x + m_y * m_y + m_z * m_z);
		}

		void normalize()
		{
			float mag = magnitude();
			m_x /= mag;
			m_y /= mag;
			m_z /= mag;
		}

	};


