// fieldselectordialog.h
#ifndef FIELDSELECTORDAILOG_H
#define FIELDSELECTORDAILOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QListWidget>
#include <QCheckBox>
#include <QLabel>
#include <QScrollArea>
#include <QGroupBox>
#include <QLineEdit>
#include <QMap>
#include <QList>
#include <QScrollArea>
#include <QGroupBox>
#include <QMetaObject>
#include <QMetaProperty>
#include "../tcpclient/tcpthread.h"

class FieldSelectorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FieldSelectorDialog(QWidget *parent = nullptr);
    ~FieldSelectorDialog();

private:
    void setupUI();
    void setupConnections();
    void fillParameters();
    void filterFields(const QString &searchText);
    void updateFieldList();
    QList<QString> getFieldsNamesByType(const QString &typeName = QString());

    //UI элементы
    QVBoxLayout *mainLayout = nullptr;
    QHBoxLayout *buttonLayout = nullptr;
    
    QLineEdit *searchLineEdit = nullptr;
    QCheckBox *chbShowSelected = nullptr;
    QListWidget *fieldListWidget = nullptr;
    QPushButton *btnSelectAll = nullptr;
    QPushButton *btnDeselectAll = nullptr;
    QPushButton *btnApply = nullptr;
    QPushButton *btnCancel = nullptr;
    
    //Данные
    QMap<QString, QCheckBox*> mfieldCheckBoxes;

public:
    void setFieldsParameters(const QVariant &var);
    void setFields(const QVariantList &fields);
    void updateMetadata(const bool &add, const QString &symbol);

signals:
    void addGraph(const QVariant &var);

};

#endif // FIELDSELECTORDAILOG_H
