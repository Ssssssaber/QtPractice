#ifndef CIRCUITDATARECEIVER_H
#define CIRCUITDATARECEIVER_H

#include <QObject>
#undef _GNU_SOURCE
#define _GNU_SOURCE
#undef __USE_GNU
#define __USE_GNU

/*
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <ctype.h>
#include <stdbool.h>
#include <getopt.h>
#include <pthread.h>
#include <signal.h>
#if defined(__GLIBC__) && !(defined(__UCLIBC__) || defined(__MUSL__))
#include <execinfo.h>
#endif
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/signalfd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
*/
#include <libusb-1.0/libusb.h>
#include "libnii.h"
#include <QApplication>
#include "dataprocessor.h"

#define NII_MAX_PACKET_SIZE 64

#define VENDOR_ID      0x0483
#define PRODUCT_ID     0xf125

class CircuitDataReceiver : public QObject
{
    Q_OBJECT

private:
    libnii_device *handle = NULL;
    static void receiveData(void *user_ptr, enum libnii_data_type type, int packet_number, void *data);
    static void handleError(void *user_ptr, int error_code);

public:
    explicit CircuitDataReceiver();
    ~CircuitDataReceiver();

signals:
    void signalSentData(QString data);
};

#endif // CIRCUITDATARECEIVER_H
