#ifndef CHARTMANAGER_H
#define CHARTMANAGER_H

#include <QObject>
#include <QWidget>
#include "chartwidget.h"
#include "xyzcircuitdata.h"

class ChartManager : public QWidget
{
    Q_OBJECT
private:
    ChartWidget* chartA;
    ChartWidget* chartG;
    ChartWidget* chartM;

    int dataLifespanInSeconds = 10;
    int timeToTimeout = 1000; // miliseconds
    QTimer *cleanupTimer; // cleaner timer
    void cleanup(); // clean all
    void cleanSeriesList(QList<QPoint>* dataToClean, int timeInSeconds); // clean Series

public:
    ChartManager(QWidget* pwgt = 0);

public slots:
    void slotDataReceived(xyzCircuitData);
    void slotAnalysisRecived(xyzAnalysisResult);

signals:

};

#endif // CHARTMANAGER_H
