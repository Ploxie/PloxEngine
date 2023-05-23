//
// Created by Ploxie on 2023-05-23.
//

#pragma once

class Sampler
{
public:
    virtual ~Sampler()			  = default;
    virtual void* GetNativeHandle() const = 0;
};
