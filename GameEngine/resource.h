#pragma once

class Resource
{
public:
	Resource(const wchar_t* full_path);
	Resource();
	virtual ~Resource();
private:
	const wchar_t* m_full_path;
};
