#ifndef TCPSERVER_H
#define TCPSERVER_H
#include <QTcpServer>
#include <QTcpSocket>
#include <QList>
#include <QDataStream>
#include <QDebug>

class TCPServer : public QTcpServer
{
    Q_OBJECT
public:
    TCPServer(QObject *parent = nullptr) : QTcpServer{parent}
    {
        if(this->listen(QHostAddress::LocalHost, 8000))
        {
            qDebug() << "Server is started";
        }
        else
        {
            qDebug() << "Caught error, server isn't started";
        }
    }
public slots:
    void readyRead();
private:
    void incomingConnection(qintptr socketDescriptor) override;
    void sendToClient(QString msg);
private:
    QTcpSocket *socket;
    QByteArray data;
    QList<QTcpSocket*> clients;
};

#endif // TCPSERVER_H
