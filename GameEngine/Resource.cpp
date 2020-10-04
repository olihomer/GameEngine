#include "Resource.h"
#include <Windows.h>

Resource::Resource(const wchar_t* full_path):m_full_path(full_path)
{
}

Resource::Resource()
{
}

Resource::~Resource()
{
	OutputDebugString("CLEANUP Resource destructor\n");
}
