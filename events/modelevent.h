/* Модель для хранения журнала сообщений проекта (не сервера OPCUA) */
#ifndef MODELEVENT_H
#define MODELEVENT_H

#include <QAbstractTableModel>
#include <QDateTime>
#include <QColor>
#include <QStringList>

struct EventData {
    QString id;
    QDateTime time;
    bool bactiveState;
    quint16 severity;
    QString sourceName;
    QString type;
    QString message;

    EventData() : bactiveState(false) {}
};


class ModelEvent : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit ModelEvent(QObject *parent = nullptr);
    ~ModelEvent();

private:
    QList<EventData> eventList;
    QStringList columnHeaders;
    uint MAX_EVENTS = 2000;

    //Переопределенные методы QAbstractTableModel
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    //Методы для управления строками
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

public:
    //Публичные методы
    void setMaxEvents(const uint max);
    void addEvent(const EventData &event);
    void append(const QVariantMap &map);
    void removeFirst();
    void clear();

    //Дополнительные методы для удобства
    QList<EventData> getEvents() const;
    QString getEventId(int row) const;
    QString getEventMessage(int row) const;
    void updateEvent(int row, const EventData &event);

signals:

};

#endif // MODELEVENT_H
