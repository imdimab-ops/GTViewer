#include "mainwindow.h"

#include <QApplication>
#include <QStyleFactory>
#include <QPalette>
#include <QColor>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts); //расшаривание контекстов OpenGL
    QApplication a(argc, argv);

    // Принудительно задаём стандартную светлую палитру (Fusion light)
    QPalette lightPalette;

    // Основные цвета окон и текста
    lightPalette.setColor(QPalette::Window,          QColor(240, 240, 240)); // фон окон
    lightPalette.setColor(QPalette::WindowText,      QColor(0, 0, 0));       // текст в окнах
    lightPalette.setColor(QPalette::Base,            QColor(255, 255, 255)); // фон полей ввода (QLineEdit, QTextEdit)
    lightPalette.setColor(QPalette::AlternateBase,   QColor(245, 245, 245)); // чередующийся фон в таблицах/списках
    lightPalette.setColor(QPalette::Text,            QColor(0, 0, 0));       // текст в полях ввода
    lightPalette.setColor(QPalette::PlaceholderText, QColor(128, 128, 128)); // плейсхолдер

    // Кнопки — текст чёрный
    lightPalette.setColor(QPalette::Button,          QColor(240, 240, 240)); // фон кнопок
    lightPalette.setColor(QPalette::ButtonText,      QColor(0, 0, 0));       // текст на кнопках — чёрный
    lightPalette.setColor(QPalette::WindowText,      QColor(0, 0, 0));       // дублируем для стилей, берущих цвет отсюда
    lightPalette.setColor(QPalette::Text,            QColor(0, 0, 0));       // дублируем для стилей, берущих цвет отсюда
    lightPalette.setColor(QPalette::BrightText,      QColor(255, 0, 0));     // яркий текст (ошибки)

    // Подсказки
    lightPalette.setColor(QPalette::ToolTipBase,     QColor(255, 255, 220)); // фон тултипов (классический жёлтый)
    lightPalette.setColor(QPalette::ToolTipText,     QColor(0, 0, 0));       // текст тултипов

    // Ссылки и выделение
    lightPalette.setColor(QPalette::Link,            QColor(0, 0, 255));     // гиперссылки
    lightPalette.setColor(QPalette::LinkVisited,     QColor(128, 0, 128));   // посещённые ссылки
    lightPalette.setColor(QPalette::Highlight,       QColor(48, 140, 198));  // фон выделения (как в Fusion)
    lightPalette.setColor(QPalette::HighlightedText, QColor(255, 255, 255)); // текст в выделении

    // Неактивные (disabled) элементы — тёмно-серый текст, чтобы кнопки читались
    lightPalette.setColor(QPalette::Disabled, QPalette::WindowText,      QColor(80, 80, 80));
    lightPalette.setColor(QPalette::Disabled, QPalette::Text,            QColor(80, 80, 80));
    lightPalette.setColor(QPalette::Disabled, QPalette::ButtonText,      QColor(80, 80, 80)); // тёмный текст на disabled-кнопках
    lightPalette.setColor(QPalette::Disabled, QPalette::Highlight,       QColor(220, 220, 220));
    lightPalette.setColor(QPalette::Disabled, QPalette::HighlightedText, QColor(80, 80, 80));
    lightPalette.setColor(QPalette::Disabled, QPalette::Base,            QColor(245, 245, 245));

    a.setPalette(lightPalette);

    a.setWindowIcon(QIcon(":/img/logo.ico"));
    MainWindow w;
    w.show();
    return a.exec();
}
