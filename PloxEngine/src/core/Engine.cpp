//
// Created by Ploxie on 2023-03-21.
//
#include "Engine.h"
#include "GameLogic.h"
#include "Logger.h"

void* __cdecl operator new[](size_t size,
							 const char* /*name*/,
							 int /*flags*/,
							 unsigned /*debugFlags*/,
							 const char*  /*file*/,
							 int  /*line*/)
{
	return new uint8_t[size];
}

void* __cdecl operator new[](size_t size,
							 size_t  /*alignment*/,
							 size_t  /*alignmentOffset*/,
							 const char*  /*pName*/,
							 int  /*flags*/,
							 unsigned  /*debugFlags*/,
							 const char*  /*file*/,
							 int  /*line*/)
{
	return new uint8_t[size];
}

Engine* Engine::s_instance = nullptr;
EventManager* Engine::EventManager = nullptr;
Input* Engine::Input = nullptr;

int Engine::Start(int  /*argc*/, char*  /*argv*/[], GameLogic* gameLogic) noexcept
{
	s_instance = this;

	EventManager = &m_eventManager;
	Input = &m_input;

	Logger::Initialize();

	m_gameLogic = gameLogic;
	Window window(800, 600, Window::WindowMode::WINDOWED, "PloxEngine");
	m_window = &window;

	m_gameLogic->Initialize(this);

	while(!m_window->ShouldClose())
	{
		m_window->PollEvents();
		m_gameLogic->Update(0.0F);
	}

	m_gameLogic->Shutdown();
	return 0;
}
