
// common_iodevicelerner.hpp
// 2017 Jul 21

#ifndef __common_iodevicelerner_hpp__
#define __common_iodevicelerner_hpp__

#include "common_iodevice.hpp"
#include <stdint.h>
#include "common_hashtbl.hpp"

namespace common{

struct SMem{
	void* mem; size_t size, allocedSize;
	SMem(); SMem(const ::common::SMem& cM); ~SMem();
	::common::SMem& operator=(const ::common::SMem& am);
	int SetMemory(const void* mem, size_t size);
};

class IoDeviceLerner
{
public:
	IoDeviceLerner(IODevice* device,uint32_t unSize=1024);
	virtual ~IoDeviceLerner();

	void SetAndGet(const ::common::SMem& input, ::common::SMem* output,int* pSet, int* pGet);
	void SetTimeoutMaxAndMinAndStep(int32_t timeoutMax,int32_t timeoutMin, int32_t timeoutStep);

protected:
	struct SEntry {
		SEntry(const ::common::SMem& input, int32_t timeoutMs); ~SEntry();
		SMem input, output; uint64_t lastUpdate; int32_t makesDirty, timeoutMs;
	};

protected:
	HashTbl<SEntry*>		m_table;
	uint64_t				m_lastUpdate;
	IODevice*				m_pDevice;
	int32_t					m_nTimeoutMsMax;
	int32_t					m_nTimeoutMsMin;
	int32_t					m_nTimeoutMsStep;
};

}


#endif  // #ifndef __common_iodevicelerner_hpp__
