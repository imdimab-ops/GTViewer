/* Класс с сообщениями для добавления виджет ui->stackedWidget */

#ifndef MESSAGE_H
#define MESSAGE_H

#include <QWidget>
#include <QTableView>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QScrollArea>
#include <QScrollBar>

#include "modelevent.h"
#include "proxyeventmodel.h"
#include "../tcpclient/tcpthread.h"

class Message : public QWidget
{
    Q_OBJECT
public:
    explicit Message(QWidget *parent = nullptr);
    ~Message();

private:
    ModelEvent *pmodelEvent = nullptr;
    ProxyEventModel *pproxyModel = nullptr;

    void setupUI();
    void setupConnections();

    QWidget *createGroupCheckBox();
    void resizeTableView(QTableView *table);

signals:

};

#endif // MESSAGE_H
