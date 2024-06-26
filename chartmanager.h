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

public:
    ChartManager(QWidget* pwgt = 0);

public slots:
    void slotDataReceived(xyzCircuitData);
signals:
    void signalAData(xyzCircuitData);
    void signalGData(xyzCircuitData);
    void signalMData(xyzCircuitData);
};

#endif // CHARTMANAGER_H
