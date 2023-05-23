//
// Created by Ploxie on 2023-05-22.
//
#include "TLSFAllocator.h"

TLSFAllocator::TLSFAllocator(uint32_t memorySize, uint32_t pageSize)
    : m_memorySize(memorySize), m_pageSize(pageSize), m_firstLevelBitset(), m_smallBitset(), m_firstPhysicalChunk(nullptr), m_allocationCount(), m_freeSize(memorySize), m_usedSize(), m_requiredDebugChunkCount(1), m_chunkPool(256)
{
    memset(m_secondLevelBitsets, 0, sizeof(m_secondLevelBitsets));
    memset(m_freeChunks, 0, sizeof(m_freeChunks));
    memset(m_smallFreeChunks, 0, sizeof(m_smallFreeChunks));

    Chunk* chunk = m_chunkPool.Allocate();
    memset(chunk, 0, sizeof(Chunk));

    chunk->Size		 = m_memorySize;
    m_firstPhysicalChunk = chunk;
    AddChunkToFreeList(chunk);
}

bool TLSFAllocator::Allocate(uint32_t size, uint32_t alignment, uint32_t& chunkOffset, void*& backingChunk)
{
    ASSERT(size > 0);

    Chunk* freeChunk	   = nullptr;
    uint32_t alignedOffset = 0;

    for(int i = 0; i < 2; i++)
    {
	Chunk* chunk = FindFreeChunk(i == 0 ? size : size + alignment - 1);

	if(!chunk)
	{
	    return false;
	}

	alignedOffset = Util::AlignUp(chunk->Offset, alignment);

	ASSERT(i == 0 || alignedOffset + size <= chunk->Offset + chunk->Size);

	if(alignedOffset + size <= chunk->Offset + chunk->Size)
	{
	    freeChunk = chunk;
	    break;
	}
    }

    if(!freeChunk)
    {
	return false;
    }

    ASSERT(freeChunk);
    ASSERT(freeChunk->Size >= size);
    ASSERT(!freeChunk->Previous);
    ASSERT(!freeChunk->PreviousPhysical || freeChunk->PreviousPhysical->Offset + freeChunk->PreviousPhysical->Size == freeChunk->Offset);
    ASSERT(!freeChunk->NextPhysical || freeChunk->Offset + freeChunk->Size == freeChunk->NextPhysical->Offset);

    RemoveChunkFromFreeList(freeChunk);

    uint32_t nextLowerPageSizeOffset = Util::AlignDown(alignedOffset, m_pageSize);
    ASSERT(nextLowerPageSizeOffset <= alignedOffset);
    ASSERT(nextLowerPageSizeOffset >= freeChunk->Offset);

    uint32_t nextUpperPageSizeOffset = Util::AlignUp(alignedOffset + size, m_pageSize);
    ASSERT(nextUpperPageSizeOffset >= alignedOffset + size);
    ASSERT(nextUpperPageSizeOffset <= freeChunk->Offset + freeChunk->Size);

    const uint32_t beginMargin = nextLowerPageSizeOffset - freeChunk->Offset;
    const uint32_t endMargin   = freeChunk->Offset + freeChunk->Size - nextUpperPageSizeOffset;

    if(beginMargin >= m_pageSize)
    {
	Chunk* beginChunk = m_chunkPool.Allocate();
	memset(beginChunk, 0, sizeof(Chunk));

	beginChunk->PreviousPhysical = freeChunk->PreviousPhysical;
	beginChunk->NextPhysical     = freeChunk;
	beginChunk->Offset	     = freeChunk->Offset;
	beginChunk->Size	     = beginMargin;

	if(freeChunk->PreviousPhysical)
	{
	    ASSERT(freeChunk->PreviousPhysical->NextPhysical == freeChunk);
	    freeChunk->PreviousPhysical->NextPhysical = beginChunk;
	}
	else
	{
	    m_firstPhysicalChunk = beginChunk;
	}

	freeChunk->Offset += beginMargin;
	freeChunk->Size -= beginMargin;
	freeChunk->PreviousPhysical = beginChunk;

	AddChunkToFreeList(beginChunk);
	m_requiredDebugChunkCount++;
    }

    if(endMargin >= m_pageSize)
    {
	Chunk* endChunk = m_chunkPool.Allocate();
	memset(endChunk, 0, sizeof(Chunk));

	endChunk->PreviousPhysical = freeChunk;
	endChunk->NextPhysical	   = freeChunk->NextPhysical;
	endChunk->Offset	   = nextUpperPageSizeOffset;
	endChunk->Size		   = endMargin;

	if(freeChunk->NextPhysical)
	{
	    ASSERT(freeChunk->NextPhysical->PreviousPhysical == freeChunk);
	    freeChunk->NextPhysical->PreviousPhysical = endChunk;
	}

	freeChunk->NextPhysical = endChunk;
	freeChunk->Size -= endMargin;

	AddChunkToFreeList(endChunk);
	m_requiredDebugChunkCount++;
    }

    m_allocationCount++;

    chunkOffset	 = alignedOffset;
    backingChunk = freeChunk;

    freeChunk->UsedOffset = alignedOffset;
    freeChunk->UsedSize	  = size;

    m_freeSize -= freeChunk->Size;
    m_usedSize += freeChunk->UsedSize;
    m_requiredDebugChunkCount += freeChunk->UsedOffset > freeChunk->Offset ? 1 : 0;
    m_requiredDebugChunkCount += (freeChunk->Offset + freeChunk->Size) < (freeChunk->UsedOffset + freeChunk->UsedSize) ? 1 : 0;

#ifdef _DEBUG
    CheckIntegrity();
#endif

    return true;
}

void TLSFAllocator::GetFreeUsedWastedSizes(uint32_t& free, uint32_t& used, uint32_t& wasted) const
{
    free   = m_freeSize;
    used   = m_usedSize;
    wasted = m_memorySize - m_freeSize - m_usedSize;
}

void TLSFAllocator::AddChunkToFreeList(Chunk* chunk)
{
    Chunk** list = nullptr;

    if(chunk->Size < SMALL_BLOCK)
    {
	list = &m_smallFreeChunks[chunk->Size];
	m_smallBitset |= 1 << chunk->Size;
    }
    else
    {
	uint32_t firstLevelIndex  = 0;
	uint32_t secondLevelIndex = 0;
	MappingInsert(chunk->Size, firstLevelIndex, secondLevelIndex);

	ASSERT(firstLevelIndex < MAX_FIRST_LEVELS);
	ASSERT(secondLevelIndex < MAX_SECOND_LEVELS);

	list = &m_freeChunks[firstLevelIndex][secondLevelIndex];

	m_secondLevelBitsets[firstLevelIndex] |= 1 << secondLevelIndex;
	m_firstLevelBitset |= 1 << firstLevelIndex;
    }

    Chunk* previousHead = *list;
    *list		= chunk;

    chunk->Previous   = nullptr;
    chunk->Next	      = previousHead;
    chunk->UsedOffset = 0;
    chunk->UsedSize   = 0;
    if(previousHead)
    {
	ASSERT(!previousHead->Previous);
	previousHead->Previous = chunk;
    }
}

void TLSFAllocator::RemoveChunkFromFreeList(Chunk* chunk)
{
    if(chunk->Size < SMALL_BLOCK)
    {
	if(!chunk->Previous)
	{
	    ASSERT(chunk == m_smallFreeChunks[chunk->Size]);

	    m_smallFreeChunks[chunk->Size] = chunk->Next;

	    if(chunk->Next)
	    {
		chunk->Next->Previous = nullptr;
	    }
	    else
	    {
		m_smallBitset &= ~(1 << chunk->Size);
	    }
	}
	else
	{
	    chunk->Previous->Next = chunk->Next;

	    if(chunk->Next)
	    {
		chunk->Next->Previous = chunk->Previous;
	    }
	}
    }
    else
    {
	uint32_t firstLevelIndex  = 0;
	uint32_t secondLevelIndex = 0;
	MappingInsert(chunk->Size, firstLevelIndex, secondLevelIndex);

	ASSERT(firstLevelIndex < MAX_FIRST_LEVELS);
	ASSERT(secondLevelIndex < MAX_SECOND_LEVELS);

	if(!chunk->Previous)
	{
	    ASSERT(chunk == m_freeChunks[firstLevelIndex][secondLevelIndex]);

	    m_freeChunks[firstLevelIndex][secondLevelIndex] = chunk->Next;

	    if(chunk->Next)
	    {
		chunk->Next->Previous = nullptr;
	    }
	    else
	    {
		m_secondLevelBitsets[firstLevelIndex] &= ~(1 << secondLevelIndex);

		if(m_secondLevelBitsets[firstLevelIndex] == 0)
		{
		    m_firstLevelBitset &= ~(1 << firstLevelIndex);
		}
	    }
	}
	else
	{
	    chunk->Previous->Next = chunk->Next;
	    if(chunk->Next)
	    {
		chunk->Next->Previous = chunk->Previous;
	    }
	}
    }

    chunk->Next	      = nullptr;
    chunk->Previous   = nullptr;
    chunk->UsedOffset = chunk->Offset;
    chunk->UsedSize   = chunk->Size;
}

void TLSFAllocator::MappingInsert(uint32_t size, uint32_t& firstLevelIndex, uint32_t& secondLevelIndex)
{
    ASSERT(size >= SMALL_BLOCK);

    firstLevelIndex  = Util::FindLastSetBit(size);
    secondLevelIndex = (size >> (firstLevelIndex - MAX_LOG2_SECOND_LEVELS)) - MAX_SECOND_LEVELS;

    ASSERT(firstLevelIndex < MAX_FIRST_LEVELS);
    ASSERT(secondLevelIndex < MAX_SECOND_LEVELS);
}

void TLSFAllocator::MappingSearch(uint32_t size, uint32_t& firstLevelIndex, uint32_t& secondLevelIndex)
{
    ASSERT(size >= SMALL_BLOCK);

    uint32_t t = (1 << (Util::FindLastSetBit(size) - MAX_LOG2_SECOND_LEVELS)) - 1;
    size += t;
    firstLevelIndex  = Util::FindLastSetBit(size);
    secondLevelIndex = (size >> (firstLevelIndex - MAX_LOG2_SECOND_LEVELS)) - MAX_SECOND_LEVELS;
    size &= ~t;

    ASSERT(firstLevelIndex < MAX_FIRST_LEVELS);
    ASSERT(secondLevelIndex < MAX_SECOND_LEVELS);
}

bool TLSFAllocator::FindFreeChunk(uint32_t& firstLevelIndex, uint32_t& secondLevelIndex)
{
    ASSERT(firstLevelIndex < MAX_FIRST_LEVELS);
    ASSERT(secondLevelIndex < MAX_SECOND_LEVELS);

    uint32_t bitsetTemp = m_secondLevelBitsets[firstLevelIndex] & (~0u << secondLevelIndex);

    if(bitsetTemp)
    {
	secondLevelIndex = Util::FindFirstSetBit(bitsetTemp);
	return true;
    }

    firstLevelIndex = Util::FindFirstSetBit(m_firstLevelBitset & (~0u << (firstLevelIndex + 1)));
    if(firstLevelIndex != UINT32_MAX)
    {
	secondLevelIndex = Util::FindFirstSetBit(m_secondLevelBitsets[firstLevelIndex]);
	return true;
    }

    return false;
}

TLSFAllocator::Chunk* TLSFAllocator::FindFreeChunk(uint32_t size)
{
    Chunk* result = nullptr;

    if(size < SMALL_BLOCK && m_smallBitset != 0)
    {
	const uint32_t index = Util::FindFirstSetBit(m_smallBitset & (~0u << size));
	if(index != UINT32_MAX)
	{
	    result = m_smallFreeChunks[index];
	}
    }
    else
    {
	size = size < SMALL_BLOCK ? SMALL_BLOCK : size;

	uint32_t firstLevelIndex  = 0;
	uint32_t secondLevelIndex = 0;

	MappingSearch(size, firstLevelIndex, secondLevelIndex);

	for(int i = 0; i < 2; i++)
	{
	    if(i != 0)
	    {
		if(secondLevelIndex == 0)
		{
		    secondLevelIndex = 31;
		    firstLevelIndex--;
		}
		else
		{
		    secondLevelIndex--;
		}
	    }

	    uint32_t tempFirstLevelIndex  = firstLevelIndex;
	    uint32_t tempSecondLevelIndex = secondLevelIndex;

	    if(FindFreeChunk(tempFirstLevelIndex, tempSecondLevelIndex))
	    {
		Chunk* chunk = m_freeChunks[tempFirstLevelIndex][tempSecondLevelIndex];

		if(chunk->Size >= size)
		{
		    result = chunk;
		}
	    }
	}
    }

    return result;
}

void TLSFAllocator::CheckIntegrity()
{
    Chunk* chunk	   = m_firstPhysicalChunk;
    uint32_t currentOffset = 0;
    uint32_t free	   = 0;
    uint32_t used	   = 0;
    uint32_t wasted	   = 0;
    while(chunk)
    {
	ASSERT(chunk->Offset == currentOffset);
	currentOffset += chunk->Size;
	free += chunk->UsedSize == 0 ? chunk->Size : 0;
	used += chunk->UsedSize;
	wasted += chunk->UsedSize == 0 ? 0 : chunk->Size - chunk->UsedSize;
	chunk = chunk->NextPhysical;
    }

    ASSERT(currentOffset == m_memorySize);
    ASSERT(free == m_freeSize);
    ASSERT(used == m_usedSize);
    ASSERT(wasted == (m_memorySize - m_freeSize - m_usedSize));
    ASSERT((free + used + wasted) == m_memorySize);
}