#include "tcpserver.h"

void TCPServer::incomingConnection(qintptr socketDescriptor)
{
    socket = new QTcpSocket(this);
    socket->setSocketDescriptor(socketDescriptor);
    //Додамо зв'язки сигналів та слотів
    connect(socket, &QTcpSocket::readyRead, this, &TCPServer::readyRead);
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
    clients.push_back(socket);

    qDebug() << "Client connected:" << socketDescriptor;
    socket->waitForReadyRead();
}

void TCPServer::readyRead()
{
    socket = static_cast<QTcpSocket*>(sender());
    QDataStream in{socket};//Клас для вводу-виводу
    if(in.status() == QDataStream::Ok)
    {
        qDebug() << "Received data";
        QString str;
        in >> str;
        qDebug() << str;
        sendToClient(str);
    }
    else
    {
        qDebug()<< "Error occured";
    }
}

void TCPServer::sendToClient(QString msg)
{
    data.clear();
    QDataStream out(&data, QIODevice::WriteOnly);
    out << msg;
//    qDebug() << data;
    socket->write(data);
    qDebug() << "Message sent";
}
