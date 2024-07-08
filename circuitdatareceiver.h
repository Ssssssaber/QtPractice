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
#include <dataprocessor.h>

#define NII_MAX_PACKET_SIZE 64

#define VENDOR_ID      0x0483
#define PRODUCT_ID     0xf125

class CircuitDataReceiver : public QObject
{
    Q_OBJECT

private:
    static std::atomic_bool m_terminator;
    static libnii_device *handle;
    static uint64_t accel_duration, gyro_duration, accel_ts, gyro_ts;
    static struct libnii_params params;
    static bool configExec;

    static void receiveData(void *user_ptr, enum libnii_data_type type, int packet_number, void *data);
    static void handleError(void *user_ptr, int error_code);
    static void printError(QString format, uint64_t diff);
    static void printError(QString format, const char *libnii_strerror);
    static void printError(QString format, int error_code, const char *libnii_strerror);

public:
    CircuitDataReceiver();
    static void connectCircuit();
    static void disconnectCircuit();
    static QString handleConfigParams(char type, int freq, int avr, int range);
    int setCircuitParams();
    void stopConfigExec();
    static int calcDuration(int freq, int avr, int d);

signals:
};

#endif // CIRCUITDATARECEIVER_H
