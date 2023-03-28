//
// Created by Ploxie on 2023-03-21.
//

#include "HandleManager.h"
#include <algorithm>
#include <cassert>

HandleManager::HandleManager(uint32_t maxHandle) noexcept : m_maxHandle(maxHandle)
{ }

uint32_t HandleManager::Allocate(bool transient) noexcept
{
	uint32_t result = 0;
	if(!m_freeHandles.empty())
	{
		result = m_freeHandles.back();
		m_freeHandles.pop_back();
	}
	else if(m_nextFreeHandle <= m_maxHandle)
	{
		result = m_nextFreeHandle++;
	}

	if(transient && result != 0)
	{
		m_transientHandles.push_back(result);
	}
	return result;
}

void HandleManager::Free(uint32_t handle) noexcept
{
	if(handle != 0)
	{
		assert(IsValidHandle(handle));
		m_freeHandles.push_back(handle);
	}
}

void HandleManager::FreeTransientHandles() noexcept
{
	if(!m_transientHandles.empty())
	{
		m_freeHandles.insert(m_freeHandles.end(), m_transientHandles.begin(), m_transientHandles.end());
		m_transientHandles.clear();
	}
}

bool HandleManager::IsValidHandle(uint32_t handle) const noexcept
{
	if(handle == 0)
	{
		return false;
	}

	if(handle >= m_nextFreeHandle)
	{
		return false;
	}

	if(std::find(m_freeHandles.begin(), m_freeHandles.end(), handle) != m_freeHandles.end())
	{
		return false;
	}

	if(std::find(m_transientHandles.begin(), m_transientHandles.end(), handle) != m_transientHandles.end())
	{
		return true;
	}

	return true;
}
