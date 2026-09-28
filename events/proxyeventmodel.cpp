#include "proxyeventmodel.h"

ProxyEventModel::ProxyEventModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
}

void ProxyEventModel::setFilterTypes(const QSet<QString> &types)
{
    m_filterTypes = types;
    m_showAll = types.isEmpty(); //если типы не выбраны, показываем всё
    this->invalidateFilter(); //обновляем фильтрацию
}

void ProxyEventModel::addFilterType(const QString &type)
{
    if (!m_filterTypes.contains(type)) {
        m_filterTypes.insert(type);
        m_showAll = false;
        this->invalidateFilter();
    }
}

void ProxyEventModel::removeFilterType(const QString &type)
{
    if (m_filterTypes.remove(type)) {
        m_showAll = m_filterTypes.isEmpty();
        this->invalidateFilter();
    }
}

void ProxyEventModel::clearFilter()
{
    if (!m_filterTypes.isEmpty() || !m_showAll) {
        m_filterTypes.clear();
        m_showAll = true;
        this->invalidateFilter();
    }
}

bool ProxyEventModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    //если нужно показать все строки или модель не существует
    if (m_showAll || !sourceModel()) return true;

    //если нет типов для фильтрации, показываем всё
    if (m_filterTypes.isEmpty()) return true;

    //получаем индекс колонки типа сообщения (колонка 5), получаем тип сообщения
    QModelIndex index = sourceModel()->index(source_row, 5, source_parent);
    if (!index.isValid()) return false;
    QString type = sourceModel()->data(index, Qt::DisplayRole).toString();

    //проверяем, содержится ли тип в наборе для фильтрации
    return m_filterTypes.contains(type);
}
