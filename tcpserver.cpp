#include "tcpserver.h"
#include <iostream>
#include <thread>
#include <QtConcurrent/QtConcurrent>
#include <omp.h>

void TCPServer::incomingConnection(qintptr socketDescriptor)
{
    socket = new QTcpSocket(this);
    socket->setSocketDescriptor(socketDescriptor);
    //Додамо зв'язки сигналів та слотів
    connect(socket, &QTcpSocket::readyRead, this, &TCPServer::readyRead);
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
    qDebug() << "Client connected:" << socketDescriptor;
//        socket->waitForReadyRead();
}


void TCPServer::rowTranspose(int thread_id,std::vector<std::vector<quint32>>& matrix, int k)
{
    for(int i=thread_id;i<matrix.size();i+=k)
    {
        for(int j=i+1;j<matrix.size();++j)
        {
            std::swap(matrix[i][j],matrix[j][i]);
        }
    }
    std::this_thread::sleep_for(std::chrono::seconds(5));
}

void TCPServer::clientThreadFunc(std::vector<std::vector<quint32> > &matrix)
{
    workers.resize(k);
    for(int thread_id=0; thread_id<k; ++thread_id)
    {
        workers[thread_id] = QtConcurrent::run(&TCPServer::rowTranspose, this, thread_id,std::ref(matrix),k);
    }
}

void TCPServer::openmpFunc(std::vector<std::vector<quint32> > &matrix)
{
    omp_set_num_threads(k);
    openmpFinished = false;
    #pragma omp parallel shared(matrix,k,openmpFinished)
    {
        int thread_id = omp_get_thread_num();
        rowTranspose(thread_id,matrix,k);
        #pragma omp barrier
        #pragma omp single
        openmpFinished = true;
    }
}

void TCPServer::readyRead()
{
//    socket = static_cast<QTcpSocket*>(sender());
    QDataStream inStream(socket);
    inStream.setVersion(QDataStream::Qt_5_15);
    QString res;
    inStream >> res;
    if(res == "start")
    {
        quint32 size;
        inStream >> size >> k;
        matrix.resize(size);
        for(int i=0; i<size;++i)
        {
            matrix[i].resize(size);
            for(int j=0; j<size;++j)
            {
                inStream >> matrix[i][j];
            }
        }

        inStream >> parallelWay;
        qDebug() << "Calculation begins";
        std::thread clientThread;
        qDebug() << parallelWay;
        if(parallelWay == 1)
        {
            clientThread = std::thread(&TCPServer::clientThreadFunc,this, std::ref(matrix));
        }
        else
        {
            clientThread = std::thread(&TCPServer::openmpFunc,this,std::ref(matrix));
        }
        clientThread.detach();
    }
    else if(res == "result")
    {
        qDebug() << "Checking results";
        data.clear();
        QDataStream outStream(&data, QIODevice::WriteOnly);
        outStream.setByteOrder(QDataStream::BigEndian);
        bool workRes = true;
        if(parallelWay == 1)
        {
            for(auto& work : workers)
            {
                if(work.isRunning())
                {
                    workRes = false;
                    break;
                }
            }
        }
        else
        {
            workRes = openmpFinished;
        }

        QString resultText;
        if(!workRes)
        {
            resultText = "Calculation in progress";
            outStream << resultText;
            socket->write(data);
            socket->waitForBytesWritten(500);
        }
        else
        {
            resultText = "Calculation completed";
            outStream << resultText;
            socket->write(data);
            socket->waitForBytesWritten(500);
            data.clear();
            QDataStream outMatrix(&data, QIODevice::WriteOnly);
            outMatrix.setByteOrder(QDataStream::BigEndian);
            for(auto& row : matrix)
            {
                for(auto& it:row)
                {
                    outMatrix << it;
                }
            }
            socket->write(data);
            socket->waitForBytesWritten();
        }
    }
    else if(res == "exit")
    {
        data.clear();
        data = socket->readAll();
        qDebug() << "Клієнт від'єднався";
    }
}
