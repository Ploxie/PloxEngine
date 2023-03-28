//
// Created by Ploxie on 2023-03-21.
//

#include "core/Engine.h"
#include "core/logger.h"
#include "eastl/vector.h"
#include "eastl/string.h"
#include "core/gamelogic.h"
#include "platform/platform.h"
#include "platform/filesystem/Filesystem.h"

class Game : public GameLogic
{
public:
    void Initialize(Engine* engine) noexcept override
    {


		Path path("W:/Test/Test2/test.txt");

		LOG_INFO("Path: {0}", path.GetParentPath());
		LOG_INFO("File size: {0}", Platform::FileSystem->Size("W:/Test/asd.txt"));

        LOG_INFO("Game Initialized");
    }
    void Update(float deltaTime) noexcept override
    {
        if(Engine::Input->IsKeyDown(Key::U, true))
        {
            LOG_TRACE("ASD");
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
