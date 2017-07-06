#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QtNetwork>
#include <QTimer>

namespace Ui {
class MainWindow;
}

// Distribution request package
typedef struct {
    QString ip;
    long    status;
}SturctDistribRequest;

// Local anchor settings
typedef struct {
    QHostAddress localIpv4Address;
    int          Area;
    int          SubNumber;
    double        PositionX;
    double        PositionY;
}StructAnchorSettings;

// ini file
#define ERRORVALUE -1

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private:
    Ui::MainWindow *ui;

private slots:
    void on_pushQuit_clicked();
    void processPendingDatagram();
    void timerslotSendRequest();

protected:
    void closeEvent(QCloseEvent *);

private:
    void readLocalConfig(void);
    void writeLocalConfig(void);

public:

private:
    QUdpSocket* sender;
    QUdpSocket* receiver;
    QHostAddress remoteIpv4Address;
    SturctDistribRequest stRequest;
    StructAnchorSettings stSettings;
    QTimer *timerSendRequest;

};

#endif // MAINWINDOW_H
