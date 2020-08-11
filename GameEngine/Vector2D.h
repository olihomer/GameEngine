#pragma once
#pragma once
#include <math.h>
#include <iostream>

class Vector2D {
public:

	float m_x;
	float m_y;
	
	Vector2D() :m_x(0), m_y(0)
	{}
	Vector2D(float x, float y) :m_x(x), m_y(y)
	{}
	Vector2D(const Vector2D& vector) :m_x(vector.m_x), m_y(vector.m_y)
	{}

	Vector2D operator+(const Vector2D& vector)
	{
		Vector2D sum;
		sum.m_x = vector.m_x - m_x;
		sum.m_y = vector.m_y - m_y;
		return sum;
	}

	Vector2D operator-(const Vector2D& vector)
	{
		Vector2D diff;
		diff.m_x = m_x - vector.m_x;
		diff.m_y = m_y - vector.m_y;
		return diff;
	}

	friend std::ostream& operator<< (std::ostream& os, const Vector2D& vector)
	{
		os << "(" << vector.m_x << "," << vector.m_y << ")";
		return os;
	}

	static Vector2D lerp(const Vector2D& start, const Vector2D& end, float delta)
	{
		Vector2D v;
		v.m_x = start.m_x * (1.0f - delta) + end.m_x * delta;
		v.m_y = start.m_y * (1.0f - delta) + end.m_y * delta;
		
		return v;
	}

	float magnitude()
	{
		return sqrtf(m_x * m_x + m_y * m_y );
	}

	void normalize()
	{
		float mag = magnitude();
		m_x /= mag;
		m_y /= mag;
	}

	void cross(Vector2D v1, Vector2D v2)
	{
		this->m_x = v1.m_x * v2.m_y - v1.m_y * v2.m_x;
		this->m_y = v1.m_y * v2.m_x - v1.m_x * v2.m_y;
		
	}
};