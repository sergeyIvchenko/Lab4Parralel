#include <QCoreApplication>
#include "tcpserver.h"
#include <thread>
#include <chrono>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>
#include <functional>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    TCPServer server;
//    std::vector<qint32> arr(5);
//    for(int i=0; i<arr.size();++i)
//    {
//       arr[i] = i;
//    }
//    std::vector<qint32> outarr(5);
//    QByteArray barray;
//    QDataStream stream(&barray, QIODevice::WriteOnly);
//    stream.setByteOrder(QDataStream::BigEndian);
//    for(int i=0; i<arr.size();++i)
//    {
//        stream << arr[i];
//    }
//    QDataStream out(barray);
//    for(int i=0; i<outarr.size();++i)
//    {
//        out >> outarr[i];
//    }
//    for(auto it : outarr)
//    {
//        qDebug() << it;
//    }
    return a.exec();
}
