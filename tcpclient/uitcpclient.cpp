#include "uitcpclient.h"
#include "./ui_uitcpclient.h"

UiTcpClient::UiTcpClient(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::UiTcpClient)
{
    ui->setupUi(this);
    this->setWindowTitle("Connect to server");
    this->setFixedSize(this->width(), this->height());
    setupUI();
    setupConnections();
}

UiTcpClient::~UiTcpClient()
{
    delete ui;
}

void UiTcpClient::setupUI()
{
    //валидатор
    QRegularExpression ipRegex(
        "^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
        "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
        "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
        "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$"
        );
    QRegularExpressionValidator *validator = new QRegularExpressionValidator(ipRegex, this);
    ui->le_ip->setValidator(validator);
    ui->le_ip->setPlaceholderText("0.0.0.0");

    ui->le_ip->setText("5.140.165.28");
    ui->le_port->setText("5577");
    ui->le_username->setText("userINGCgts");
    ui->le_pass->setText("userINGC");
    ui->le_pass->setEchoMode(QLineEdit::Password);

    ui->rb_connect->setIcon(QIcon(":img/icon/plugdisconnected.svg"));
    ui->rb_connect->setAttribute(Qt::WA_TransparentForMouseEvents, true); //отключаем событие мыши
    ui->rb_connect->setAutoExclusive(false); //отключаем отметку только одной кнопки
}

void UiTcpClient::setupConnections()
{
    auto tcpClient = TcpThread::getInstance();
    auto modelSysLog = ModelSystemLog::getInstance();

    connect(tcpClient, &TcpThread::status, this, [this, tcpClient, modelSysLog](const QString &status){
        modelSysLog->append("TcpClient", status);
    });
    connect(ui->pb_connect, &QPushButton::clicked, this, [this, tcpClient](){
        emit tcpClient->connectToHost(ui->le_ip->text(), ui->le_port->text(), ui->le_username->text(), ui->le_pass->text());
    });
    connect(ui->pb_disconn, &QPushButton::clicked, this, [this, tcpClient](){
        emit tcpClient->disconnectToHost();
    });
    connect(tcpClient, &TcpThread::clientUpdated, this, [this](const STWcli &stw){
        if (stw.conn) {
            ui->rb_connect->setIcon(QIcon(":img/icon/plugconnected.svg"));
        } else {
            ui->rb_connect->setIcon(QIcon(":img/icon/plugdisconnected.svg"));
        }
        ui->rb_connect->setChecked(stw.conn);
        ui->le_ip->setDisabled(stw.conn);
        ui->le_port->setDisabled(stw.conn);
        ui->le_username->setDisabled(stw.conn);
        ui->le_pass->setDisabled(stw.conn);
        ui->pb_connect->setDisabled(stw.conn);
    });

    //вызываем с задержкой автоматическое подключение к серверу данных
    QTimer::singleShot(500, this, [this, tcpClient](){
        emit tcpClient->connectToHost(ui->le_ip->text(), ui->le_port->text(), ui->le_username->text(), ui->le_pass->text());
    });
}
