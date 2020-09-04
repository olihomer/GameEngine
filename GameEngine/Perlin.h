#pragma once
#include "Vector2D.h"
#include <time.h>

constexpr float ROOT2 = 1.4142136f;

class Perlin
{
public:
	Perlin(bool randomize=true);
	float rand01();
	
	static const int xmax = 200;
	static const int ymax = 200;

	Vector2D gradients[xmax][ymax];
	float getNoise(float x, float y);
	static float interp(float x, float a, float b);
	static float smoothinterp(float x, float a, float b);
};


Perlin::Perlin(bool randomize)
{
	OutputDebugString("Entering Perlin Constructor\n");

	//SET RANDOM SEED
	if(randomize)srand((unsigned int)time(NULL));

	//PREPARE GRADIENT VECTORS AT INTEGER NODES
	for (int i = 0; i < xmax; i++)
	{
		for (int j = 0; j < ymax; j++)
		{
			float r=rand01();
			if (r < 0.125f)gradients[i][j] = Vector2D(-1, -1);
			else
			if (r < 0.250f)gradients[i][j] = Vector2D(-1, 1);
			else
			if (r < 0.375f)gradients[i][j] = Vector2D(1, -1);
			else
			if (r < 0.500f)gradients[i][j] = Vector2D(1, 1);
			else
			if (r < 0.625f)gradients[i][j] = Vector2D(-ROOT2, 0);
			else
			if (r < 0.750f)gradients[i][j] = Vector2D(ROOT2, 0);
			else
			if (r < 0.875f)gradients[i][j] = Vector2D(0, ROOT2);
			else
			gradients[i][j] = Vector2D(0, -ROOT2);

			//gradients[i][j] = Vector2D(-1+2*rand01(), -1*2*rand01());
			//gradients[i][j].normalize();
		}
	}
}

float Perlin::rand01()
{
	float r = (float)rand() / RAND_MAX;
	return r;
}

float Perlin::getNoise(float x, float y)
{
	int cellx = (int)x;
	int celly = (int)y;

	Vector2D distance1 = Vector2D(x, y) - Vector2D((float)cellx, (float)celly);
	Vector2D distance2 = Vector2D(x, y) - Vector2D((float)cellx+1, (float)celly);
	Vector2D distance3 = Vector2D(x, y) - Vector2D((float)cellx, (float)celly+1);
	Vector2D distance4 = Vector2D(x, y) - Vector2D((float)cellx + 1, (float)celly + 1);

	Vector2D gradient1 = gradients[cellx][celly];
	Vector2D gradient2 = gradients[cellx+1][celly];
	Vector2D gradient3 = gradients[cellx][celly+1];
	Vector2D gradient4 = gradients[cellx+1][celly+1];

	float dot1 = Vector2D::dot(distance1, gradient1);
	float dot2 = Vector2D::dot(distance2, gradient2);
	float dot3 = Vector2D::dot(distance3, gradient3);
	float dot4 = Vector2D::dot(distance4, gradient4);

	float interp1 = smoothinterp(x - cellx, dot1, dot2);
	float interp2 = smoothinterp(x - cellx, dot3, dot4);
	float interp3 = smoothinterp(y - celly, interp1, interp2);

	float noise = interp3;

	return noise;
}


float Perlin::smoothinterp(float x, float a, float b)
{
	float index = 6 * powf(x, 5) - 15 * powf(x, 4) + 10 * pow(x, 3);
	float value = a + index * (b-a);
	return value;
}


float Perlin::interp(float x, float a, float b)
{
	float index = x;
	float value = a + index * (b - a);
	return value;
}