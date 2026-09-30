#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupUI();
    setupQML();
    setupConnections();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupUI()
{
    ui->actionQuit->setIcon(QIcon(":img/icon/mainmenu/close.svg"));
    ui->actionSystemLog->setIcon(QIcon(":img/icon/mainmenu/systemlog.svg"));
    ui->actionStatistics->setIcon(QIcon(":img/icon/mainmenu/statistics.svg"));
    ui->actionConnect->setIcon(QIcon(":img/icon/mainmenu/server.svg"));
    ui->actionFullScreen_F11->setIcon(QIcon(":img/icon/mainmenu/fullscreen.svg"));
    ui->actionAbout->setIcon(QIcon(":img/icon/mainmenu/about.svg"));
    ui->actionAboutQt->setIcon(QIcon(":img/icon/mainmenu/aboutqt.svg"));

    //tableView (поле с MessageBar)
    setupMessageBar();
    ui->pb_clearFirst->setIcon(QIcon(":img/icon/check_single.svg"));
    ui->pb_clearAll->setIcon(QIcon(":img/icon/check_double.svg"));

    //часы реального времени (правая часть экрана)
    auto lcdClock = new QLCDNumber(this);
    lcdClock->setSegmentStyle(QLCDNumber::Filled);
    lcdClock->setStyleSheet("color: red;");  //цвет
    lcdClock->setDigitCount(8); //8 значащих символов
    lcdClock->display("00:00:00");
    auto timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this, lcdClock](){
        lcdClock->display(QTime::currentTime().toString("hh:mm:ss"));
    });
    timer->start(1000);
    QVBoxLayout *labelLayout = new QVBoxLayout(ui->label_time);
    labelLayout->addWidget(lcdClock);
    labelLayout->setContentsMargins(0, 0, 0, 0);

    //логотип
    QMovie *movie = new QMovie(":/img/logo_slow.gif");
    movie->start();
    /* выполняем отложенное масштабирование и запуск т.к. layout'ы обновляются асинхронно, и размер некорректен */
    QTimer::singleShot(50, this, [this, movie](){
        movie->setScaledSize(QSize(ui->label_logo->height(), ui->label_logo->height()));
        ui->label_logo->setMovie(movie);
        ui->label_logo->setMinimumSize(QSize(ui->label_logo->height() * 1.5, ui->label_logo->height()));
        ui->label_logo->setAlignment(Qt::AlignCenter);

        ui->label_time->setMinimumSize(QSize(ui->label_logo->height() * 1.5, ui->label_logo->height()));
        ui->label_time->setAlignment(Qt::AlignCenter);
    });

    ui->hgb_panelLabel->setStyleSheet("QGroupBox {"
                                      "   border: 1px solid #e0e0e0;"
                                      "   border-radius: 1px;"
                                      "}");

    ui->label_MODE_DESCR->setStyleSheet("QLabel {"
                                        "    font-weight: bold;"
                                        "    font-size: 14px;"
                                        "    font-family: 'Arial';"
                                        "    color: #5a5a2d;"      //Темные оттенки
                                        "}");
    ui->label_STEP_DESCR->setStyleSheet("QLabel {"
                                        "    font-weight: bold;"
                                        "    font-size: 14px;"
                                        "    font-family: 'Arial';"
                                        "    color: #5a5a2d;"      //Темные оттенки
                                        "}");
    ui->label_COUNTDOWN->setStyleSheet("QLabel {"
                                       "    font-weight: bold;"
                                       "    font-size: 14px;"
                                       "    font-family: 'Arial';"
                                       "    color: #5a5a2d;"      //Темные оттенки
                                       "}");
    ui->label_COUNTDOWN->setText("Шаг:");

    //добавляем в stack виджет с сообщениями
    m_messageWidget = new Message(this);
    ui->stackedWidget->addWidget(m_messageWidget);

    //добавялем с стек виджет с графиками
    ui->stackedWidget->addWidget(plotWidget);

    //Layout с кнопками перехода по мнемосхемам
    QLayout *groupBoxLayout = ui->gb_button->layout();
    if (groupBoxLayout) {
        groupBoxLayout->setContentsMargins(0, 2, 0, 2); //отступы layout'а
        groupBoxLayout->setSpacing(5);                  //расстояние между элементами
    }
    ui->gb_button->setStyleSheet("QGroupBox {"
                                 "   border: 1px solid #e0e0e0;"
                                 "   border-radius: 1px;"
                                 "}");
    showStatusConnect(false, false);
}

void MainWindow::setupQML()
{
    //создаем QScrollArea
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(false);  //добавленный виджет не меняет размер отображения QScrollArea
    m_scrollArea->setAlignment(Qt::AlignCenter);
    m_scrollArea->setStyleSheet("QScrollArea { border: none; background: transparent; }");

    //инициализируем QML виджет, создаем Хэш
    createQmlMnemonic("MAIN", QUrl("qrc:/qml/Main.qml"));
    createQmlMnemonic("GTU", QUrl("qrc:/qml/GTU.qml"));

    ui->stackedWidget->addWidget(m_scrollArea);

    //загружаем начальную мнемосхему
    changeQmlMnemonic("GTU");
}

void MainWindow::setupConnections()
{
    //Mainmenu
    connect(ui->actionQuit, &QAction::triggered, this, &MainWindow::close);
    connect(ui->actionSystemLog, &QAction::triggered, this, [this](){
        m_uiSystemLog.close();
        m_uiSystemLog.show();
    });
    connect(ui->actionStatistics, &QAction::triggered, this, [this](){
        m_uiStatistics.close();
        m_uiStatistics.show();
    });
    connect(ui->actionFullScreen_F11, &QAction::triggered, this, [this](){
        toggleFullScreen();
    });
    connect(ui->actionConnect, &QAction::triggered, this, [this](){
        m_uiTcpClient.close();
        m_uiTcpClient.show();
    });
    connect(ui->actionAboutQt, &QAction::triggered, this, [this](){
        QMessageBox::aboutQt(this, "Application");
    });
    connect(ui->actionAbout, &QAction::triggered, this, [this](){
        QString message = QString("ООО \"ИНГК-Промтех\" \n"
                                  "г.Москва, Научный проезд 14А, строение 3, тел.+7(495)231-46-61 \n"
                                  "E-mail: info@ingc.ru \n"
                                  "GTViewer просмотр параметров станции \n \n"
                                  "%1\n"
                                  "%2\n"
                                  "%3\n"
                                  "%4\n"
                                  "%5\n").arg(AppLib::getAppVersion(),
                                   m_stwsrv.srvVersion,
                                   AppLib::getOpenSSLVersionString(),
                                   AppLib::getOSVersionString(),
                                   AppLib::getHostName());
        QMessageBox::about(this, "Application", message);
    });
    //Mnemonic
    connect(ui->pb_main, &QPushButton::clicked, this, [this](){
        changeQmlMnemonic("MAIN");
    });
    connect(ui->pb_gtu, &QPushButton::clicked, this, [this](){
        changeQmlMnemonic("GTU");
    });
    connect(ui->pb_events, &QPushButton::clicked, this, [this](){
        this->setWindowTitle("SCADA - " + m_messageWidget->windowTitle());    //обновляем заголовок окна
        ui->stackedWidget->setCurrentWidget(m_messageWidget); //устанавливаем текущий виджет
    });
    connect(ui->pb_charts, &QPushButton::clicked, this, [this](){
        this->setWindowTitle("SCADA - " + plotWidget->windowTitle());
        ui->stackedWidget->setCurrentWidget(plotWidget);
    });
    //TCP status
    connect(tcpClient, &TcpThread::clientUpdated, this, [this](const STWcli &stw){
        m_stwcli = stw;
        showStatusConnect(m_stwcli.conn, m_stwsrv.opcconn);
    });
    connect(tcpClient, &TcpThread::serverUpdated, this, [this](const STWsrv &stw){
        m_stwsrv = stw;
        showStatusConnect(m_stwcli.conn, m_stwsrv.opcconn);
    });
    connect(tcpClient, &TcpThread::status, this, [this](const QString &status){
        /* отображаем статусы tcpClient в статус баре manwindow */
        showStatusbarMessage(status);
    });
    connect(tcpClient, &TcpThread::dataUpdated, this, [this](const Data &data, const DataMTR &dataMTR){
        QString mode = data.MODE_DESCR.pv.toString();
        QString step = data.STEP_DESCR.pv.toString();
        QString count = QString("count: %1").arg(QString::number(data.COUNTDOWN.pv.toUInt()));
        if (ui->label_MODE_DESCR->text() != mode) ui->label_MODE_DESCR->setText(mode);
        if (ui->label_STEP_DESCR->text() != step) ui->label_STEP_DESCR->setText(step);
        if (ui->label_COUNTDOWN->text() != count) ui->label_COUNTDOWN->setText(count);
        emit fdataUpdate(data);
        emit fdataUpdateMTR(dataMTR);
    });
    connect(tcpClient, &TcpThread::tagChanged, this, [this](const QVariant &tagname, const TagValue &tagValue){
        plotWidget->addPointToGraph(tagname, tagValue);

    });
    connect(tcpClient, &TcpThread::addHisoryData, this, [this](const QVariant &tagname, const QList<TagValue> &tagValues){
        plotWidget->addPointsToGraph(tagname, tagValues);
    });
}

void MainWindow::setupMessageBar()
{
    auto modelEvent = new ModelEvent(this);
    modelEvent->setMaxEvents(50);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableView->setModel(modelEvent);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->horizontalHeader()->setStretchLastSection(true); //растянуть последнюю колонку
    ui->tableView->verticalHeader()->setDefaultSectionSize(20); //20px
    connect(modelEvent, &ModelEvent::rowsInserted, this, [this](){
        QTimer::singleShot(20, this, [this]() {
            ui->tableView->scrollToTop();
            ui->tableView->viewport()->update(); //принудительно обновляем viewport
        });
    });
    connect(ui->pb_clearFirst, &QPushButton::clicked, this, [this, modelEvent](){
        modelEvent->removeFirst();
    });
    connect(ui->pb_clearAll, &QPushButton::clicked, this, [this, modelEvent](){
        modelEvent->clear();
    });

    //Заполняем модель
    connect(tcpClient, &TcpThread::dataUpdated, this, [this, modelEvent](const Data &data){
        for (const Event &event : data.events) {
            //добавляем в модель, если только пришли новые сообщения в посылке
            if (modelEvent && data.events.last().id.toInt() != modelEvent->getEventId(0).toInt()) {
                modelEvent->append(QVariantMap{{"id", event.id},
                                               {"time", event.time},
                                               {"severity", event.severity},
                                               {"sourceName", event.sourceName},
                                               {"message", event.message}});
            }
        }
    });
    connect(tcpClient, &TcpThread::clientUpdated, this, [this, modelEvent](const STWcli &stw){
        if (!stw.conn && modelEvent) {
            modelEvent->clear();
        }
    });
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    m_uiStatistics.close();
    m_uiSystemLog.close();
    m_uiTcpClient.close();
    plotWidget->close();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    updateQmlSize();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_F11) {
        toggleFullScreen();
    } else {
        QMainWindow::keyPressEvent(event);
    }
}

void MainWindow::toggleFullScreen()
{
    if (this->isFullScreen()) {
        // this->setWindowFlags(windowFlags() & ~Qt::WindowStaysOnTopHint);
        this->showNormal();
    } else {
        // this->setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint); //показать поверх всех окон и скрыть панель задач
        this->showFullScreen();
    }
}

void MainWindow::createQmlMnemonic(const QString &mnemonicName, const QUrl &url)
{
    if (mnemonicName.isEmpty() || url.isEmpty()) return;

    auto quickWidget = new QQuickWidget();
    hqmlMnemonics.insert(mnemonicName, quickWidget); //добавили созданный виджет в хэш
    quickWidget->setResizeMode(QQuickWidget::SizeViewToRootObject); //устанавливаем размеры QQuickWidget в соответствии с QML
    //обработка статусов готовности виджета с QML
    connect(quickWidget, &QQuickWidget::statusChanged, this, [this, quickWidget, mnemonicName](QQuickWidget::Status status){
        if (status == QQuickWidget::Ready) {
            QQuickItem *proot = quickWidget->rootObject();
            if (proot) {
                qDebug() << Q_FUNC_INFO << mnemonicName << "Size QML:" << proot->width() << proot->height();
            }
        }
        if (status == QQuickWidget::Error) {
            qDebug() << Q_FUNC_INFO << mnemonicName << "QML Errors:" << quickWidget->errors();
        }
    });
    //передаем общий контекст при создании (можно передавать или обновлять при вызове в методе changeQmlMnemonic())
    quickWidget->engine()->rootContext()->setContextProperty("appEng", this);
    //загружаем QML
    quickWidget->setSource(url);
}

void MainWindow::changeQmlMnemonic(const QString &mnemonicName)
{
    if (auto it = hqmlMnemonics.find(mnemonicName); it != hqmlMnemonics.end()) {
        auto quickWidget = it.value(); //получаем доступ к QQuickWidget (QML виджет)
        auto takeWidget = m_scrollArea->takeWidget();
        Q_UNUSED(takeWidget);
        if (quickWidget && m_scrollArea) {
            m_scrollArea->setWidget(quickWidget);

            this->setWindowTitle("SCADA - " + mnemonicName); //обновляем заголовок окна
            ui->stackedWidget->setCurrentWidget(m_scrollArea); //устанавливаем текущий виджет
            //чтобы успела загрузить сцена Qml, пересчет размеров вызываем в следующем цикле событий!!!!
            QMetaObject::invokeMethod(this, [this](){
                updateQmlSize();
            }, Qt::QueuedConnection);
        }
    }
}

void MainWindow::updateQmlSize()
{
    /* Обновляем размер текущего QML виджета */
    static const QSize MIN_SIZE(1280, 720); //HD
    if (m_scrollArea && m_scrollArea->widget()) {
        QQuickWidget *quickWidget = qobject_cast<QQuickWidget*>(m_scrollArea->widget());
        if (quickWidget && quickWidget->rootObject()) {
            QSize newSize = m_scrollArea->viewport()->size();
            if (newSize.width() > MIN_SIZE.width() && newSize.height() > MIN_SIZE.height()) {
                quickWidget->resize(newSize); //устанавливаем размер виджета
                quickWidget->rootObject()->setSize(newSize); //устанавливаем размер корневого элемента QML
                // qDebug() << Q_FUNC_INFO << "Resized QML to:" << newSize;
            } else {
                quickWidget->resize(MIN_SIZE);
                quickWidget->rootObject()->setSize(MIN_SIZE);
            }
        }
    }
}

void MainWindow::showStatusConnect(const bool &client, const bool &server)
{
    static bool m_warningShown = false;

    if (client && server) {
        QPixmap pixmap(":img/icon/success.svg");
        ui->label_conn->setPixmap(pixmap.scaled(28, 28, Qt::KeepAspectRatio));

        m_warningShown = false; //повторно предупреждение показываем только после того как будет удачный коннект
    } else {
        QPixmap pixmap(":img/icon/successErr.svg");
        ui->label_conn->setPixmap(pixmap.scaled(28, 28, Qt::KeepAspectRatio));

        //всплывающее предупреждение
        if (!m_warningShown) {
            m_warningShown = true;
            QTimer::singleShot(2500, this, [this](){
                if (!(m_stwcli.conn && m_stwsrv.opcconn)) {
                    int result = QMessageBox::warning(this, "✗ Disconnect", "Отсутствует связь с сервером данных!", QMessageBox::Ok);
                    if (result == QMessageBox::Ok){}
                }
            });
        }
    }
}

void MainWindow::showStatusbarMessage(const QString &status)
{
    /* отображаем сообщение в статус баре */
    //удаляем старые виджеты
    const QList<QLabel*> oldLabels = ui->statusbar->findChildren<QLabel*>();
    for (QLabel *label : oldLabels) {
        ui->statusbar->removeWidget(label);
        label->deleteLater();
    }

    //создаем и добавляем новые виджеты
    QLabel *label = new QLabel(this);
    QColor color;
    if (status.contains("[Error]")) color = QColor(255, 0, 0);            //красный
    else if (status.contains("[Warning]")) color = QColor(255, 167, 0);   //оранжевый
    else if (status.contains("[Info]")) color = QColor(51, 51, 255);      //синий
    else if (status.contains("[Success]")) color = QColor(0, 128, 0);     //зеленый
    else color = QColor(10, 9, 9); //черный

    //применяем цвет через HTML
    label->setText(QString("<span style='color: %1;'>%2</span>").arg(color.name(), status.toHtmlEscaped()));
    label->setTextFormat(Qt::RichText);
    ui->statusbar->addWidget(label);

    //удаляем виджет
    QPointer<QLabel> labelPtr = label;
    QTimer::singleShot(10000, this, [this, labelPtr](){
        if (labelPtr) {  // Проверяем, существует ли еще виджет
            ui->statusbar->removeWidget(labelPtr);
            labelPtr->deleteLater();
        }
    });
}
