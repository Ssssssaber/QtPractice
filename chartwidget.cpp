#include "chartwidget.h"
#include "QSpacerItem"
#include "QPushButton"

ChartWidget::ChartWidget(const QString title, QWidget* pwgt) : QWidget(pwgt)
{
    p7Trace = P7_Get_Shared_Trace("ClientDataChannel");

    if (!p7Trace)
    {
        qDebug() << "chart widget is not tracing";
    }
    else
    {
        p7Trace->Register_Module(TM("CWgt"), &moduleName);
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
    // serX->setUseOpenGL(true);


    serY = new QLineSeries(sraY);
    serY->setName("Y-axis");
    // serY->setUseOpenGL(true);


    serZ = new QLineSeries(sraZ);
    serZ->setName("Z-axis");
    // serZ->setUseOpenGL(true);



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
    // serXaxisX->setTitleText("x, м");
    // serXaxisX->setLabelFormat("%.1f");

    QValueAxis* serXaxisY = new QValueAxis(sraX);
    // serXaxisY->setTitleText("x, м");
    // serXaxisY->setLabelFormat("%.1f");

    cViewX->chart()->addAxis(serXaxisX, Qt::AlignLeft);
    cViewX->chart()->addAxis(serXaxisY, Qt::AlignBottom);
    serX->attachAxis(serXaxisX);
    serX->attachAxis(serXaxisY);


    cViewY = new QChartView(sraY);
    cViewY->chart()->addSeries(serY);
    cViewY->chart()->legend()->setAlignment(Qt::AlignRight);
    cViewY->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);

    QValueAxis* serYaxisX = new QValueAxis(sraY);
    // serYaxisX->setTitleText("x, м");
    // serYaxisX->setLabelFormat("%.1f");

    QValueAxis* serYaxisY = new QValueAxis(sraY);
    // serYaxisY->setTitleText("x, м");
    // serYaxisY->setLabelFormat("%.1f");

    cViewY->chart()->addAxis(serYaxisX, Qt::AlignLeft);
    cViewY->chart()->addAxis(serYaxisY, Qt::AlignBottom);
    serY->attachAxis(serYaxisX);
    serY->attachAxis(serYaxisY);


    cViewZ = new QChartView(sraZ);
    cViewZ->chart()->addSeries(serZ);
    cViewZ->chart()->legend()->setAlignment(Qt::AlignRight);
    cViewZ->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);

    QValueAxis* serZaxisX = new QValueAxis(sraZ);
    // serZaxisX->setTitleText("x, м");
    // serZaxisX->setLabelFormat("%.1f");

    QValueAxis* serZaxisY = new QValueAxis(sraZ);
    // serZaxisY->setTitleText("x, м");
    // serZaxisY->setLabelFormat("%.1f");

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

    QString miniTitle;

    if (title == "A")
    {
        miniTitle = "m/s^2";
    }
    if (title == "G")
    {
        miniTitle = "°/s";
    }
    if (title == "M")
    {
        miniTitle = "mT";
    }

    serXaxisX->setTitleText(miniTitle);
    serXaxisY->setTitleText("sec");
    serYaxisX->setTitleText(miniTitle);
    serYaxisY->setTitleText("sec");
    serZaxisX->setTitleText(miniTitle);
    serZaxisY->setTitleText("sec");

    serXaxisX->setLabelFormat("%.2f");
    serXaxisY->setLabelFormat("%.2f");
    serYaxisX->setLabelFormat("%.2f");
    serYaxisY->setLabelFormat("%.2f");
    serZaxisX->setLabelFormat("%.2f");
    serZaxisY->setLabelFormat("%.2f");


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

    QTimer *cleanupTimer = new QTimer(this);
    cleanupTimer->setSingleShot(false);
    cleanupTimer->setInterval(100);

    connect(cleanupTimer, &QTimer::timeout, this, &ChartWidget::slotCleanDataListToTime);
    cleanupTimer->start();
}


void ChartWidget::setWindowResult(xyzCircuitData data)
{
    resX->setText(QString::fromStdString("Windowing result: " + std::to_string(data.x)));
    resY->setText(QString::fromStdString("Windowing result: " + std::to_string(data.y)));
    resZ->setText(QString::fromStdString("Windowing result: " + std::to_string(data.z)));
}

void ChartWidget::addChartDot(xyzCircuitData data)
{
    currentDataList.push_back(data);
}

void ChartWidget::drawAllSeries(int timeInSeconds)
{
    if (currentDataList.empty()) return;

    cViewX->chart()->removeSeries(serX); // removing series from view
    cViewY->chart()->removeSeries(serY);
    cViewZ->chart()->removeSeries(serZ);

    serX->clear();
    serY->clear();
    serZ->clear();

    chartBoundries.Xmin = 1000;
    chartBoundries.Ymin = 1000;
    chartBoundries.Zmin = 1000;
    chartBoundries.Xmax = -1000;
    chartBoundries.Ymax = -1000;
    chartBoundries.Zmax = -1000;
    std::vector<xyzCircuitData>::iterator it;
    for (it = currentDataList.begin(); it != currentDataList.end(); it++)
    {
        serX->append(it->timestamp, it->x);
        serY->append(it->timestamp, it->y);
        serZ->append(it->timestamp, it->z);
        chartBoundries.Xmin = chartBoundries.Xmin < it->x ? chartBoundries.Xmin : it->x;
        chartBoundries.Ymin = chartBoundries.Ymin < it->y ? chartBoundries.Ymin : it->y;
        chartBoundries.Zmin = chartBoundries.Zmin < it->z ? chartBoundries.Zmin : it->z;
        chartBoundries.Xmax = chartBoundries.Xmax > it->x ? chartBoundries.Xmax : it->x;
        chartBoundries.Ymax = chartBoundries.Ymax > it->y ? chartBoundries.Ymax : it->y;
        chartBoundries.Zmax = chartBoundries.Zmax > it->z ? chartBoundries.Zmax : it->z;
    }

    // foreach(xyzCircuitData data, currentDataList)
    // {
    //     serX->append(data.timestamp, data.x);
    //     serY->append(data.timestamp, data.y);
    //     serZ->append(data.timestamp, data.z);
    //     chartBoundries.Xmin = chartBoundries.Xmin < data.x ? chartBoundries.Xmin : data.x;
    //     chartBoundries.Ymin = chartBoundries.Ymin < data.y ? chartBoundries.Ymin : data.y;
    //     chartBoundries.Zmin = chartBoundries.Zmin < data.z ? chartBoundries.Zmin : data.z;
    //     chartBoundries.Xmax = chartBoundries.Xmax > data.x ? chartBoundries.Xmax : data.x;
    //     chartBoundries.Ymax = chartBoundries.Ymax > data.y ? chartBoundries.Ymax : data.y;
    //     chartBoundries.Zmax = chartBoundries.Zmax > data.z ? chartBoundries.Zmax : data.z;
    // }

    float time = currentDataList.back().timestamp;

    cViewX->chart()->axes(Qt::Vertical).back()->setRange(chartBoundries.Xmin,chartBoundries.Xmax); // setting chart axies range
    cViewX->chart()->axes(Qt::Horizontal).back()->setRange(serX->points().first().rx(), time); // setting chart axies range
    cViewX->chart()->addSeries(serX); // restoring series

    cViewY->chart()->axes(Qt::Horizontal).back()->setRange(serY->points().first().rx(), time); // setting chart axies range
    cViewY->chart()->axes(Qt::Vertical).back()->setRange(chartBoundries.Ymin,chartBoundries.Ymax);
    cViewY->chart()->addSeries(serY);

    cViewZ->chart()->axes(Qt::Horizontal).back()->setRange(serZ->points().first().rx(), time); // setting chart axies range
    cViewZ->chart()->axes(Qt::Vertical).back()->setRange(chartBoundries.Zmin,chartBoundries.Zmax);
    cViewZ->chart()->addSeries(serZ);
}

void ChartWidget::changeDataLifespan(int newTime)
{
    dataLifespan = newTime;
}

void ChartWidget::slotCleanDataListToTime()
{
    if (currentDataList.empty()) return;

    int initial = currentDataList.size();
    foreach (xyzCircuitData data, currentDataList)
    {
        if (currentDataList.back().timestamp - data.timestamp <= dataLifespan)
            break;
        currentDataList.erase(currentDataList.begin());
    }

    qDebug() << " before and after " << initial << currentDataList.size();
    // p7Trace->P7_TRACE(moduleName, TM("Cleared %c analyzer arrays: from %d to %d"),
    //                   dataToClean->back().group,
    //                   initial, dataToClean->length());
}

void ChartWidget::slotToggleVisible()
{
    isVisible = !isVisible;
}

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
