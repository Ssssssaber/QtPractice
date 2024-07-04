#ifndef NII_TEST_H
#define NII_TEST_H

#endif // NII_TEST_H

#undef _GNU_SOURCE
#define _GNU_SOURCE
#undef __USE_GNU
#define __USE_GNU

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

#include <libusb-1.0/libusb.h>
#include "libnii.h"
#include <QApplication>

//#include <gperftools/profiler.h>

#define NII_MAX_PACKET_SIZE 64

#define VENDOR_ID      0x0483
#define PRODUCT_ID     0xf125

typedef struct {
    int accel_freq;
    int accel_range;
    int accel_avr;
    int gyro_freq;
    int gyro_range;
    int gyro_avr;
    int magnet_freq;
    int magnet_avr;
    int press_freq;
    int press_avr;
    int nv08c_freq;
    int display_refresh;
} nii_conf_t;

static struct libnii_params params = {
    .accel_freq = 0,
    .accel_range = 2,
    .accel_avr = 8,
    .gyro_freq = 0,
    .gyro_range = 3,
    .gyro_avr = 8,
    .magnet_duty = 0,
    .magnet_avr = 8,
    .press_freq = 0,
    .press_filter = 0,
    .nv08c_freq = 0,
    .display_refresh = 2
};

static uint64_t accel_duration = 0,
    gyro_duration = 0,
    accel_ts = 0,
    gyro_ts = 0;

static int erase_EEPROM = 0;

static const char *dump = NULL, *conf = NULL;

static int conf_fd = -1, dump_fd = -1;

static int running = 1;
static int exit_flag = 0;
static int silent = 0;

static libnii_device *handle = NULL;

static int ctrl_thread_valid = 0;
static pthread_t ctrl_thread;

static pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;

int start(int argc, char **argv);
