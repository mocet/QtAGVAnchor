#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QHostInfo>
#include <QNetworkInterface>
#include <QtWidgets>

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /* Get IP address */
    QNetworkInterface interface = QNetworkInterface::interfaceFromName("eth0");
    QList<QNetworkAddressEntry> list = interface.addressEntries();
    if (!list.isEmpty())
    {
        stSettings.localIpv4Address = list.at(0).ip();
        ui->labelIPAddress->setText(stSettings.localIpv4Address.toString());
    }

    // Load ini file
    QFile file("Settings.ini");
    if (file.exists())
    {
        // Load ini file
        readLocalConfig();
    }
    else
    {
        // Create ini file
        writeLocalConfig();
    }

//    /* Not used */
//    QString localHostname = QHostInfo::localHostName();
//    QHostInfo hostInfo = QHostInfo::fromName(localHostname);
//    QString localIpv4Address;
//    QList<QHostAddress> listAddress = hostInfo.addresses();
//    if (!listAddress.isEmpty())
//    {
//        qDebug()<<localHostname;
//        for (int i = 0; i < listAddress.size(); i++)
//        {
//            qDebug()<<listAddress.at(i).toString();
//        }
//    }

    // Setup UDP
    sender = new QUdpSocket(this);
    //sender->bind(5488, QUdpSocket::ShareAddress);
    stRequest.status = 1;
    stRequest.ip = stSettings.localIpv4Address.toString();
    remoteIpv4Address = QHostAddress("192.168.1.2"); // PC ip
    timerSendRequest = new QTimer(this);
    connect(timerSendRequest, SIGNAL(timeout()), this, SLOT(timerslotSendRequest()));
    timerSendRequest->start(10000);

    receiver = new QUdpSocket(this);
    receiver->bind(5487,QUdpSocket::ShareAddress);
    connect(receiver,SIGNAL(readyRead()),this,SLOT(processPendingDatagram()));



}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // close udp socket
    sender->close();
    receiver->close();
    qDebug() << "Close";

    event->accept();
}

void MainWindow::readLocalConfig()
{
    QSettings settings("Settings.ini",QSettings::IniFormat);
    int area = settings.value("Base/Area", -1).toInt();
    int subnumber = settings.value("Base/Subnumber", -1).toInt();
    double positionX = settings.value("Base/Positionx", 0.0).toFloat();
    double positionY = settings.value("Base/Positiony", 0.0).toFloat();

    // Update ip value
    settings.beginGroup("Base");
    settings.setValue("IP", stSettings.localIpv4Address.toString());
    settings.endGroup();

    // Load the config file vaules
    stSettings.Area = area;
    stSettings.SubNumber = subnumber;
    stSettings.PositionX = positionX;
    stSettings.PositionY = positionY;

    // Show in the main window
    if (area != ERRORVALUE)
        ui->labelArea->setText(QString::number(area));
    else
        ui->labelArea->setText("");
    if (subnumber != ERRORVALUE)
        ui->labelNumber->setText(QString::number(subnumber));
    else
        ui->labelNumber->setText("");
    QString strx = QString::number(positionX);
    QString stry = QString::number(positionY);
    ui->labelPosition->setText(strx + ", " + stry);
}

void MainWindow::writeLocalConfig()
{
    QSettings settings("Settings.ini", QSettings::IniFormat);
    settings.beginGroup("Base");
    settings.setValue("IP", stSettings.localIpv4Address.toString());
    settings.setValue("Area",-1);
    settings.setValue("Subnumber",-1);
    settings.setValue("Positionx",0.0);
    settings.setValue("Positiony",0.0);
    settings.endGroup();

    // Show
    ui->labelArea->setText("");
    ui->labelNumber->setText("");
    ui->labelPosition->setText("0, 0");
}


void MainWindow::on_pushQuit_clicked()
{

}

void MainWindow::processPendingDatagram()
{
    while(receiver->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(receiver->pendingDatagramSize());
        receiver->readDatagram(datagram.data(),datagram.size());

    }
}

void MainWindow::timerslotSendRequest()
{
    sender->writeDatagram((char*)&stRequest,sizeof(stRequest),remoteIpv4Address,5488);
    qDebug() << "Send";
}
