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

    std::vector<xyzCircuitData> currentDataList;


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
        float Xmin =  0;
        float Xmax = 0;
        float Ymin =  0;
        float Ymax = 0;
        float Zmin =  0;
        float Zmax = 0;
    } chartBoundries;

    void cleanSeries(QLineSeries* data, int timeInSeconds);

public:
    ChartWidget(const QString title = "", QWidget* pwgt = 0);
    void setWindowResult(xyzCircuitData);
    void addChartDot(xyzCircuitData);
    void drawAllSeries(int timeInSeconds);
    void changeDataLifespan(int newTime);
private slots:
    void slotCleanDataListToTime();
public slots:
    void slotToggleVisible();
};

#endif // CHARTWIDGET_H
