//AppLib бибилиотека приложения
//описываем UDT - пользовательские типы данных

#ifndef APPLIB_H
#define APPLIB_H

#include <QObject>
#include <QOperatingSystemVersion>
#include <QWidget>
#include <QScreen>
#include <QSysInfo>
#include <openssl/aes.h>
#include "version/version.h"

class AppLib : public QObject
{
    Q_OBJECT

private:
    explicit AppLib(QObject *parent = nullptr);
    ~AppLib();

    struct QSS {
        //стили QRadioButton
        QString rb_redGreen;
        QString rb_whiteGreen;
        QString rb_whiteYellow;
        QString rb_whiteRed;
        QString treeView;
        QString treeView_height25;
        QString pb_dragdrop;
    };

public:
    QSS qss;
    static AppLib *getInstance();  //создаем один экземпляр класса (паттерн Singleton)
    static void moveWindowRelative(QWidget* parent, QWidget* child, int margin = 5); //ф-я центровки открытого окна
    /* getterts */
    static QString getOSVersionString();
    static QString getOpenSSLVersionString();
    static QString getHostName() { return QSysInfo::machineHostName(); }
    static QString getAppVersion() { return PROJECT_VERSION_FULL; }
    static QDateTime getUptime();
    static QString getQSSGreyStyle();

signals:

};

#endif // APPLIB_H
