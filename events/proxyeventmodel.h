/* Прокси модель для фильтрации и отображения выбранного в фильтре количества строк */

#ifndef PROXYEVENTMODEL_H
#define PROXYEVENTMODEL_H

#include <QSortFilterProxyModel>

class ProxyEventModel : public QSortFilterProxyModel {
    Q_OBJECT

public:
    explicit ProxyEventModel(QObject *parent = nullptr);

    void setFilterTypes(const QSet<QString> &types);
    void addFilterType(const QString &type);
    void removeFilterType(const QString &type);
    void clearFilter();

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

private:
    QSet<QString> m_filterTypes; //типы для фильтрации (QSet - не упорядоченный контейнер)
    bool m_showAll = true;
};

#endif // PROXYEVENTMODEL_H
