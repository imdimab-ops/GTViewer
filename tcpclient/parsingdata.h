/* ParsingData класс, получаем и разбираем данные */
#ifndef PARSINGDATA_H
#define PARSINGDATA_H

#include <QObject>
#include <QVariantMap>
#include <QDateTime>
#include <QTimer>
#include <QStringLiteral>
#include <QStringBuilder>

//-----------------регестрируем глобальные типы данных----------------//
//Слово-состояние сервера
struct STWsrv {
    bool opcconn = false;
    QString srvVersion;
    QVariant timestamp;
    QVariant client_size = QVariant::fromValue(0);
    QVariant uptime;
};
Q_DECLARE_METATYPE(STWsrv)

struct Event {
    QVariant id;
    QVariant time;
    QVariant severity;
    QVariant sourceName;
    QVariant message;
    bool operator==(const Event &other) const { //оператор сравнения "==", чтобы moc не ругался
        return id == other.id &&
               time == other.time &&
               sourceName == other.sourceName &&
               message == other.message;
    }
    bool operator!=(const Event &other) const {
        return !(*this == other);
    }
};
Q_DECLARE_METATYPE(Event)

//Структура аналогового сигнала
struct AI {
    Q_GADGET  //Макрос для QML-доступа без наследования от QObject (при обновлении полей структуры не работают сигналы и слоты, обновлять в ручную!)
    Q_PROPERTY(QVariant timestamp MEMBER timestamp) //для обновления данных в QML (иначе undefined)
    Q_PROPERTY(QVariant pv MEMBER pv)
    Q_PROPERTY(QVariant unit MEMBER unit)
    Q_PROPERTY(QVariant description MEMBER description)
    Q_PROPERTY(QVariant ah MEMBER ah)
    Q_PROPERTY(QVariant ah2 MEMBER ah2)
    Q_PROPERTY(QVariant al MEMBER al)
    Q_PROPERTY(QVariant al2 MEMBER al2)
    Q_PROPERTY(QVariant hyst MEMBER hyst)
    Q_PROPERTY(QVariant wh MEMBER wh)
    Q_PROPERTY(QVariant wl MEMBER wl)
    Q_PROPERTY(QVariant lrv MEMBER lrv)
    Q_PROPERTY(QVariant urv MEMBER urv)
    Q_PROPERTY(QVariant bcw MEMBER bcw)
    Q_PROPERTY(QVariant stw MEMBER stw)
    Q_PROPERTY(QVariant sim MEMBER sim)
    Q_PROPERTY(QVariant tagname MEMBER tagname)
public:
    QVariant timestamp;
    QVariant pv;            //(Process Variable) - текущее значение
    QVariant unit;          //(Unit) - еденица измерения
    QVariant description;   //(Description) - описание
    QVariant ah;    //(Alarm high)
    QVariant ah2;   //(Alarm high 2)
    QVariant al;    //(Alarm low)
    QVariant al2;   //(Alarm low 2)
    QVariant hyst;  //(Hysteresis)
    QVariant wh;    //(Warning high)
    QVariant wl;    //(Warning low)
    QVariant lrv;   //(Lower Range Value) – нижний предел шкалы измерения (0% шкалы)
    QVariant urv;   //(Upper Range Value) – верхний предел шкалы измерения (100% шкалы)
    QVariant bcw = 0;   //borders check word (Byte)
    QVariant stw = 1;   //Word (0bit - rez, 1bit - Bad, 2bit - Warning, 3bit - Alarm) (инициализ.1, т.к. при измеении на 0 Qml не обновляется почему то!(в AI.qml stw инициализ 0))
    QVariant sim = 0;//Manual on (Симуляция включена)
    QVariant tagname;
    bool operator==(const AI &other) const { //оператор сравнения "==", чтобы moc не ругался
        return timestamp == other.timestamp &&
               pv == other.pv &&
               unit == other.unit &&
               description == other.description &&
               ah == other.ah &&
               ah2 == other.ah2 &&
               al == other.al &&
               al2 == other.al2 &&
               hyst == other.hyst &&
               wh == other.wh &&
               wl == other.wl &&
               lrv == other.lrv &&
               urv == other.urv &&
               bcw == other.bcw &&
               stw == other.stw &&
               sim == other.sim &&
               tagname == other.tagname;
    }
    bool operator!=(const AI &other) const {
        return !(*this == other);
    }
};
Q_DECLARE_METATYPE(AI)

//Структура параметра TAG (ModbusTCP, и тд...)
struct TAG {
    Q_GADGET
    Q_PROPERTY(QVariant timestamp MEMBER timestamp)
    Q_PROPERTY(QVariant pv MEMBER pv)
    Q_PROPERTY(QVariant unit MEMBER unit)
    Q_PROPERTY(QVariant tagname MEMBER tagname)
public:
    QVariant timestamp;
    QVariant pv;
    QVariant unit;
    QVariant tagname;
    bool operator==(const TAG &other) const { //оператор сравнения "==", чтобы moc не ругался
        return timestamp == other.timestamp &&
               pv == other.pv &&
               unit == other.unit &&
               tagname == other.tagname;
    }
    bool operator!=(const TAG &other) const {
        return !(*this == other);
    }
};
Q_DECLARE_METATYPE(TAG)

//Структура состояния электродвигателя Motors, Pump, Valves, etc
struct MTR {
    Q_GADGET
    Q_PROPERTY(QVariant timestamp MEMBER timestamp)
    Q_PROPERTY(QVariant mode MEMBER mode)
    Q_PROPERTY(QVariant state MEMBER state)
    Q_PROPERTY(QVariant ctlw MEMBER ctlw)
    Q_PROPERTY(QVariant diagnw MEMBER diagnw)
    Q_PROPERTY(QVariant blockw MEMBER blockw)
    Q_PROPERTY(QVariant diagnd MEMBER diagnd)
    Q_PROPERTY(QVariant blockd MEMBER blockd)
    Q_PROPERTY(QVariant load MEMBER load)
    Q_PROPERTY(QVariant worktime MEMBER worktime)
    Q_PROPERTY(QVariant description MEMBER description)
    Q_PROPERTY(QVariant tagname MEMBER tagname)
public:
    QVariant timestamp;
    QVariant mode;      //enum фактический режим управления
    QVariant state;     //emum состояние двигателя (описание в библиотеке PsTechEE Regul)
    QVariant ctlw;      //byte сигналы управления на ВУ см.PsTechEE
    QVariant diagnw;    //word диагностики неисправностей
    QVariant blockw;    //dword признаки запретов
    QVariant diagnd;    //description
    QVariant blockd;    //description
    QVariant load;      //загрузка (по загрузке обновляем метку времени и выводим на график)
    QVariant worktime;  //наработка, ч
    QVariant description;
    QVariant tagname;
    bool operator==(const MTR &other) const { //оператор сравнения "==", чтобы moc не ругался
        return timestamp == other.timestamp &&
               mode == other.mode &&
               state == other.state &&
               ctlw == other.ctlw &&
               diagnw == other.diagnw &&
               blockw == other.blockw &&
               diagnd == other.diagnd &&
               blockd == other.blockd &&
               load == other.load &&
               worktime == other.worktime &&
               description == other.description &&
               tagname == other.tagname;
    }
    bool operator!=(const MTR &other) const {
        return !(*this == other);
    }
};
Q_DECLARE_METATYPE(MTR)

//Структура точки для добавления на график
struct TagValue {
    QVariant timestamp;
    QVariant value;
};
Q_DECLARE_METATYPE(TagValue)

struct Data {
    Q_GADGET  //Макрос для QML-доступа без наследования от QObject (при обновлении полей структуры не работают сигналы и слоты, обновлять в ручную!)
    //Метеостанция
    Q_PROPERTY(AI PT908 MEMBER PT908) //для обновления данных в QML (иначе undefined)
    Q_PROPERTY(AI AT901 MEMBER AT901)
    Q_PROPERTY(AI TT903 MEMBER TT903)
    //Топливная система
    Q_PROPERTY(AI PT421 MEMBER PT421)
    Q_PROPERTY(AI PT422 MEMBER PT422)
    Q_PROPERTY(AI TT421 MEMBER TT421)
    Q_PROPERTY(AI FC424 MEMBER FC424)
    Q_PROPERTY(AI TT401 MEMBER TT401)
    Q_PROPERTY(AI FV401_2 MEMBER FV401_2)
    Q_PROPERTY(AI PT403 MEMBER PT403)
    Q_PROPERTY(AI PT404 MEMBER PT404)
    //Маслосистема
    Q_PROPERTY(AI LT201 MEMBER LT201)
    Q_PROPERTY(AI TT204 MEMBER TT204)
    Q_PROPERTY(AI PT203 MEMBER PT203)
    Q_PROPERTY(AI PDT204 MEMBER PDT204)
    Q_PROPERTY(AI PT206B MEMBER PT206B)
    Q_PROPERTY(AI PT206A MEMBER PT206A)
    Q_PROPERTY(AI TT205 MEMBER TT205)
    Q_PROPERTY(AI TT206 MEMBER TT206)
    Q_PROPERTY(AI TT207 MEMBER TT207)
    //GTU
    Q_PROPERTY(AI TE521 MEMBER TE521)
    Q_PROPERTY(AI TE522 MEMBER TE522)
    Q_PROPERTY(AI TE523 MEMBER TE523)
    Q_PROPERTY(AI TE524 MEMBER TE524)
    Q_PROPERTY(AI TE525 MEMBER TE525)
    Q_PROPERTY(AI TE531 MEMBER TE531)
    Q_PROPERTY(AI TE532 MEMBER TE532)
    Q_PROPERTY(AI TE533 MEMBER TE533)
    Q_PROPERTY(AI TE534 MEMBER TE534)
    Q_PROPERTY(AI TE535 MEMBER TE535)
    Q_PROPERTY(AI ST50 MEMBER ST50)
    Q_PROPERTY(AI SR50 MEMBER SR50)
    Q_PROPERTY(AI TE701 MEMBER TE701)
    Q_PROPERTY(AI PT701 MEMBER PT701)
    Q_PROPERTY(AI FT901 MEMBER FT901)
    Q_PROPERTY(AI PDT902 MEMBER PDT902)
    Q_PROPERTY(AI FC901 MEMBER FC901)
    Q_PROPERTY(AI TT901 MEMBER TT901)
    Q_PROPERTY(AI TE71 MEMBER TE71)
    Q_PROPERTY(AI VT507 MEMBER VT507)
    Q_PROPERTY(AI VT508 MEMBER VT508)
    Q_PROPERTY(AI VT509 MEMBER VT509)
    Q_PROPERTY(AI ZT503 MEMBER ZT503)
    Q_PROPERTY(AI ZT504 MEMBER ZT504)
    Q_PROPERTY(AI TE501A MEMBER TE501A)
    Q_PROPERTY(AI TE501B MEMBER TE501B)
    Q_PROPERTY(AI TE502A MEMBER TE502A)
    Q_PROPERTY(AI TE502B MEMBER TE502B)
    Q_PROPERTY(AI TE503A MEMBER TE503A)
    Q_PROPERTY(AI TE503B MEMBER TE503B)
    Q_PROPERTY(AI TE504A MEMBER TE504A)
    Q_PROPERTY(AI TE504B MEMBER TE504B)
    //Генератор
    Q_PROPERTY(AI VT501 MEMBER VT501)
    Q_PROPERTY(AI VT502 MEMBER VT502)
    Q_PROPERTY(AI VT503 MEMBER VT503)
    Q_PROPERTY(AI VT504 MEMBER VT504)
    Q_PROPERTY(AI TE590 MEMBER TE590)
    Q_PROPERTY(AI TE591 MEMBER TE591)
    Q_PROPERTY(AI TE592 MEMBER TE592)
    Q_PROPERTY(AI TE593 MEMBER TE593)
    Q_PROPERTY(AI TE594 MEMBER TE594)
    Q_PROPERTY(AI TE595 MEMBER TE595)
    Q_PROPERTY(AI TE596 MEMBER TE596)
    Q_PROPERTY(AI TE597 MEMBER TE597)
    Q_PROPERTY(AI PLoad MEMBER PLoad)
    //Reductor
    Q_PROPERTY(AI VT505 MEMBER VT505)
    Q_PROPERTY(AI VT506 MEMBER VT506)
    Q_PROPERTY(AI VT510 MEMBER VT510)
    Q_PROPERTY(AI ZT501 MEMBER ZT501)
    Q_PROPERTY(AI ZT502 MEMBER ZT502)
    Q_PROPERTY(AI TE576 MEMBER TE576)
    Q_PROPERTY(AI TE577 MEMBER TE577)
    Q_PROPERTY(AI TE578 MEMBER TE578)
    Q_PROPERTY(AI TE579 MEMBER TE579)
    Q_PROPERTY(AI TE580 MEMBER TE580)
    Q_PROPERTY(AI TE581 MEMBER TE581)
    Q_PROPERTY(AI TE582 MEMBER TE582)
    Q_PROPERTY(AI TE583 MEMBER TE583)
    //Starter
    Q_PROPERTY(AI M801_2 MEMBER M801_2)
    Q_PROPERTY(TAG OUT_CURR MEMBER OUT_CURR)
    Q_PROPERTY(TAG OUT_M MEMBER OUT_M)

    Q_PROPERTY(TAG F72 MEMBER F72)
    Q_PROPERTY(TAG WRKTM MEMBER WRKTM)
    Q_PROPERTY(TAG MODE_DESCR MEMBER MODE_DESCR)

public:
    QList<Event> events;
    //Метеостанция
    AI PT908;   //Ратм
    AI AT901;   //Влажность
    AI TT903;   //Татм
    //Топливная система
    AI PT421;   //Ртг на фильтре
    AI PT422;   //Ртг компр
    AI TT421;   //Tтг компр
    AI FC424;   //Расход ТГ
    AI TT401;   //Tтг
    AI FV401_2; //Положение ДГ
    AI PT403;   //Ртг FV401 т.1
    AI PT404;   //Ртг FV401 т.2
    //Маслосистема
    AI LT201;   //Уровень масла
    AI TT204;   //Tмасла МБ
    AI PT203;   //Рм
    AI PDT204;  //Перепад
    AI PT206B;  //Рм ГТД_2
    AI PT206A;  //Рм ГТД_1
    AI TT205;   //Тмвых АВОМ
    AI TT206;   //Tмвых ред
    AI TT207;   //Tмвых ГТД
    //GTU
    AI TE521;   //TE521
    AI TE522;   //TE522
    AI TE523;   //TE523
    AI TE524;   //TE524
    AI TE525;   //TE525
    AI TE531;   //TE531
    AI TE532;   //TE532
    AI TE533;   //TE533
    AI TE534;   //TE534
    AI TE535;   //TE535
    AI ST50;    //Обороты турбины
    AI SR50;    //Ускорение
    AI TE701;   //T после компр
    AI PT701;   //Pk
    AI FT901;   //Gвх турбины
    AI PDT902;  //Разр возд
    AI FC901;   //Gвозд ГТД
    AI TT901;   //Tвх компр
    AI TE71;    //Т выхлопа (средняя)
    AI VT507;   //X
    AI VT508;   //Y
    AI VT509;   //Z
    AI ZT503;   //Вибропер т.1
    AI ZT504;   //Вибропер т.2
    AI TE501A;  //ОК ОП т.1
    AI TE501B;  //ОК ОП т.2
    AI TE502A;  //ОК ОУП т.1
    AI TE502B;  //ОК ОУП т.2
    AI TE503A;  //УРК ОУП т.1
    AI TE503B;  //УРК ОУП т.2
    AI TE504A;  //УУК ОУП т.1
    AI TE504B;  //УУК ОУП т.2
    //Генератор
    AI VT501;   //X, DE
    AI VT502;   //Y, DE
    AI VT503;   //X, NDE
    AI VT504;   //Y, NDE
    AI TE590;   //U, т.1
    AI TE591;   //U, т.2
    AI TE592;   //V, т.1
    AI TE593;   //V, т.2
    AI TE594;   //W, т.1
    AI TE595;   //W, т.2
    AI TE596;   //NDE
    AI TE597;   //DE
    AI PLoad;   //Нагрузка генератора
    //Reductor
    AI VT505;   //X
    AI VT506;   //Y
    AI VT510;   //Z
    AI ZT501;   //НВ т.1
    AI ZT502;   //НВ т.2
    AI TE576;   //Т подш.планет.ряда вх.сторона т.1
    AI TE577;   //Т подш.планет.ряда вх.сторона т.2
    AI TE578;   //Т подш.планет.ряда вх.сторона т.3
    AI TE579;   //Т подш.планет.ряда вых.сторона т.1
    AI TE580;   //Т подш.планет.ряда вых.сторона т.2
    AI TE581;   //Т подш.планет.ряда вых.сторона т.3
    AI TE582;   //НВвх
    AI TE583;   //НВвых
    //Starter
    AI M801_2;      //контроль скорости
    TAG OUT_CURR;   //выходной ток
    TAG OUT_M;      //выходной момент

    TAG F72;    //Частота вращения генератора
    TAG WRKTM;  //Наработка
    TAG MODE_DESCR; //Текущий режим или процесс
    TAG STEP_DESCR; //Описание действий на шаге
    TAG COUNTDOWN;  //Обратный отсчет шагов
};
Q_DECLARE_METATYPE(Data)

struct DataMTR {
    Q_GADGET  //Макрос для QML-доступа без наследования от QObject (при обновлении полей структуры не работают сигналы и слоты, обновлять в ручную!)
    //Насосы
    Q_PROPERTY(MTR M201 MEMBER M201) //для обновления данных в QML (иначе undefined)
    Q_PROPERTY(MTR M202 MEMBER M202)
public:
    MTR M201;   //насос масла 1
    MTR M202;   //насос масла 2
};
Q_DECLARE_METATYPE(DataMTR)

class ParsingData : public QObject
{
    Q_OBJECT

private:
    explicit ParsingData(QObject *parent = nullptr);
    ~ParsingData();

public:
    static ParsingData *getInstance();

private:
    STWsrv m_stw;
    Data m_data;
    DataMTR m_dataMTR;
    QTimer *m_throttleTimer = nullptr;

    void setBit(QVariant &value, const quint8 &bitPosition, const bool &state);
    TagValue fetchValue(const QString &key, const QVariantMap &map);
    void parsingAI(AI &tag, const QString &str, const QVariantMap &map);        //аналоговые параметры
    void parsingMTCP(TAG &tag, const QString &str, const QVariantMap &map);     //данные полученные с ModbusTCP
    void parsingLogic(TAG &tag, const QString &str, const QVariantMap &map);    //данные переменных из алгоритмов (расчетные)
    void parsingMTR(MTR &tag, const QString &str, const QVariantMap &map);
    void currentData(const QVariantMap &map);
    void onThrottleTimer(); //тротлинг обновлений
    void historyData(const QVariantMap &map);

public:
    void engineResponse(const QVariantMap &map);
    void clientRequest(const QVariant &tagname);
    STWsrv getServerSTW(const QVariantMap &map);

signals:
    void dataUpdated(const Data &, const DataMTR &);
    void tagChanged(const QVariant &tagname, const TagValue &tagValue); //сигнал для добавления точки на график
    void sendData(const QVariantMap &);
    void addHisoryData(const QVariant &, const QList<TagValue> &);
    void status(const QString &);
};

#endif // PARSINGDATA_H
