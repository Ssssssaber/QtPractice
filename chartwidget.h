#ifndef CHARTWIDGET_H
#define CHARTWIDGET_H
#include <QtCore>
#include "xyzcircuitdata.h"
#include <QBoxLayout>
#include <QtCharts/QChartView> // to display charts
#include <QtCharts/QLegend> // to add legend to charts
#include <QtCharts/QLineSeries> // to draw line-chart
#include <QtCharts/QCategoryAxis> // to change names of axis

class ChartWidget : public QWidget
{
    Q_OBJECT
private:
    QHBoxLayout *hlayout; // horisontal lay out
    QChartView* cViewX; // creating chart with 1 axies
    QChartView* cViewY;
    QChartView* cViewZ;
    QLineSeries *serX; // line with X-axis
    QLineSeries *serY;
    QLineSeries *serZ;



public:
    ChartWidget(QWidget* pwgt = 0);
public slots:
    void slotAddData(xyzCircuitData);
};

#endif // CHARTWIDGET_H
