//
// Created by Ploxie on 2023-03-21.
//
#include "Engine.h"
#include "GameLogic.h"
#include "Logger.h"
#include "platform/Platform.h"

void* __cdecl operator new[](size_t size,
			     const char* /*name*/,
			     int /*flags*/,
			     unsigned /*debugFlags*/,
			     const char* /*file*/,
			     int /*line*/)
{
    return new uint8_t[size];
}

void* __cdecl operator new[](size_t size,
			     size_t /*alignment*/,
			     size_t /*alignmentOffset*/,
			     const char* /*pName*/,
			     int /*flags*/,
			     unsigned /*debugFlags*/,
			     const char* /*file*/,
			     int /*line*/)
{
    return new uint8_t[size];
}

Engine Engine::s_instance;

int Engine::Start(int argc, char* argv[], GameLogic* gameLogic) noexcept
{
    s_instance.Initialize(argc, argv, gameLogic);
    while(s_instance.Run())
	;
    s_instance.Shutdown();
    return 0;
}

Window* Engine::GetWindow()
{
    WindowHandle window = Platform::GetActiveWindow();
    if(window == NULL_WINDOW_HANDLE)
    {
	return nullptr;
    }

    return Platform::GetWindow(window);
}

Renderer& Engine::GetRenderer()
{
    return s_instance.m_renderer;
}

void Engine::Initialize(int argc, char* argv[], GameLogic* gameLogic)
{
    m_gameLogic = gameLogic;

    Logger::Initialize();
    Platform::Initialize("PloxieApplication");

    m_window = Platform::CreatePlatformWindow("TestWindow", -1, -1, 1024, 768);
    m_renderer.Initialize(Platform::GetWindow(m_window), GraphicsBackendType::VULKAN);

    m_gameLogic->Initialize(this);

    m_isRunning = true;
}

static bool asd = false;

bool Engine::Run()
{
    m_isRunning = Platform::PumpMessages();

    m_gameLogic->Update(0.0F);

    if(Input::IsKeyDown(Key::K))
    {
	if(!asd)
	{
	    Platform::GetWindow(m_window)->SetWindowMode(WindowMode::FULLSCREEN);
	    asd = true;
	}
    }

    m_renderer.Render();

    return m_isRunning;
}
void Engine::Shutdown()
{
    m_gameLogic->Shutdown();
    m_renderer.Shutdown();
}
