#ifndef CIRCUITDATARECEIVER_H
#define CIRCUITDATARECEIVER_H

#undef _GNU_SOURCE
#define _GNU_SOURCE
#undef __USE_GNU
#define __USE_GNU

#include <QApplication>
#include <QObject>
#include <QThread>

#include <libusb-1.0/libusb.h>
#include "libnii.h"
#include "xyzcircuitdata.h"
#include "CircuitConfiguration.h"

#define NII_MAX_PACKET_SIZE 64

#define VENDOR_ID      0x0483
#define PRODUCT_ID     0xf125

class CircuitDataReceiver : public QObject
{
    Q_OBJECT

private:

    static libnii_device *handle;
    static uint64_t accel_duration, gyro_duration, accel_ts, gyro_ts;
    static struct libnii_params params;

    static void receiveData(void *user_ptr, enum libnii_data_type type, int packet_number, void *data);
    static void handleError(void *user_ptr, int error_code);
    static void printError(QString format, uint64_t diff);
    static void printError(QString format, const char* str_error);
    static void printError(QString format, int error_code, const char* str_error);
    static void sendError(QString error);
    static xyzCircuitData convertToXyzData(char type, int packet_number, void *rawData);

public:
    CircuitDataReceiver();
    static void connectCircuit();
    static void disconnectCircuit();
    static int checkForValidConfigParams(fullConfig config);
    int setConfigParams();

signals:
};

#endif // CIRCUITDATARECEIVER_H
