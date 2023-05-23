//
// Created by Ploxie on 2023-05-23.
//

#pragma once

#include "Format.h"
#include <cstdint>
class Buffer;

struct BufferViewCreateInfo
{
    Buffer* Buffer;
    Format Format;
    uint64_t Offset;
    uint64_t Range;
};

class BufferView
{
public:
    virtual ~BufferView()				       = default;
    virtual void* GetNativeHandle() const		       = 0;
    virtual const Buffer* GetBuffer() const		       = 0;
    virtual const BufferViewCreateInfo& GetDescription() const = 0;
};