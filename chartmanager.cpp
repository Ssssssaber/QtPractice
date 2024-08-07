#include "chartmanager.h"
#include "collapseelement.h"

ChartManager::ChartManager(QWidget* pwgt) : QWidget(pwgt)
{
    p7Trace = P7_Get_Shared_Trace("ClientDataChannel");

    if (!p7Trace)
    {
        qDebug() << "chart manager is not tracing";
    }
    else
    {
        p7Trace->Register_Module(TM("CMan"), &moduleName);
    }

    QVBoxLayout* vlayout = new QVBoxLayout(); // main layout for all ChartWidgets


    CollapseElement* collapseA = new CollapseElement("A"); // creating collapse element for this widget
    chartA = new ChartWidget("A"); // creating ChartWidget
    collapseA->addNewElement(*chartA->layout()); // collapse takes layout of Widget

    CollapseElement* collapseG = new CollapseElement("G");
    chartG = new ChartWidget("G");
    collapseG->addNewElement(*chartG->layout());

    CollapseElement* collapseM = new CollapseElement("M");
    chartM = new ChartWidget("M");
    collapseM->addNewElement(*chartM->layout());


    QSpacerItem* verSpacer = new QSpacerItem(0,0,QSizePolicy::Expanding, QSizePolicy::Expanding);


    vlayout->addWidget(collapseA, Qt::AlignTop);
    vlayout->addWidget(collapseG, Qt::AlignTop);
    vlayout->addWidget(collapseM, Qt::AlignTop);
    vlayout->addItem(verSpacer);

    setLayout(vlayout);

    // cleanUp setUp

    cleanupTimer = new QTimer(this);
    cleanupTimer->setSingleShot(false);
    cleanupTimer->setInterval(timeToTimeout);

    connect(cleanupTimer, &QTimer::timeout, this, &ChartManager::cleanup);
    cleanupTimer->start();
}

void ChartManager::slotDataLifespanChanged(int newTime)
{
    dataLifespanInSeconds = newTime;
    chartA->changeDataLifespan(newTime);
    chartG->changeDataLifespan(newTime);
    chartM->changeDataLifespan(newTime);
}

void ChartManager::slotDataReceived(xyzCircuitData data)
{
    if (data.group == 'A')
    {
        chartA->addChartDot(data);
    }
    else if (data.group == 'G')
    {
        chartG->addChartDot(data);
    }
    else if (data.group == 'M')
    {
        chartM->addChartDot(data);
    }
    else
    {
        p7Trace->P7_ERROR(moduleName, TM("Wrong data received by chart"));
        return;
    }
    p7Trace->P7_TRACE(moduleName, TM("%c chart set data"), data.group);
}

void ChartManager::slotAnalysisRecived(xyzCircuitData data)
{
    if (data.group == 'A')
    {
        chartA->setWindowResult(data);
    }
    else if (data.group == 'G')
    {
        chartG->setWindowResult(data);
    }
    else if (data.group == 'M')
    {
        chartM->setWindowResult(data);
    }
    else
    {
        p7Trace->P7_ERROR(moduleName, TM("Wrong data received by chart"));
    }
}



void ChartManager::cleanup()
{
    chartA->drawAllSeries(dataLifespanInSeconds);
    chartG->drawAllSeries(dataLifespanInSeconds);
    chartM->drawAllSeries(dataLifespanInSeconds);
}
