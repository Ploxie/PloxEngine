//
// Created by Ploxie on 2023-05-11.
//

#pragma once
#include <cstdint>

struct Extent2D
{
    uint32_t Width;
    uint32_t Height;
};

struct Extent3D
{
    uint32_t Width;
    uint32_t Height;
    uint32_t Depth;
};

struct Offset2D
{
    int32_t X;
    int32_t Y;
};

struct Offset3D
{
    int32_t X;
    int32_t Y;
    int32_t Z;
};