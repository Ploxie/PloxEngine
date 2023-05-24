//
// Created by Ploxie on 2023-05-23.
//

#pragma once
#include "EASTL/vector.h"
#include <cstdint>

class GraphicsAdapter;
class Command;
class Queue;
class CommandPool;

class CommandFramePool
{
public:
    explicit CommandFramePool() noexcept = default;
    ~CommandFramePool() noexcept;

    CommandFramePool(const CommandFramePool&)		  = delete;
    CommandFramePool(const CommandFramePool&&)		  = delete;
    CommandFramePool& operator=(const CommandFramePool&)  = delete;
    CommandFramePool& operator=(const CommandFramePool&&) = delete;

    void Initialize(GraphicsAdapter* adapter, uint32_t allocationChunkSize = 64) noexcept;

    Command* Acquire(Queue* queue) noexcept;

    void Reset() noexcept;

private:
    GraphicsAdapter* m_adapter		  = nullptr;
    uint32_t m_allocationChunkSize	  = 64;
    CommandPool* m_commandPools[3]	  = {};
    eastl::vector<Command*> m_commands[3] = {};
    uint32_t m_nextFreeCommand[3]	  = {};
};