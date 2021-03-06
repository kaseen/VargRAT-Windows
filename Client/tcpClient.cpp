#include "tcpClient.h"

TcpClient::TcpClient(QObject *parent) : QObject{parent}
{
    connect(&_socket, &QTcpSocket::connected, this, &TcpClient::onConnected);
    connect(&_socket, &QTcpSocket::errorOccurred, this, &TcpClient::onErrorOccurred);
    connect(&_socket, &QTcpSocket::readyRead, this, &TcpClient::onReadyRead);
}

void TcpClient::connectToHost(const QString &ip, const QString &port)
{
    _socket.connectToHost(ip, port.toUInt());
}

void TcpClient::sendMessage(const QString &message)
{
    _socket.write("[MSG]");
    _socket.write(message.toUtf8());
    _socket.flush();
}

void TcpClient::sendFile(const QString &path)
{
    _socket.write("[FILE]");

    QFile file(path);
    file.open(QIODevice::ReadOnly);
    QByteArray ba = file.readAll();

    // Write [filename]
    _socket.write("[" + QFileInfo(file).fileName().toUtf8() + "]");

    file.close();

    _socket.write(ba);
    _socket.flush();
}

void TcpClient::onConnected()
{
    emit connectionSuccess();
    qInfo() << "Connected";
}

void TcpClient::onErrorOccurred(QAbstractSocket::SocketError error)
{
    qWarning() << "Error:" << error;
}

void TcpClient::onReadyRead()
{
    const QByteArray message = _socket.readAll();
    emit newMessageQML(message);
}
