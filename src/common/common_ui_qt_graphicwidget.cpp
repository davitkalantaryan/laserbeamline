

// common_ui_qt_graphicwidget
// 2017 Jul 04

#include "common_ui_qt_graphicwidget.hpp"
#include <QPaintEvent>
#include <stddef.h>
#include <QResizeEvent>
#include <QPainter>

#define RIGHT_MARGIN    1
#define BOTTOM_MARGIN   5
#define UPPER_MARGIN    5

const int g_nDoubleLusancq = BOTTOM_MARGIN+UPPER_MARGIN;

common::ui::qt::GraphicWidget::GraphicWidget()
{
    m_pPoints = NULL;
    m_nShowEndingPlus1 = m_nShowStarting =
            m_nNumberOfPointsToShow2 = m_nNumberOfPointsTotal = 0;
    m_bLastIsFromPersistantMemory = false;

    connect(this,SIGNAL(DoUpdateSignal()),this,SLOT(update()));
}


common::ui::qt::GraphicWidget::~GraphicWidget()
{
    free(m_pPoints);
}


void common::ui::qt::GraphicWidget::paintEvent(QPaintEvent* a_pPaintEvent)
{
    QPainter aPainter(this);

    QWidget::paintEvent(a_pPaintEvent);
    aPainter.drawPolyline(m_pPoints+m_nShowStarting,m_nNumberOfPointsToShow2);
}



void common::ui::qt::GraphicWidget::resizeEvent(QResizeEvent* a_pResizeEvent)
{
    QWidget::resizeEvent(a_pResizeEvent);
    //m_sizeThis = a_pResizeEvent->size();
    //m_rectThis = rect();
    RearrangeX(a_pResizeEvent->size());
}


void common::ui::qt::GraphicWidget::RearrangeX(const QSize& a_size)
{
    double lfInterval =
            ((double)(a_size.width()-(LEFT_MARGIN+RIGHT_MARGIN)))
                                /((double)m_nNumberOfPointsToShow2);
    double lfX = (double)LEFT_MARGIN;
    //int m_nShowEndingPlus1(m_nShowStarting+m_nNumberOfPointsToShow2);

    for(int i(m_nShowStarting);i<m_nShowEndingPlus1;++i,lfX += lfInterval)
    {
        m_pPoints[i].setX((int)lfX);
    }

}
