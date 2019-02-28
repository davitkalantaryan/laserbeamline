
// common_iodevice.hpp
// 2017 Jul 6

#ifndef __common_iodevice_hpp__
#define __common_iodevice_hpp__

#if defined(_MSC_VER) & (_MSC_VER>=1913)
#ifndef CINTERFACE
#define CINTERFACE
#endif
#endif

#include <stddef.h>
#include <stdint.h>

namespace common{ namespace io{


class Device
{
public:
	Device();
	Device(const Device& cM);
	virtual ~Device();

	virtual bool		isOpenC(void)const { return false; }
	virtual void		closeC(void);

	virtual int			readC(void* buffer, int bufferLen)const=0;
	virtual int			writeC(const void* buffer, int bufferLen)=0;

	virtual int			setTimeout(int timeoutMS) = 0;
	virtual ptrdiff_t	handle()const=0;

	Device&				operator=(const Device& aM);
	virtual				Device* Clone()const;

protected:
	virtual void		closeHard(void) {}
	virtual void		cloneFromOther(const Device& other);

protected:
	const Device*		m_pPrev;
	mutable Device*		m_pNext;
};

}}  // namespace common{ namespace io{


#endif  // #ifndef __common_iodevice_hpp__
