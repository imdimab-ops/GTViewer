#include "tcpclient.h"

TcpClient::TcpClient(QObject *parent)
    : QObject{parent}
{
    connect(&m_processData, &ProcessData::status, this, &TcpClient::status);
    connect(&m_processData, &ProcessData::readPacketCompleted, this, &TcpClient::packetCompleted);
    connect(parsingData, &ParsingData::sendData, this, [this](const QVariantMap &map){
        socketWrite(m_processData.createPacket(map));
    });
    initClient();
}

TcpClient::~TcpClient()
{
    if (m_tcpSocket != nullptr) {
        disconnectHost();
        m_tcpSocket->deleteLater();
        m_tcpSocket = nullptr;
    };
}

void TcpClient::initClient()
{
    if (m_tcpSocket == nullptr) {
        m_tcpSocket = new QTcpSocket(this);
        QAbstractSocket::connect(m_tcpSocket, &QTcpSocket::connected, this, &TcpClient::slotConnected);
        QAbstractSocket::connect(m_tcpSocket, &QTcpSocket::disconnected, this, &TcpClient::slotDisconnected);
        QAbstractSocket::connect(m_tcpSocket, &QTcpSocket::errorOccurred, this, &TcpClient::slotHandleError);
        QAbstractSocket::connect(m_tcpSocket, &QTcpSocket::stateChanged, this, &TcpClient::slotStateChanged);
        QAbstractSocket::connect(m_tcpSocket, &QTcpSocket::readyRead, this, &TcpClient::socketReadyRead);

        //Проверка соединения (ping), отправляем ping, если соединение разорвано физически сработает QAbstractSocket::KeepAliveOption и разорвет соединение
        m_tcpSocket->setSocketOption(QAbstractSocket::KeepAliveOption, 1);
        QTimer *ppingTimer = new QTimer(this);
        ppingTimer->setInterval(5000); // Каждые 10 секунд
        connect(ppingTimer, &QTimer::timeout, this, [this, ppingTimer]() {
            if (m_tcpSocket && m_tcpSocket->state() == QAbstractSocket::ConnectedState) {
                socketWrite(m_processData.createPacket(QVariantMap({ {"PING", QDateTime::currentMSecsSinceEpoch()} })));
            }
        });
        connect(m_tcpSocket, &QTcpSocket::stateChanged, this, [this, ppingTimer](QAbstractSocket::SocketState state) {
            switch (state) {
            case QAbstractSocket::ConnectedState:
                ppingTimer->start();
                break;
            case QAbstractSocket::UnconnectedState:
                ppingTimer->stop();
                break;
            default:
                break;
            }
            // qDebug() << Q_FUNC_INFO << state << pingTimer->isActive();
        });
    };
    m_stw.OSver = AppLib::getOSVersionString();
    m_stw.objectName = AppLib::getHostName();
    m_stw.appver = AppLib::getAppVersion();
}

void TcpClient::connectHost(const QVariant &host, const QVariant &port)
{
    if (m_tcpSocket != nullptr && m_tcpSocket->state() == QAbstractSocket::ConnectedState) {
        emit status(QString("[Info] %1, Connected: %2:%3").arg(Q_FUNC_INFO, host.toString(), port.toString()));
        return;
    }

    if (m_tcpSocket != nullptr && m_tcpSocket->state() == QAbstractSocket::UnconnectedState) {
        emit status(QString("[Info] %1, Connecting... %2:%3").arg(Q_FUNC_INFO, host.toString(), port.toString()));
        m_tcpSocket->connectToHost(host.toString(), port.toUInt(), QIODevice::ReadWrite, QAbstractSocket::IPv4Protocol);

        if (!m_tcpSocket->waitForConnected(2000)) {
            emit status(QString("[Error] %1, %2").arg(Q_FUNC_INFO, m_tcpSocket->errorString()));
        }
    }
}

void TcpClient::disconnectHost()
{
    if (m_tcpSocket != nullptr && m_tcpSocket->state() == QAbstractSocket::ConnectedState) {
        m_tcpSocket->disconnectFromHost();
    }
}

void TcpClient::slotConnected()
{
    //авторизуемся на сервере
    QVariantMap map;
    map.insert("CLIENT.LOGIN", m_processData.getCredentials().login);
    map.insert("CLIENT.PASS", m_processData.getCredentials().pass);
    map.insert("CLIENT.NAME", m_stw.objectName);
    map.insert("CLIENT.OSVER", m_stw.OSver);
    map.insert("CLIENT.APPVER", m_stw.appver);
    socketWrite(m_processData.createPacket(map));

    m_stw.conn = true;
    emit clientUpdated(m_stw);
}

void TcpClient::slotDisconnected()
{
    m_stw.conn = false;
    m_stw.Rx = 0;
    m_stw.Tx = 0;
    emit clientUpdated(m_stw);
}

void TcpClient::slotHandleError(QAbstractSocket::SocketError error)
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (socket) {
        emit status(QString("[Error] %1, %2:%3, %4").arg(Q_FUNC_INFO,
                                                         socket->peerAddress().toString(),
                                                         QString::number(socket->peerPort()),
                                                         socket->errorString()));
    }
}

void TcpClient::slotStateChanged(QAbstractSocket::SocketState state)
{
    static const QMap<QAbstractSocket::SocketState, QString> stateMap = {
        {QAbstractSocket::UnconnectedState, tr("Unconnected")},
        {QAbstractSocket::HostLookupState, tr("Host Lookup")},
        {QAbstractSocket::ConnectingState, tr("Connecting")},
        {QAbstractSocket::ConnectedState, tr("Connected")},
        {QAbstractSocket::BoundState, tr("Bound")},
        {QAbstractSocket::ListeningState, tr("Listening")},
        {QAbstractSocket::ClosingState, tr("Closing")}
    };
    emit this->status(QString("[Info] %1, Socket state changed: %2").arg(Q_FUNC_INFO, stateMap.value(state, tr("Unknown State"))));
}

void TcpClient::socketReadyRead()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (socket->state() == QAbstractSocket::ConnectedState) {
        qint64 bytes = socket->bytesAvailable();
        m_stw.Rx += bytes;
        emit clientUpdated(m_stw);
        m_processData.readPacket(m_tcpSocket->readAll()); //обратный вызов по готовности через сигнал
    }
}

void TcpClient::packetCompleted(const QVariantMap &map)
{
    /* Переданный пакет готов (собран и расшифрован) */
    parsingData->engineResponse(map);
    emit serverUpdated(parsingData->getServerSTW(map));
    // qDebug() << Q_FUNC_INFO << map;
}

void TcpClient::socketWrite(const QByteArray &abyte)
{
    if (m_tcpSocket->state() == QAbstractSocket::ConnectedState) {
        m_stw.Tx += m_processData.writeSocketWidthControl(m_tcpSocket, abyte);
        emit clientUpdated(m_stw);
    }
}

void TcpClient::connectToHost(const QVariant &host, const QVariant &port, const QVariant &login, const QVariant &pass)
{
    m_processData.setCred(login.toString(), pass.toString());
    connectHost(host, port);
}

void TcpClient::disconnectToHost()
{
    disconnectHost();
}
