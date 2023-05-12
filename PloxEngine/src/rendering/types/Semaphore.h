//
// Created by Ploxie on 2023-05-11.
//

#pragma once
#include <cstdint>

class Semaphore
{
public:
    virtual ~Semaphore()			    = default;
    virtual void* GetNativeHandle() const	    = 0;
    virtual uint64_t GetCompletedValue() const	    = 0;
    virtual void Wait(uint64_t waitValue) const	    = 0;
    virtual void Signal(uint64_t signalValue) const = 0;
};