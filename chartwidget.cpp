#include "chartwidget.h"
#include "QSpacerItem"

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

    QVBoxLayout* vlayout = new QVBoxLayout(); // main layout
    QHBoxLayout* hlayout = new QHBoxLayout(); // sub layout

    vlayout->setSpacing(0);

    QVBoxLayout* vboxX = new QVBoxLayout();
    QVBoxLayout* vboxY = new QVBoxLayout();
    QVBoxLayout* vboxZ = new QVBoxLayout();

    serX = new QLineSeries(); // creating lines
    serY = new QLineSeries();
    serZ = new QLineSeries();

    // serX->setName("X-axis"); // setting the names
    // serY->setName("Y-axis");
    // serZ->setName("Z-axis");

    cViewX = new QChartView(); // creating chart-views
    cViewY = new QChartView();
    cViewZ = new QChartView();

    resX = new QLabel("Windowing result: None");
    resY = new QLabel("Windowing result: None");
    resZ = new QLabel("Windowing result: None");

    cViewX->chart()->addSeries(serX); // adding series
    cViewY->chart()->addSeries(serY);
    cViewZ->chart()->addSeries(serZ);

    cViewX->chart()->legend()->setAlignment(Qt::AlignRight);
    cViewY->chart()->legend()->setAlignment(Qt::AlignRight);
    cViewZ->chart()->legend()->setAlignment(Qt::AlignRight);

    cViewX->chart()->createDefaultAxes(); // adding DefaultAxes lines to chart-views
    cViewY->chart()->createDefaultAxes();
    cViewZ->chart()->createDefaultAxes();

    vboxX->addWidget(cViewX); // adding widgets to VBox pair
    vboxX->addWidget(resX);

    vboxY->addWidget(cViewY);
    vboxY->addWidget(resY);

    vboxZ->addWidget(cViewZ);
    vboxZ->addWidget(resZ);

    hlayout->addItem(vboxX); // adding VBoxes to sub layout
    hlayout->addItem(vboxY);
    hlayout->addItem(vboxZ);

    ltitle = new QLabel(title); // setting title
    vlayout->addWidget(ltitle);
    vlayout->addItem(hlayout);

    setLayout(vlayout);

}

void ChartWidget::setWindowResult(xyzAnalysisResult data)
{
    resX->setText(QString::fromStdString("Windowing result: " + std::to_string(data.x)));
    resY->setText(QString::fromStdString("Windowing result: " + std::to_string(data.y)));
    resZ->setText(QString::fromStdString("Windowing result: " + std::to_string(data.z)));
}

void ChartWidget::setChartData(xyzCircuitData data)
{
    qDebug() << data.timestamp << data.x;

    cViewX->chart()->removeSeries(serX); // removing series from view
    cViewY->chart()->removeSeries(serY);
    cViewZ->chart()->removeSeries(serZ);

    serX->append(data.timestamp, data.x); //adding new point to axis-line
    axisrn.xmax = data.x > axisrn.xmax ? data.x : axisrn.xmax;
    axisrn.xmin = data.x < axisrn.xmin ? data.x : axisrn.xmin;

    serY->append(data.timestamp, data.y);
    axisrn.ymax = data.x > axisrn.ymax ? data.x : axisrn.ymax;
    axisrn.ymin = data.x < axisrn.ymin ? data.x : axisrn.ymin;

    serZ->append(data.timestamp, data.z);
    axisrn.zmax = data.x > axisrn.zmax ? data.x : axisrn.zmax;
    axisrn.zmin = data.x < axisrn.zmin ? data.x : axisrn.zmin;

    cViewX->chart()->axes(Qt::Vertical).back()->setRange(axisrn.xmin,axisrn.xmax); // setting chart axies range
    cViewX->chart()->axes(Qt::Horizontal).back()->setRange(0,data.timestamp); // setting chart axies range
    cViewX->chart()->addSeries(serX); // restoring series

    cViewY->chart()->axes(Qt::Vertical).back()->setRange(axisrn.ymin,axisrn.ymax);
    cViewY->chart()->axes(Qt::Horizontal).back()->setRange(0,data.timestamp);
    cViewY->chart()->addSeries(serY);

    cViewZ->chart()->axes(Qt::Vertical).back()->setRange(axisrn.zmin,axisrn.zmax);
    cViewZ->chart()->axes(Qt::Horizontal).back()->setRange(0,data.timestamp);
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

void ChartWidget::cleanSeries(QLineSeries* datasource, int timeInSeconds)
{
    QList<QPointF> data = datasource->points();
    int initial = data.length();
    foreach(QPointF point, data.toList()){
        if (data.last().x() - point.x() <= timeInSeconds)
            break;
        data.pop_front();
    }
    p7Trace->P7_TRACE(moduleName, TM("Cleared chart arrays: from %d to %d"), initial, data.length());

    datasource->clear();
    datasource->append(data);
}
