//
// Created by Ploxie on 2023-03-21.
//

#pragma once
#include "Event.h"
#include "eastl/vector.h"
#include "platform/window/window.h"

class GameLogic;

class Engine
{
public:
	static int Start(int argc, char* argv[], GameLogic* gameLogic) noexcept;

	static Window* GetWindow();
private:

	void Initialize(int argc, char* argv[], GameLogic* gameLogic);
	bool Run();
	void Shutdown();

private:
	static Engine s_instance;

private:
	bool m_isRunning;
	GameLogic* m_gameLogic = nullptr;
	EventManager m_eventManager;
	WindowHandle m_window;
};
