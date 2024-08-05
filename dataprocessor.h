#ifndef DATAPROCESSOR_H
#define DATAPROCESSOR_H

#include "qudpsocket.h"

#include "windowworker.h"

#include <QFile>
#include <QDebug>

#include "xyzcircuitdata.h"
#include "circuitmanager.h"

#include "P7_Trace.h"
#include <vector>
#include <queue>

class Server;


const int maxQueueSize = 100000;


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
    Server *server;

    DataContainer *container;

    WindowWorker *windowWorker;

    QMap<int, float> aMap;
    QMap<int, float> gMap;
    const float mConstant = .25f * .0001f; // mT
    const long timeConstant = 1e9;

    QMap<char, int> lostData;
    QMap<char, int> lastReceived;

    int udpPort;
    QHostAddress clientAddress;
    QUdpSocket* udpDataSocket;

    // WindowWorker *windowWorker;
    void processReceivedData(xyzCircuitData data);
    void checkLost(xyzCircuitData data);
    xyzCircuitData transformXyzData(xyzCircuitData data, float transitionConst);
    void addDataWithAnalysisCheck(xyzCircuitData newData);
    std::vector <xyzCircuitData> createListSlice(std::vector<xyzCircuitData> dataList, int size);

    void readData();
public:

    explicit DataProcessor(int udpPort, QHostAddress clientAddress, CircuitManager *manager, Server *server, QObject *parent = nullptr);
    static void receiveDataFromDataReceiver(xyzCircuitData);
public slots:
    void slotStart();
    void slotSendData(xyzCircuitData);
signals:

    void signalDataProcessed(xyzCircuitData data);
    void signalPerformWindowing(xyzCircuitData data);

};

#endif // DATAPROCESSOR_H
