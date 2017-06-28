/*
 *	File      : pitz_poluxclient_connectiondetails.cpp
 *
 *	Created on: 23 Mar, 2017
 *	Author    : Davit Kalantaryan (Email: davit.kalantaryan@desy.de)
 *
 *
 */

#include "pitz_poluxclient_connectiondetails.hpp"
#include <QMessageBox>


#if 0
QVBoxLayout m_mainLayout;
QHBoxLayout m_hblFacility;
QHBoxLayout m_hblDevice;
QHBoxLayout m_hblLocation;
QLabel      m_lbFacility;
QLabel      m_lbDevice;
QLabel      m_lbLocation;

QLineEdit   m_leFacility;
QLineEdit   m_leDevice;
QLineEdit   m_leLocation;
#endif


pitz::PoluxClient::ConnectionDetails::ConnectionDetails()
{
    m_lbFacility.setText("Facility:");
    m_lbDevice.setText(  "Device  :");
    m_lbLocation.setText("Location:");

    m_ok.setText("ok");
    m_cancel.setText("cancel");

    m_hblFacility.addWidget(&m_lbFacility);
    m_hblFacility.addWidget(&m_leFacility);

    m_hblDevice.addWidget(&m_lbDevice);
    m_hblDevice.addWidget(&m_leDevice);

    m_hblLocation.addWidget(&m_lbLocation);
    m_hblLocation.addWidget(&m_leLocation);

    m_hblButtons.addWidget(&m_ok);
    m_hblButtons.addWidget(&m_cancel);

    m_mainLayout.addLayout(&m_hblFacility);
    m_mainLayout.addLayout(&m_hblDevice);
    m_mainLayout.addLayout(&m_hblLocation);
    m_mainLayout.addLayout(&m_hblButtons);

    setLayout(&m_mainLayout);

    connect(&m_ok,SIGNAL(clicked()),this,SLOT(OkPushedSlot()));
    connect(&m_cancel,SIGNAL(clicked()),this,SLOT(close()));
}


pitz::PoluxClient::ConnectionDetails::~ConnectionDetails()
{
    m_mainLayout.removeItem(&m_hblButtons);
    m_mainLayout.removeItem(&m_hblLocation);
    m_mainLayout.removeItem(&m_hblDevice);
    m_mainLayout.removeItem(&m_hblFacility);

    m_hblButtons.removeWidget(&m_cancel);
    m_hblButtons.removeWidget(&m_ok);

    m_hblLocation.removeWidget(&m_leLocation);
    m_hblLocation.removeWidget(&m_lbLocation);

    m_hblDevice.removeWidget(&m_leDevice);
    m_hblDevice.removeWidget(&m_lbDevice);

    m_hblFacility.removeWidget(&m_leFacility);
    m_hblFacility.removeWidget(&m_lbFacility);
}


pitz::PoluxClient::DialogRet pitz::PoluxClient::ConnectionDetails::ExecNew()
{
    QString qsTextToSet;
    Application* pApp = GetCurentApplication();

    m_leFacility.setText(pApp->m_strFacility.c_str());
    m_leDevice.setText(  pApp->m_strDevice.c_str());
    m_leLocation.setText(pApp->m_strLocation.c_str());

    m_return = DialogRet::CANCEL;
    QDialog::exec();

    if(m_return==DialogRet::OK)
    {
        qsTextToSet = m_leFacility.text();
        pApp->m_strFacility = qsTextToSet.toStdString();

        qsTextToSet = m_leDevice.text();
        pApp->m_strDevice = qsTextToSet.toStdString();

        qsTextToSet = m_leLocation.text();
        pApp->m_strLocation = qsTextToSet.toStdString();
    }

    return m_return;
}


void pitz::PoluxClient::ConnectionDetails::OkPushedSlot()
{
    SCallArgsAll aSCallArgsAll(TaskType::CheckDoocsAdr);

    SetNewTaskGlb(&ConnectionDetails::TaskCallbackGUI,this,aSCallArgsAll);
}

void pitz::PoluxClient::ConnectionDetails::TaskCallbackGUI(SCallArgsAll a_args,int64_t a_err)
{
    if(a_err == 0)
    {
        m_return = DialogRet::OK;
        close();
    }
    else
    {
        // error report
        QMessageBox aMessageBox(QMessageBox::Critical,
                                QObject::tr("error"),QObject::tr(a_args.args.input.c_str()),
                                QMessageBox::Ok,this);
        //aMessageBox.setStyleSheet(QMessageBox::);
        aMessageBox.setDetailedText(QObject::tr("Should be done!"));
        aMessageBox.exec();
    }
}
