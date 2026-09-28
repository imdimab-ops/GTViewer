#include "uistatistics.h"
#include "ui_uistatistics.h"

UiStatistics::UiStatistics(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::UiStatistics)
{
    ui->setupUi(this);
    this->setWindowTitle("Statistics");
    this->setFixedSize(this->width(), this->height());
    setupUI();
    setupConnections();
}

UiStatistics::~UiStatistics()
{
    delete ui;
}

void UiStatistics::setupUI()
{

}

void UiStatistics::setupConnections()
{
    //server
    auto tcpClient = TcpThread::getInstance();
    connect(tcpClient, &TcpThread::serverUpdated, this, [this, tcpClient](const STWsrv &stw){
        if (stw.opcconn){
            ui->label_opcconn->setText("✓ Connected");
            ui->label_opcconn->setStyleSheet("QLabel { color: green; }");
        } else {
            ui->label_opcconn->setText("✗ Disconnected");
            ui->label_opcconn->setStyleSheet("QLabel { color: red; }");
        }
        ui->label_srvVersion->setText(stw.srvVersion);
        ui->label_timestamp->setText(stw.timestamp.toDateTime().toString("dd.MM.yyyy HH:mm:ss"));
        ui->label_client_size->setText(stw.client_size.toString());
        ui->label_srvuptime->setText(stw.uptime.toDateTime().toString("dd.MM.yyyy HH:mm:ss"));
    });
    //client
    connect(tcpClient, &TcpThread::clientUpdated, this, [this, tcpClient](const STWcli &stw){
        if (stw.conn){
            ui->label_conn->setText("✓ Connected");
            ui->label_conn->setStyleSheet("QLabel { color: green; }");
        } else {
            ui->label_conn->setText("✗ Disconnected");
            ui->label_conn->setStyleSheet("QLabel { color: red; }");

            ui->label_opcconn->setText("✗ Disconnected");
            ui->label_opcconn->setStyleSheet("QLabel { color: red; }");
        }
        ui->label_Rx->setText(bytesToString(stw.Rx)); //количество принятых байт
        ui->label_Tx->setText(bytesToString(stw.Tx)); //количество переданных байт
        ui->label_cliuptime->setText(getUptime(stw.conn).toString("dd.MM.yyyy HH:mm:ss"));
    });
}

QString UiStatistics::bytesToString(const quint64 &bytes)
{
    const char *units[] = {" B", " KB", " MB", " GB", " TB"};
    double size = static_cast<double>(bytes);
    uint unit = 0;
    while (size >= 1024 && unit < 4) {
        size /= 1024;
        unit++;
    }
    return QString::number(size, 'f', unit ? 2 : 0) + units[unit];
}

QDateTime UiStatistics::getUptime(const bool &conn)
{
    static QDateTime startTime = QDateTime::currentDateTime(); //static - инициировали при первом вызове
    QDateTime zeroTime = QDateTime(QDate(1, 1, 1), QTime(0, 0, 0)); //cоздаем валидный QDateTime QDate(1, 1, 1) - минимальное значение
    if (!conn) {
        startTime = QDateTime::currentDateTime();
        return zeroTime;
    }
    auto secondsPassed = startTime.secsTo(QDateTime::currentDateTime()); //сколько прошло времени
    return zeroTime.addSecs(secondsPassed);
}
