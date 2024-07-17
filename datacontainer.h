#ifndef DATACONTAINER_H
#define DATACONTAINER_H

#include "xyzcircuitdata.h"
#include <QObject>

const int maxVectorSize = 2000;
class DataContainer : public QObject
{
    Q_OBJECT

    std::vector<xyzCircuitData> aData;
    std::vector<xyzCircuitData> gData;
    std::vector<xyzCircuitData> mData;

    int dataLifespanInSeconds = 10;

    std::vector<xyzCircuitData> cleanDataListToTime(std::vector<xyzCircuitData> dataToClean, int timeInSeconds);
    std::vector<xyzCircuitData> createListSlice(std::vector<xyzCircuitData> dataList, int size);

public:
    void setDataLifeSpan(int newTime);
    DataContainer(QObject *parent = 0);
    void addData(xyzCircuitData data);
    std::vector<xyzCircuitData> getData(char group);
    void clearData();

private slots:
    void slotCleanup();
};

#endif // DATACONTAINER_H
