/*
 *	File      : pitz_poluxclient_mainwindow.hpp
 *
 *	Created on: 22 Mar, 2017
 *	Author    : Davit Kalantaryan (Email: davit.kalantaryan@desy.de)
 *
 *
 */
#ifndef PITZ_POLUXCLIENT_MAINWINDOW_HPP
#define PITZ_POLUXCLIENT_MAINWINDOW_HPP

#include <QMainWindow>
#include "pitz_poluxclient_centralwidget.hpp"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QAction>
#include <QToolBar>
#include "pitz_poluxclient_connectiondetails.hpp"

namespace pitz{ namespace PoluxClient{

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow();

private:
    void CreateActions();
    void CreateMenus();

private slots:
    void ConnectSlot();
    void DisconnectSlot();

private:
    CentralWidget       m_CentralWidget;
    QVBoxLayout         m_MenuAndToolbarLayout;
    QHBoxLayout         m_MenuLayout;
    QMenuBar            m_MenuBarLeft ;
    QMenuBar            m_MenuBarRight;
    QToolBar            m_mainToolBar;
    QAction             m_ActionQuit;
    QAction             m_ActionConnect;
    QAction             m_ActionDisconnect;
    QAction             m_ActionPort;
    QAction             m_ActionRescan;
    ConnectionDetails   m_detailsDlg;
};

}}

#endif // PITZ_POLUXCLIENT_MAINWINDOW_HPP
