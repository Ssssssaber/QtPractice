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

    bool isVisible = false;

    QList<xyzCircuitData> currentDataList;


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

    int dataLifespan = 10;

    struct{
        float Xmin = -8.192f;
        float Xmax = 8.192f;
        float Ymin = -8.192f;
        float Ymax = 8.192f;
        float Zmin = -8.192f;
        float Zmax = 8.192f;
    } chartBoundries;

    void cleanSeries(QLineSeries* data, int timeInSeconds);

public:
    ChartWidget(const QString title = "", QWidget* pwgt = 0);
    void setWindowResult(xyzAnalysisResult);
    void addChartDot(xyzCircuitData);
    void drawAllSeries(int timeInSeconds);
private slots:
    void slotCleanDataListToTime();
public slots:
    void slotToggleVisible();
};

#endif // CHARTWIDGET_H
