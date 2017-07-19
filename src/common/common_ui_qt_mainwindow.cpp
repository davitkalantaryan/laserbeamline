
// common_ui_qt_mainwindow
// 2017 Jul 04

#include "common_ui_qt_mainwindow.hpp"
#include <QMenuBar>


common::ui::qt::MainWindow::MainWindow()
    :
      m_actionQuit(QIcon(":/img/quit_pik.png"),tr("Close"),this)
{
    //QMenuBar* pMenuBar = &m_MenuBarLeft;
    QMenu* pCurMenu ;
    QMenuBar* pMenuBar = menuBar();

    m_actionQuit.setStatusTip( tr("Exit Program") );
    connect( &m_actionQuit, SIGNAL(triggered()), this, SLOT(close()) );

    m_toolBar.addAction(&m_actionQuit);
    pCurMenu = pMenuBar->addMenu( tr("&File") );
    pCurMenu->addAction(&m_actionQuit);

    addToolBar(&m_toolBar);
    //setCentralWidget(&m_centrawWidget);
}



common::ui::qt::MainWindow::~MainWindow()
{
    //
}
