//
// Created by Ploxie on 2023-03-21.
//

#pragma once
#include "Event.h"
#include "input/Input.h"
#include "window/Window.h"

class GameLogic;

class Engine
{
public:
	int Start(int argc, char* argv[], GameLogic* gameLogic) noexcept;

	inline static Window* GetWindow()
	{
		return s_instance->m_window;
	}

public:
	static EventManager* EventManager;
	static Input* Input;

private:
	static Engine* s_instance;

	GameLogic* m_gameLogic = nullptr;
	Window* m_window = nullptr;
	class EventManager m_eventManager;
	class Input m_input;
};
