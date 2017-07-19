
// common_ui_qt_mainwindow
// 2017 Jul 04

#ifndef COMMON_UI_QT_MAINWINDOW_HPP
#define COMMON_UI_QT_MAINWINDOW_HPP

#include <QMainWindow>
#include <QToolBar>
#include <QAction>
//#include "common_ui_qt_centralwidget.hpp"

namespace common{ namespace ui{ namespace qt{

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow();
    virtual ~MainWindow();

protected:
    //CentralWidget   m_centrawWidget;
    QToolBar        m_toolBar;
    QAction         m_actionQuit;
};

}}}

#endif // COMMON_UI_QT_MAINWINDOW_HPP
