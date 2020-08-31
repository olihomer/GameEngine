#include "Graphics2D.h"
#include <iostream>

Graphics2D::Graphics2D(ID2D1DeviceContext* device_context, RenderSystem* system):m_device_context(device_context),m_system(system)
{
	OutputDebugString("Entering Graphics2D Constructor\n");

	HRESULT hr=DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)&m_factory);

	if (FAILED(hr))
	{
		throw std::exception("Couldn't create DWrite Factory");
	}

	OutputDebugString("DWriteFactory created ok\n");

	setFont(m_font);	
}

Graphics2D::~Graphics2D()
{
	OutputDebugString("CLEANUP Graphics2D destructor\n");
	m_factory->Release();
	m_text_format->Release();
	m_bitmap->Release();
	delete[] m_plot_target;
}

void Graphics2D::drawRectangle(float x1, float y1, float x2, float y2, D3DCOLORVALUE color, float thickness, bool filled)
{
	ID2D1SolidColorBrush* pBrush;
	
	m_device_context->CreateSolidColorBrush(color, &pBrush);

	D2D1_RECT_F rect = D2D1::RectF(x1, y1, x2, y2);

	m_device_context->BeginDraw();

	if (!filled)
	{
		m_device_context->DrawRectangle(rect, pBrush, thickness);
	}
	else
	{
		m_device_context->FillRectangle(rect, pBrush);
	}
	
	m_device_context->EndDraw();

	pBrush->Release();
}

void Graphics2D::drawText(const WCHAR* string, float x1, float y1, float x2, float y2, D3DCOLORVALUE color, int size)
{
	ID2D1SolidColorBrush* pBrush;

	m_device_context->CreateSolidColorBrush(color, &pBrush);
	m_device_context->BeginDraw();

	D2D1_RECT_F rect = D2D1::RectF(x1, y1, x2, y2);

	m_device_context->DrawTextA(string, (int)wcslen(string), m_text_format, &rect, pBrush);
	m_device_context->EndDraw();

	pBrush->Release();
}

void Graphics2D::setFont(const WCHAR* string)
{
	m_font = string;
	if(m_text_format)m_text_format->Release();
	HRESULT hr=m_factory->CreateTextFormat(m_font, NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 24, L"en-UK", &m_text_format);
}

void Graphics2D::createBitmap(unsigned int x, unsigned int y)
{

	D2D1_BITMAP_PROPERTIES bitmapProp;
	bitmapProp.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE);
	bitmapProp.dpiX = 96;
	bitmapProp.dpiY = 96;

	m_bitmap_width = x;
	m_bitmap_height = y;

	HRESULT hr = m_device_context->CreateBitmap(D2D1::SizeU(x, y), bitmapProp, &m_bitmap);

	if (FAILED(hr))
	{
		throw std::exception("Couldn't create bitmap");
	}

	D2D_RECT_U rect = { 0,0,x,y};

	m_plot_target = new BGRA[x*y];
	
	for (unsigned int j = 0; j < y; j++)
		for (unsigned int i = 0; i < x; i++)
		{
			m_plot_target[j * x + i] = BGRA{};
		}

	m_bitmap->CopyFromMemory(&rect, m_plot_target, x*sizeof(BGRA));

}

BGRA* Graphics2D::getBitmap()
{
	return m_plot_target;
}

void Graphics2D::beginDraw()
{
	m_device_context->BeginDraw();
}


void Graphics2D::endDraw()
{
	m_device_context->EndDraw();
}

void Graphics2D::plotBitmap(unsigned int x, unsigned int y, BGRA color)
{
	if (x >= m_bitmap_width || x < 0 || y >= m_bitmap_height || y < 0)return;
	m_plot_target[y * m_bitmap_width + x] = color;
}

void Graphics2D::updateBitmap()
{
	D2D_RECT_U rect = { 0,0,m_bitmap_width,m_bitmap_height };
	m_bitmap->CopyFromMemory(&rect, m_plot_target, m_bitmap_width * sizeof(BGRA));
}

void Graphics2D::drawBitmap(float x1, float y1, float x2, float y2)
{
	D2D1_RECT_F rect = D2D1::RectF(x1, y1, x2, y2);
	
	m_device_context->BeginDraw();
	m_device_context->DrawBitmap(m_bitmap, rect);
	m_device_context->EndDraw();
}
