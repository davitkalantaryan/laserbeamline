/*
 *	File: common_fifofast.impl.hpp
 *
 *	Created on: Jun 15, 2016
 *	Author    : Davit Kalantaryan (Email: davit.kalantaryan@desy.de)
 *
 *
 */

#ifndef __fifofast_impl_hpp__
#define __fifofast_impl_hpp__

#ifndef __COMMON_FIFOFAST_HPP__
#error This file should be included only from fifofast.h
#include "common_fifofast.hpp"
#else
#endif  // #ifndef __COMMON_FIFOFAST_HPP__


//////////////////////////////////////////////////////////////
////////// class FifoFast_base
//////////////////////////////////////////////////////////////
template <class Type>
common::FifoFast_base<Type>::FifoFast_base(int a_nMaxSize, int a_nCashSize, SListStr<Type>**a_ppCashedEntries)
	:	m_cnMaxSize(a_nMaxSize),
		m_cnCashSize(a_nCashSize),
		m_nIndexInCashPlus1(a_nCashSize),
		m_nNumOfElemets(0),
		m_Mutex(),
		m_pFirst(NULL),
		m_pLast(NULL),
		m_ppCashedEntries(a_ppCashedEntries)
{
	for (int i(0); i < a_nCashSize; ++i)
	{
		m_ppCashedEntries[i] = new SListStr<Type>;
	}
}


template <class Type>
common::FifoFast_base<Type>::~FifoFast_base()
{
	SListStr<Type> *pToDel;

	for (int i(0); i < m_nIndexInCashPlus1; ++i)
	{
		delete m_ppCashedEntries[i];
	}

	while (m_pFirst)
	{
		pToDel = m_pFirst->m_pNext;
		delete m_pFirst;
		m_pFirst = pToDel;
	}
}


template <class Type>
bool common::FifoFast_base<Type>::AddElement(const Type& a_ptNew)
{
	bool bRet(true);
	SListStr<Type>* pNewEntr;

        m_Mutex.lock();
	if (m_nNumOfElemets<m_cnMaxSize)
	{
		pNewEntr = LIKELY(m_nIndexInCashPlus1) ? m_ppCashedEntries[--m_nIndexInCashPlus1] : new SListStr < Type >;
		pNewEntr->m_tValue = a_ptNew;pNewEntr->m_pNext = NULL;

		if (!m_nNumOfElemets++) { m_pLast = m_pFirst = pNewEntr; }
		else { m_pLast->m_pNext = pNewEntr; m_pLast = pNewEntr; }
	}
	else
	{
		bRet = false;
	}
        m_Mutex.unlock();

	return bRet;
}

template <class Type>
bool common::FifoFast_base<Type>::Extract(Type*const& a_ptBuf)
{
	bool bRet(true);

        m_Mutex.lock();
	if (m_nNumOfElemets)
	{
		*a_ptBuf = m_pFirst->m_tValue;
		if (LIKELY(m_nNumOfElemets <= m_cnCashSize))
		{
			m_ppCashedEntries[m_nIndexInCashPlus1++] = m_pFirst;
			m_pFirst = m_pFirst->m_pNext;
		}
		else
		{
			SListStr<Type>* pForDelete = m_pFirst;
			m_pFirst = m_pFirst->m_pNext;
			delete pForDelete;
		}

		--m_nNumOfElemets;
	}
	else
	{
		bRet = false;
	}
        m_Mutex.unlock();
	return bRet;
}


template <class Type>
int common::FifoFast_base<Type>::size()const
{
	int nRet;
        m_Mutex.lock();
	nRet = m_nNumOfElemets;
        m_Mutex.unlock();
	return nRet;
}
//////////////////////////////////////////////////////////////
////////// end class FifoFast_base
//////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////
////////// class FifoFastDyn
//////////////////////////////////////////////////////////////
template <class Type>
common::FifoFastDyn<Type>::FifoFastDyn(int a_nMaxSize, int a_nCashSize)
	:	FifoFast_base<Type>(a_nMaxSize, a_nCashSize, 
                                (SListStr<Type>**)malloc(sizeof(SListStr<Type>*)*a_nCashSize))
{
	if (!FifoFast_base<Type>::m_ppCashedEntries) throw "Low memory!";
}


template <class Type>
common::FifoFastDyn<Type>::~FifoFastDyn()
{
	free(FifoFast_base<Type>::m_ppCashedEntries);
}
//////////////////////////////////////////////////////////////
////////// end class FifoFastDyn
//////////////////////////////////////////////////////////////



#endif  // #ifndef __fifofast_tos__
