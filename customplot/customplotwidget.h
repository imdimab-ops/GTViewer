#ifndef CUSTOMPLOTWIDGET_H
#define CUSTOMPLOTWIDGET_H

#include <QWidget>
#include "qcustomplot.h"
#include "millisecondticker.h"
#include "fieldselectordialog.h"
#include "../tcpclient/tcpthread.h"

//-----------------регестрируем глобальные типы данных----------------//
struct PlotMetadata {
    bool added;
    QString symbol;
    QString name;
    QString unit; //еденицы измерения
    QString description; //еденицы измерения
    QString tagname;
};
//Для легенды
struct TableItem {
    QTableWidgetItem *ptimeItem = nullptr;
    QTableWidgetItem *pvalueItem = nullptr;
    QTableWidgetItem *ptimeItem_2 = nullptr;
    QTableWidgetItem *pvalueItem_2 = nullptr;
    QTableWidgetItem *ptimeItem_d = nullptr;
    QTableWidgetItem *pvalueItem_d = nullptr;
};

class CustomPlotWidget : public QWidget
{
    Q_OBJECT

private:
    explicit CustomPlotWidget(QWidget *parent = nullptr);
    ~CustomPlotWidget();

    void setupUI();
    void setupConnections();
    void closeEvent(QCloseEvent *event) override; //переопределим closeEvent()
    void initCustomPlot();
    void updateCustomPlotSize();
    bool addGraph(const PlotMetadata &metadata);
    void removeGraph(QCPGraph *graph);
    void removeAxis(QCPAxis *axis);
    QCPGraph *pgetGraphByTag(const QString &tagname);
    void updateAxisMargins();
    void clearLegend();
    bool createdLegend();
    void updateLegendSize();
    void fixCursorPositions();
    void updateCursorPosition(QCPItemStraightLine *cursorLine, const double &x);
    void fillParametersRowLegend(QCPItemStraightLine *cursorLine, TableItem &item, const double &time, const double &value);
    void updateGraphDataCount();
    void extensionGraphLines(const double &time); //"подтяжка графика" добавление предыдущих точек к графикам, чтобы не отставали
    void trimToLimit(QCPGraph *graph); //подрезка до лимита точек

    void resizeEvent(QResizeEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

    QCustomPlot *m_customPlot = nullptr;
    inline static CustomPlotWidget *instance = nullptr;
    QCheckBox *m_chbAScroll = nullptr;
    QLabel *m_labelPoints = nullptr;

    static constexpr quint8 AXIS_SPACING = 17; //смещение оси Y

    //"подтяжка графика"
    bool m_throttlingGraphLines = false; //тротлинг для добавления предыдущих точек к графикам, чтобы не отставали
    double m_maxTimeGraph = 0; //время графика который левее продвинулся относительно остальных

    //Для легенды
    QHash<QCPGraph*, TableItem> m_hashLegendItems; //Хэш для хранения графика с кастомными строками легенды (строки хранятся в контейнере)
    QWidget *m_legendWidget = nullptr;
    QTableWidget *m_legendTable = nullptr;

    //Хэш для хранения коннектов связных осей (connect для синхронизации движения осей), для дальнейшего удаления
    QHash<QCPAxis*, QMetaObject::Connection> m_hashAxisConnections;

    //Для визирной линии
    QCPItemStraightLine *m_cursorLine1 = nullptr;
    QCPItemStraightLine *m_cursorLine2 = nullptr;
    QCheckBox *m_chbInterpolation = nullptr;
    double m_cursorLine1PixelX = 0;  //позиция курсора 1 в пикселях
    double m_cursorLine2PixelX = 0;  //позиция курсора 2 в пикселях

    static constexpr quint16 MAX_LENGTH = 20000; //max длина вектора графика
    static constexpr int GRAPH_SIZE = 15;  //количество возможных графиков
    int m_settingGraphCount = 0;
    struct SettingGraph {
        SettingGraph(const QColor &c, const QCPScatterStyle &s) : color(c), scatter(s) {}
        QColor color;
        QCPScatterStyle scatter;
    };
    QList <SettingGraph> m_storeSettingGraph = {
                                                {Qt::black, QCPScatterStyle::ssDot},
                                                {Qt::green, QCPScatterStyle::ssCross},
                                                {Qt::red, QCPScatterStyle::ssPlus},
                                                {Qt::gray, QCPScatterStyle::ssCircle},
                                                {Qt::blue, QCPScatterStyle::ssDisc},
                                                {Qt::cyan, QCPScatterStyle::ssSquare},
                                                {Qt::magenta, QCPScatterStyle::ssDiamond},
                                                {Qt::darkGreen, QCPScatterStyle::ssStar},
                                                {Qt::darkMagenta, QCPScatterStyle::ssTriangle},
                                                {Qt::darkYellow, QCPScatterStyle::ssTriangleInverted},
                                                {Qt::darkCyan, QCPScatterStyle::ssCrossSquare},
                                                {Qt::darkRed, QCPScatterStyle::ssPlusSquare},
                                                };

    FieldSelectorDialog m_fieldSelectorDialog;
    QVariantList m_pendingParameters;

    PlotMetadata parsePlotMetadata(const QVariant &var);

public:
    static CustomPlotWidget *getInstance(QWidget *parent = nullptr);

public slots:
    //Get from UI (Frontend)

    //Set from UI (Frontend)
    void fsetAutoScroll(const bool &arg);
    void fsetCursorLine(QCPItemStraightLine *cursorLine, const bool &arg);
    void fsetAddGraph(const QVariant &var);
    void fsetFillParameters(const QVariant &var);

    void addPointsToGraph(const QVariant &tagname, const QList<TagValue> &tagValues);
    void addPointToGraph(const QVariant &tagname, const TagValue &tagValue);

signals:
    //Local
    void addedGraph(QCPGraph *graph);
    //Update UI (Frontend)
    void fautoScroll(const bool &arg);
    void fcursorLine(const bool &arg);
    void fdataCount(const quint32 &arg); //общее количество точек на графике
    void femitAddGraphW(const QVariant &var);
    void femitStateGgaph(const QVariant &var); //сигнал о состоянии графика (добавили/удалили) подключен в этом классе и в *.js
};

#endif // CUSTOMPLOTWIDGET_H
