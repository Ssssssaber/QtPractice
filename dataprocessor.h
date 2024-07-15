#ifndef DATAPROCESSOR_H
#define DATAPROCESSOR_H

#include "qudpsocket.h"
#include "windowworker.h"


#include <QFile>
#include <QDebug>
#include "xyzcircuitdata.h"
#include "circuitmanager.h"

#include "P7_Trace.h"



class CDRWorker;
class DataProcessor : public QObject
{
    Q_OBJECT

private:
    IP7_Trace *p7Trace;
    IP7_Trace::hModule moduleName;

    static QQueue<xyzCircuitData> dataQueue;

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
    QList<xyzCircuitData> createListSlice(QList<xyzCircuitData> dataList, int size);
    void sendData(xyzCircuitData);
public:
    explicit DataProcessor(int udpPort, QHostAddress clientAddress, CircuitManager *manager, QObject *parent = nullptr);
    static void receiveDataFromDataReceiver(xyzCircuitData);
public slots:
    void slotReadData();

};

#endif // DATAPROCESSOR_H
