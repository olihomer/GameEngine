#pragma once
#include <d2d1_1.h>
#include <dwrite.h>
#include "Prerequisites.h"
#include "Vector2D.h"

struct BGRA
{
	BYTE b;
	BYTE g;
	BYTE r;
	BYTE a;
};

#undef RGB
#define RGB(r,g,b) BGRA{(BYTE)b,(BYTE)g,(BYTE)r,0}
#define RGBA(r,g,b,a) BGRA{(BYTE)b,(BYTE)g,(BYTE)r,(BYTE)a}

class Graphics2D
{
public:
	Graphics2D(ID2D1DeviceContext* device_context, RenderSystem* system);
	~Graphics2D();

	void drawRectangle(float x1, float y1, float x2, float y2, D3DCOLORVALUE color, float thickness=1.0f, bool filled=false);
	void drawText(const WCHAR* string, float x1, float y1, float x2, float y2, D3DCOLORVALUE color, int size);
	void setFont(const WCHAR* string);
	void createBitmap(unsigned int x, unsigned int y);
	BGRA* getBitmap();
	Vector2D getBitmapWidthHeight();
	void beginDraw();
	void endDraw();
	void plotBitmap(unsigned int x, unsigned int y, BGRA color);
	void updateBitmap();
	void drawBitmap(float x1, float y1, float x2, float y2);

private:
	ID2D1DeviceContext* m_device_context;
	ID2D1Bitmap* m_bitmap;
	unsigned int m_bitmap_width = 0;
	unsigned int m_bitmap_height = 0;
	BGRA* m_plot_target;
	IDWriteFactory* m_factory;
	RenderSystem* m_system = nullptr;
	IDWriteTextFormat* m_text_format=nullptr;
	const WCHAR* m_font = L"Arial";
};

