#include "circuitconfiguratorwidget.h"
#include "qgridlayout.h"
#include "qpushbutton.h"

QSlider *CircuitConfiguratorWidget::createSlider(int min, int max, void (CircuitConfiguratorWidget::*fptr)(int) )
{
    QSlider *slider = new QSlider(Qt::Horizontal, this);
    slider->setRange(min, max);
    slider->setValue(0);
    slider->setTickPosition(QSlider::TicksBelow);
    slider->setSingleStep(1);

    connect(slider, &QSlider::valueChanged, this, fptr);

    return slider;
}

void CircuitConfiguratorWidget::setRangeLabelValue(int value)
{
    rangeLabel->setText("Current range: " + QString::number(value));
}

void CircuitConfiguratorWidget::setFreqLabelValue(int value)
{
    freqLabel->setText("Current freq: " + QString::number(value));
}

void CircuitConfiguratorWidget::setAvgLabelValue(int value)
{
    avgLabel->setText("Current avr: " + QString::number(value));
}

void CircuitConfiguratorWidget::prepeareConfig()
{
    cConfig config;
    config.type = type;
    if (type != 'M') config.range = rangeSlider->value();
    config.freq = freqSlider->value();
    config.avg = avgSlider->value();

    emit configChanged(config);
}

CircuitConfiguratorWidget::CircuitConfiguratorWidget(const char type, QWidget *parent)
    : QWidget{parent}, type(type)
{
    QLabel *headLabel = new QLabel(QString(type) + " configuration");
    rangeLabel = new QLabel("Current range: 0");
    freqLabel = new QLabel("Current freq: 0");
    avgLabel = new QLabel("Current avr: 0");

    if (type == 'A')
    {
        rangeSlider = createSlider(0, 2, &CircuitConfiguratorWidget::setRangeLabelValue);
        freqSlider = createSlider(0, 2, &CircuitConfiguratorWidget::setFreqLabelValue);
        avgSlider = createSlider(0, 8, &CircuitConfiguratorWidget::setAvgLabelValue);
    }
    else if (type == 'G')
    {
        rangeSlider = createSlider(0, 3, &CircuitConfiguratorWidget::setRangeLabelValue);
        freqSlider = createSlider(0, 2, &CircuitConfiguratorWidget::setFreqLabelValue);
        avgSlider = createSlider(0, 8, &CircuitConfiguratorWidget::setAvgLabelValue);
    }
    else if (type == 'M')
    {
        // rangeSlider = createSlider(0, 3, &CircuitConfiguratorWidget::setRangeLabelValue);
        freqSlider = createSlider(0, 2, &CircuitConfiguratorWidget::setFreqLabelValue);
        avgSlider = createSlider(0, 8, &CircuitConfiguratorWidget::setAvgLabelValue);
    }

    QPushButton *setButton = new QPushButton("Change config");
    connect(setButton, &QPushButton::clicked, this, &CircuitConfiguratorWidget::prepeareConfig);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(headLabel, 0, 0, 1, 2);

    layout->addWidget(freqLabel, 1, 0, 1, 1);
    layout->addWidget(freqSlider, 1, 2, 1, 3);

    layout->addWidget(avgLabel, 2, 0, 1, 1);
    layout->addWidget(avgSlider, 2, 2, 1, 3);

    if (type != 'M')
    {
        layout->addWidget(rangeLabel, 3, 0, 1, 1);
        layout->addWidget(rangeSlider, 3, 2, 1, 3);
    }

    layout->addWidget(setButton, 4, 4, 1, 1);
    setLayout(layout);
}
