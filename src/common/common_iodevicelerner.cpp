
// common_iodevicelerner.cpp
// 2017 Jul 21

#include "common_iodevicelerner.hpp"
#include <malloc.h>
#include <memory.h>

common::IoDeviceLerner::IoDeviceLerner(::common::IODevice* a_pDevice, uint32_t  a_unTableSize)
	:
	m_table(a_unTableSize),
	m_lastUpdate(1),
	m_pDevice(a_pDevice)
{
	m_nTimeoutMsMax = 100;
	m_nTimeoutMsMax = 10;
	m_nTimeoutMsStep = 10;
}


common::IoDeviceLerner::~IoDeviceLerner()
{
}


void common::IoDeviceLerner::SetAndGet(const ::common::SMem& a_input, ::common::SMem* a_pOutput, int* a_pnSet, int* a_pnGet)
{
	SEntry* pEntry;

	if(m_table.FindEntry(a_input.mem,a_input.size,&pEntry)){
		if (pEntry->lastUpdate == m_lastUpdate){
			*a_pOutput = pEntry->output;
			return;
		}	
	}
	else{
		pEntry = new SEntry(a_input,m_nTimeoutMsMax);
		if(!pEntry){throw "Low memory!";}
		m_table.AddEntry(a_input.mem, a_input.size, pEntry);
	}
	*a_pnSet = m_pDevice->writeC(a_input.mem, a_input.size);
	*a_pnGet = m_pDevice->readC(a_pOutput->mem, a_pOutput->size,pEntry->timeoutMs);

	// recalculate timeout
	if(*a_pnGet>0){
		pEntry->timeoutMs = m_nTimeoutMsMax;
	}
	else {
		int32_t timeoutMs = pEntry->timeoutMs-m_nTimeoutMsStep;
		if(timeoutMs<m_nTimeoutMsMin){timeoutMs=m_nTimeoutMsMin;}
		pEntry->timeoutMs=timeoutMs;
	}
}


void common::IoDeviceLerner::SetTimeoutMaxAndMinAndStep(int32_t a_timeoutMax, int32_t a_timeoutMin, int32_t a_timeoutStep)
{
	m_nTimeoutMsMax = a_timeoutMax;
	m_nTimeoutMsMax = a_timeoutMin;
	m_nTimeoutMsStep = a_timeoutStep;
}


/*////////////////////////////////////////////////////////////*/
common::IoDeviceLerner::SEntry::SEntry(const ::common::SMem& a_input, int32_t a_timeoutMs)
	: input(a_input), lastUpdate(0),makesDirty(0),timeoutMs(a_timeoutMs)
{
}


common::IoDeviceLerner::SEntry::~SEntry()
{
}


/*////////////////////////////////////////////////////////////*/
common::SMem::SMem() : mem(NULL), size(0),allocedSize(0)
{
}


common::SMem::SMem(const ::common::SMem& a_am)
{
	mem = realloc(mem, a_am.size);
	if (!mem) { throw "Low memory!"; }
	allocedSize = a_am.size;
	memcpy(mem, a_am.mem, a_am.size);
	size = a_am.size;
}


common::SMem::~SMem()
{
}


::common::SMem& common::SMem::operator=(const ::common::SMem& a_am)
{
	if(allocedSize<a_am.size){
		mem = realloc(mem, a_am.size);
		if(!mem){throw "Low memory!";}
		allocedSize = a_am.size;
	}

	memcpy(mem, a_am.mem, a_am.size);
	size = a_am.size;
	return *this;
}


int common::SMem::SetMemory(const void* a_mem, size_t a_size)
{
	if (allocedSize<a_size) {
		mem = realloc(mem, a_size);
		if (!mem) { return -1; }
		allocedSize = a_size;
	}

	memcpy(mem, a_mem, a_size);
	size = a_size;
	return 0;
}
