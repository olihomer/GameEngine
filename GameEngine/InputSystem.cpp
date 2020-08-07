#include "InputSystem.h"
#include <Windows.h>
#include "Point.h"

InputSystem* InputSystem::get()
{
	static InputSystem system;
	return &system;
}

void InputSystem::update()
{
	POINT current_mouse_pos = {};
	::GetCursorPos(&current_mouse_pos);

	if (m_first_time)
	{
		m_first_time = false;
		m_old_mouse_pos = Point(current_mouse_pos.x, current_mouse_pos.y);
	}
		
	if (current_mouse_pos.x != m_old_mouse_pos.m_x || current_mouse_pos.y != m_old_mouse_pos.m_y)
	{
		std::unordered_set<InputListener*>::iterator it = m_set_listeners.begin();
		while (it != m_set_listeners.end())
		{
			(*it)->onMouseMove(Point(current_mouse_pos.x-m_old_mouse_pos.m_x,current_mouse_pos.y-m_old_mouse_pos.m_y));
			++it;

		}
	}
		
	if (::GetKeyboardState(m_keys_state))
	{
		for (int i = 0; i < 256; i++)
		{
			if (m_keys_state[i] & 0x80)
			{
				//KEY DOWN
				if (i == VK_LBUTTON && (m_keys_state[i] != m_old_keys_state[i]))
				{
					//ACTUALLY LEFT MOUSE DOWN
					std::unordered_set<InputListener*>::iterator it = m_set_listeners.begin();
					while (it != m_set_listeners.end())
					{
						(*it)->onLeftMouseDown(Point(current_mouse_pos.x, current_mouse_pos.y));
						++it;

					}
				}
				else
				if (i == VK_RBUTTON && (m_keys_state[i] != m_old_keys_state[i]))
				{
					//ACTUALLY RIGHT MOUSE DOWN
					std::unordered_set<InputListener*>::iterator it = m_set_listeners.begin();
					while (it != m_set_listeners.end())
					{
							(*it)->onRightMouseDown(Point(current_mouse_pos.x, current_mouse_pos.y));
							++it;

					}
				}
				else
				{
					std::unordered_set<InputListener*>::iterator it = m_set_listeners.begin();
					while (it != m_set_listeners.end())
					{
						(*it)->onKeyDown(i);
						++it;

					}
				}
			}
			else
			{
				//KEY UP
				if (m_keys_state[i] != m_old_keys_state[i])
				{
					if (i == VK_LBUTTON)
					{
						//ACTUALLY LEFT MOUSE UP
						std::unordered_set<InputListener*>::iterator it = m_set_listeners.begin();
						while (it != m_set_listeners.end())
						{
							(*it)->onLeftMouseUp(Point(current_mouse_pos.x, current_mouse_pos.y));
							++it;

						}
					}
					else
						if (i == VK_RBUTTON)
						{
							//ACTUALLY RIGHT MOUSE DOWN
							std::unordered_set<InputListener*>::iterator it = m_set_listeners.begin();
							while (it != m_set_listeners.end())
							{
								(*it)->onRightMouseUp(Point(current_mouse_pos.x, current_mouse_pos.y));
								++it;

							}
						}
						else
						{

							std::unordered_set<InputListener*>::iterator it = m_set_listeners.begin();
							while (it != m_set_listeners.end())
							{
								(*it)->onKeyUp(i);
								++it;
							}
						}
				}
			}
		}
		::memcpy(m_old_keys_state, m_keys_state, sizeof(unsigned char) * 256);
		m_old_mouse_pos = Point(current_mouse_pos.x, current_mouse_pos.y);

	}
}

void InputSystem::addListener(InputListener* listener)
{
	OutputDebugString("Entering InputSystem::addListener\n");
	m_set_listeners.insert(listener);
}

void InputSystem::removeListener(InputListener* listener)
{
	OutputDebugString("Entering InputSystem::eraseListener\n");
	m_set_listeners.erase(listener);
}
