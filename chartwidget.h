#ifndef CHARTWIDGET_H
#define CHARTWIDGET_H
#include <QtCore>
#include "P7_Trace.h"
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
    IP7_Trace *p7Trace;
    IP7_Trace::hModule moduleName;

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
        float min = -8.192f;
        float max = 8.192f;
    } chartBoundries;

    void cleanSeries(QLineSeries* data, int timeInSeconds);

public:
    ChartWidget(const QString title = "", QWidget* pwgt = 0);
    void setNewYBoundries(float min, float max);
    void setWindowResult(xyzAnalysisResult);
    void setChartData(xyzCircuitData);
    void cleanAllSeries(int timeInSeconds);
public slots:

};

#endif // CHARTWIDGET_H
