#include "applib.h"

AppLib::AppLib(QObject *parent)
    : QObject{parent}
{
    //стили QRadioButton
    qss.rb_redGreen = "QRadioButton::indicator {"
                      "width: 15px;"
                      "height: 15px;}"
                      "QRadioButton::indicator::unchecked {"
                      "border: 1px solid;"
                      "border-radius: 5px;"
                      "border-color: black;"
                      "background-color: red; }"
                      "QRadioButton::indicator::checked {"
                      "border: 1px solid;"
                      "border-radius: 5px;"
                      "border-color: black;"
                      "background-color: green; }";
    qss.rb_whiteGreen = "QRadioButton::indicator {"
                        "width: 15px;"
                        "height: 15px;}"
                        "QRadioButton::indicator::unchecked {"
                        "border: 1px solid;"
                        "border-radius: 5px;"
                        "border-color: black;"
                        "background-color: white; }"
                        "QRadioButton::indicator::checked {"
                        "border: 1px solid;"
                        "border-radius: 5px;"
                        "border-color: black;"
                        "background-color: green; }";
    qss.rb_whiteYellow = "QRadioButton::indicator {"
                         "width: 15px;"
                         "height: 15px;}"
                         "QRadioButton::indicator::unchecked {"
                         "border: 1px solid;"
                         "border-radius: 5px;"
                         "border-color: black;"
                         "background-color: white; }"
                         "QRadioButton::indicator::checked {"
                         "border: 1px solid;"
                         "border-radius: 5px;"
                         "border-color: black;"
                         "background-color: yellow; }";
    qss.rb_whiteRed = "QRadioButton::indicator {"
                      "width: 15px;"
                      "height: 15px;}"
                      "QRadioButton::indicator::unchecked {"
                      "border: 1px solid;"
                      "border-radius: 5px;"
                      "border-color: black;"
                      "background-color: white; }"
                      "QRadioButton::indicator::checked {"
                      "border: 1px solid;"
                      "border-radius: 5px;"
                      "border-color: black;"
                      "background-color: red; }";
    qss.treeView = "QTreeView {"
                   "alternate-background-color: #f8f8ff;"
                   "background-color: #fffff8;"
                   "}"
                   "QTreeView::item {"
                   "border-bottom: 1px solid #e0e0e0;"
                   "padding: 1px;"
                   "}"
                   "QTreeView::item:selected {"
                   "background-color: #fffff8;"     /* Основной цвет */
                   "color: black;"
                   "border: 1px dotted #ca3a27;"    /* Пунктирная граница вместо заливки */
                   "}"
                   "QTreeView::item:selected:alternate {"
                   "background-color: #f8f8ff;"     /* Альтернативный цвет */
                   "}";
    qss.treeView_height25 = "QTreeView {"
                            "alternate-background-color: #f8f8ff;"
                            "background-color: #fffff8;"
                            "}"
                            "QTreeView::item {"
                            "border-bottom: 1px solid #e0e0e0;"
                            "padding: 1px;"
                            "height: 25px;"
                            "}"
                            "QTreeView::item:selected {"
                            "background-color: #fffff8;"     /* Основной цвет */
                            "color: black;"
                            "border: 1px dotted #ca3a27;"    /* Пунктирная граница вместо заливки */
                            "}"
                            "QTreeView::item:selected:alternate {"
                            "background-color: #f8f8ff;"     /* Альтернативный цвет */
                            "}";
    qss.pb_dragdrop = "QPushButton {"
                      "    background-color: #4CAF50;"
                      "    color: white;"
                      "    border: none;"
                      "    padding: 8px;"
                      "    border-radius: 4px;"
                      "}"
                      "QPushButton:hover {"
                      "    background-color: #45a049;"
                      "}"
                      "QPushButton:pressed {"
                      "    background-color: #3d8b40;"
                      "}";

}

AppLib::~AppLib()
{

}

AppLib *AppLib::getInstance()
{
    static AppLib instance;
    return &instance;
}

void AppLib::moveWindowRelative(QWidget *parent, QWidget *child, int margin)
{
    child->close();
    QScreen *screen = parent->screen();
    if (!screen) return;

    QRect screenRect = screen->availableGeometry();

    int x_left = parent->x() - child->width() - margin;
    int x_right = parent->x() + parent->width() + margin;
    int y = parent->y();

    int x_pos = (x_left >= screenRect.x()) ? x_left :
                    ((x_right + child->width() <= screenRect.right()) ? x_right :
                         qMax(screenRect.x(), parent->x() + (parent->width() - child->width()) / 2));

    y = qBound(screenRect.y(), y, screenRect.bottom() - child->height());
    child->move(x_pos, y);
    child->show();
}

QString AppLib::getOSVersionString()
{
    auto OS = QOperatingSystemVersion::current();
    return QString("OS: %1 %2.%3.%4").arg(OS.name()).arg(OS.majorVersion()).arg(OS.minorVersion()).arg(OS.microVersion());
}

QString AppLib::getOpenSSLVersionString()
{
    return OPENSSL_VERSION_TEXT;
}

QString AppLib::getQSSGreyStyle()
{
    /* Синтаксис R"(...)" - это raw string literal (сырая строковая константа), которая позволяет:
     * - Писать многострочные строки
     * - Не экранировать кавычки и обратные слэши
     * - Сохранять форматирование (пробелы, переносы строк) */
    return QString(R"(
        QMainWindow {
            background-color: #e8e8e8;
        }
        QWidget {
            background-color: #e8e8e8;
            color: #333333;
        }
        QMenuBar {
            background-color: #f0f0f0;
            color: #333333;
            border-bottom: 1px solid #d0d0d0;
        }
        QMenuBar::item {
            background-color: transparent;
            padding: 5px 10px;
        }
        QMenuBar::item:selected {
            background-color: #d0d0d0;
            color: #000000;
        }
        QMenu {
            background-color: #f0f0f0;
            color: #333333;
            border: 1px solid #d0d0d0;
        }
        QMenu::item {
            padding: 5px 30px 5px 20px;
        }
        QMenu::item:selected {
            background-color: #d0d0d0;
            color: #000000;
        }
        QMenu::separator {
            height: 1px;
            background-color: #d0d0d0;
            margin: 5px 10px;
        }
        QPushButton {
            background-color: #f0f0f0;
            border: 1px solid #c0c0c0;
            border-radius: 3px;
            padding: 4px 10px;
            color: #333333;
            min-width: 60px;
        }
        QPushButton:hover {
            background-color: #e0e0e0;
            border-color: #b0b0b0;
        }
        QPushButton:pressed {
            background-color: #d0d0d0;
            border-color: #a0a0a0;
        }
        QPushButton:disabled {
            background-color: #f5f5f5;
            color: #999999;
        }
        QPushButton:default {
            background-color: #d0d0d0;
            border-color: #a0a0a0;
        }
        QLineEdit, QTextEdit, QPlainTextEdit {
            background-color: #ffffff;
            border: 1px solid #c0c0c0;
            border-radius: 3px;
            color: #333333;
            padding: 3px 6px;
            selection-background-color: #d0d0d0;
        }
        QLineEdit:focus, QTextEdit:focus, QPlainTextEdit:focus {
            border-color: #a0a0a0;
        }
        QTableWidget, QListView, QTreeView {
            background-color: #ffffff;
            alternate-background-color: #f5f5f5;
            color: #333333;
            gridline-color: #d0d0d0;
            selection-background-color: #d0d0d0;
            selection-color: #000000;
        }
        QTableWidget::item, QListView::item, QTreeView::item {
            padding: 4px;
        }
        QTableWidget::item:selected, QListView::item:selected, QTreeView::item:selected {
            background-color: #d0d0d0;
            color: #000000;
        }
        QHeaderView::section {
            background-color: #f0f0f0;
            color: #333333;
            padding: 5px;
            border: 1px solid #d0d0d0;
            border-right: none;
        }
        QHeaderView::section:last {
            border-right: 1px solid #d0d0d0;
        }
        QTabWidget::pane {
            border: 1px solid #d0d0d0;
            background-color: #f5f5f5;
        }
        QTabBar::tab {
            background-color: #f0f0f0;
            color: #666666;
            padding: 6px 15px;
            margin-right: 2px;
            border: 1px solid #d0d0d0;
            border-bottom: none;
            border-top-left-radius: 4px;
            border-top-right-radius: 4px;
        }
        QTabBar::tab:selected {
            background-color: #f5f5f5;
            color: #000000;
            border-bottom: 1px solid #f5f5f5;
        }
        QTabBar::tab:hover:!selected {
            background-color: #e5e5e5;
            color: #333333;
        }
        QCheckBox, QRadioButton {
            color: #333333;
            spacing: 8px;
        }
        QCheckBox::indicator, QRadioButton::indicator {
            width: 16px;
            height: 16px;
        }
        QGroupBox {
            border: 1px solid #d0d0d0;
            border-radius: 5px;
            margin-top: 12px;
            padding-top: 10px;
            color: #333333;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 8px;
        }
        QStatusBar {
            background-color: #f0f0f0;
            color: #333333;
            border-top: 1px solid #d0d0d0;
        }
        QToolBar {
            background-color: #f0f0f0;
            border: none;
            spacing: 4px;
            padding: 4px;
        }
        QToolBar::separator {
            width: 2px;
            background-color: #d0d0d0;
            margin: 4px 8px;
        }
        QScrollBar:vertical {
            background-color: #f5f5f5;
            width: 14px;
            margin: 0px;
        }
        QScrollBar::handle:vertical {
            background-color: #d0d0d0;
            border-radius: 7px;
            min-height: 20px;
            margin: 2px;
        }
        QScrollBar::handle:vertical:hover {
            background-color: #c0c0c0;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0px;
        }
        QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {
            background-color: transparent;
        }
        QScrollBar:horizontal {
            background-color: #f5f5f5;
            height: 14px;
            margin: 0px;
        }
        QScrollBar::handle:horizontal {
            background-color: #d0d0d0;
            border-radius: 7px;
            min-width: 20px;
            margin: 2px;
        }
        QScrollBar::handle:horizontal:hover {
            background-color: #c0c0c0;
        }
        QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
            width: 0px;
        }
        QComboBox {
            background-color: #ffffff;
            border: 1px solid #c0c0c0;
            border-radius: 3px;
            padding: 3px 8px;
            color: #333333;
            min-height: 20px;
        }
        QComboBox:hover {
            border-color: #a0a0a0;
        }
        QComboBox::drop-down {
            border: none;
            width: 20px;
        }
        QComboBox::down-arrow {
            image: none;
            border-left: 5px solid transparent;
            border-right: 5px solid transparent;
            border-top: 5px solid #666666;
            margin-right: 5px;
        }
        QComboBox QAbstractItemView {
            background-color: #ffffff;
            color: #333333;
            border: 1px solid #d0d0d0;
            selection-background-color: #d0d0d0;
            selection-color: #000000;
        }
        QSpinBox, QDoubleSpinBox {
            background-color: #ffffff;
            border: 1px solid #c0c0c0;
            border-radius: 3px;
            color: #333333;
            padding: 3px;
            min-height: 20px;
        }
        QSpinBox:hover, QDoubleSpinBox:hover {
            border-color: #a0a0a0;
        }
        QSpinBox::up-button, QDoubleSpinBox::up-button,
        QSpinBox::down-button, QDoubleSpinBox::down-button {
            background-color: #f0f0f0;
            border: none;
            width: 16px;
        }
        QProgressBar {
            background-color: #f5f5f5;
            border: 1px solid #d0d0d0;
            border-radius: 3px;
            text-align: center;
            color: #333333;
        }
        QProgressBar::chunk {
            background-color: #b0b0b0;
            border-radius: 2px;
        }
        QLabel {
            color: #333333;
        }
        QDockWidget::title {
            background-color: #f0f0f0;
            padding: 4px;
            color: #333333;
        }
        QDockWidget::close-button, QDockWidget::float-button {
            background-color: transparent;
        }
        QDockWidget::close-button:hover, QDockWidget::float-button:hover {
            background-color: #d0d0d0;
        }
        QToolTip {
            background-color: #ffffe0;
            color: #333333;
            border: 1px solid #d0d0d0;
            border-radius: 3px;
            padding: 4px 8px;
        }
        QMessageBox {
            background-color: #f5f5f5;
        }
        QListWidget, QListWidget::item {
            background-color: #ffffff;
            color: #333333;
        }
        QListWidget::item:selected {
            background-color: #d0d0d0;
            color: #000000;
        }
        QScrollArea {
            background-color: #f5f5f5;
            border: none;
        }
        QSplitter::handle {
            background-color: #e0e0e0;
        }
        QSplitter::handle:hover {
            background-color: #d0d0d0;
        }
        QFrame {
            background-color: transparent;
        }
    )");
}
