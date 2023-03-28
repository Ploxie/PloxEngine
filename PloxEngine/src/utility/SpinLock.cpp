//
// Created by Ploxie on 2023-03-28.
//

#include "SpinLock.h"
#include <thread>
#include <random>
#include <stdint.h>

static constexpr size_t SPINS_BEFORE_CPU_YIELD = 64;
static constexpr size_t SPINS_BEFORE_CPU_RELAX = 0;
static constexpr size_t LOCK_CONGESTION_MAX_CPU_RELAX_CYCLES = 65536;

void SpinLock::Lock() noexcept
{
	static thread_local std::default_random_engine e;

	size_t collisions = 0;

	while(true)
	{
		size_t retries = 0;
		while(m_state.test(eastl::memory_order_relaxed))
		{
			retries++;

			if(retries >= SPINS_BEFORE_CPU_YIELD)
			{
				std::this_thread::yield();
			}
			else if(retries >= SPINS_BEFORE_CPU_RELAX)
			{
				eastl::cpu_pause();
			}
		}

		if(!m_state.test_and_set(eastl::memory_order_acquire))
		{
			return;
		}

		collisions++;
		auto maxRelaxCycles = static_cast<uint32_t>min((size_t)1U << collisions, LOCK_CONGESTION_MAX_CPU_RELAX_CYCLES);
		std::uniform_int_distribution<uint32_t> d(1, maxRelaxCycles);
		uint32_t randomRelaxCycles = d(e);

		for(uint32_t i = 0; i < randomRelaxCycles; i++)
		{
			eastl::cpu_pause();
		}
	}
}

bool SpinLock::TryLock() noexcept
{
	return !m_state.test_and_set(eastl::memory_order_acquire);
}

bool SpinLock::Status() noexcept
{
	return m_state.test(eastl::memory_order_acquire);
}

void SpinLock::Unlock() noexcept
{
	m_state.clear(eastl::memory_order_release);
}

SpinLockHolder::SpinLockHolder(SpinLock& lock)
	:m_lock(lock)
{
	m_lock.Lock();
}

SpinLockHolder::~SpinLockHolder()
{
	m_lock.Unlock();
}