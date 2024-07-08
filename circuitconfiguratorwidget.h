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
    const char type;
    QLabel *rangeLabel;
    QLabel *freqLabel;
    QLabel *avgLabel;

    QSlider *rangeSlider;
    QSlider *freqSlider;
    QSlider *avgSlider;

    QSlider *createSlider(int min, int max, void (CircuitConfiguratorWidget::*)(int));
    void setRangeLabelValue(int value);
    void setFreqLabelValue(int value);
    void setAvgLabelValue(int value);
    void prepeareConfig();

public:
    explicit CircuitConfiguratorWidget(const char type, QWidget *parent = nullptr);

signals:
    void configChanged(cConfig config);
};

#endif // CIRCUITCONFIGURATORWIDGET_H
