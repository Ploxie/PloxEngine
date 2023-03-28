//
// Created by Ploxie on 2023-03-23.
//
#include "Input.h"

void Input::Update()
{
    m_mouseDeltaPosition = (m_mousePosition - m_previousMousePosition);
    m_previousMousePosition = m_mousePosition;
    m_scrollOffset = {};
}

bool Input::IsMouseButtonDown(MouseButton button) const
{
    return m_mouseButtons[static_cast<size_t>(button)];
}

glm::vec2 Input::GetMousePosition() const
{
    return m_mousePosition;
}

glm::vec2 Input::GetMouseDeltaPosition() const
{
    return m_mouseDeltaPosition;
}

glm::vec2 Input::GetScrollOffset() const
{
    return m_scrollOffset;
}

void Input::SetMousePosition(float x, float y)
{

}

bool Input::IsKeyDown(Key key, bool ignoreRepeated) const
{
    auto i = static_cast<size_t>(key);
    return i < m_repeatedKeys.size() && i < m_repeatedKeys.size() && m_pressedKeys[i] && (!ignoreRepeated || !m_repeatedKeys[i]);
}

void Input::OnMouseMove(double x, double y)
{
    m_mousePosition.x = static_cast<float>(x);
    m_mousePosition.y = static_cast<float>(y);
}

void Input::OnScroll(double xOffset, double yOffset)
{
    m_scrollOffset.x += static_cast<float>(xOffset);
    m_scrollOffset.y += static_cast<float>(yOffset);
}

void Input::OnMouseButton(MouseButton button, InputAction action)
{
    if(action == InputAction::RELEASE)
    {
        m_mouseButtons.set(static_cast<size_t>(button), false);
    }
    else if(action == InputAction::PRESS)
    {
        m_mouseButtons.set(static_cast<size_t>(button), true);
    }
}

void Input::OnKey(Key key, InputAction action)
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

void Input::OnChar(unsigned int codepoint)
{

}