#ifndef UITCPCLIENT_H
#define UITCPCLIENT_H

#include <QWidget>
#include <QRegularExpressionValidator>

#include "tcpthread.h"
#include "../systemlog/modelsystemlog.h"

namespace Ui {
class UiTcpClient;
}

class UiTcpClient : public QWidget
{
    Q_OBJECT

public:
    explicit UiTcpClient(QWidget *parent = nullptr);
    ~UiTcpClient();

private:
    Ui::UiTcpClient *ui;

    void setupUI();
    void setupConnections();
};

#endif // UITCPCLIENT_H
