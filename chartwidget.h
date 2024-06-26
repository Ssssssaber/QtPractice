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
    QChart *chart; // creating chart for accelerator with all axies
    QLineSeries *serX; // line with X-axis
    QLineSeries *serY;
    QLineSeries *serZ;
    QChartView* cView;


public:
    ChartWidget(QWidget* pwgt = 0);
public slots:
    void slotAddData(xyzCircuitData);
};

#endif // CHARTWIDGET_H
