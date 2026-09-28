#include "customplotwidget.h"

CustomPlotWidget::CustomPlotWidget(QWidget *parent)
    : QWidget{parent}
{
    this->setWindowTitle("Charts");

    /* 1.инициализируем QCustomPlot,
     * 2.добавляем уже созданый QCustomPlot на виджет */
    initCustomPlot();
    setupUI();
    setupConnections();
}

CustomPlotWidget::~CustomPlotWidget()
{
    this->setEnabled(false); //Отключаем все входящие события
    if (m_customPlot) {
        m_customPlot->clearGraphs();
        delete m_customPlot;
        m_customPlot = nullptr;
        m_cursorLine1 = nullptr; //создан как дочерний элемент m_customPlot, удаляется при удалении m_customPlot
        m_cursorLine2 = nullptr; //создан как дочерний элемент m_customPlot, удаляется при удалении m_customPlot

    }
    qDebug() << Q_FUNC_INFO;
}

void CustomPlotWidget::setupUI()
{
    m_customPlot->setBackground(QColor(242, 242, 242));

    QGridLayout *gridLayout = new QGridLayout(this);
    gridLayout->setContentsMargins(0, 0, 0, 0);
    gridLayout->setSpacing(0);
    gridLayout->addWidget(m_customPlot, 0, 0, 1, 1);  //добавляем график в нужную ячейку

    //верхняя панель
    QWidget *topPanel = new QWidget(this);
    topPanel->setFixedHeight(35);
    topPanel->setStyleSheet(
        "QWidget {"
        "   background-color: rgba(44, 62, 80, 51);" //80% прозрачность (255 * 0.2 ≈ 51)
        "   border-bottom: 1px solid rgba(52, 73, 94, 51);"
        "}"
        );

    QHBoxLayout *topLayout = new QHBoxLayout(topPanel);
    topLayout->setContentsMargins(10, 2, 10, 2);
    topLayout->setSpacing(10);

    QPushButton *btn = new QPushButton("📊 Параметры", topPanel);
    btn->setStyleSheet(
        "QPushButton {"
        "   background-color: rgba(52, 152, 219, 200);" //~78% непрозрачности
        "   color: white;"
        "   border: none;"
        "   border-radius: 4px;"
        "   padding: 5px 15px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background-color: rgba(41, 128, 185, 230);"
        "}"
        "QPushButton:pressed {"
        "   background-color: rgba(26, 82, 118, 255);"
        "}"
        );
    topLayout->addWidget(btn);

    connect(btn, &QPushButton::clicked, this, [this](){
        m_fieldSelectorDialog.close();
        m_fieldSelectorDialog.open();
    });
    topLayout->addStretch(); //заглушка справа

    //чекбокс с включением курсора
    QCheckBox *chb_cursorLine1 = new QCheckBox("👆 Cursor Line 1", topPanel);
    chb_cursorLine1->setStyleSheet(
        "QCheckBox {"
        "   color: white;"
        "   font-weight: bold;"
        "   font-size: 12px;"
        "   background-color: transparent;"
        "   spacing: 8px;"
        "}"
        );
    chb_cursorLine1->setChecked(false);
    topLayout->addWidget(chb_cursorLine1);
    connect(chb_cursorLine1, &QCheckBox::checkStateChanged, this, [this](Qt::CheckState state){
        fsetCursorLine(m_cursorLine1, state == Qt::Checked);
    });

    //чекбокс с включением курсора
    QCheckBox *chb_cursorLine2 = new QCheckBox("👆 Cursor Line 2", topPanel);
    chb_cursorLine2->setStyleSheet(
        "QCheckBox {"
        "   color: white;"
        "   font-weight: bold;"
        "   font-size: 12px;"
        "   background-color: transparent;"
        "   spacing: 8px;"
        "}"
        );
    chb_cursorLine2->setChecked(false);
    topLayout->addWidget(chb_cursorLine2);
    connect(chb_cursorLine2, &QCheckBox::checkStateChanged, this, [this](Qt::CheckState state){
        fsetCursorLine(m_cursorLine2, state == Qt::Checked);
    });

    m_chbInterpolation = new QCheckBox("Interpolation", topPanel);
    m_chbInterpolation->setIcon(QApplication::style()->standardIcon(QStyle::SP_ComputerIcon));
    m_chbInterpolation->setStyleSheet(
        "QCheckBox {"
        "   color: white;"
        "   font-weight: bold;"
        "   font-size: 12px;"
        "   background-color: transparent;"
        "   spacing: 8px;"
        "}"
        );
    m_chbInterpolation->setChecked(false);
    topLayout->addWidget(m_chbInterpolation);

    //чекбокс с включенным автоскролом
    m_chbAScroll = new QCheckBox("▶ Autoscroll", topPanel);
    m_chbAScroll->setStyleSheet(
        "QCheckBox {"
        "   color: white;"
        "   font-weight: bold;"
        "   font-size: 12px;"
        "   background-color: transparent;"
        "   spacing: 8px;"
        "}"
        );
    m_chbAScroll->setChecked(true);
    topLayout->addWidget(m_chbAScroll);

    //метка с количеством точек на графиках
    m_labelPoints = new QLabel("📈 Точки: 0", topPanel);
    m_labelPoints->setStyleSheet(
        "QLabel {"
        "   color: white;"
        "   font-weight: bold;"
        "   font-size: 12px;"
        "   background-color: transparent;"
        "}"
        );
    topLayout->addWidget(m_labelPoints);

    //информационная метка
    QLabel *infoLabel = new QLabel("📈 Графики", topPanel);
    infoLabel->setStyleSheet("QLabel { color: white; font-weight: bold; }");
    infoLabel->setToolTip(QString::fromUtf8("Используйте Ctrl/Shift + колесо мыши для масштабирования графиков"));
    topLayout->addWidget(infoLabel);

    gridLayout->addWidget(topPanel, 0, 0, 1, 1, Qt::AlignTop); //добавляем панель в ту же ячейку что и график

    //легенда
    m_legendWidget = new QWidget(this);
    m_legendWidget->setStyleSheet(
        "QWidget {"
        "   background-color: rgba(44, 62, 80, 200)"
        "   border-top: 1px solid rgba(52, 73, 94, 100);"
        "}"
        );
    m_legendWidget->setVisible(false); //по умолчанию скрыта

    QHBoxLayout *legendLayout = new QHBoxLayout(m_legendWidget);
    legendLayout->setContentsMargins(5, 2, 5, 2);
    legendLayout->setSpacing(2);

    //сама таблица с легендой
    m_legendTable = new QTableWidget(m_legendWidget);
    m_legendTable->setStyleSheet(
        "QTableWidget {"
        "   background-color: rgba(143, 154, 166, 200);"
        "   color: white;"
        "   gridline-color: rgba(80, 100, 120, 80);"
        "}"
        "QTableWidget::item {"
        "   background-color: transparent;"
        "   padding: 0px 8px;"
        "   height: 18px;"
        "}"
        "QTableWidget::item:selected {"
        "   background-color: rgba(52, 152, 219, 80);"
        "   color: white;"
        "}"
        "QHeaderView::section {"
        "   background-color: rgba(52, 73, 94, 150);"
        "   color: white;"
        "   padding: 1px 4px;"
        "   height: 20px;"
        "}"
        );
    m_legendTable->setAlternatingRowColors(false);
    m_legendTable->setShowGrid(true);
    m_legendTable->setSelectionMode(QAbstractItemView::NoSelection);
    m_legendTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_legendTable->verticalHeader()->setVisible(true);
    m_legendTable->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    m_legendTable->horizontalHeader()->setStretchLastSection(false);
    m_legendTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_legendTable->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_legendTable->setStyle(QStyleFactory::create("Fusion"));
    // m_legendTable->setFrameShape(QFrame::NoFrame); //убираем рамку виджета

    legendLayout->addStretch();
    legendLayout->addWidget(m_legendTable);
    legendLayout->addStretch();

    gridLayout->addWidget(m_legendWidget, 1, 0, 1, 1);

    //настройка растяжения
    gridLayout->setRowStretch(0, 0); //верхняя панель - без растяжения
    gridLayout->setRowStretch(0, 1); //график - растягивается
    gridLayout->setRowStretch(1, 0); //легенда - без растяжения (фиксированная высота)
}

void CustomPlotWidget::setupConnections()
{
    connect(&m_fieldSelectorDialog, &FieldSelectorDialog::addGraph, this, [this](const QVariant &var){
        /* Виджет FieldSelectorDialog с перечнем тегов для добавления на график заполняется в момент регестрации всех
         * компоненов AI на стороне Qml, переменные с тегами "tagname" в AI.qml для запроса исторических данных
         * записываются в момент начала обмена с сервером данных, следовательно в момент заполнения виджета переменные "tagname" - пустые
         * Поэтому при нажатии чекбокса в FieldSelectorDialog с нужним тегом, формируется сигнал с параметрами тега,
         * далее в "connectionsLogic.js" подключен коннект к этому сигналу femitAddGraphW и по известным параметрам в "_componentStore"
         * выполняется поиск компонента AI, из которого беру недостающий параметр "tagname".
         * Поэтому добавление графиков и вызов метода fsetAddGraph(const QVariant &var) выполняется на стороне Qml (connectionsLogic.js) */
        emit femitAddGraphW(var);
    });
    connect(this, &CustomPlotWidget::femitStateGgaph, this, [this](const QVariant &var){
        PlotMetadata plotMetadata = parsePlotMetadata(var);
        m_fieldSelectorDialog.updateMetadata(plotMetadata.added, plotMetadata.symbol);
    });

    /* пересчет координат визирной линии
     * static_cast<void (QCPAxis::*)(const QCPRange &)>(&QCPAxis::rangeChanged - т.к. сигнал rangeChanged перегружен и имеет разные аргументы
     * поэтому явно указываем какой сигнал использовать */
    connect(m_customPlot->xAxis, static_cast<void (QCPAxis::*)(const QCPRange &)>(&QCPAxis::rangeChanged), this, [this](const QCPRange &range) {
        Q_UNUSED(range);
        fixCursorPositions();
    });
    connect(m_customPlot->yAxis, static_cast<void (QCPAxis::*)(const QCPRange &)>(&QCPAxis::rangeChanged), this, [this](const QCPRange &range) {
        Q_UNUSED(range);
        fixCursorPositions();
    });
}

void CustomPlotWidget::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    m_fieldSelectorDialog.close();
}

CustomPlotWidget *CustomPlotWidget::getInstance(QWidget *parent)
{
    /* Делаем так, т.к. CustomPlotWidget - является виджетом,
     * чтобы он по иерархии разрушался при разрушении первого родительского виджета
     * при последующих вызовах проверять на nullptr!!! */
    if (!instance && parent) {
        instance = new CustomPlotWidget(parent);
    }
    return instance;
}

void CustomPlotWidget::initCustomPlot()
{
    if (m_customPlot) return;

    m_customPlot = new QCustomPlot(this);
    m_customPlot->installEventFilter(this);
    connect(m_customPlot, &QCustomPlot::afterReplot, this, [this](){ update(); }); //л-функция (обновление после replot)
    // updateCustomPlotSize();
    //оптимизация производительности
    // m_customPlot->setOpenGl(on, 8);
    // m_customPlot->setNotAntialiasedElements(QCP::aeAll);  //отключаем сглаживание для всех элементов при движении
    m_customPlot->setNoAntialiasingOnDrag(true);          //отключаем сглаживания для элементов графика

    //Включаем возможность перемещения, зумирования, выбор графиков
    m_customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iMultiSelect | QCP::iSelectPlottables);

    //Настраиваем оси (они будут использованы для первого графика)
    m_customPlot->axisRect()->setAutoMargins(QCP::msNone);        //Отключить автоматические отступы
    m_customPlot->axisRect()->setMargins(QMargins(15, 0, 0, 15)); //Установить минимальные отступы
    m_customPlot->plotLayout()->setMargins(QMargins(0, 0, 0, 0)); //Установить отступы макета

    //Настройки шрифта
    QFont font("DejaVu Sans", 10, QFont::Normal);
    font.setStyleStrategy(QFont::NoAntialias); //для четкого отображения
    // font.setStyleStrategy(QFont::PreferAntialias);      //Сглаживание ВКЛ
    font.setHintingPreference(QFont::PreferVerticalHinting); //Только вертикальный хинтинг
    font.setWeight(QFont::Normal);
    font.setStyleHint(QFont::SansSerif);                // Без засечек

    m_customPlot->xAxis->setRange(0, 12);
    m_customPlot->xAxis->setTickLabelPadding(0);  //Отступ до значений оси
    m_customPlot->xAxis->setTickLabelFont(font);
    //Устанавливаем формат оси X как время
    // QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
    // dateTicker->setDateTimeFormat("hh:mm:ss");
    // m_customPlot->xAxis->setTicker(dateTicker);
    QSharedPointer<MilliSecondTicker> msecTicker(new MilliSecondTicker()); //кастомный формат в msec
    m_customPlot->xAxis->setTicker(msecTicker);

    m_customPlot->yAxis->setRange(0, 5);
    m_customPlot->yAxis->setLabelFont(font);
    m_customPlot->yAxis->setTickLabelPadding(-2);
    m_customPlot->yAxis->setTickLabelRotation(-90);
    m_customPlot->yAxis->setTickLabelFont(font);
    m_customPlot->yAxis->setLabelPadding(-12);

    if (!m_customPlot->size().isNull()) m_customPlot->replot();    //Перед отрисовкой проверям размер области, должна быть не 0, иначе crash

    m_customPlot->addLayer("cursorLayer", 0, QCustomPlot::LayerInsertMode::limBelow); //Размещаем на слое cursorLayer (limBelow - под слоем)
    m_customPlot->layer("cursorLayer")->setMode(QCPLayer::lmBuffered);

    //Инициализация визирных линий
    m_cursorLine1 = new QCPItemStraightLine(m_customPlot);
    m_cursorLine1->setPen(QPen(Qt::black, 1, Qt::SolidLine));
    m_cursorLine1->point1->setCoords(0, 0);
    m_cursorLine1->point2->setCoords(0, 1);
    m_cursorLine1->setVisible(false);
    m_cursorLine1->setProperty("checked", false); //свойство выбора, чтобы при перетаскивании линии не слипались
    m_cursorLine1->setLayer("cursorLayer"); //Размещаем на слое

    m_cursorLine2 = new QCPItemStraightLine(m_customPlot);
    m_cursorLine2->setPen(QPen(Qt::black, 1, Qt::SolidLine));
    m_cursorLine2->point1->setCoords(0, 0);
    m_cursorLine2->point2->setCoords(0, 1);
    m_cursorLine2->setVisible(false);
    m_cursorLine1->setProperty("checked", false); //свойство выбора, чтобы при перетаскивании линии не слипались
    m_cursorLine2->setLayer("cursorLayer"); //Размещаем на слое cursorLayer

    qDebug() << Q_FUNC_INFO << "Initializing QCustomPlot with size:" << this->width() << "x" << this->height();
}

void CustomPlotWidget::updateCustomPlotSize()
{
    /* используем, если добавляем виджет с QCustomPlot на прямую, т.к. его размеры будут равны 0,
     * если добавляем через Layout, то размеры задает сам контейнер */
    if (!m_customPlot) return;
    m_customPlot->setGeometry(this->rect());
    m_customPlot->setViewport(this->rect());
}

bool CustomPlotWidget::addGraph(const PlotMetadata &metadata)
{
    if (!m_customPlot) return false;
    if (GRAPH_SIZE <= m_customPlot->graphCount()) return false;
    auto isFirstGraph = (m_customPlot->graphCount() == 0 || m_customPlot->yAxis->label().isEmpty());

    QCPGraph *graph = m_customPlot->addGraph();
    if (!graph) return false;
    graph->setName(QString("%1_%2").arg(QString::number(m_customPlot->graphCount()), metadata.name));
    graph->setProperty("symbol", metadata.symbol);
    graph->setProperty("unit", metadata.unit);
    graph->setProperty("description", metadata.description);
    graph->setProperty("tagname", metadata.tagname); //т.к. графиков не много через хэш не стал хранить

    //настройки, которые позволили убрать тормоза при перемещении графиков
    graph->setAdaptiveSampling(true); //адаптивная выборка (прореживание)
    QCPSelectionDecorator *decorator = graph->selectionDecorator(); //стиль выделенной линии
    decorator->setPen(QPen(Qt::blue, 1, Qt::DashDotLine));

    //настраиваем цвет графика
    graph->setPen(m_storeSettingGraph[m_settingGraphCount].color);
    graph->setScatterStyle(m_storeSettingGraph[m_settingGraphCount].scatter);
    m_settingGraphCount++;
    if (m_settingGraphCount >= m_storeSettingGraph.size() - 1) m_settingGraphCount = 0;

    //ось Y
    if (isFirstGraph) {
        //привязываем первый график к существующим осям
        graph->setKeyAxis(m_customPlot->xAxis);   //существующая ось X
        graph->setValueAxis(m_customPlot->yAxis); //существующая ось Y

        m_customPlot->yAxis->setLabel(graph->name());
        m_customPlot->yAxis->setLabelColor(graph->pen().color());
        m_customPlot->yAxis->setBasePen(graph->pen().color());
        m_customPlot->yAxis->setTickPen(graph->pen().color());
        m_customPlot->yAxis->setTickLabelColor(graph->pen().color());
        m_customPlot->yAxis->setSubTickPen(graph->pen().color());
    } else {
        QCPAxisRect *axisRect = m_customPlot->axisRect();
        QCPAxis *yAxis = new QCPAxis(axisRect, QCPAxis::atLeft);
        axisRect->addAxis(QCPAxis::atLeft, yAxis);  //Добавляем оси
        const auto axisCount = axisRect->axes(QCPAxis::atLeft).size();
        const auto offset = AXIS_SPACING * (axisCount - 1);   //Adjust multiplier as needed

        // yAxis->setPadding(-10);
        yAxis->setOffset(offset);
        yAxis->setRange(0, 5);
        yAxis->setLabel(graph->name());
        yAxis->setLabelColor(graph->pen().color());
        yAxis->setLabelPadding(m_customPlot->yAxis->labelPadding());
        yAxis->setLabelFont(m_customPlot->yAxis->labelFont());

        yAxis->setBasePen(graph->pen().color());       //цвет линии
        yAxis->setTickPen(graph->pen().color());       //основные засечки
        yAxis->setTickLabelColor(graph->pen().color());//цвет чисел
        yAxis->setTickLabelPadding(m_customPlot->yAxis->tickLabelPadding());
        yAxis->setTickLabelRotation(m_customPlot->yAxis->tickLabelRotation());
        yAxis->setTickLabelFont(m_customPlot->yAxis->tickLabelFont());
        yAxis->setSubTickPen(graph->pen().color());    //цвет стрелки

        //Сдвигаем весь QCPAxisRect вправо, чтобы ось X начиналась после всех Y
        axisRect->setMargins(QMargins(offset + 15,  //Левый отступ (основной сдвиг)
                                      0,            //Верхний отступ
                                      0,            //Правый отступ
                                      15));         //Нижний отступ (для подписей X)

        //Привязываем созданный график к нужным осям
        graph->setKeyAxis(m_customPlot->xAxis);   //Ось X (ключевая)
        graph->setValueAxis(yAxis); //Ось Y (значений)

        //connect для синхронизации движения осей
        //connect к Л-функции, рассчитываем изменение нижней границы базовой шкалы, и на это изменение перемещаем остальные оси (синхронизация перемещения всех осей)
        QMetaObject::Connection conn = connect(m_customPlot->yAxis,
                                               static_cast<void (QCPAxis::*)(const QCPRange &, const QCPRange &)>(&QCPAxis::rangeChanged), //т.к. два сигнала QCPAxis::rangeChanged
                                               this, [this, yAxis](const QCPRange &newRange, const QCPRange &oldRange) {
                                                   auto moveY = (newRange.center() - oldRange.center()) * yAxis->range().size() / newRange.size();    //рассчитываем пермещение с учетом масштаба текущей и базовых осей
                                                   if (m_customPlot->selectedGraphs().isEmpty()) yAxis->moveRange(moveY);   //если выбран один из графиков для перемещения то отключаем перемещение всей сцены
                                                   // qDebug() << "dY" << moveY << newRange.lower;
                                               });
        //сохраняем connect в хэш (чтобы при удалении графика можно было удалить хэш
        m_hashAxisConnections.insert(yAxis, conn);
        // qDebug() << Q_FUNC_INFO << m_customPlot->graphCount() << axisCount << offset;

    }

    if (m_cursorLine1 && m_cursorLine2) createdLegend(); //если открыта визирка, перестраиваем легенду
    if (!m_customPlot->size().isNull()) m_customPlot->replot(QCustomPlot::rpQueuedReplot);    //перед отрисовкой проверяем размер области, должна быть не 0, иначе crash
    emit addedGraph(graph);
    return true;
}

void CustomPlotWidget::removeGraph(QCPGraph *graph)
{
    if (!m_customPlot || !graph) return;

    //1.очищаем UI-структуры, ссылающиеся на граф
    clearLegend();
    //2.cохраняем диапазоны всех осей
    QHash<QCPAxis*, QCPRange> hAxisRanges;
    QList<QCPAxis*> allAxes = m_customPlot->axisRect()->axes();
    for (QCPAxis *axis : std::as_const(allAxes)) {
        if (axis) hAxisRanges[axis] = axis->range();
    }
    bool isBaseAxis = false;
    QCPAxis *yAxis = graph->valueAxis();

    if (yAxis) { //Проверяем, что это не главная ось
        isBaseAxis = (yAxis == m_customPlot->yAxis);
        if (isBaseAxis) {
            yAxis->setRange(0, 5);
            yAxis->setLabel("");
            yAxis->setLabelColor(QColor(Qt::black));
            yAxis->setBasePen(QPen(Qt::black));
            yAxis->setTickPen(QPen(Qt::black));
            yAxis->setTickLabelColor(Qt::black);
            yAxis->setSubTickPen(QPen(Qt::black));
        } else {
            removeAxis(yAxis);
        }
    }
    //3.удаляем граф
    m_customPlot->removeGraph(graph); //(QCustomPlot автоматически удалит связанные оси)

    //4.переносим первый график на базовую ось, если она освобождена
    if (isBaseAxis && m_customPlot->graphCount() > 0) {
        QCPGraph *firstGraph = m_customPlot->graph(0);
        if (firstGraph) {
            QCPAxis *firstAxis = firstGraph->valueAxis();
            //Удаляем старую ось, если на ней нет графиков
            if (firstAxis && firstAxis != m_customPlot->yAxis) {
                firstGraph->setValueAxis(m_customPlot->yAxis); //переносим график на базовую ось

                /* берем из хэша hAxisRanges сохраненный диапазон, привязанной к данной оси до удаления графика,
                 * если брать диапазон после операций удаления с графиком из указателя "firstAxisа", то он не будет
                 * соответствовать тому диапазону, который был до удаления первого графика и сдвига ветора "m_customPlot->graph(0)" */
                if (auto it = hAxisRanges.find(firstAxis); it != hAxisRanges.end()) { //C++17 init-statement, it живет только внутри if
                    m_customPlot->yAxis->setRange(it.value());
                }
                m_customPlot->yAxis->setLabel(firstGraph->name());
                m_customPlot->yAxis->setLabelColor(firstGraph->pen().color());
                m_customPlot->yAxis->setBasePen(firstGraph->pen().color());
                m_customPlot->yAxis->setTickPen(firstGraph->pen().color());
                m_customPlot->yAxis->setTickLabelColor(firstGraph->pen().color());
                m_customPlot->yAxis->setSubTickPen(firstGraph->pen().color());
                if (firstAxis->graphs().isEmpty()) removeAxis(firstAxis); //удаляем освободившуюся ось, тк на ней нет графиков
            }
        }
    }
    //5.восстанавливаем диапазоны для всех остальных осей
    QList<QCPAxis*> updatedAxes = m_customPlot->axisRect()->axes();
    for (QCPAxis *axis : std::as_const(updatedAxes)) {
        /* в векторе указателей на оси "updatedAxes" хранится указатель на базовую ось, которая принадлежит "m_customPlot->yAxis",
         * соответственно, при обновлении через хэш hAxisRanges, диапазон базовой оси просто перезапишется на тот который был в момент сохранения,
         * поэтому при обновлении диапазонов осей, обновляем кроме базовой оси "axis != m_customPlot->yAxis", т.к. она обновлена ранее в условии */
        if (axis && axis != m_customPlot->yAxis) {
            if (auto it = hAxisRanges.find(axis); it != hAxisRanges.end()) {
                axis->setRange(it.value());
            }
        }
    }
    hAxisRanges.clear();
    qDebug() << Q_FUNC_INFO << allAxes.size() << updatedAxes.size();
    updateAxisMargins();
    if (!m_customPlot->size().isNull()) m_customPlot->replot(QCustomPlot::rpQueuedReplot); //асинхронная перестройка
    if (m_cursorLine1 && m_cursorLine2) createdLegend(); //если открыта визирка, перестраиваем легенду
    updateGraphDataCount();
}

void CustomPlotWidget::removeAxis(QCPAxis *axis)
{
    if (!m_customPlot || !axis) return;
    m_customPlot->axisRect()->removeAxis(axis);
    //разрываем connect для данной оси
    if (auto it = m_hashAxisConnections.find(axis); it != m_hashAxisConnections.end()) {
        disconnect(it.value()); //Разрываем соединение
        m_hashAxisConnections.erase(it); //Удаляем из хэша
    }
    axis = nullptr; //QCustomPlot удаляет ось в методе removeAxis()
}

QCPGraph *CustomPlotWidget::pgetGraphByTag(const QString &tagname)
{
    /* имена тэгов храним как свойста, т.к. графиков не много через хэш не стал делать */
    if (!m_customPlot) return nullptr;
    for (auto i = 0; i < m_customPlot->graphCount(); ++i) {
        QCPGraph *graph = m_customPlot->graph(i);
        if (graph && graph->property("tagname").toString() == tagname) {
            return graph;
        }
    }
    return nullptr;
}

void CustomPlotWidget::updateAxisMargins()
{
    /* обновление отступов, после удаления графика */
    if (!m_customPlot) return;
    QCPAxisRect *axisRect = m_customPlot->axisRect();
    if (!axisRect) return;

    QList<QCPAxis*> leftAxes = axisRect->axes(QCPAxis::atLeft);
    int maxOffset = 0;
    //находим максимальный отступ среди всех осей Y
    for (int i = 0; i < leftAxes.size(); ++i) {
        QCPAxis *yAxis = leftAxes[i];
        if (!yAxis) continue;
        if (yAxis != m_customPlot->yAxis) { //Игнорируем главную ось
            yAxis->setOffset(AXIS_SPACING * i);
            maxOffset = qMax(maxOffset, yAxis->offset());
        }
    }
    //устанавливаем отступ для axisRect (область всех осей Y, для сдвига оси X в лево!)
    axisRect->setMargins(QMargins(maxOffset + 15, 0, 0, 15));
}

void CustomPlotWidget::clearLegend()
{
    if (!m_customPlot || !m_legendTable) return;
    m_hashLegendItems.clear();
    if (m_legendTable) {
        m_legendTable->clearContents();
        m_legendTable->setRowCount(0);
    }
    if (m_legendWidget) m_legendWidget->setVisible(false);
}

bool CustomPlotWidget::createdLegend()
{
    if (!m_customPlot || !m_legendTable || !m_legendWidget) return false;

    //всегда очищаем перед созданием
    m_legendTable->clearContents();
    m_legendTable->setRowCount(0);
    m_hashLegendItems.clear();

    if (!m_cursorLine1->visible() && !m_cursorLine2->visible()) {
        m_legendWidget->setVisible(false);  //если курсоры не видны, скрываем легенду
        return false;
    }

    //настраиваем таблицу
    bool twoCursors = m_cursorLine1->visible() && m_cursorLine2->visible();
    QStringList headers;
    if (twoCursors) {
        headers << "Цвет" << "Поз" << "График" << "Ед.изм" << "Описание" << "Время 1" << "Знач 1" << "Время 2" << "Знач 2" << "Δ Время" << "Δ Знач";
    } else {
        headers << "Цвет" << "Поз" << "График" << "Ед.изм" << "Описание" << "Время" << "Значение";
    }
    m_legendTable->setColumnCount(headers.size());
    m_legendTable->setHorizontalHeaderLabels(headers);
    m_legendTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    //заполняем таблицу данными
    for (int i = 0; i < m_customPlot->graphCount(); ++i) {
        QCPGraph *graph = m_customPlot->graph(i);
        if (!graph) continue;

        TableItem tableItem;
        int row = m_legendTable->rowCount();
        m_legendTable->insertRow(row);  //вставляем строку в конец
        m_legendTable->setRowHeight(row, 18);

        //цвет (создаем цветной индикатор)
        QWidget *container = new QWidget();
        QHBoxLayout *layout = new QHBoxLayout(container);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setAlignment(Qt::AlignCenter);
        QLabel *colorLabel = new QLabel();
        colorLabel->setFixedSize(15, 15);
        colorLabel->setStyleSheet(QString("QLabel { background-color: %1; border-radius: 3px; }").arg(graph->pen().color().name()));
        colorLabel->setAlignment(Qt::AlignCenter);
        layout->addWidget(colorLabel);
        m_legendTable->setCellWidget(row, 0, container);

        //symbol
        QTableWidgetItem *symbolItem = new QTableWidgetItem(graph->property("symbol").toString());
        symbolItem->setTextAlignment(Qt::AlignVCenter);
        m_legendTable->setItem(row, 1, symbolItem);

        //имя графика
        QString graphName = graph->name();
        if (graphName.isEmpty()) graphName = QString("График %1").arg(i + 1);
        QTableWidgetItem *nameItem = new QTableWidgetItem(graphName);
        m_legendTable->setItem(row, 2, nameItem);

        //еденицы измерения
        QTableWidgetItem *unitItem = new QTableWidgetItem(graph->property("unit").toString());
        unitItem->setTextAlignment(Qt::AlignVCenter);
        m_legendTable->setItem(row, 3, unitItem);

        //описание
        QTableWidgetItem *descriptionItem = new QTableWidgetItem(graph->property("description").toString());
        descriptionItem->setTextAlignment(Qt::AlignVCenter);
        m_legendTable->setItem(row, 4, descriptionItem);

        if (twoCursors) {
            //время 2
            QTableWidgetItem *timeItem1 = new QTableWidgetItem("--:--:--");
            timeItem1->setTextAlignment(Qt::AlignCenter);
            m_legendTable->setItem(row, 5, timeItem1);
            tableItem.ptimeItem = timeItem1;

            //значение 2
            QTableWidgetItem *valueItem1 = new QTableWidgetItem("0.000");
            valueItem1->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
            m_legendTable->setItem(row, 6, valueItem1);
            tableItem.pvalueItem = valueItem1;

            //время 2
            QTableWidgetItem *timeItem2 = new QTableWidgetItem("--:--:--");
            timeItem2->setTextAlignment(Qt::AlignCenter);
            m_legendTable->setItem(row, 7, timeItem2);
            tableItem.ptimeItem_2 = timeItem2;

            //значение 2
            QTableWidgetItem *valueItem2 = new QTableWidgetItem("0.000");
            valueItem2->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
            m_legendTable->setItem(row, 8, valueItem2);
            tableItem.pvalueItem_2 = valueItem2;

            //Δ время
            QTableWidgetItem *timeItemD = new QTableWidgetItem("--:--:--");
            timeItemD->setTextAlignment(Qt::AlignCenter);
            m_legendTable->setItem(row, 9, timeItemD);
            tableItem.ptimeItem_d = timeItemD;

            //Δ значение
            QTableWidgetItem *valueItemD = new QTableWidgetItem("0.000");
            valueItemD->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
            m_legendTable->setItem(row, 10, valueItemD);
            tableItem.pvalueItem_d = valueItemD;
        } else {
            //время 1
            QTableWidgetItem *timeItem = new QTableWidgetItem("--:--:--");
            timeItem->setTextAlignment(Qt::AlignCenter);
            m_legendTable->setItem(row, 5, timeItem);
            tableItem.ptimeItem = timeItem;

            //значение 1
            QTableWidgetItem *valueItem = new QTableWidgetItem("0.000");
            valueItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
            m_legendTable->setItem(row, 6, valueItem);
            tableItem.pvalueItem = valueItem;
        }
        m_hashLegendItems[graph] = tableItem;
    }

    //показываем легенду
    m_legendWidget->setVisible(true);
    updateLegendSize();
    return true;
}

void CustomPlotWidget::updateLegendSize()
{
    /* подстраиваем размеры таблицы легенды после создания и заполнения ячеек при изменении позиции курсора */
    //автоматически подстраиваем размеры
    m_legendTable->resizeColumnsToContents();
    //автоматически подстраиваем высоту таблицы под количество строк
    int rowHeight = m_legendTable->rowHeight(m_legendTable->rowCount() - 1); //высота строки
    int totalHeight = qMin(rowHeight * m_legendTable->rowCount() + m_legendTable->horizontalHeader()->height() + 2, 800);
    m_legendTable->setMaximumHeight(totalHeight);
    m_legendTable->setMinimumHeight(totalHeight);
    //минимальная ширина таблицы
    int totalWidth = m_legendTable->verticalHeader()->width();
    for (int col = 0; col < m_legendTable->columnCount(); ++col) {
        totalWidth += m_legendTable->columnWidth(col);
    }
    m_legendTable->setMinimumWidth(totalWidth + 2);
}

void CustomPlotWidget::fixCursorPositions()
{
    if (!m_customPlot) return;
    /* сохраняем позиции курсоров в пикселях и восстанавливаем их */
    if (m_cursorLine1 && m_cursorLine1->visible()) {
        double newX = m_customPlot->xAxis->pixelToCoord(m_cursorLine1PixelX);
        updateCursorPosition(m_cursorLine1, newX);
    }
    if (m_cursorLine2 && m_cursorLine2->visible()) {
        double newX = m_customPlot->xAxis->pixelToCoord(m_cursorLine2PixelX);
        updateCursorPosition(m_cursorLine2, newX);
    }
}

void CustomPlotWidget::updateCursorPosition(QCPItemStraightLine *cursorLine, const double &x)
{
    if (!m_customPlot || !cursorLine || !cursorLine->visible()) return;

    //ограничиваем позицию в пределах видимой области
    auto xBound = qBound(m_customPlot->xAxis->range().lower, x, m_customPlot->xAxis->range().upper);
    cursorLine->point1->setCoords(xBound, m_customPlot->yAxis->range().lower);
    cursorLine->point2->setCoords(xBound, m_customPlot->yAxis->range().upper);

    //обновляем ячейки легенды
    for (auto it = m_hashLegendItems.begin(); it != m_hashLegendItems.end(); ++it) {
        QCPGraph *graph = it.key();
        TableItem &item = it.value();
        if (!graph) continue;

        auto data = graph->data();
        if (data.isNull() || data->isEmpty()) continue; //проверяем что указатель не пустой (убеждаемся, что контейнер существует)
        if (auto itData = data->findBegin(xBound); itData != data->constEnd()) {
            auto itNext = itData;
            ++itNext;
            //линейная интерполяция
            if (m_chbInterpolation && m_chbInterpolation->isChecked() && itData != data->constBegin() && itNext != data->constEnd()) {
                double x1 = itData->key;
                double y1 = itData->value;
                double x2 = itNext->key;
                double y2 = itNext->value;
                double interpolatedY = y1 + (y2 - y1) * (xBound - x1) / (x2 - x1);
                fillParametersRowLegend(cursorLine, item, xBound, interpolatedY);
            } else {
                fillParametersRowLegend(cursorLine, item, itData->key, itData->value);
            }
        }
    }
    //сохраняем текущие положения курсоров в пикселах, чтобы при движении сцены восстановить положения курсоров в тех же точках на экране
    if (cursorLine == m_cursorLine1) {
        m_cursorLine1PixelX = m_customPlot->xAxis->coordToPixel(xBound);
    } else if (cursorLine == m_cursorLine2) {
        m_cursorLine2PixelX = m_customPlot->xAxis->coordToPixel(xBound);
    }
    updateLegendSize();
    if (m_customPlot->layer("cursorLayer")) m_customPlot->layer("cursorLayer")->replot();
    update();
}

void CustomPlotWidget::fillParametersRowLegend(QCPItemStraightLine *cursorLine, TableItem &item, const double &time, const double &value)
{
    if (!cursorLine) return;

    /* если перемещаем курсор1 или курсор2 (при этом курсор1 не включен),
     * если оба курсора включены, то они обновляют соответствующие ячейки */
    if (cursorLine == m_cursorLine1 || (cursorLine == m_cursorLine2 && !m_cursorLine1->visible())) {
        if (item.ptimeItem) item.ptimeItem->setText(QDateTime::fromMSecsSinceEpoch(time).toString("dd.MM.yyyy hh:mm:ss.zzz"));
        if (item.pvalueItem) item.pvalueItem->setText(QString("%1").arg(value, 0, 'f', 2)); //Оставляем Y как число с 2 знаками после запятой
    } else if (cursorLine == m_cursorLine2) {
        if (item.ptimeItem_2) item.ptimeItem_2->setText(QDateTime::fromMSecsSinceEpoch(time).toString("dd.MM.yyyy hh:mm:ss.zzz"));
        if (item.pvalueItem_2) item.pvalueItem_2->setText(QString("%1").arg(value, 0, 'f', 2)); //Оставляем Y как число с 2 знаками после запятой
    }
    /* рассчитываем разницу между ячейками таблицы легенды */
    if (item.ptimeItem && item.ptimeItem_2 && item.ptimeItem_d) {
        //парсим строки в QDateTime
        QDateTime dt1 = QDateTime::fromString(item.ptimeItem->text(), "dd.MM.yyyy hh:mm:ss.zzz");
        QDateTime dt2 = QDateTime::fromString(item.ptimeItem_2->text(), "dd.MM.yyyy hh:mm:ss.zzz");
        if (dt1.isValid() && dt2.isValid()) {
            //вычисляем абсолютную разницу в миллисекундах
            qint64 diffMSecs = qAbs(dt1.toMSecsSinceEpoch() - dt2.toMSecsSinceEpoch());
            //преобразуем в формат времени
            item.ptimeItem_d->setText(QTime::fromMSecsSinceStartOfDay(diffMSecs).toString("hh:mm:ss.zzz"));
        } else {
            item.ptimeItem_d->setText("--:--:--.---");
        }
    }
    if (item.pvalueItem && item.pvalueItem_2 && item.pvalueItem_d) {
        double val1 = item.pvalueItem->text().toDouble();
        double val2 = item.pvalueItem_2->text().toDouble();
        double diffVal = qAbs(val1 - val2);
        item.pvalueItem_d->setText(QString("%1").arg(diffVal, 0, 'f', 2));
    }
}

void CustomPlotWidget::updateGraphDataCount()
{
    if (!m_customPlot) return;
    quint32 dataCount = 0;
    for (auto i = 0; i < m_customPlot->graphCount(); ++i) {
        QCPGraph *graph = m_customPlot->graph(i);
        if (graph) dataCount += graph->dataCount();
    }
    if (m_labelPoints) m_labelPoints->setText(QString("📈 Точки: %1").arg(dataCount));
    emit fdataCount(dataCount);
}

void CustomPlotWidget::extensionGraphLines(const double &time)
{
    /* "подтяжка графика" - при отсутствии изменений параметра и отставании графика,
     * добавляем предыдущие точки для отображения одинаковой длины на графиках */
    if (!m_customPlot) return;
    if (m_maxTimeGraph < time) m_maxTimeGraph = time;

    if (!m_throttlingGraphLines) {
        m_throttlingGraphLines = true;
        //throttling обновлений
        QTimer::singleShot(2500, this, [this]() {
            m_throttlingGraphLines = false;
            for (auto i = 0; i < m_customPlot->graphCount(); ++i) {
                QCPGraph *graph = m_customPlot->graph(i);
                if (!graph) continue;
                QSharedPointer<QCPGraphDataContainer> data = graph->data();
                if (data->isEmpty()) continue;
                auto lastPoint = data->constEnd() - 1; //берем последний элемент
                auto lastTime = lastPoint->key;
                //если график отстает - добавляем ему повторно последнюю точку
                if (lastTime < m_maxTimeGraph) {
                    graph->addData(m_maxTimeGraph, lastPoint->value);
                }
                //Оптимизация!!! Удаляем повторяющиеся точки при подтяжке графика
                if (data->size() >= 3) {
                    auto prevPoint2 = data->constEnd() - 2;  //берем предыдущую точку
                    auto prevPoint3 = data->constEnd() - 3;  //берем предыдущую точку
                    static constexpr float EPSILON = 0.0001;
                    if (qAbs(lastPoint->value - prevPoint2->value) < EPSILON && qAbs(lastPoint->value - prevPoint3->value) < EPSILON) {
                        data->remove(prevPoint2->key); //удаляем предыдущую точку 3 т.к. она соответствует последующей точке, для оптимизации
                    }
                }
            }
            if (!m_customPlot->size().isNull()) m_customPlot->replot(QCustomPlot::rpQueuedReplot); //перед отрисовкой проверям размер области, должна быть не 0, иначе crash
        });
    }
}

void CustomPlotWidget::trimToLimit(QCPGraph *graph)
{
    /* удаляем точки из начала контейнера, если достигнут лимит */
    if (graph && graph->dataCount() >= MAX_LENGTH) {
        QSharedPointer<QCPGraphDataContainer> data = graph->data();
        while (data->size() >= MAX_LENGTH) {
            if (data->isEmpty()) break;
            double firstKey = data->constBegin()->key;
            data->remove(firstKey);
        }
    }
}

void CustomPlotWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (!m_customPlot) return;

    if (QApplication::mouseButtons() == Qt::LeftButton) {
        qDebug() << Q_FUNC_INFO;
    }
}

bool CustomPlotWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (!m_customPlot || obj != m_customPlot || !m_cursorLine1 || !m_cursorLine2) return false;
    static QPointF lastPos;
    bool cursorVisible = m_cursorLine1->visible() || m_cursorLine2->visible();

    switch (event->type()) {
    case QEvent::MouseButtonPress: {
        m_chbAScroll->setChecked(false);

        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            lastPos = mouseEvent->position();
        }

        //определяем кликнули ли мы рядом с каким-то из курсоров (допуск по оси X)
        if (QApplication::mouseButtons() == Qt::LeftButton && cursorVisible) {
            double tolerance = m_customPlot->xAxis->range().size() * 0.02; // 2% от ширины экрана
            double coordX = m_customPlot->xAxis->pixelToCoord(mouseEvent->pos().x());
            if (qAbs(m_cursorLine1->point1->coords().x() - coordX) < tolerance) {
                m_customPlot->setInteraction(QCP::iRangeDrag, false); //отключаем перемещение на уровне библиотеки QCustomPlot
                m_cursorLine1->setProperty("checked", true);
            }
            if (qAbs(m_cursorLine2->point1->coords().x() - coordX) < tolerance) {
                m_customPlot->setInteraction(QCP::iRangeDrag, false); //отключаем перемещение на уровне библиотеки QCustomPlot
                m_cursorLine2->setProperty("checked", true);
            }
        }
        break;
    }
    case QEvent::MouseMove: {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        QPointF currentPos = mouseEvent->position();

        //перемещение только выбранных осей по Y (перемещение всей сцены выполняется на уровне библиотеки, свойство QCP::iRangeDrag)
        if (mouseEvent->buttons() == Qt::LeftButton) {
            QList<QCPGraph*> selectedGraphs = m_customPlot->selectedGraphs();
            if (!selectedGraphs.isEmpty()) {
                m_customPlot->setInteraction(QCP::iRangeDrag, false); //отключаем перемещение на уровне библиотеки QCustomPlot

                QPointF delta = currentPos - lastPos;
                for (QCPGraph *graph : std::as_const(selectedGraphs)) {
                    QCPAxis *yAxis = graph->valueAxis();
                    if (yAxis && yAxis->orientation() == Qt::Vertical) {
                        yAxis->moveRange(delta.y() * yAxis->range().size() / yAxis->axisRect()->height());
                    }
                }
                m_customPlot->replot(QCustomPlot::rpQueuedReplot);
                lastPos = currentPos;
            }
        }
        //обновляем координаты выбранных визирных линий
        if (QApplication::mouseButtons() == Qt::LeftButton && cursorVisible) {
            double coordX = m_customPlot->xAxis->pixelToCoord(mouseEvent->pos().x());
            if (m_cursorLine1->property("checked") == true) updateCursorPosition(m_cursorLine1, coordX);
            if (m_cursorLine2->property("checked") == true) updateCursorPosition(m_cursorLine2, coordX);
        }
        break;
    }
    case QEvent::MouseButtonRelease: {
        m_customPlot->setInteraction(QCP::iRangeDrag, true);
        m_customPlot->setInteraction(QCP::iRangeZoom, true);
        m_cursorLine1->setProperty("checked", false);
        m_cursorLine2->setProperty("checked", false);
        break;
    }
    case QEvent::Wheel: {
        m_chbAScroll->setChecked(false);

        QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);
        QPoint angleDelta = wheelEvent->angleDelta();
        int delta = angleDelta.y();
        if (delta == 0) break;

        //позиция курсора
        QPointF currentPos = wheelEvent->position();

        //коэффициенты зума как в QCustomPlot
        static constexpr double ZOOM_IN_FACTOR = 0.85;
        static constexpr double ZOOM_OUT_FACTOR = 1.15;
        double factor = (delta > 0) ? ZOOM_IN_FACTOR : ZOOM_OUT_FACTOR;

        //получаем текущий прямоугольник в пикселях
        QRectF currentRect = m_customPlot->axisRect()->rect();

        //вычисляем новый прямоугольник с учетом позиции курсора
        double newLeft = currentPos.x() + (currentRect.left() - currentPos.x()) * factor;
        double newRight = currentPos.x() + (currentRect.right() - currentPos.x()) * factor;
        double newTop = currentPos.y() + (currentRect.top() - currentPos.y()) * factor;
        double newBottom = currentPos.y() + (currentRect.bottom() - currentPos.y()) * factor;

        QRectF newRect(newLeft, newTop, newRight - newLeft, newBottom - newTop);
        Qt::KeyboardModifiers modifiers = QApplication::keyboardModifiers();
        QList<QCPAxis*> affectedAxes;

        //только X - используем только горизонтальные координаты
        if (modifiers == Qt::ShiftModifier) {
            m_customPlot->setInteraction(QCP::iRangeZoom, false); //отключаем зумирование на уровне библиотеки QCustomPlot
            affectedAxes.append(m_customPlot->xAxis);
        }
        //только Y - используем только вертикальные координаты
        else if (modifiers == Qt::ControlModifier) {
            m_customPlot->setInteraction(QCP::iRangeZoom, false); //отключаем зумирование на уровне библиотеки QCustomPlot
            QList<QCPGraph*> selectedGraphs = m_customPlot->selectedGraphs();
            //зум только выбранных осей
            if (!selectedGraphs.isEmpty()) {
                for (QCPGraph *graph : std::as_const(selectedGraphs)) {
                    QCPAxis *yAxis = graph->valueAxis();
                    if (yAxis && yAxis->orientation() == Qt::Vertical) {
                        affectedAxes.append(graph->valueAxis());
                    }
                }
                //зум всех осей
            } else {
                for (int i = 0; i < m_customPlot->axisRect()->axes().size(); ++i) {
                    QCPAxis *axis = m_customPlot->axisRect()->axes().at(i);
                    if (axis->orientation() == Qt::Vertical) {
                        affectedAxes.append(axis);
                    }
                }
            }
        }
        if (!affectedAxes.isEmpty()) {
            m_customPlot->axisRect()->zoom(newRect, affectedAxes);
            m_customPlot->replot(QCustomPlot::rpQueuedReplot);
        }
        break;
    }
    case QEvent::MouseButtonDblClick: {
        break;
    }
    default:
        break;
    }

    //передаем событие дальше для стандартной обработки
    return QWidget::eventFilter(obj, event);
}

PlotMetadata CustomPlotWidget::parsePlotMetadata(const QVariant &var)
{
    PlotMetadata plotMetadata;
    if (!var.canConvert<QVariantMap>()) {
        qWarning() << Q_FUNC_INFO << "Invalid QVariant type, expected QVariantMap";
        return plotMetadata;
    }
    QVariantMap map = var.toMap();

    plotMetadata.added = map.value("addTag", false).toBool();
    plotMetadata.symbol = map.value("symbol", QString()).toString();
    plotMetadata.name = map.value("name", QString()).toString();
    plotMetadata.unit = map.value("unit", QString()).toString();
    plotMetadata.description = map.value("description", QString()).toString();
    plotMetadata.tagname = map.value("tagname", QString()).toString();
    return plotMetadata;
}

void CustomPlotWidget::fsetAutoScroll(const bool &arg)
{
    m_chbAScroll->setChecked(arg);
}

void CustomPlotWidget::fsetCursorLine(QCPItemStraightLine *cursorLine, const bool &arg)
{
    if (!m_customPlot || !cursorLine) return;
    cursorLine->setVisible(arg);

    //активируем визирную линию и отображем в центре экрана
    if (createdLegend() && cursorLine->visible()) {
        //Выводим в середину экрана
        auto range = m_customPlot->xAxis->range();
        double x = 0;
        if (cursorLine == m_cursorLine1) x = range.lower + (1.0 * range.size() / 3.0); //1/3 экрана
        if (cursorLine == m_cursorLine2) x = range.lower + (2.0 * range.size() / 3.0); //2/3 экрана
        updateCursorPosition(cursorLine, x);
    }
    m_customPlot->replot(QCustomPlot::rpQueuedReplot);
    qDebug() << Q_FUNC_INFO << arg;
}

void CustomPlotWidget::fsetAddGraph(const QVariant &var)
{
    PlotMetadata metadata = parsePlotMetadata(var);
    QCPGraph *graph = pgetGraphByTag(metadata.tagname);
    if (!graph && metadata.added) {
        if (addGraph(metadata)) {
            TcpThread *client = TcpThread::getInstance();
            emit client->clientRequest(metadata.tagname); //формируем запрос истории с сервера
            emit femitStateGgaph(var); //испускаем сигнал для обновления UI
        } else {
            //если график не добавлен испускаем сигнал что график не добавлен
            emit femitStateGgaph(QVariantMap{
                {"addTag", false},
                {"symbol", metadata.symbol},
                {"name", metadata.name},
                {"tagname", metadata.tagname}
            }); //испускаем сигнал для обновления UI
        }
    }
    if (graph && !metadata.added) {
        removeGraph(graph);
        emit femitStateGgaph(var); //испускаем сигнал для обновления UI
    }
    qDebug() << Q_FUNC_INFO << metadata.added << metadata.symbol << metadata.name << metadata.tagname;
}

void CustomPlotWidget::fsetFillParameters(const QVariant &var)
{
    PlotMetadata metadata = parsePlotMetadata(var);

    //проверяем, существует ли уже такой symbol в списке
    bool exists = false;
    for (const QVariant &item : std::as_const(m_pendingParameters)) {
        QVariantMap existingMap = item.toMap();
        if (existingMap["symbol"].toString() == metadata.symbol) {
            exists = true;
            break;
        }
    }
    if (!exists) m_pendingParameters.append(var);

    //throttling обновления
    static QTimer *timer = nullptr;
    if (!timer) {
        timer = new QTimer(this);
        timer->setSingleShot(true);
        connect(timer, &QTimer::timeout, this, [this]() {
            m_fieldSelectorDialog.setFields(m_pendingParameters);
            m_pendingParameters.clear();
        });
    }
    timer->start(250);
}

void CustomPlotWidget::addPointsToGraph(const QVariant &tagname, const QList<TagValue> &tagValues)
{
    if (m_customPlot) {
        QCPGraph *graph = pgetGraphByTag(tagname.toString());
        if (!graph) return;

        graph->data().clear();
        for (const auto &tagValue : tagValues) {
            qint64 msecs = tagValue.timestamp.toDateTime().toMSecsSinceEpoch();
            double timeInMsecs = static_cast<double>(msecs);
            graph->addData(timeInMsecs, tagValue.value.toDouble());
        }
        trimToLimit(graph);

        QCPAxis *yAxis = graph->valueAxis();
        if (yAxis) yAxis->rescale(true);

        updateGraphDataCount();
        QSharedPointer<QCPGraphDataContainer> data = graph->data();
        if (!data->isEmpty()) {
            QCPRange currentRange = m_customPlot->xAxis->range(); //сохраняем текущий диапазон X
            if (m_customPlot->graphCount() > 1) {
                m_customPlot->xAxis->setRange(currentRange);
            } else {
                auto lastPoint = data->constEnd() - 1;
                auto lastTime  = lastPoint->key;   //вычитаем предыдущее значение видимой части оси Х
                static constexpr double TIME_WINDOW = 600*1000; //ms (10минут)
                m_customPlot->xAxis->setRange(lastTime - TIME_WINDOW, lastTime);
            }
        };
        if (!m_customPlot->size().isNull()) m_customPlot->replot(QCustomPlot::rpQueuedReplot);    //перед отрисовкой проверяем размер области, должна быть не 0, иначе crash
    }
}

void CustomPlotWidget::addPointToGraph(const QVariant &tagname, const TagValue &tagValue)
{
    if (m_customPlot) {
        QCPGraph *graph = pgetGraphByTag(tagname.toString());
        if (!graph) return;

        qint64 msecs = tagValue.timestamp.toDateTime().toMSecsSinceEpoch();
        double timeInMsecs = static_cast<double>(msecs);
        graph->addData(timeInMsecs, tagValue.value.toDouble());
        trimToLimit(graph);

        updateGraphDataCount();
        QSharedPointer<QCPGraphDataContainer> data = graph->data();
        if (!data->isEmpty() && m_chbAScroll->isChecked()) {
            QCPRange currentRange = m_customPlot->xAxis->range(); //сохраняем текущий диапазон X
            auto lastPoint = data->constEnd() - 1;  //последняя точка
            auto lastTime  = lastPoint->key;
            auto rightMargin = currentRange.size() * 0.02; //2%
            QCPRange newRange(lastTime - currentRange.size() + rightMargin, lastTime);
            m_customPlot->xAxis->setRange(newRange.lower, newRange.upper + rightMargin);  //пересчитываем масштаб для автоскролла
        };
        if (!m_customPlot->size().isNull()) m_customPlot->replot(QCustomPlot::rpQueuedReplot); //перед отрисовкой проверяем размер области, должна быть не 0, иначе crash
        extensionGraphLines(timeInMsecs); //выравниваем остальные графики
    }
}
