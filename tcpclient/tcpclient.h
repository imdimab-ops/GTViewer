#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QNetworkInterface>
#include <QtNetwork/QTcpSocket>
#include <QNetworkInformation>
#include "processdata.h"
#include "parsingdata.h"
#include "../applib.h"


//-----------------регестрируем глобальные типы данных----------------//
//Слово-состояние сервера
struct STWcli {
    bool conn = false;
    QString objectName;
    QString OSver;
    QString appver;
    quint64 Rx = 0;  //количество принятых байт
    quint64 Tx = 0;  //количество переданных байт
};
//-------------------------------------------------------------------//

class TcpClient : public QObject
{
    Q_OBJECT

public:
    explicit TcpClient(QObject *parent = nullptr);
    ~TcpClient();

private:
    QTcpSocket *m_tcpSocket = nullptr;
    STWcli m_stw;
    void initClient();

    ProcessData m_processData;
    ParsingData *parsingData = ParsingData::getInstance();

private slots:
    void connectHost(const QVariant &host, const QVariant &port);
    void disconnectHost();
    void slotConnected();
    void slotDisconnected();
    void slotHandleError(QAbstractSocket::SocketError error);
    void slotStateChanged(QAbstractSocket::SocketState state);
    void socketReadyRead();
    void packetCompleted(const QVariantMap &map);
    void socketWrite(const QByteArray &abyte);

public slots:
    void connectToHost(const QVariant &host, const QVariant &port, const QVariant &login, const QVariant &pass);
    void disconnectToHost();

signals:
    void serverUpdated(const STWsrv &);
    void clientUpdated(const STWcli &);
    void status(const QString &);
};

#endif // TCPCLIENT_H
