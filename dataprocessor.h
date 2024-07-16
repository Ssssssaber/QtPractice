#ifndef DATAPROCESSOR_H
#define DATAPROCESSOR_H

#include "CustomQueue.h"
#include "qudpsocket.h"
#include "windowworker.h"


#include <QFile>
#include <QDebug>
#include <QLinkedList>

#include "xyzcircuitdata.h"
#include "circuitmanager.h"

#include "P7_Trace.h"
#include <vector>
#include<queue>


class CDRWorker;
class DataProcessor : public QObject
{
    Q_OBJECT

private:
    IP7_Trace *p7Trace;
    IP7_Trace::hModule moduleName;

    int queueSize = 0;

    static std::queue<xyzCircuitData> currentVector;

    CircuitManager *manager;

    QMap<int, float> aMap;
    QMap<int, float> gMap;
    const float mConstant = .25f * .0001f; // mT
    const float timeConstant = 1000000000;

    QMap<char, int> lostData;
    QMap<char, int> lastReceived;

    int udpPort;
    QHostAddress clientAddress;
    QUdpSocket* udpDataSocket;

    WindowWorker *windowWorker;
    void processReceivedData(xyzCircuitData data);
    void checkLost(xyzCircuitData data);
    xyzCircuitData transformXyzData(xyzCircuitData data, float transitionConst);
    void addDataWithAnalysisCheck(xyzCircuitData newData);
    std::vector <xyzCircuitData> createListSlice(std::vector<xyzCircuitData> dataList, int size);
    void sendData(xyzCircuitData);
    void readData();
public:
    explicit DataProcessor(int udpPort, QHostAddress clientAddress, CircuitManager *manager, QObject *parent = nullptr);
    static void receiveDataFromDataReceiver(xyzCircuitData);
public slots:
    void slotStart();

};

#endif // DATAPROCESSOR_H
