#include "circuitconfiguratorwidget.h"
#include "qgridlayout.h"
#include "qpushbutton.h"

QSlider *CircuitConfiguratorWidget::createSlider(int min, int max, void (CircuitConfiguratorWidget::*fptr)(int) )
{
    QSlider *slider = new QSlider(Qt::Horizontal, this);
    slider->setRange(min, max);
    slider->setTickPosition(QSlider::TicksBelow);
    slider->setSingleStep(1);
    connect(slider, &QSlider::valueChanged, this, fptr);

    return slider;
}

void CircuitConfiguratorWidget::setRangeValue(int value)
{
    rangeLabel->setText("Current range: " + QString::number(rangeMap[value]) + rangeMeasure);
    rangeSlider->setValue(value);
}

void CircuitConfiguratorWidget::setFreqValue(int value)
{
    if (type != 'M') freqLabel->setText("Current freq: " + QString::number(freqMap[value]) + " oper/s");
    else freqLabel->setText("Current duty: " + QString::number(freqMap[value]) + "ms");
    freqSlider->setValue(value);
}

void CircuitConfiguratorWidget::setAvgValue(int value)
{
    avgLabel->setText("Current avr: " +  QString::number(avgMap[value]) );
    avgSlider->setValue(value);
}

void CircuitConfiguratorWidget::slotPrepeareConfig()
{
    cConfig config;
    config.type = type;
    if (type != 'M') config.range = rangeSlider->value();
    else config.range = -1;
    config.freq = freqSlider->value();
    config.avg = avgSlider->value();

    currentConfig = config;

    emit signalConfigChanged(config);
}

CircuitConfiguratorWidget::CircuitConfiguratorWidget(const char type, QWidget *parent)
    : QWidget{parent}, type(type)
{
    QLabel *headLabel = new QLabel(QString(type) + " configuration");
    rangeLabel = new QLabel("Current range: 0");
    freqLabel = new QLabel("Current freq: 0");
    avgLabel = new QLabel("Current avr: 0");

    avgMap[0] = 1; avgMap[1] = 2; avgMap[2] = 4;
    avgMap[3] = 8; avgMap[4] = 16; avgMap[5] = 32;
    avgMap[6] = 64; avgMap[7] = 128; avgMap[8] = 256;

    if (type == 'A')
    {
        freqMap[0] = 1000;
        freqMap[1] = 2000;
        freqMap[2] = 4000;

        rangeMeasure = " m/s^2";
        rangeMap[0] = 20.01;
        rangeMap[1] = 40.18;
        rangeMap[2] = 80.36;

        // struct libnii_params CircuitDataReceiver::params = {
        //     .accel_freq = 0,
        //     .accel_range = 2,
        //     .accel_avr = 8,
        //     .gyro_freq = 0,
        //     .gyro_range = 3,
        //     .gyro_avr = 8,
        //     .magnet_duty = 0,
        //     .magnet_avr = 8,
        //     .press_freq = 0,
        //     .press_filter = 0,
        //     .nv08c_freq = 0,
        //     .display_refresh = 2
        // };
        currentConfig = {.type = QString(type), .freq = 0, .avg = 8, .range = 2};

        rangeSlider = createSlider(0, 2, &CircuitConfiguratorWidget::setRangeValue);
        setRangeValue(currentConfig.range);
        freqSlider = createSlider(0, 2, &CircuitConfiguratorWidget::setFreqValue);
        setFreqValue(currentConfig.freq);
        avgSlider = createSlider(0, 8, &CircuitConfiguratorWidget::setAvgValue);
        setAvgValue(currentConfig.avg);
    }
    else if (type == 'G')
    {
        freqMap[0] = 1000;
        freqMap[1] = 2000;
        freqMap[2] = 4000;

        rangeMeasure = " °/s";
        rangeMap[0] = 75;
        rangeMap[1] = 150;
        rangeMap[2] = 300;
        rangeMap[3] = 900;

        currentConfig = {.type = QString(type), .freq = 0, .avg = 8, .range = 3};

        rangeSlider = createSlider(0, 3, &CircuitConfiguratorWidget::setRangeValue);
        setRangeValue(currentConfig.range);
        freqSlider = createSlider(0, 2, &CircuitConfiguratorWidget::setFreqValue);
        setFreqValue(currentConfig.freq);
        avgSlider = createSlider(0, 8, &CircuitConfiguratorWidget::setAvgValue);
        setAvgValue(currentConfig.avg);
    }
    else if (type == 'M')
    {
        freqMap[0] = 10;
        freqMap[1] = 5;
        freqMap[2] = 2.5;
        freqMap[3] = 1.6;

        currentConfig = {.type = QString(type), .freq = 0, .avg = 8, .range = -1};
        // rangeSlider = createSlider(0, 3, &CircuitConfiguratorWidget::setRangeLabelValue);
        freqSlider = createSlider(0, 3, &CircuitConfiguratorWidget::setFreqValue);
        setFreqValue(currentConfig.freq);
        avgSlider = createSlider(0, 8, &CircuitConfiguratorWidget::setAvgValue);
        setAvgValue(currentConfig.avg);
    }

    keepConfig();



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

    setLayout(layout);
}

void CircuitConfiguratorWidget::resetConfig()
{
    setAvgValue(lastConfig.avg);
    setFreqValue(lastConfig.freq);
    if (type != 'M') setRangeValue(lastConfig.range);
}

void CircuitConfiguratorWidget::keepConfig()
{
    lastConfig = currentConfig;
}
