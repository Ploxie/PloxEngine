//
// Created by Ploxie on 2023-05-23.
//
#include "CommandFramePool.h"
#include "rendering/GraphicsAdapter.h"
#include "rendering/types/CommandPool.h"

CommandFramePool::~CommandFramePool() noexcept
{
    Reset();
    for(size_t i = 0; i < 3; i++)
    {
	if(m_commandPools[i])
	{
	    m_adapter->DestroyCommandPool(m_commandPools[i]);
	}
    }
}

void CommandFramePool::Initialize(GraphicsAdapter* adapter, uint32_t allocationChunkSize) noexcept
{
    m_adapter		  = adapter;
    m_allocationChunkSize = allocationChunkSize;
}

Command* CommandFramePool::Acquire(Queue* queue) noexcept
{
    const uint32_t poolIndex = queue == m_adapter->GetGraphicsQueue() ? 0 : queue == m_adapter->GetComputeQueue() ? 1 :
														    2;

    if(!m_commandPools[poolIndex])
    {
	m_adapter->CreateCommandPool(queue, &m_commandPools[poolIndex]);
    }

    const size_t currentPoolSize = m_commands[poolIndex].size();
    if(m_nextFreeCommand[poolIndex] == currentPoolSize)
    {
	m_commands[poolIndex].resize(currentPoolSize + m_allocationChunkSize);
	m_commandPools[poolIndex]->Allocate(m_allocationChunkSize, m_commands[poolIndex].data() + currentPoolSize);
    }

    return m_commands[poolIndex][m_nextFreeCommand[poolIndex]++];
}

void CommandFramePool::Reset() noexcept
{
    for(size_t i = 0; i < 3; i++)
    {
	if(m_commandPools[i])
	{
	    m_commandPools[i]->Reset();
	}
	m_nextFreeCommand[i] = 0;
    }
}
