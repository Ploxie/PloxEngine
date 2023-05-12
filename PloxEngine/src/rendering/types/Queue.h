//
// Created by Ploxie on 2023-05-10.
//
#pragma once

class Queue
{
public:
    enum class QueueType
    {
	GRAPHICS,
	COMPUTE,
	TRANSFER
    };

public:
    virtual ~Queue()				       = default;
    virtual void* GetNativeHandle() const	       = 0;
    virtual QueueType GetQueueType() const	       = 0;
    virtual unsigned int GetTimestampValidBits() const = 0;
    virtual float GetTimestampPeriod() const	       = 0;
    virtual bool CanPresent() const		       = 0;
};
