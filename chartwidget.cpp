#include "chartwidget.h"
#include "QSpacerItem"
#include "QPushButton"

ChartWidget::ChartWidget(const QString title, QWidget* pwgt) : QWidget(pwgt)
{
    p7Trace = P7_Get_Shared_Trace("ClientChannel");

    if (!p7Trace)
    {
        qDebug() << "chart widget is not tracing";
    }
    else
    {
        p7Trace->Register_Module(TM("CWgt"), &moduleName);
    }

    if (title == "A")
    {
        chartBoundries.min = -8.192f;
        chartBoundries.max = 8.192f;
    }
    if (title == "G")
    {
        chartBoundries.min = -900;
        chartBoundries.max = 900;
    }
    if (title == "M")
    {
        chartBoundries.min = -8;
        chartBoundries.max = 8;
    }

    QGridLayout* lyChartPairs = new QGridLayout(); // main layout of chartwidget

    QScrollArea* sraX = new QScrollArea(); // carrier of sublayouts
    QVBoxLayout* vboxX = new QVBoxLayout(sraX); // sublayouts of chartviews

    QScrollArea* sraY = new QScrollArea();
    QVBoxLayout* vboxY = new QVBoxLayout(sraY); // a.k.a pairs

    QScrollArea* sraZ = new QScrollArea();
    QVBoxLayout* vboxZ = new QVBoxLayout(sraZ);


    QPushButton* openfullX = new QPushButton(sraX); // button possible will be removed
    openfullX->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed); // can be hor.expanded but have fixed ver.size
    openfullX->setText("◱");

    QPushButton* openfullY = new QPushButton(sraY);
    openfullY->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    openfullY->setText("◱");

    QPushButton* openfullZ = new QPushButton(sraZ);
    openfullZ->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    openfullZ->setText("◱");


    serX = new QLineSeries(sraX); // creating lines with data
    serX->setName("X-axis"); // setting the names of this line
    //serX->setUseOpenGL(true);


    serY = new QLineSeries(sraY);
    serY->setName("Y-axis");
    //serY->setUseOpenGL(true);


    serZ = new QLineSeries(sraZ);
    serZ->setName("Z-axis");
    //serZ->setUseOpenGL(true);



    resX = new QLabel("Windowing result: None", sraX); // Result of windowing
    resX->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding); // takes minimum hor.space and ver.size but can bee expanded

    resY = new QLabel("Windowing result: None", sraY);
    resY->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);

    resZ = new QLabel("Windowing result: None", sraZ);
    resZ->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);


    cViewX = new QChartView(sraX); // creating chart-views
    cViewX->chart()->addSeries(serX); // adding series
    cViewX->chart()->legend()->setAlignment(Qt::AlignRight);
    cViewX->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);

    QValueAxis* serXaxisX = new QValueAxis(sraX);
    serXaxisX->setTitleText("x, м");
    serXaxisX->setLabelFormat("%.1f");

    QValueAxis* serXaxisY = new QValueAxis(sraX);
    serXaxisY->setTitleText("x, м");
    serXaxisY->setLabelFormat("%.1f");

    cViewX->chart()->addAxis(serXaxisX, Qt::AlignLeft);
    cViewX->chart()->addAxis(serXaxisY, Qt::AlignBottom);
    serX->attachAxis(serXaxisX);
    serX->attachAxis(serXaxisY);


    cViewY = new QChartView(sraY);
    cViewY->chart()->addSeries(serY);
    cViewY->chart()->legend()->setAlignment(Qt::AlignRight);
    cViewY->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);

    QValueAxis* serYaxisX = new QValueAxis(sraY);
    serYaxisX->setTitleText("x, м");
    serYaxisX->setLabelFormat("%.1f");

    QValueAxis* serYaxisY = new QValueAxis(sraY);
    serYaxisY->setTitleText("x, м");
    serYaxisY->setLabelFormat("%.1f");

    cViewY->chart()->addAxis(serYaxisX, Qt::AlignLeft);
    cViewY->chart()->addAxis(serYaxisY, Qt::AlignBottom);
    serY->attachAxis(serYaxisX);
    serY->attachAxis(serYaxisY);


    cViewZ = new QChartView(sraZ);
    cViewZ->chart()->addSeries(serZ);
    cViewZ->chart()->legend()->setAlignment(Qt::AlignRight);
    cViewZ->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);

    QValueAxis* serZaxisX = new QValueAxis(sraZ);
    serZaxisX->setTitleText("x, м");
    serZaxisX->setLabelFormat("%.1f");

    QValueAxis* serZaxisY = new QValueAxis(sraZ);
    serZaxisY->setTitleText("x, м");
    serZaxisY->setLabelFormat("%.1f");

    cViewZ->chart()->addAxis(serZaxisX, Qt::AlignLeft);
    cViewZ->chart()->addAxis(serZaxisY, Qt::AlignBottom);
    serZ->attachAxis(serZaxisX);
    serZ->attachAxis(serZaxisY);


    vboxX->addWidget(cViewX, Qt::AlignCenter); // adding widgets to pair
    vboxX->addWidget(openfullX, Qt::AlignCenter);
    vboxX->addWidget(resX);

    sraX->setLayout(vboxX);

    vboxY->addWidget(cViewY, Qt::AlignCenter);
    vboxY->addWidget(openfullY, Qt::AlignCenter);
    vboxY->addWidget(resY);

    vboxZ->addWidget(cViewZ, Qt::AlignCenter);
    vboxZ->addWidget(openfullZ, Qt::AlignCenter);
    vboxZ->addWidget(resZ);


    QScrollArea* scX = new QScrollArea();
    QVBoxLayout* sclayX = new QVBoxLayout(scX);
    scX->setLayout(sclayX);
    scX->setMinimumSize(400,300);

    QScrollArea* scY = new QScrollArea();
    QVBoxLayout* sclayY = new QVBoxLayout(scY);
    scY->setLayout(sclayY);
    scY->setMinimumSize(400,300);

    QScrollArea* scZ = new QScrollArea();
    QVBoxLayout* sclayZ = new QVBoxLayout(scZ);
    scZ->setLayout(sclayZ);
    scZ->setMinimumSize(400,300);



    lyChartPairs->addWidget(sraX,0,0); // adding pairs to main layout
    lyChartPairs->addWidget(sraY,0,1);
    lyChartPairs->addWidget(sraZ,0,2);

    setLayout(lyChartPairs);

    QWidget::connect(openfullX, &QPushButton::released, [=]() {
        if (scX->isHidden()){
            sclayX->addWidget(sraX);
            scX->show();
        }
        else {
            lyChartPairs->addWidget(sraX,0,0);
            scX->close();
        }
    });

    QWidget::connect(openfullY, &QPushButton::released, [=]() {
        if (scY->isHidden()){
            sclayY->addWidget(sraY);
            scY->show();
        }
        else {
            lyChartPairs->addWidget(sraY,0,1);
            scY->close();
        }
    });

    QWidget::connect(openfullZ, &QPushButton::released, [=]() {
        if (scZ->isHidden()){
            sclayZ->addWidget(sraZ);
            scZ->show();
        }
        else {
            lyChartPairs->addWidget(sraZ,0,2);
            scZ->close();
        }
    });

}


void ChartWidget::setNewYBoundries(float min, float max)
{
    chartBoundries.min = min;
    chartBoundries.max = max;
}


void ChartWidget::setWindowResult(xyzAnalysisResult data)
{
    resX->setText(QString::fromStdString("Windowing result: " + std::to_string(data.x)));
    resY->setText(QString::fromStdString("Windowing result: " + std::to_string(data.y)));
    resZ->setText(QString::fromStdString("Windowing result: " + std::to_string(data.z)));
}

void ChartWidget::setChartData(xyzCircuitData data)
{
    if (data.group == 'A')
    {
        qDebug() << "CHART DATA" <<data.toString();
    }
    cViewX->chart()->removeSeries(serX); // removing series from view
    cViewY->chart()->removeSeries(serY);
    cViewZ->chart()->removeSeries(serZ);


    serX->append(data.timestamp, data.x); //adding new point to axis-line
    //axisrn.xmax = data.x > axisrn.xmax ? data.x : axisrn.xmax;
    //axisrn.xmin = data.x < axisrn.xmin ? data.x : axisrn.xmin;

    serY->append(data.timestamp, data.y);
    //axisrn.ymax = data.x > axisrn.ymax ? data.x : axisrn.ymax;
    //axisrn.ymin = data.x < axisrn.ymin ? data.x : axisrn.ymin;

    serZ->append(data.timestamp, data.z);
    //axisrn.zmax = data.x > axisrn.zmax ? data.x : axisrn.zmax;
    //axisrn.zmin = data.x < axisrn.zmin ? data.x : axisrn.zmin;

    cViewX->chart()->axes(Qt::Vertical).back()->setRange(chartBoundries.min,chartBoundries.max); // setting chart axies range
    cViewX->chart()->axes(Qt::Horizontal).back()->setRange(serX->points().first().rx(),data.timestamp); // setting chart axies range
    // serX->detachAxis(serXaxisX);
    cViewX->chart()->addSeries(serX); // restoring series

    cViewY->chart()->axes(Qt::Horizontal).back()->setRange(serY->points().first().rx(),data.timestamp); // setting chart axies range
    cViewY->chart()->axes(Qt::Vertical).back()->setRange(chartBoundries.min,chartBoundries.max);
    cViewY->chart()->addSeries(serY);

    cViewZ->chart()->axes(Qt::Horizontal).back()->setRange(serZ->points().first().rx(),data.timestamp); // setting chart axies range
    cViewZ->chart()->axes(Qt::Vertical).back()->setRange(chartBoundries.min,chartBoundries.max);
    cViewZ->chart()->addSeries(serZ);
}

void ChartWidget::cleanAllSeries(int timeInSeconds)
{
    cViewX->chart()->removeSeries(serX); // removing series from view
    cViewY->chart()->removeSeries(serY);
    cViewZ->chart()->removeSeries(serZ);

    cleanSeries(serX, timeInSeconds); // clean first points (no relatable)
    cleanSeries(serY, timeInSeconds);
    cleanSeries(serZ, timeInSeconds);

    cViewX->chart()->addSeries(serX); // restoring series
    cViewY->chart()->addSeries(serY);
    cViewZ->chart()->addSeries(serZ);
}

// void ChartWidget::slotBoundriesChanged(chartBoundries boundries)
// {

// }

void ChartWidget::cleanSeries(QLineSeries* datasource, int timeInSeconds)
{
    QList<QPointF> data = datasource->points();
    int initial = data.length();
    // float timeDelta = 0;
    foreach(QPointF point, data.toList()){
        if (data.last().x() - point.x() <= timeInSeconds)
            break;
        data.pop_front();
    }
    p7Trace->P7_TRACE(moduleName, TM("Cleared chart arrays: from %d to %d"), initial, data.length());

    datasource->clear();
    datasource->append(data);
}
