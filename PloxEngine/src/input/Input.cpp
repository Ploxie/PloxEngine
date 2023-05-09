//
// Created by Ploxie on 2023-03-23.
//
#include "Input.h"
#include "core/engine.h"


bool Input::IsMouseButtonDown(MouseButton button)
{
	Window* window = Engine::GetWindow();
	if(window == nullptr)
	{
		return false;
	}
	return window->GetMouse().IsButtonDown(button);
}

glm::vec2 Input::GetMousePosition()
{
	Window* window = Engine::GetWindow();
	if(window == nullptr)
	{
		return {};
	}
    return window->GetMouse().GetPosition();
}

glm::vec2 Input::GetMouseDeltaPosition()
{
	Window* window = Engine::GetWindow();
	if(window == nullptr)
	{
		return {};
	}
    return window->GetMouse().GetDeltaPosition();
}

glm::vec2 Input::GetScrollOffset()
{
	Window* window = Engine::GetWindow();
	if(window == nullptr)
	{
		return {};
	}
    return window->GetMouse().GetScrollOffset();
}

bool Input::IsKeyDown(Key key, bool ignoreRepeated)
{
	Window* window = Engine::GetWindow();
	if(window == nullptr)
	{
		return false;
	}
	return window->GetKeyboard().IsKeyDown(key, ignoreRepeated);
}