#ifndef UISTATISTICS_H
#define UISTATISTICS_H

#include <QWidget>
#include "./tcpclient/tcpthread.h"

namespace Ui {
class UiStatistics;
}

class UiStatistics : public QWidget
{
    Q_OBJECT

public:
    explicit UiStatistics(QWidget *parent = nullptr);
    ~UiStatistics();

private:
    Ui::UiStatistics *ui;

    void setupUI();
    void setupConnections();
    QString bytesToString(const quint64 &bytes);
    QDateTime getUptime(const bool &conn);

};

#endif // UISTATISTICS_H
