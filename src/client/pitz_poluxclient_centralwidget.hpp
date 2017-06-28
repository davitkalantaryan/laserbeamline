/*
 *	File      : pitz_poluxclient_centralwidget.hpp
 *
 *	Created on: 22 Mar, 2017
 *	Author    : Davit Kalantaryan (Email: davit.kalantaryan@desy.de)
 *
 *
 */
#ifndef PITZ_POLUXCLIENT_CENTRALWIDGET_HPP
#define PITZ_POLUXCLIENT_CENTRALWIDGET_HPP

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QGridLayout>
#include <QDoubleSpinBox>
#include "pitz_poluxclient_application.hpp"

#define RETUN_LAYOUT   QVBoxLayout

namespace pitz{ namespace PoluxClient{

class CentralWidget : public QWidget
{
    Q_OBJECT

public:
    CentralWidget();
    ~CentralWidget();

    RETUN_LAYOUT* GetWholeLayout();

private:
    void TaskCallbackGUI(SCallArgsAll a_args,int64_t a_err);

private slots:
    void ExecuteCommandSlot();

private:
    QVBoxLayout     m_MainLayout;
    QVBoxLayout     m_MainLayoutDownPart;
    QHBoxLayout     m_exeCmdAndSmlBtnLayout;
    QVBoxLayout     m_exeCmdAndDownLayout;
    QGridLayout     m_smallBtnsLayout;
    QLineEdit       m_anyCommandToExecute;
    QPushButton     m_executeCommandBtn;
    QTextEdit       m_commandsHistory;
    QDoubleSpinBox  m_spinBox;
    QPushButton     m_nmButton;
    QPushButton     m_ncalButton;
    QPushButton     m_nsaveButton;
    QPushButton     m_nr1Button;
    QPushButton     m_nr2Button;
    QPushButton     m_nrmButton;
    QPushButton     m_nresetButton;
    QPushButton     m_STOP_ALLButton;
    QPushButton     m_homeButton;
    QPushButton     m_zeroButton;
};

}}

#endif // PITZ_POLUXCLIENT_CENTRALWIDGET_HPP
