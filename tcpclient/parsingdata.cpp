#include "parsingdata.h"

ParsingData::ParsingData(QObject *parent)
    : QObject{parent}
{
    /* троттлинг обновлений (хотя данные приходят реже, сделал так, как надо :) */
    m_throttleTimer = new QTimer(this);
    m_throttleTimer->setInterval(200); //200 мс = 5 раз в секунду
    connect(m_throttleTimer, &QTimer::timeout, this, &ParsingData::onThrottleTimer);
}

ParsingData::~ParsingData()
{

}

ParsingData *ParsingData::getInstance()
{
    static ParsingData instance;
    return &instance;
}

void ParsingData::setBit(QVariant &value, const quint8 &bitPosition, const bool &state)
{
    /* устанавливаем битовую маску для статуса канала */
    int intValue = value.toInt();
    if (state) {
        intValue = intValue | (1 << bitPosition); // | - или, & - и
    } else {
        intValue = intValue & ~(1 << bitPosition); //~ инвертируем маску
    }
    value = QVariant::fromValue(intValue);
}

TagValue ParsingData::fetchValue(const QString &key, const QVariantMap &map)
{
    if (key.isEmpty() || map.isEmpty()) return TagValue{};
    if (auto it = map.constFind(key); it != map.constEnd()) { //C++17 init-statement, it живет только внутри if
        const QVariantMap tag = it.value().toMap();
        return TagValue{tag.value(QStringLiteral("TIMESTAMP")), tag.value(QStringLiteral("VALUE"))}; //QStringLiteral - компилируется в статический массив, без аллокации
    }
    return TagValue{};
}

void ParsingData::parsingAI(AI &tag, const QString &str, const QVariantMap &map)
{
    /* Парсим теги "TAG.AI.%1.XXX" - тэг со стороны сервера, пишется в сокет только при изменении!!!
     * если изменения параметра нет, то в протоле его ключ отсутствует
     * тег "TAG.AI.%1.XXX" состоит из контейнера QVariantMap, который содержит в себе данные по следующим ключам:
     * - "VALUE" - значение
     * - "STATUS" - статус данных
     * - "TIMESTAMP" - метка времени данного значения */
    const QString prefix = QStringLiteral("TAG.AI.") % str; //% - QStringBuilder, выделение строки происходит быстрее чем QString("TAG.MTCP.%1").arg(str)
    if (TagValue v = fetchValue(prefix % QStringLiteral(".PV"), map); v.value.isValid() && v.timestamp.isValid()) {
        tag.pv = v.value;
        tag.timestamp = v.timestamp;
        tag.tagname = QString(QStringLiteral("AI.") % str % QStringLiteral(".PV")); //пишем имя тега для запроса исторических данных
        emit tagChanged(tag.tagname, TagValue{tag.timestamp, tag.pv});
    }
    if (TagValue v = fetchValue(prefix % QStringLiteral(".UNIT"), map); v.value.isValid()) tag.unit = v.value;
    if (TagValue v = fetchValue(prefix % QStringLiteral(".DESCRIPTION"), map); v.value.isValid()) tag.description = v.value;
    if (TagValue v = fetchValue(prefix % QStringLiteral(".CONFIG.AH"), map); v.value.isValid()) tag.ah = v.value;
    if (TagValue v = fetchValue(prefix % QStringLiteral(".CONFIG.AH2"), map); v.value.isValid()) tag.ah2 = v.value;
    if (TagValue v = fetchValue(prefix % QStringLiteral(".CONFIG.AL"), map); v.value.isValid()) tag.al = v.value;
    if (TagValue v = fetchValue(prefix % QStringLiteral(".CONFIG.AL2"), map); v.value.isValid()) tag.al2 = v.value;
    if (TagValue v = fetchValue(prefix % QStringLiteral(".CONFIG.HYST"), map); v.value.isValid()) tag.hyst = v.value;
    if (TagValue v = fetchValue(prefix % QStringLiteral(".CONFIG.WH"), map); v.value.isValid()) tag.wh = v.value;
    if (TagValue v = fetchValue(prefix % QStringLiteral(".CONFIG.WL"), map); v.value.isValid()) tag.wl = v.value;
    if (TagValue v = fetchValue(prefix % QStringLiteral(".CONFIG.YMIN"), map); v.value.isValid()) tag.lrv = v.value;
    if (TagValue v = fetchValue(prefix % QStringLiteral(".CONFIG.YMAX"), map); v.value.isValid()) tag.urv = v.value;
    if (TagValue v = fetchValue(prefix % QStringLiteral(".CONFIG.SIGN_CHECK"), map); v.value.isValid()) tag.bcw = v.value;
    if (TagValue v = fetchValue(prefix % QStringLiteral(".Fault"), map); v.value.isValid()) setBit(tag.stw, 1, v.value.toBool());
    if (TagValue v = fetchValue(prefix % QStringLiteral(".Warn"), map); v.value.isValid()) setBit(tag.stw, 2, v.value.toBool());
    if (TagValue v = fetchValue(prefix % QStringLiteral(".Alarm"), map); v.value.isValid()) setBit(tag.stw, 3, v.value.toBool());
    if (TagValue v = fetchValue(prefix % QStringLiteral(".MAN_ON"), map); v.value.isValid()) tag.sim = v.value;
}

void ParsingData::parsingMTCP(TAG &tag, const QString &str, const QVariantMap &map)
{
    /* Парсим теги "TAG.MTCP.%1.XXX" - тэг со стороны сервера, пишется в сокет только при изменении!!!
     * если изменения параметра нет, то в протоле его ключ отсутствует
     * тег "TAG.MTCP.%1.XXX" состоит из контейнера QVariantMap, который содержив в себе данные по следующим ключам:
     * - "VALUE" - значение
     * - "STATUS" - статус данных
     * - "TIMESTAMP" - метка времени данного значения */
    const QString prefix = QStringLiteral("TAG.MTCP.") % str; //% - QStringBuilder, выделение строки происходит быстрее чем QString("TAG.MTCP.%1").arg(str)
    if (TagValue v = fetchValue(prefix, map); v.value.isValid() && v.timestamp.isValid()) {
        tag.pv = v.value;
        tag.timestamp = v.timestamp;
        tag.tagname = QString(QStringLiteral("MTCP.") % str); //пишем имя тега для запроса исторических данных
        emit tagChanged(tag.tagname, TagValue{tag.timestamp, tag.pv});
    }
    if (TagValue v = fetchValue(prefix % QStringLiteral(".UNIT"), map); v.value.isValid()) tag.unit = v.value;
}

void ParsingData::parsingLogic(TAG &tag, const QString &str, const QVariantMap &map)
{
    /* Парсим теги "TAG.LOGIC.%1.XXX" - тэг со стороны сервера, пишется в сокет только при изменении!!!
     * если изменения параметра нет, то в протоле его ключ отсутствует
     * тег "TAG.LOGIC.%1.XXX" состоит из контейнера QVariantMap, который содержив в себе данные по следующим ключам:
     * - "VALUE" - значение
     * - "STATUS" - статус данных
     * - "TIMESTAMP" - метка времени данного значения */
    const QString prefix = QStringLiteral("TAG.LOGIC.") % str; //% - QStringBuilder, выделение строки происходит быстрее чем QString("TAG.MTCP.%1").arg(str)
    if (TagValue v = fetchValue(prefix, map); v.value.isValid() && v.timestamp.isValid()) {
        tag.pv = v.value;
        tag.timestamp = v.timestamp;
        tag.tagname = QString(QStringLiteral("LOGIC.") % str); //пишем имя тега для запроса исторических данных
        emit tagChanged(tag.tagname, TagValue{tag.timestamp, tag.pv});
    }
}

void ParsingData::parsingMTR(MTR &tag, const QString &str, const QVariantMap &map)
{
    /* Парсим теги "TAG.MTR.%1.XXX" - тэг со стороны сервера, пишется в сокет только при изменении!!!
     * если изменения параметра нет, то в протоле его ключ отсутствует
     * тег "TAG.LOGIC.%1.XXX" состоит из контейнера QVariantMap, который содержив в себе данные по следующим ключам:
     * - "VALUE" - значение
     * - "STATUS" - статус данных
     * - "TIMESTAMP" - метка времени данного значения */
    const QString prefix = QStringLiteral("TAG.MTR.") % str; //% - QStringBuilder, выделение строки происходит быстрее чем QString("TAG.MTCP.%1").arg(str)
    if (TagValue v = fetchValue(prefix % QStringLiteral(".CURRENT.VALUE"), map); v.value.isValid() && v.timestamp.isValid()) {
        tag.load = v.value;
        tag.timestamp = v.timestamp;
        tag.tagname = QString(QStringLiteral("MTR.") % str % QStringLiteral(".CURRENT.VALUE")); //пишем имя тега для запроса исторических данных
        emit tagChanged(tag.tagname, TagValue{tag.timestamp, tag.load});
    }
    if (TagValue v = fetchValue(prefix % QStringLiteral(".MODE"), map); v.value.isValid()) tag.mode = v.value;
    if (TagValue v = fetchValue(prefix % QStringLiteral(".STATE"), map); v.value.isValid() && v.timestamp.isValid()) tag.state = v.value;
    if (TagValue v = fetchValue(prefix % QStringLiteral(".CTLW"), map); v.value.isValid()) tag.ctlw = v.value;
    if (TagValue v = fetchValue(prefix % QStringLiteral(".DIAGN.VALUE"), map); v.value.isValid()) tag.diagnw = v.value;
    if (TagValue v = fetchValue(prefix % QStringLiteral(".BLOCK.VALUE"), map); v.value.isValid()) tag.blockw = v.value;
    if (TagValue v = fetchValue(prefix % QStringLiteral(".WORKTIME"), map); v.value.isValid()) tag.worktime = v.value;
    if (TagValue v = fetchValue(prefix % QStringLiteral(".DESCRIPTION"), map); v.value.isValid()) tag.description = v.value;
}

void ParsingData::currentData(const QVariantMap &map)
{
    if (map.isEmpty()) return;

    /* Events */
    if (auto it = map.constFind("EVENTS"); it != map.constEnd() && it->isValid()) { //C++17 init-statement, it живет только внутри if
        m_data.events.clear();
        const QList<QVariant> variantList  = it.value().toList();
        m_data.events.reserve(variantList.size());
        for (const auto &event : variantList) {
            QVariantMap vm = event.toMap();
            Event &e = m_data.events.emplaceBack(Event()); //C++17 конструирование на месте
            e.id  = vm.value(QStringLiteral("id"));
            e.time = vm.value(QStringLiteral("time"));
            e.severity = vm.value(QStringLiteral("severity"));
            e.sourceName = vm.value(QStringLiteral("sourceName"));
            e.message = vm.value(QStringLiteral("message"));
        }
    }
    /* Tags */
    //Метеостанция
    parsingAI(m_data.PT908, "PT908", map);
    parsingAI(m_data.AT901, "AT901", map);
    parsingAI(m_data.TT903, "TT903", map);
    //Топливная система
    parsingAI(m_data.PT421, "PT421", map);
    parsingAI(m_data.PT422, "PT422", map);
    parsingAI(m_data.TT421, "TT421", map);
    parsingAI(m_data.FC424, "FC424", map);
    parsingAI(m_data.TT401, "TT401", map);
    parsingAI(m_data.FV401_2, "FV401_2", map);
    parsingAI(m_data.PT403, "PT403", map);
    parsingAI(m_data.PT404, "PT404", map);
    //Маслосистема
    parsingAI(m_data.LT201, "LT201", map);
    parsingAI(m_data.TT204, "TT204", map);
    parsingAI(m_data.PT203, "PT203", map);
    parsingAI(m_data.PDT204, "PDT204", map);
    parsingAI(m_data.PT206B, "PT206B", map);
    parsingAI(m_data.PT206A, "PT206A", map);
    parsingAI(m_data.TT205, "TT205", map);
    parsingAI(m_data.TT206, "TT206", map);
    parsingAI(m_data.TT207, "TT207", map);
    //GTU
    parsingAI(m_data.TE521, "TE521", map);
    parsingAI(m_data.TE522, "TE522", map);
    parsingAI(m_data.TE523, "TE523", map);
    parsingAI(m_data.TE524, "TE524", map);
    parsingAI(m_data.TE525, "TE525", map);
    parsingAI(m_data.TE531, "TE531", map);
    parsingAI(m_data.TE532, "TE532", map);
    parsingAI(m_data.TE533, "TE533", map);
    parsingAI(m_data.TE534, "TE534", map);
    parsingAI(m_data.TE535, "TE535", map);
    parsingAI(m_data.ST50, "ST50", map);
    parsingAI(m_data.SR50, "SR50", map);
    parsingAI(m_data.TE701, "TE701", map);
    parsingAI(m_data.PT701, "PT701", map);
    parsingAI(m_data.FT901, "FT901", map);
    parsingAI(m_data.PDT902, "PDT902", map);
    parsingAI(m_data.FC901, "FC901", map);
    parsingAI(m_data.TT901, "TT901", map);
    parsingAI(m_data.TE71, "TE71", map);
    parsingAI(m_data.VT507, "VT507", map);
    parsingAI(m_data.VT508, "VT508", map);
    parsingAI(m_data.VT509, "VT509", map);
    parsingAI(m_data.ZT503, "ZT503", map);
    parsingAI(m_data.ZT504, "ZT504", map);
    parsingAI(m_data.TE501A, "TE501A", map);
    parsingAI(m_data.TE501B, "TE501B", map);
    parsingAI(m_data.TE502A, "TE502A", map);
    parsingAI(m_data.TE502B, "TE502B", map);
    parsingAI(m_data.TE503A, "TE503A", map);
    parsingAI(m_data.TE503B, "TE503B", map);
    parsingAI(m_data.TE504A, "TE504A", map);
    parsingAI(m_data.TE504B, "TE504B", map);
    //Генератор
    parsingAI(m_data.VT501, "VT501", map);
    parsingAI(m_data.VT502, "VT502", map);
    parsingAI(m_data.VT503, "VT503", map);
    parsingAI(m_data.VT504, "VT504", map);
    parsingAI(m_data.TE590, "TE590", map);
    parsingAI(m_data.TE591, "TE591", map);
    parsingAI(m_data.TE592, "TE592", map);
    parsingAI(m_data.TE593, "TE593", map);
    parsingAI(m_data.TE594, "TE594", map);
    parsingAI(m_data.TE595, "TE595", map);
    parsingAI(m_data.TE596, "TE596", map);
    parsingAI(m_data.TE597, "TE597", map);
    parsingAI(m_data.PLoad, "PLOAD", map);
    //Reductor
    parsingAI(m_data.VT505, "VT505", map);
    parsingAI(m_data.VT506, "VT506", map);
    parsingAI(m_data.VT510, "VT510", map);
    parsingAI(m_data.ZT501, "ZT501", map);
    parsingAI(m_data.ZT502, "ZT502", map);
    parsingAI(m_data.TE576, "TE576", map);
    parsingAI(m_data.TE577, "TE577", map);
    parsingAI(m_data.TE578, "TE578", map);
    parsingAI(m_data.TE579, "TE579", map);
    parsingAI(m_data.TE580, "TE580", map);
    parsingAI(m_data.TE581, "TE581", map);
    parsingAI(m_data.TE582, "TE582", map);
    parsingAI(m_data.TE583, "TE583", map);
    //Starter
    parsingAI(m_data.M801_2, "M801_2", map);
    parsingMTCP(m_data.OUT_CURR, "DELTA_VFD.OUT_CURR", map);
    parsingMTCP(m_data.OUT_M, "DELTA_VFD.OUT_M", map);
    //MTCP
    parsingMTCP(m_data.F72, "TIK.F72", map); //частота вращения генератора
    //Logic
    parsingLogic(m_data.WRKTM, "WRKTM.OH", map);
    parsingLogic(m_data.MODE_DESCR, "MODE_DESCRIPTION", map);
    parsingLogic(m_data.STEP_DESCR, "STEP_DESCRIPTION", map);
    parsingLogic(m_data.COUNTDOWN, "COUNTDOWN.RT", map);

    //Motors
    parsingMTR(m_dataMTR.M201, "M201", map);
    parsingMTR(m_dataMTR.M202, "M202", map);

    if (m_throttleTimer && !m_throttleTimer->isActive()) {
        m_throttleTimer->start();
    }
    // qDebug() << Q_FUNC_INFO << m_data.M801_2.pv << m_data.M801_2.unit << m_data.OUT_CURR.pv << m_data.OUT_CURR.unit;
}

void ParsingData::onThrottleTimer()
{
    emit dataUpdated(m_data, m_dataMTR);
}

void ParsingData::historyData(const QVariantMap &map)
{
    QVariant tag = map.value("TAG.HISTORY");
    if (tag.isValid()) {
        QVariantList historyList = map.value("DATA").toList();
        QList<TagValue> points;
        for(const auto &variant : std::as_const(historyList)) {
            auto point = variant.toMap();
            TagValue tv = TagValue{point.value(QStringLiteral("TIMESTAMP")), point.value(QStringLiteral("VALUE"))}; //QStringLiteral - компилируется в статический массив, без аллокации
            points.append(std::move(tv));
        }
        emit addHisoryData(tag, points);
        emit status(QString("[Info] %1, TAG.HISTORY: %2, size: %3").arg(Q_FUNC_INFO, tag.toString(), QString::number(historyList.size())));
    }
    // qDebug() << Q_FUNC_INFO << map.size() << map;
}

void ParsingData::engineResponse(const QVariantMap &map)
{
    /* обрабатываем ответы сервера */
    if (map.isEmpty()) return;
    QString response = map.value("SERVER.RESPONSE").toString();

    if (response == QString("ALL_DATA") || response == QString("CURRENT_DATA")) {
        currentData(map);
    }
    if (response == QString("HISTORY_DATA")) {
        historyData(map);
    }
}

void ParsingData::clientRequest(const QVariant &tagname)
{
    if (tagname.isNull()) return;
    emit sendData(QVariantMap{{"CLIENT.REQUEST", "HISTORY_DATA"}, {"TAG.HISTORY", tagname}});
}

STWsrv ParsingData::getServerSTW(const QVariantMap &map)
{
    if (map.isEmpty()) return m_stw;
    if (auto it = map.constFind(QStringLiteral("SERVER.OPCCONNECT")); it != map.constEnd()) m_stw.opcconn = it->toBool();
    if (auto it = map.constFind(QStringLiteral("SERVER.VERSION")); it != map.constEnd()) m_stw.srvVersion = it->toString();
    if (auto it = map.constFind(QStringLiteral("SERVER.TIMESTAMP")); it != map.constEnd()) m_stw.timestamp = it.value();
    if (auto it = map.constFind(QStringLiteral("SERVER.CLIENTSIZE")); it != map.constEnd()) m_stw.client_size = it.value();
    if (auto it = map.constFind(QStringLiteral("SERVER.UPTIME")); it != map.constEnd()) m_stw.uptime = it.value();
    return m_stw;
}
