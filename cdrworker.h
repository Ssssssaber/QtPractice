#ifndef CDRWORKER_H
#define CDRWORKER_H

#include <QObject>
#include <QApplication>
#include "circuitdatareceiver.h"

class CircuitDataReceiver;
class CDRWorker : public QObject
{
    Q_OBJECT

private:
    CircuitDataReceiver *cdr;

public:
    CDRWorker();
    ~CDRWorker();

public slots:
    void process();

signals:
    void finished(int);
};

#endif // CDRWORKER_H
