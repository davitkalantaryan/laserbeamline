
// common_ui_qt_graphicwidget.impl.hpp
// 2017 Jul 04

#ifndef COMMON_UI_QT_GRAPHICWIDGET_IMPL_HPP
#define COMMON_UI_QT_GRAPHICWIDGET_IMPL_HPP

#ifndef COMMON_UI_QT_GRAPHICWIDGET_HPP
#error do not include directly
#include "common_ui_qt_graphicwidget.hpp"
#endif

#include <malloc.h>
#include <QCoreApplication>
#include <QPaintEvent>

#define LEFT_MARGIN     1
extern const int g_nDoubleLusancq;

template <typename TypeSpectr>
int common::ui::qt::GraphicWidget::SetSpectrum(
        TypeSpectr* a_spectr, int a_nNumberOfElements,
        const bool* a_fromPersistantMemory)
{
    QSize aSize(size());
    if(a_fromPersistantMemory){m_bLastIsFromPersistantMemory = *a_fromPersistantMemory;}
    if(a_nNumberOfElements>m_nNumberOfPointsTotal){
        m_pPoints=(QPoint*)realloc(m_pPoints,a_nNumberOfElements*sizeof(QPoint));
        if(!m_pPoints){
            m_nShowStarting = m_nNumberOfPointsToShow2 = m_nNumberOfPointsTotal = 0;
            m_bLastIsFromPersistantMemory = false;
            return -1;
        }//if(!m_pPoints){

        if(m_nShowEndingPlus1==0){m_nNumberOfPointsToShow2=m_nShowEndingPlus1=a_nNumberOfElements;}

    }//if(a_nNumberOfElements>m_nNumberOfPointsTotal){
    else if(m_nShowEndingPlus1>a_nNumberOfElements){
        if(m_nShowStarting>=a_nNumberOfElements){m_nShowStarting=a_nNumberOfElements-1;}
        m_nShowEndingPlus1 = a_nNumberOfElements;
    }
    m_nNumberOfPointsTotal=a_nNumberOfElements;
    RearrangeX(aSize);
    SetSpectrumPrivate(a_spectr,aSize);

    return 0;
}


template <typename TypeSpectr>
void common::ui::qt::GraphicWidget::SetSpectrumPrivate(TypeSpectr* a_scectr, const QSize& a_size)
{
    double lfY;
    int i(m_nShowStarting);
    int heigth = a_size.height();
    TypeSpectr tSpMin(a_scectr[m_nShowStarting]);
    TypeSpectr tSpMax(a_scectr[m_nShowStarting]);

    for(;i<m_nShowEndingPlus1;++i)
    {
        if(a_scectr[i]<tSpMin){tSpMin=a_scectr[i];}
        if(a_scectr[i]>tSpMax){tSpMax=a_scectr[i];}
    }

    m_lfKoefY =
            ((double)(heigth-g_nDoubleLusancq))/
            ((double)(tSpMax-tSpMin));

    for(i=m_nShowStarting; i<m_nShowEndingPlus1;++i)
    {
        lfY = (double)(tSpMax - a_scectr[i]);
        m_pPoints[i].setY(LEFT_MARGIN+(int)(lfY*m_lfKoefY));
    }

    //update();
    //QPaintEvent* pEvent = new QPaintEvent(m_rectThis);
    //QCoreApplication::postEvent(this,pEvent);
    emit DoUpdateSignal();
}

#endif // COMMON_UI_QT_GRAPHICWIDGET_IMPL_HPP
