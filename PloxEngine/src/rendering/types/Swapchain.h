//
// Created by Ploxie on 2023-05-11.
//

#pragma once
#include "Extent2D.h"
#include "Format.h"
#include "Image.h"
#include "Queue.h"
#include "Semaphore.h"

enum class PresentMode
{
    IMMEDIATE,
    V_SYNC,
};

class Swapchain
{
public:
    virtual ~Swapchain()									   = default;
    virtual void* GetNativeHandle() const							   = 0;
    virtual void Resize(uint32_t width, uint32_t height, bool fullscreen, PresentMode presentMode) = 0;
    virtual unsigned int GetCurrentImageIndex()							   = 0;
    virtual void Present(Semaphore* waitSemaphore, uint64_t semaphoreWaitValue, Semaphore* signalSemaphore,
			 uint64_t semaphoreSignalValue)						   = 0;
    virtual Extent2D GetExtent() const								   = 0;
    virtual Extent2D GetRecreationExtent() const						   = 0;
    virtual Format GetImageFormat() const							   = 0;
    virtual Image* GetImage(uint32_t index) const						   = 0;
    virtual Queue* GetPresentQueue() const							   = 0;
};