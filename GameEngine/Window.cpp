#include "Window.h"
#include <exception>

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

Window::Window()
{
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = NULL;
	wc.lpfnWndProc = &WndProc;
	wc.cbWndExtra = NULL;
	wc.cbClsExtra = NULL;
	wc.hInstance = NULL;
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wc.lpszClassName = "MyWindowClass";
	wc.lpszMenuName = NULL;

	OutputDebugString("Entering Window constructor\n");

	if (!::RegisterClassEx(&wc))
		throw std::exception("Couldn't register Window class");

	OutputDebugString("About to call CreateWindowEx\n");

	m_hwnd = ::CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, "MyWindowClass", "Oli's Windows", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 1024, 768, NULL, NULL, NULL, this);

	if (!m_hwnd) {
		throw std::exception("Couldn't CreateWindowEx");
	}

	OutputDebugString("Created Window\n");

	::ShowWindow(m_hwnd, SW_SHOW);
	::UpdateWindow(m_hwnd);
	m_is_run = true;

}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
		case WM_CREATE:
		{
			//window creation event
			break;
		}
		case WM_DESTROY:
		{
			//window close event
			auto* const window =
				reinterpret_cast<Window* const>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
			window->onDestroy();
			PostQuitMessage(0);
			break;
		}
		case WM_SETFOCUS:
		{
			auto* const window =
				reinterpret_cast<Window* const>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
			if(window)window->onFocus();
			break;
		}
		case WM_KILLFOCUS:
		{
			auto* const window =
				reinterpret_cast<Window* const>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
			window->onKillFocus();
			break;
		}
		default:
		{
			return ::DefWindowProc(hwnd, msg, wparam, lparam);
		}
	}
	return NULL;
}


void Window::broadcast()
{
	MSG msg;
	
	if (!this->m_is_init)
	{
		SetWindowLongPtr(m_hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
		this->onCreate();
		this->m_is_init = true;
	}

	this->onUpdate();

	while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	Sleep(1);

}

bool Window::isRun()
{
	if (m_is_run)broadcast();
	return m_is_run;
}

RECT Window::getClientWindowRect()
{
	RECT rc;
	::GetClientRect(this->m_hwnd, &rc);
	return rc;
}

void Window::onCreate()
{
	OutputDebugString("Entering Window::onCreate\n");
}

void Window::onUpdate()
{
}

void Window::onDestroy()
{
	m_is_run = false;
}

void Window::onFocus()
{
}

void Window::onKillFocus()
{
}

Window::~Window()
{
	OutputDebugString("CLEANUP Window destructor\n");
	if (m_hwnd)
		DestroyWindow(m_hwnd);
}
