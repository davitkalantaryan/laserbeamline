/*
 *	File      : main_polux_client.cpp
 *
 *	Created on: 22 Mar, 2017
 *	Author    : Davit Kalantaryan (Email: davit.kalantaryan@desy.de)
 *
 *
 */
#include "pitz_poluxclient_application.hpp"
#include "pitz_poluxclient_mainwindow.hpp"
#include <stdio.h>

int main(int argc, char* argv[])
{
    freopen( "/dev/null", "w", stderr);

    pitz::PoluxClient::Application app(argc,argv);
    //QCoreApplication::instance();

    qRegisterMetaType<pitz::PoluxClient::STaskDoneStruct>( "pitz::PoluxClient::STaskDoneStruct" );

    pitz::PoluxClient::MainWindow aMainWnd;
    aMainWnd.show();

    app.exec();

    return 0;
}
