//
// Created by Ploxie on 2023-03-21.
//

#include "core/Engine.h"
#include "core/gamelogic.h"
#include "core/logger.h"
#include "eastl/string.h"
#include "eastl/vector.h"
#include "platform/platform.h"

class Game : public GameLogic
{
public:
    void Initialize(Engine* engine) noexcept override
    {


		LOG_INFO("Game Initialized");
    }
    void Update(float deltaTime) noexcept override
    {
		static int i;
		if(Input::IsKeyDown(Key::U, true))
		{
			LOG_TRACE("{0}", i++);
		}

    }
    void Shutdown() noexcept override
    {
		LOG_INFO("Game Shutdown");
    }
};

int main(int argc, char* argv[])
{
    Game game;
    Engine engine;
    return engine.Start(argc, argv, &game);
}
