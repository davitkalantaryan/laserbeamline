
// common_iodevice.cpp
// 2017 Jul 6

#include <common/io/device.hpp>

common::io::Device::Device()
	:
	m_pPrev(nullptr),
	m_pNext(nullptr)
{
}


common::io::Device::Device(const Device& a_cM)
{
	cloneFromOther(a_cM);
}


common::io::Device::~Device()
{
	closeC();
}


void common::io::Device::closeC(void)
{
	if(isOpenC()){
		if (!m_pPrev && !m_pNext) {
			this->closeHard();
		}
		else {
			if (m_pPrev) { m_pPrev->m_pNext = m_pNext; }
			if (m_pNext) { m_pNext->m_pPrev = m_pPrev; }
		}
		m_pPrev = m_pNext = NULL;
	}
}


common::io::Device& common::io::Device::operator=(const Device& a_cM)
{
	cloneFromOther(a_cM);
	return *this;
}


void common::io::Device::cloneFromOther(const Device& a_cM)
{
	if (a_cM.isOpenC()) {
		Device* pNext = a_cM.m_pNext;
		a_cM.m_pNext = this;
		m_pNext = pNext;
		if (pNext) { pNext->m_pPrev = this; }
		m_pPrev = &a_cM;
	}
}


common::io::Device* common::io::Device::Clone()const
{
	return nullptr;
}
