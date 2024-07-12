#ifndef CIRCUITCONFIGURATORWIDGET_H
#define CIRCUITCONFIGURATORWIDGET_H

#include "CircuitConfiguration.h"
#include "qlabel.h"
#include "qslider.h"
#include <QWidget>

class CircuitConfiguratorWidget : public QWidget
{
    Q_OBJECT
private:
    QString rangeMeasure;
    QMap<int, float> rangeMap;
    QMap<int, float> freqMap;
    QMap<int, int> avgMap;

    const char type;
    QLabel *rangeLabel;
    QLabel *freqLabel;
    QLabel *avgLabel;

    QSlider *rangeSlider;
    QSlider *freqSlider;
    QSlider *avgSlider;

    cConfig lastConfig;
    cConfig currentConfig;

    QSlider *createSlider(int min, int max, void (CircuitConfiguratorWidget::*)(int));
    void setRangeValue(int value);
    void setFreqValue(int value);
    void setAvgValue(int value);


public:
    explicit CircuitConfiguratorWidget(const char type, QWidget *parent = nullptr);
    void resetConfig();
    void keepConfig();
public slots:
     void slotPrepeareConfig();


signals:
    void signalConfigChanged(cConfig config);
};

#endif // CIRCUITCONFIGURATORWIDGET_H
