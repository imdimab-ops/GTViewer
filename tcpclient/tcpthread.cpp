#include "tcpthread.h"

TcpThread::TcpThread(QObject *parent)
    : QThread{parent}
{
    createThread();
}

TcpThread::~TcpThread()
{
    if(m_thread.isRunning()) {
        m_thread.quit();
        m_thread.wait();
    }
    delete m_tcpClient;
    m_tcpClient = nullptr;
}

TcpThread *TcpThread::getInstance()
{
    static TcpThread instance;
    return &instance;
}


bool TcpThread::createThread()
{
    m_tcpClient = new TcpClient;
    m_tcpClient->moveToThread(&m_thread);
    connect(&m_thread, &QThread::finished, m_tcpClient, &QObject::deleteLater);
    connect(this, &TcpThread::connectToHost, m_tcpClient, &TcpClient::connectToHost);
    connect(this, &TcpThread::disconnectToHost, m_tcpClient, &TcpClient::disconnectToHost);

    connect(m_tcpClient, &TcpClient::status, this, &TcpThread::status);
    connect(m_tcpClient, &TcpClient::clientUpdated, this, &TcpThread::clientUpdated);
    connect(m_tcpClient, &TcpClient::serverUpdated, this, &TcpThread::serverUpdated);

    ParsingData *parsingData = ParsingData::getInstance();
    parsingData->moveToThread(&m_thread);
    connect(&m_thread, &QThread::finished, parsingData, &QObject::deleteLater);
    connect(this, &TcpThread::clientRequest, parsingData, &ParsingData::clientRequest);
    connect(parsingData, &ParsingData::dataUpdated, this, &TcpThread::dataUpdated);
    connect(parsingData, &ParsingData::tagChanged, this, &TcpThread::tagChanged);
    connect(parsingData, &ParsingData::addHisoryData, this, &TcpThread::addHisoryData);
    connect(parsingData, &ParsingData::status, this, &TcpThread::status);
    m_thread.start();

    //кол-во логических процессоров (отображаем с задержкой, чтобы Ui с логгером успел создаться)
    QTimer::singleShot(50, this, [this]() {
        if (this->m_thread.isRunning()) emit status(QString("[Info] %1, idealThreadCount: %2").arg(Q_FUNC_INFO, QString::number(this->m_thread.idealThreadCount())));
    });
    return true;
}
