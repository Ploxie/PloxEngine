//
// Created by Ploxie on 2023-03-21.
//

#pragma once
#include <cstdint>
#include "eastl/vector.h"

using Handle = uint32_t;

class HandleManager
{
public:
	explicit HandleManager(Handle maxHandle = UINT32_MAX) noexcept;

	Handle Allocate(bool transient = false) noexcept;
	void Free(Handle handle) noexcept;
	void FreeTransientHandles() noexcept;
	bool IsValidHandle(Handle handle) const noexcept;

private:
	eastl::vector<Handle> m_freeHandles;
	eastl::vector<Handle> m_transientHandles;
	Handle m_nextFreeHandle = 1;
	Handle m_maxHandle = UINT32_MAX;
};