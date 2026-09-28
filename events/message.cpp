#include "message.h"

Message::Message(QWidget *parent)
    : QWidget{parent}
{
    pmodelEvent = new ModelEvent(this);
    pmodelEvent->setMaxEvents(5000);

    this->setWindowTitle("Message");
    setupUI();
    setupConnections();
}

Message::~Message()
{

}

void Message::setupUI()
{
    auto tableView = new QTableView(this);
    pproxyModel = new ProxyEventModel(this);
    pproxyModel->setSourceModel(pmodelEvent);

    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->horizontalHeader()->setStretchLastSection(true); //растянуть последнюю колонку
    tableView->verticalHeader()->setDefaultSectionSize(20); //20px
    tableView->installEventFilter(this);
    tableView->setModel(pproxyModel);

    //подписываемся на изменение модели, для пересчета tableView (geometriesChanged срабатывает после всех изменений)
    connect(pproxyModel, &ProxyEventModel::rowsInserted, this, [this, tableView](){
        resizeTableView(tableView);
    });
    connect(pproxyModel, &ProxyEventModel::rowsRemoved, this, [this, tableView](){
        resizeTableView(tableView);
    });

    //1.родительскиий layout
    QVBoxLayout *layout = new QVBoxLayout(this);
    //2.добавили чекбоксы
    layout->addWidget(createGroupCheckBox());

    //QScrollArea виджет со скроллбарами для таблицы
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidget(tableView);
    scrollArea->setWidgetResizable(true);  //true - таблица tableView передает размер QScrollArea, меняя размер QScrollArea (работает вместе с политикой Expanding в tableView)
    scrollArea->setAlignment(Qt::AlignHCenter);
    scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); //политика QScrollArea растягивается по вертикали и горизонтали, Expanding - занимает все доступное пространство
    scrollArea->setFrameShape(QFrame::NoFrame); //без рамки

    //3.добавили виджет с таблицей
    layout->addWidget(scrollArea);
    layout->setContentsMargins(0, 0, 0, 0); //убираем отступы
}

void Message::setupConnections()
{
    auto tcpClient = TcpThread::getInstance();
    connect(tcpClient, &TcpThread::dataUpdated, this, [this, tcpClient](const Data &data){
        for (const Event &event : data.events) {
            //добавляем в модель, если только пришли новые сообщения в посылке
            if (pmodelEvent && data.events.last().id.toInt() != pmodelEvent->getEventId(0).toInt()) {
                pmodelEvent->append(QVariantMap{{"id", event.id},
                                                {"time", event.time},
                                                {"severity", event.severity},
                                                {"sourceName", event.sourceName},
                                                {"message", event.message}});
            }
        }
    });
    connect(tcpClient, &TcpThread::clientUpdated, this, [this](const STWcli &stw){
        if (!stw.conn && pmodelEvent) {
            pmodelEvent->clear();
        }
    });
}

QWidget *Message::createGroupCheckBox()
{
    /* создаем виджет с группой чекбоксов */
    QWidget *filterWidget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(filterWidget);

    QCheckBox *chkError = new QCheckBox("[Error]", filterWidget);
    QCheckBox *chkWarning = new QCheckBox("[Warning]", filterWidget);
    QCheckBox *chkInfo = new QCheckBox("[Info]", filterWidget);
    QCheckBox *chkSuccess = new QCheckBox("[Success]", filterWidget);
    QCheckBox *chkUser = new QCheckBox("[User]", filterWidget);

    layout->addStretch(); //растяжка слева
    layout->addWidget(chkError);
    layout->addWidget(chkWarning);
    layout->addWidget(chkInfo);
    layout->addWidget(chkSuccess);
    layout->addWidget(chkUser);
    layout->addStretch(); //растяжка справа

    //ф-ия сбора выбранных типов
    auto updateFilter = [this, chkError, chkWarning, chkInfo, chkSuccess, chkUser]() {
        QSet<QString> selectedTypes;
        if (chkError->isChecked()) selectedTypes.insert("[Error]");
        if (chkWarning->isChecked()) selectedTypes.insert("[Warning]");
        if (chkInfo->isChecked()) selectedTypes.insert("[Info]");
        if (chkSuccess->isChecked()) selectedTypes.insert("[Success]");
        if (chkUser->isChecked()) selectedTypes.insert("[User]");

        //если ничего не выбрано, показываем всё
        if (selectedTypes.isEmpty()) {
            if (pproxyModel) pproxyModel->clearFilter();
        }
        if (pproxyModel) pproxyModel->setFilterTypes(selectedTypes);
    };
    connect(chkError, &QCheckBox::toggled, updateFilter);
    connect(chkWarning, &QCheckBox::toggled, updateFilter);
    connect(chkInfo, &QCheckBox::toggled, updateFilter);
    connect(chkSuccess, &QCheckBox::toggled, updateFilter);
    connect(chkUser, &QCheckBox::toggled, updateFilter);

    return filterWidget;
}

void Message::resizeTableView(QTableView *table)
{
    /* троттлинг обновлений */
    static bool m_processing = false; //static - инициировали при первом вызове
    if (m_processing) return;
    m_processing = true;

    QTimer::singleShot(50, this, [this, table](){
        int totalWidth = 0;
        if (!table) return;
        table->resizeColumnsToContents();
        for (auto i = 0; i < table->horizontalHeader()->count(); ++i){
            totalWidth += table->horizontalHeader()->sectionSize(i);
        }
        totalWidth += table->verticalHeader()->width() + table->frameWidth() * 2 + table->verticalScrollBar()->width(); //отступы
        if (table->width() != totalWidth){
            table->setFixedWidth(totalWidth);
        }
        m_processing = false;
        // qDebug() << Q_FUNC_INFO << "geometriesChanged - Total width:" << totalWidth;
    });
}

