/*
 *	File      : pitz_poluxclient_connectiondetails.hpp
 *
 *	Created on: 23 Mar, 2017
 *	Author    : Davit Kalantaryan (Email: davit.kalantaryan@desy.de)
 *
 *
 */
#ifndef PITZ_POLUXCLIENT_CONNECTIONDETAILS_HPP
#define PITZ_POLUXCLIENT_CONNECTIONDETAILS_HPP

#include <QDialog>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include "pitz_poluxclient_application.hpp"

namespace pitz{ namespace PoluxClient{

enum class DialogRet{OK,CANCEL};

class ConnectionDetails : private QDialog
{
    Q_OBJECT

public:
    ConnectionDetails();
    ~ConnectionDetails();

    pitz::PoluxClient::DialogRet ExecNew();

    void TaskCallbackGUI(SCallArgsAll a_args,int64_t a_err);

private slots:
    void OkPushedSlot();

private:
    QVBoxLayout m_mainLayout;

    QHBoxLayout m_hblFacility;
    QHBoxLayout m_hblDevice;
    QHBoxLayout m_hblLocation;
    QHBoxLayout m_hblButtons;

    QLabel      m_lbFacility;
    QLabel      m_lbDevice;
    QLabel      m_lbLocation;

    QLineEdit   m_leFacility;
    QLineEdit   m_leDevice;
    QLineEdit   m_leLocation;

    QPushButton m_ok;
    QPushButton m_cancel;

    DialogRet   m_return;
};

}}

#endif // PITZ_POLUXCLIENT_CONNECTIONDETAILS_HPP
