
// common_iodevice.hpp
// 2017 Jul 6

#ifndef __common_iodevice_hpp__
#define __common_iodevice_hpp__

#include <stddef.h>

namespace common{


class IODevice
{
public:
	IODevice();
	IODevice(const common::IODevice& cM);
	virtual ~IODevice();

	virtual bool	isOpenC(void)const { return false; }
	virtual void	closeC(void);

	virtual int		readC (void* buffer, int bufferLen, int timeoutMS)const = 0;
	virtual int		readC(void* buffer, int bufferLen)const;
	virtual int		writeC(const void* buffer, int bufferLen)=0;

	common::IODevice& operator=(const common::IODevice& aM);
	virtual common::IODevice* Clone()const;

protected:
	virtual void	closeHard(void) {}
	virtual void	cloneFromOther(const common::IODevice& other);

protected:
	const IODevice*		m_pPrev;
	mutable IODevice*	m_pNext;
};

};


#endif  // #ifndef __common_iodevice_hpp__
