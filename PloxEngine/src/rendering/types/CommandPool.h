//
// Created by Ploxie on 2023-05-19.
//

#pragma once

#include "Command.h"

class CommandPool
{
public:
    virtual ~CommandPool()					  = default;
    virtual void* GetNativeHandle() const			  = 0;
    virtual void Allocate(uint32_t count, Command** commandLists) = 0;
    virtual void Free(uint32_t count, Command** pCommandList)	  = 0;
    virtual void Reset()					  = 0;
};