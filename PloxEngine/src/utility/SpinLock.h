//
// Created by Ploxie on 2023-03-28.
//

#pragma once
#include "eastl/atomic.h"

class SpinLock
{
public:
	void Lock() noexcept;
	bool TryLock() noexcept;
	bool Status() noexcept;
	void Unlock() noexcept;
private:
	alignas(128) eastl::atomic_flag m_state = false;
};

class SpinLockHolder
{
public:
	explicit SpinLockHolder(SpinLock& lock);
	SpinLockHolder(const SpinLockHolder&) = delete;
	SpinLockHolder(SpinLockHolder&&) = delete;
	SpinLockHolder& operator=(const SpinLockHolder&) = delete;
	SpinLockHolder& operator=(const SpinLockHolder&&) = delete;
	~SpinLockHolder();
private:
	SpinLock& m_lock;
};
