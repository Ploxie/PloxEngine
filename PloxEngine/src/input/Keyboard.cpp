//
// Created by Ploxie on 2023-03-22.
//
#include "Keyboard.h"
#include "Input.h"

bool Keyboard::IsKeyDown(Key key, bool ignoreRepeated) const
{
    auto i = static_cast<size_t>(key);
    return i < m_repeatedKeys.size() && m_pressedKeys[i] && (!ignoreRepeated || !m_repeatedKeys[i]);
}

void Keyboard::OnKey(Key key, InputAction action)
{
    const auto keyIndex = static_cast<size_t>(key);

	switch(action)
	{
	case InputAction::RELEASE:
	{
		if(keyIndex < m_pressedKeys.size() && keyIndex < m_repeatedKeys.size())
		{
			m_pressedKeys.set(static_cast<size_t>(key), false);
			m_repeatedKeys.set(static_cast<size_t>(key), false);
		}
		break;
	}
	case InputAction::PRESS:
	{
		if(keyIndex < m_pressedKeys.size())
		{
			m_pressedKeys.set(static_cast<size_t>(key), true);
		}
		break;
	}
	case InputAction::REPEAT:
	{
		if(keyIndex < m_repeatedKeys.size())
		{
			m_repeatedKeys.set(static_cast<size_t>(key), true);
		}
		break;
	}
	default:
		break;
	}
}

void Keyboard::OnChar(unsigned int codepoint)
{

}


