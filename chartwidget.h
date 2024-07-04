#ifndef CHARTWIDGET_H
#define CHARTWIDGET_H
#include <QtCore>
#include "xyzcircuitdata.h"
#include <QBoxLayout>
#include <QtCharts/QChartView> // to display charts
#include <QtCharts/QLegend> // to add legend to charts
#include <QtCharts/QLineSeries> // to draw line-chart
#include <QtCharts/QCategoryAxis> // to change names of axis
#include <QLabel>

class ChartWidget : public QWidget
{
    Q_OBJECT
private:
    QChartView* cViewX; // creating chart with 1 axies
    QChartView* cViewY;
    QChartView* cViewZ;



    QLineSeries *serX; // line with X-axis
    QLineSeries *serY;
    QLineSeries *serZ;

    QLabel* resX; // labels
    QLabel* resY;
    QLabel* resZ;

    QLabel* ltitle; //title

    struct{
        int xmax;
        int xmin;
        int ymax;
        int ymin;
        int zmax;
        int zmin;
    } axisrn;

public:
    ChartWidget(const QString title = "", QWidget* pwgt = 0);
    void setWindowResult(xyzAnalysisResult);
    void setChartData(xyzCircuitData);
public slots:

};

#endif // CHARTWIDGET_H
