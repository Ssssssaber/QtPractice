#include "datacontainer.h"


DataContainer::DataContainer(QObject *parent) : QObject(parent)
{
    QTimer *cleanupTimer = new QTimer();
    cleanupTimer->setSingleShot(false);
    cleanupTimer->setInterval(2000);
    connect(cleanupTimer, &QTimer::timeout, this, &DataContainer::slotCleanup);

    cleanupTimer->start();
}

void DataContainer::addData(xyzCircuitData data)
{
    switch (data.group) {
    case 'A':
        aData.push_back(data);
        break;
    case 'G':
        gData.push_back(data);
        break;
    case 'M':
        mData.push_back(data);
        break;
    default:
        break;
    }
}

std::vector<xyzCircuitData> DataContainer::getData(char group)
{
    switch (group) {
    case 'A':
        return aData;
    case 'G':
        return gData;
    case 'M':
        return mData;
    default:
        return std::vector<xyzCircuitData>();
    }
}

void DataContainer::clearData()
{
    aData.clear();
    gData.clear();
    mData.clear();
}

std::vector<xyzCircuitData> DataContainer::cleanDataListToTime(std::vector<xyzCircuitData> dataToClean, int timeInSeconds)
{
    if (dataToClean.empty()) return std::vector<xyzCircuitData>();
    if (dataToClean.size() > maxVectorSize)
    {
        dataToClean = (createListSlice(dataToClean, maxVectorSize));
    }

    std::vector<xyzCircuitData>::iterator it;
    for (it = dataToClean.begin(); it != dataToClean.end(); it++)
    {
        if (dataToClean.front().timestamp - it->timestamp <= timeInSeconds)
            break;
        dataToClean.erase(dataToClean.begin());
    }
    return dataToClean;
}

std::vector<xyzCircuitData> DataContainer::createListSlice(std::vector<xyzCircuitData> dataList, int size)
{
    std::vector<xyzCircuitData> listSlice = std::vector<xyzCircuitData>();

    if (size > dataList.size())
    {
        return dataList;
    }
    std::vector<xyzCircuitData>::iterator it;
    for (it = dataList.end() - size; it != dataList.end(); it++)
    {
        xyzCircuitData data = *it;
        listSlice.push_back(data);
    }

    return listSlice;
}



void DataContainer::slotCleanup()
{
    aData = cleanDataListToTime(aData, dataLifespanInSeconds);
    gData = cleanDataListToTime(gData, dataLifespanInSeconds);
    mData = cleanDataListToTime(mData, dataLifespanInSeconds);
}
