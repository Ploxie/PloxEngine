//
// Created by Ploxie on 2023-03-21.
//

#pragma once
#include <cstdint>
#include "eastl/vector.h"

class HandleManager
{
public:
	explicit HandleManager(uint32_t maxHandle = UINT32_MAX) noexcept;

	uint32_t Allocate(bool transient = false) noexcept;
	void Free(uint32_t handle) noexcept;
	void FreeTransientHandles() noexcept;
	bool IsValidHandle(uint32_t handle) const noexcept;

private:
	eastl::vector<uint32_t> m_freeHandles;
	eastl::vector<uint32_t> m_transientHandles;
	uint32_t m_nextFreeHandle = 1;
	uint32_t m_maxHandle = UINT32_MAX;
};