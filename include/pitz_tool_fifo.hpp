/*
 *	File: pitz_tool_fifo.hpp
 *
 *	Created on: 04 Feb 2017
 *	Created by: Davit Kalantaryan (Email: davit.kalantaryan@desy.de)
 *
 *  This file implements ...
 *
 */
#ifndef PITZ_TOOL_FIFO_HPP
#define PITZ_TOOL_FIFO_HPP

//#define USE_FIFO_SIMPLE

#include <mutex>
#include <stddef.h>

namespace pitz { namespace tools{

template <typename TypeData>
class FiFo
{
public:
    FiFo();
    virtual ~FiFo();

    /*
     *  return
     *      NULL     -> there is no any task to handle
     *      non NULL -> pointer to tast to fullfill
     */
    bool GetFirstData(TypeData* firstTaskBuffer);
    void AddNewData(const TypeData& a_inp);

protected:
    template <typename TypeDataStr>
    struct fifoListItem{fifoListItem(const TypeDataStr& a_data):next(NULL), data(a_data){}
        struct fifoListItem*    next;
        TypeDataStr             data;
    };

protected:
    fifoListItem<TypeData>   m_InitialTaskBuffer;
    fifoListItem<TypeData>*  m_pFirstTask;
    fifoListItem<TypeData>*  m_pLastTask;
    std::mutex               m_task_mutex;
};

}}


#include "pitz_tool_fifo.tos"

#endif // DECENT_TOOL_FIFO_HPP
