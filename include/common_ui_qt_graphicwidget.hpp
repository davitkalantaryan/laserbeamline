
// common_ui_qt_graphicwidget
// 2017 Jul 04

#ifndef COMMON_UI_QT_GRAPHICWIDGET_HPP
#define COMMON_UI_QT_GRAPHICWIDGET_HPP

#include <QWidget>

namespace common{ namespace ui{ namespace qt{

class GraphicWidget : public QWidget
{
    Q_OBJECT
public:
    GraphicWidget();
    virtual ~GraphicWidget();

    template <typename TypeSpectr>
    int SetSpectrum(TypeSpectr* spectrum, int numberOfElements,
                     const bool* fromPersistantMemory);

protected:
    virtual void paintEvent( QPaintEvent* );
    virtual void resizeEvent ( QResizeEvent * event );

protected:
signals:
    void DoUpdateSignal();

protected:
    void RearrangeX(const QSize& a_size);
    template <typename TypeSpectr>
    void SetSpectrumPrivate(TypeSpectr* spectrum, const QSize& size);

protected:
    QPoint*             m_pPoints;
    //QSize               m_sizeThis;
    //QRect               m_rectThis;

    int                 m_nNumberOfPointsTotal;
    int                 m_nNumberOfPointsToShow2;
    int                 m_nShowStarting;
    int                 m_nShowEndingPlus1;
    double              m_lfKoefY;
    bool                m_bLastIsFromPersistantMemory;
};

}}}

#include "common_ui_qt_graphicwidget.impl.hpp"

#endif // COMMON_UI_QT_GRAPHICWIDGET_HPP
