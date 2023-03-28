//
// Created by Ploxie on 2023-03-21.
//

#pragma once

class Engine;

class GameLogic
{
public:

    virtual void Initialize(Engine* engine) noexcept = 0;
    virtual void Update(float deltaTime) noexcept = 0;
    virtual void Shutdown() noexcept = 0;

};
