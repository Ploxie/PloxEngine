//
// Created by Ploxie on 2023-05-10.
//
#pragma once
#include "Barrier.h"
#include <cstdint>

class Semaphore;
class Command;

struct SubmitInfo
{
    uint32_t WaitSemaphoreCount;
    const Semaphore* const* WaitSemaphores;
    const uint64_t* WaitValues;
    const PipelineStageFlags* WaitDstStageMask;
    uint32_t CommandCount;
    const Command* const* Commands;
    uint32_t SignalSemaphoreCount;
    const Semaphore* const* SignalSemaphores;
    const uint64_t* SignalValues;
};

enum class QueueType
{
    GRAPHICS,
    COMPUTE,
    TRANSFER
};

class Queue
{
public:
    virtual ~Queue()						      = default;
    virtual void* GetNativeHandle() const			      = 0;
    virtual QueueType GetQueueType() const			      = 0;
    virtual unsigned int GetTimestampValidBits() const		      = 0;
    virtual float GetTimestampPeriod() const			      = 0;
    virtual bool CanPresent() const				      = 0;
    virtual void Submit(uint32_t count, const SubmitInfo* submitInfo) = 0;
};
