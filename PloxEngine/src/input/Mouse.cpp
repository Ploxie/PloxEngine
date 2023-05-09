//
// Created by Ploxie on 2023-03-22.
//
#include "mouse.h"
#include "Input.h"

void Mouse::Update()
{
    m_deltaPosition = (m_position - m_previousPosition);
    m_previousPosition = m_position;
    m_scrollOffset = {};
}

bool Mouse::IsButtonDown(MouseButton button) const
{
    return m_buttons[static_cast<size_t>(button)];
}

glm::vec2 Mouse::GetPosition() const
{
    return m_position;
}

glm::vec2 Mouse::GetDeltaPosition() const
{
    return m_deltaPosition;
}

glm::vec2 Mouse::GetScrollOffset() const
{
    return m_scrollOffset;
}

void Mouse::SetMousePosition(float x, float y)
{

}

void Mouse::OnMove(double x, double y)
{
    m_position.x = static_cast<float>(x);
    m_position.y = static_cast<float>(y);
}

void Mouse::OnScroll(double xOffset, double yOffset)
{
    m_scrollOffset.x += static_cast<float>(xOffset);
    m_scrollOffset.y += static_cast<float>(yOffset);
}

void Mouse::OnButton(MouseButton button, InputAction action)
{
    if(action == InputAction::RELEASE)
    {
        m_buttons.set(static_cast<size_t>(button), false);
    }
    else if(action == InputAction::PRESS)
    {
        m_buttons.set(static_cast<size_t>(button), true);
    }
}