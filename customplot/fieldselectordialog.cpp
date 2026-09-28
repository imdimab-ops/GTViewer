#include "fieldselectordialog.h"

FieldSelectorDialog::FieldSelectorDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Выбор полей для отображения");
    setMinimumWidth(400);
    setMinimumHeight(500);
    setModal(true);

    setupUI();
    setupConnections();
}

FieldSelectorDialog::~FieldSelectorDialog()
{

}

void FieldSelectorDialog::setupUI()
{
    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(15, 15, 15, 15);

    //заголовок
    QLabel *titleLabel = new QLabel("Выберите поля для отображения на графиках", this);
    titleLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #2c3e50;");
    mainLayout->addWidget(titleLabel);

    //поиск
    QHBoxLayout *searchLayout = new QHBoxLayout();
    QLabel *searchLabel = new QLabel("🔍 Поиск:", this);
    searchLineEdit = new QLineEdit(this);
    searchLineEdit->setPlaceholderText("Введите имя поля...");
    searchLineEdit->setStyleSheet(
        "QLineEdit {"
        "   padding: 5px;"
        "   border: 1px solid #bdc3c7;"
        "   border-radius: 4px;"
        "}"
        "QLineEdit:focus {"
        "   border: 1px solid #3498db;"
        "}"
        );
    //чекбокс для показа только выбранных
    chbShowSelected = new QCheckBox("Выбранные", this);
    chbShowSelected->setStyleSheet(
        "QCheckBox {"
        "   color: #2c3e50;"
        "   font-weight: bold;"
        "}"
        "QCheckBox::indicator {"
        "   width: 18px;"
        "   height: 18px;"
        "}"
        );
    searchLayout->addWidget(searchLabel);
    searchLayout->addWidget(searchLineEdit);
    searchLayout->addWidget(chbShowSelected);
    mainLayout->addLayout(searchLayout);

    //список полей
    QGroupBox *fieldGroupBox = new QGroupBox("Поля", this);
    fieldGroupBox->setStyleSheet(
        "QGroupBox {"
        "   font-weight: bold;"
        "   border: 1px solid #bdc3c7;"
        "   border-radius: 4px;"
        "   margin-top: 10px;"
        "}"
        "QGroupBox::title {"
        "   subcontrol-origin: margin;"
        "   left: 10px;"
        "   padding: 0 5px 0 5px;"
        "}"
        );

    QVBoxLayout *groupLayout = new QVBoxLayout(fieldGroupBox);
    groupLayout->setSpacing(5);

    //создаем скролл-область для списка
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    QWidget *scrollWidget = new QWidget(this);
    QVBoxLayout *scrollLayout = new QVBoxLayout(scrollWidget);
    scrollLayout->setSpacing(2);
    scrollLayout->setContentsMargins(5, 5, 5, 5);
    scrollLayout->addStretch();

    scrollArea->setWidget(scrollWidget);
    groupLayout->addWidget(scrollArea);
    mainLayout->addWidget(fieldGroupBox);

    //сохраняем ссылку на layout для добавления чекбоксов
    fieldListWidget = new QListWidget();  //для совместимости, но используем QVBoxLayout

    //кнопки управления
    QHBoxLayout *controlLayout = new QHBoxLayout();
    
    // btnSelectAll = new QPushButton("Выбрать все", this);
    // btnSelectAll->setStyleSheet(
    //     "QPushButton {"
    //     "   background-color: #2ecc71;"
    //     "   color: white;"
    //     "   border: none;"
    //     "   border-radius: 4px;"
    //     "   padding: 5px 15px;"
    //     "}"
    //     "QPushButton:hover {"
    //     "   background-color: #27ae60;"
    //     "}"
    // );

    btnDeselectAll = new QPushButton("Снять все", this);
    btnDeselectAll->setStyleSheet(
        "QPushButton {"
        "   background-color: #e74c3c;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 4px;"
        "   padding: 5px 15px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #c0392b;"
        "}"
        );

    // controlLayout->addWidget(btnSelectAll);
    controlLayout->addWidget(btnDeselectAll);
    controlLayout->addStretch();
    mainLayout->addLayout(controlLayout);

    //кнопки диалога
    QHBoxLayout *dialogButtonLayout = new QHBoxLayout();
    dialogButtonLayout->addStretch();

    btnApply = new QPushButton("Применить", this);
    btnApply->setStyleSheet(
        "QPushButton {"
        "   background-color: #3498db;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 4px;"
        "   padding: 8px 25px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background-color: #2980b9;"
        "}"
        );

    btnCancel = new QPushButton("Отмена", this);
    btnCancel->setStyleSheet(
        "QPushButton {"
        "   background-color: #95a5a6;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 4px;"
        "   padding: 8px 25px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #7f8c8d;"
        "}"
        );

    dialogButtonLayout->addWidget(btnApply);
    dialogButtonLayout->addWidget(btnCancel);
    mainLayout->addLayout(dialogButtonLayout);

    //сохраняем ссылку на контейнер для чекбоксов
    this->fieldListWidget = new QListWidget();
    //заменяем scrollLayout на listWidget
    QLayoutItem *item = nullptr;
    while ((item = scrollLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
    scrollLayout->addWidget(fieldListWidget);
}

void FieldSelectorDialog::setupConnections()
{
    connect(searchLineEdit, &QLineEdit::textChanged, this, [this](const QString &text){
        filterFields(text);
    });
    connect(chbShowSelected, &QCheckBox::checkStateChanged, this, [this](){
        filterFields("");
    });
    
    connect(btnSelectAll, &QPushButton::clicked, this, [this](){
        for (auto it = mfieldCheckBoxes.begin(); it != mfieldCheckBoxes.end(); ++it) {
            // it.value()->setChecked(true);
            QCheckBox *checkBox = qobject_cast<QCheckBox*>(it.value());
            if (checkBox && !checkBox->isChecked()) {
                checkBox->setChecked(true);
                emit addGraph(QVariantMap{
                    {"addTag", checkBox->isChecked()},
                    {"symbol", checkBox->property("symbol")},
                    {"name", checkBox->property("name")}
                });
            }
        }
    });
    
    connect(btnDeselectAll, &QPushButton::clicked, this, [this](){
        for (auto it = mfieldCheckBoxes.begin(); it != mfieldCheckBoxes.end(); ++it) {
            // it.value()->setChecked(false);
            QCheckBox *checkBox = qobject_cast<QCheckBox*>(it.value());
            if (checkBox && checkBox->isChecked()) {
                checkBox->setChecked(false);
                emit addGraph(QVariantMap{
                    {"addTag", checkBox->isChecked()},
                    {"symbol", checkBox->property("symbol")},
                    {"name", checkBox->property("name")}
                });
            }
        }
    });
    
    connect(btnApply, &QPushButton::clicked, this, [this](){
        accept();
    });
    
    connect(btnCancel, &QPushButton::clicked, this, [this](){
        reject();
    });
}

void FieldSelectorDialog::filterFields(const QString &searchText)
{
    QString search = searchText.trimmed().toLower();
    bool showSelectedOnly = chbShowSelected->isChecked();
    
    for (int i = 0; i < fieldListWidget->count(); ++i) {
        QListWidgetItem *item = fieldListWidget->item(i);
        QWidget *container = fieldListWidget->itemWidget(item);
        if (!container) continue;

        QList<QLabel*> labels = container->findChildren<QLabel*>();
        QCheckBox *checkBox = container->findChild<QCheckBox*>();

        if (checkBox && !labels.isEmpty()) {
            bool visible = search.isEmpty();

            //фильтр по чекбоксу
            if (showSelectedOnly && !checkBox->isChecked()) {
                visible = false;
            }

            //фильтр по поиску (если элемент еще видим)
            if (!showSelectedOnly && !visible) {
                //проверяем все метки
                for (QLabel *label : std::as_const(labels)) {
                    QString text = label->text();
                    if (text.contains(search, Qt::CaseInsensitive)) {
                        visible = true;
                        break;
                    }
                }
            }
            item->setHidden(!visible);
        }
    }
}

QList<QString> FieldSelectorDialog::getFieldsNamesByType(const QString &typeName)
{
    QList<QString> fieldNames;
    const QMetaObject *meta = &Data::staticMetaObject;

    for (int i = 0; i < meta->propertyCount(); ++i) {
        QMetaProperty prop = meta->property(i);
        if (!prop.isValid()) continue; //пропускаем свойства, которые не являются MEMBER для структуры Data

        if (typeName.isEmpty()) {
            fieldNames.append(prop.name()); //если тип не указан - добавляем все поля
        }
        else if (QString(prop.typeName()) == typeName) { //typeName - "AI", "TAG"
            fieldNames.append(prop.name()); //иначе фильтруем по типу
        }
    }
    return fieldNames;
}

void FieldSelectorDialog::setFields(const QVariantList &fields)
{
    //очищаем список
    fieldListWidget->clear();
    mfieldCheckBoxes.clear();

    for (const QVariant &field : fields) {
        QVariantMap map = field.toMap();
        QString symbol = map["symbol"].toString();
        QString name = map["name"].toString();

        QListWidgetItem *item = new QListWidgetItem(fieldListWidget);

        //создаем контейнер для чекбокса и метки
        QWidget *container = new QWidget();
        QHBoxLayout *layout = new QHBoxLayout(container);
        layout->setContentsMargins(5, 0, 5, 0);
        layout->setSpacing(5);

        //symbol
        QLabel *symbolLabel = new QLabel();
        symbolLabel->setText(symbol);
        symbolLabel->setStyleSheet("font-weight: bold; color: #1976D2;");
        symbolLabel->setFixedWidth(80);  // Фиксированная ширина
        symbolLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        //name
        QLabel *nameLabel = new QLabel();
        nameLabel->setText(name);
        nameLabel->setStyleSheet("color: #333333;");
        nameLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        nameLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

        QCheckBox *checkBox = new QCheckBox();
        checkBox->setChecked(false);
        checkBox->setProperty("symbol", map["symbol"]);
        checkBox->setProperty("name", map["name"]);
        checkBox->setProperty("tagname", map["tagname"]);

        layout->addWidget(checkBox);
        layout->addWidget(symbolLabel);
        layout->addWidget(nameLabel);
        layout->addStretch();  //растягиваем до конца

        //сохраняем ссылку на чекбокс
        mfieldCheckBoxes[symbol] = checkBox;

        //обновляем карту видимости (&QCheckBox::clicked - только при физическом нажатии на чекбокс)
        connect(checkBox, &QCheckBox::clicked, this, [this](){
            QCheckBox *checkBox = qobject_cast<QCheckBox*>(sender());
            if (checkBox) {
                emit addGraph(QVariantMap{
                    {"addTag", checkBox->isChecked()},
                    {"symbol", checkBox->property("symbol")},
                    {"name", checkBox->property("name")}
                });
            }
        });

        //устанавливаем виджет в элемент списка
        fieldListWidget->setItemWidget(item, container);
    }
}

void FieldSelectorDialog::updateMetadata(const bool &add, const QString &symbol)
{
    if (auto it = mfieldCheckBoxes.find(symbol); it != mfieldCheckBoxes.end()) { //C++17 init-statement, it живет только внутри if
        it.value()->setChecked(add);
    }
    //если включен режим показа выбранных - обновляем фильтр
    if (chbShowSelected->isChecked()) {
        filterFields("");
    }
}
