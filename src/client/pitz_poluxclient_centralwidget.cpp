/*
 *	File      : pitz_poluxclient_centralwidget.cpp
 *
 *	Created on: 22 Mar, 2017
 *	Author    : Davit Kalantaryan (Email: davit.kalantaryan@desy.de)
 *
 *
 */

#include "pitz_poluxclient_centralwidget.hpp"

pitz::PoluxClient::CentralWidget::CentralWidget()
    :
      m_executeCommandBtn(QIcon(":/img/execute_btn.png"),tr("execute command")),
      m_nmButton(QIcon(":/img/nm_btn.png"),tr("nm")),
      m_ncalButton(QIcon(":/img/ncal_btn.png"),tr("ncal")),
      m_nsaveButton(QIcon(":/img/nsave_btn.png"),tr("nsave")),
      m_nr1Button(QIcon(":/img/nr1_btn.png"),tr("nr")),
      m_nr2Button(QIcon(":/img/nr2_btn.png"),tr("nr")),
      m_nrmButton(QIcon(":/img/nrm_btn.png"),tr("nrm")),
      m_nresetButton(QIcon(":/img/nreset_btn.png"),tr("nreset")),
      m_STOP_ALLButton(QIcon(":/img/stop_all_btn.png"),tr("STOP ALL")),
      m_homeButton(QIcon(":/img/home_btn.png"),tr("home")),
      m_zeroButton(QIcon(":/img/zero_btn.png"),tr("<<0>>"))
{
    m_commandsHistory.setReadOnly(true);

    m_smallBtnsLayout.addWidget(&m_spinBox,0,0);
    m_smallBtnsLayout.addWidget(&m_nmButton,0,1);
    m_smallBtnsLayout.addWidget(&m_ncalButton,0,2);
    m_smallBtnsLayout.addWidget(&m_nsaveButton,0,3);

    m_smallBtnsLayout.addWidget(&m_nr1Button,1,0);
    m_smallBtnsLayout.addWidget(&m_nr2Button,1,1);
    m_smallBtnsLayout.addWidget(&m_nrmButton,1,2);
    m_smallBtnsLayout.addWidget(&m_nresetButton,1,3);

    m_smallBtnsLayout.addWidget(&m_STOP_ALLButton,2,0,2,2);
    m_smallBtnsLayout.addWidget(&m_homeButton,2,2);
    m_smallBtnsLayout.addWidget(&m_zeroButton,2,3);

    m_exeCmdAndSmlBtnLayout.addLayout(&m_exeCmdAndDownLayout);
    m_exeCmdAndSmlBtnLayout.addLayout(&m_smallBtnsLayout);
    m_exeCmdAndDownLayout.addWidget(&m_executeCommandBtn);

    m_MainLayoutDownPart.addWidget(&m_commandsHistory);
    m_MainLayoutDownPart.addWidget(&m_anyCommandToExecute);
    m_MainLayoutDownPart.addLayout(&m_exeCmdAndSmlBtnLayout);
    m_MainLayout.addLayout(&m_MainLayoutDownPart);

    setLayout(&m_MainLayout);

    connect(&m_executeCommandBtn,SIGNAL(clicked()),this,SLOT(ExecuteCommandSlot()));
}


pitz::PoluxClient::CentralWidget::~CentralWidget()
{
    m_MainLayout.removeItem(&m_MainLayoutDownPart);
    m_MainLayoutDownPart.removeItem(&m_exeCmdAndSmlBtnLayout);
    m_MainLayoutDownPart.removeWidget(&m_anyCommandToExecute);
    m_MainLayoutDownPart.removeWidget(&m_commandsHistory);

    m_exeCmdAndDownLayout.removeWidget(&m_executeCommandBtn);
    m_exeCmdAndSmlBtnLayout.removeItem(&m_smallBtnsLayout);
    m_exeCmdAndSmlBtnLayout.removeItem(&m_exeCmdAndDownLayout);

    m_smallBtnsLayout.removeWidget(&m_zeroButton);
    m_smallBtnsLayout.removeWidget(&m_homeButton);
    m_smallBtnsLayout.removeWidget(&m_STOP_ALLButton);

    m_smallBtnsLayout.removeWidget(&m_nresetButton);
    m_smallBtnsLayout.removeWidget(&m_nrmButton);
    m_smallBtnsLayout.removeWidget(&m_nr2Button);
    m_smallBtnsLayout.removeWidget(&m_nr1Button);

    m_smallBtnsLayout.removeWidget(&m_nsaveButton);
    m_smallBtnsLayout.removeWidget(&m_ncalButton);
    m_smallBtnsLayout.removeWidget(&m_nmButton);
    m_smallBtnsLayout.removeWidget(&m_spinBox);
}


RETUN_LAYOUT* pitz::PoluxClient::CentralWidget::GetWholeLayout()
{
    return &m_MainLayout;
}


void pitz::PoluxClient::CentralWidget::ExecuteCommandSlot()
{
    std::string ssCommandToExecute = m_anyCommandToExecute.text().toStdString();
    SCallArgsAll aSCallArgsAll(TaskType::AnyCommand,ssCommandToExecute);

    SetNewTaskGlb(&CentralWidget::TaskCallbackGUI,this,aSCallArgsAll);
}


void pitz::PoluxClient::CentralWidget::TaskCallbackGUI(SCallArgsAll /*a_args*/,int64_t a_err)
{
    if(a_err == 0)
    {
        return;
    }
}
