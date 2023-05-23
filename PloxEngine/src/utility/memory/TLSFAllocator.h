//
// Created by Ploxie on 2023-05-22.
//

#pragma once
#include "utility/ObjectPool.h"

struct TLSFChunkDebugInfo
{
    enum class State
    {
	FREE,
	USED,
	WASTED
    };
    uint32_t Offset;
    uint32_t Size;
    State State;
};

class TLSFAllocator
{
public:
    explicit TLSFAllocator(uint32_t memorySize, uint32_t pageSize);
    bool Allocate(uint32_t size, uint32_t alignment, uint32_t& chunkOffset, void*& backingChunk);
    void Free(void* backingChunk);
    void GetFreeUsedWastedSizes(uint32_t& free, uint32_t& used, uint32_t& wasted) const;

private:
    struct Chunk;
    void AddChunkToFreeList(Chunk* chunk);
    void RemoveChunkFromFreeList(Chunk* chunk);
    static void MappingInsert(uint32_t size, uint32_t& firstLevelIndex, uint32_t& secondLevelIndex);
    static void MappingSearch(uint32_t size, uint32_t& firstLevelIndex, uint32_t& secondLevelIndex);
    bool FindFreeChunk(uint32_t& firstLevelIndex, uint32_t& secondLevelIndex);
    Chunk* FindFreeChunk(uint32_t size);
    void CheckIntegrity();

private:
    enum
    {
	MAX_FIRST_LEVELS       = 32,
	MAX_LOG2_SECOND_LEVELS = 5,
	MAX_SECOND_LEVELS      = 1 << MAX_LOG2_SECOND_LEVELS,
	SMALL_BLOCK	       = MAX_FIRST_LEVELS
    };

    struct Chunk
    {
	Chunk* Previous;
	Chunk* Next;
	Chunk* PreviousPhysical;
	Chunk* NextPhysical;
	uint32_t Offset;
	uint32_t Size;
	uint32_t UsedOffset;
	uint32_t UsedSize;
    };

    const uint32_t m_memorySize;
    const uint32_t m_pageSize;
    uint32_t m_firstLevelBitset;
    uint32_t m_secondLevelBitsets[MAX_FIRST_LEVELS];
    uint32_t m_smallBitset;
    Chunk* m_freeChunks[MAX_FIRST_LEVELS][MAX_SECOND_LEVELS];
    Chunk* m_smallFreeChunks[32];
    Chunk* m_firstPhysicalChunk;
    uint32_t m_allocationCount;
    uint32_t m_freeSize;
    uint32_t m_usedSize;
    uint32_t m_requiredDebugChunkCount;
    DynamicObjectPool<Chunk> m_chunkPool;
};