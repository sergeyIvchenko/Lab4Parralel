#ifndef TCPSERVER_H
#define TCPSERVER_H
#include <QTcpServer>
#include <QTcpSocket>
#include <QList>
#include <QDataStream>
#include <QDebug>
#include <QThreadPool>
#include <QFuture>

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
public:
    void incomingConnection(qintptr socketDescriptor) override;
    void sendToClient(QString msg);
    void clientThreadFunc(std::vector<std::vector<quint32>>& matrix);
    void openmpFunc(std::vector<std::vector<quint32>>& matrix);
    void rowTranspose(int thread_id, std::vector<std::vector<quint32>>& matrix, int k);
public:
    QTcpSocket *socket;
public:
    std::vector<std::vector<quint32>> matrix;
    std::vector<QFuture<void>> workers;
    quint32 k;
    int parallelWay;
    QByteArray data;
    bool openmpFinished;
};

#endif // TCPSERVER_H
