#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QQuickWidget>
#include <QMessageBox>
#include <QMovie>
#include <QTimer>
#include <QLCDNumber>
#include <QQuickItem>
#include <QQmlEngine>
#include <QQmlContext>

#include "tcpclient/uitcpclient.h"
#include "tcpclient/tcpthread.h"
#include "systemlog/uisystemlog.h"
#include "events/message.h"
#include "customplot/customplotwidget.h"
#include "uistatistics.h"
#include "applib.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    Q_PROPERTY(CustomPlotWidget *pcustomPlot READ fpgetCustomPlot CONSTANT)

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    QScrollArea *m_scrollArea = nullptr;
    QHash<QString, QQuickWidget*> hqmlMnemonics; //хэш для хранения виджетов с загруженным Qml

    Message *m_messageWidget = nullptr;
    CustomPlotWidget *plotWidget = CustomPlotWidget::getInstance(this);

    STWsrv m_stwsrv;
    STWcli m_stwcli;
    TcpThread *tcpClient = TcpThread::getInstance();

    UiSystemLog m_uiSystemLog;
    UiStatistics m_uiStatistics;
    UiTcpClient m_uiTcpClient;

    void setupUI();
    void setupQML();
    void setupConnections();
    void setupMessageBar();

    void closeEvent(QCloseEvent *event) override; //переопределим closeEvent()
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void toggleFullScreen();
    void createQmlMnemonic(const QString &mnemonicName, const QUrl &url);
    void changeQmlMnemonic(const QString &mnemonicName);
    void updateQmlSize();
    void showStatusConnect(const bool &client, const bool &server);
    void showStatusbarMessage(const QString &status);

public slots:
    //Get from UI (Frontend)
    CustomPlotWidget *fpgetCustomPlot() { return CustomPlotWidget::getInstance(this); } //доступ к синглтону из Qml (сделал через контекст, т.к. если импортировать в Qml как синглВ сНГтон - не всегда работет)
    //Set from UI (Frontend)

signals:
    void fdataUpdate(const Data &);

};
#endif // MAINWINDOW_H
