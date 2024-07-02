#ifndef _NII_INTERNAL_H_
#define _NII_INTERNAL_H_

#define _GNU_SOURCE
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
#include <limits.h>
#include <getopt.h>
#include <pthread.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/signalfd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/wait.h>

#include <libusb.h>

#include "libnii.h"
#include "list.h"

#define LIBNII_MAX_PACKET_SIZE 64
#define LIBNII_POOL_SIZE 256

enum libnii_responce_type {
    LIBNII_RESPONCE_VERSION,
    LIBNII_RESPONCE_CONFIG,
    LIBNII_RESPONCE_ERASE,
    LIBNII_RESPONCE_READ,
    LIBNII_RESPONCE_COUNT
};

struct libnii_device;

struct libnii_urb {
    struct list_head node;
    struct libusb_transfer * xfr;
    int num;
    struct libnii_device * dev;
};

struct libnii_task {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int done;
};

struct libnii_device {
    libusb_device_handle *handle;
    libusb_context *ctx;
    libusb_hotplug_callback_handle attach_cb_handle;
    libusb_hotplug_callback_handle detach_cb_handle;
    uint16_t vendor;
    uint16_t product;
    int bus;
    int port;
    int connected;
    int running;
    int completed;
    int completed_counter;
    int packet_counter;
    int version;
    int eeprom_erased;
    uint16_t eeprom_address;
    uint64_t eeprom_value;
    struct libnii_params params;
    void (*data_cb)(void *user_ptr, enum libnii_data_type type, int packet_number, void *data);
    void (*error_cb)(void *user_ptr, int error_code);
    void *user_ptr;
    pthread_t event_thread;
    pthread_mutex_t error_mutex;
    pthread_mutex_t eeprom_mutex;
    struct libnii_task task[LIBNII_RESPONCE_COUNT];
    struct list_head pool_list;
    struct list_head submit_list;
    struct libnii_urb urbs[LIBNII_POOL_SIZE];
    uint8_t *buffer;
};

typedef union
{
    struct {
        uint8_t Freq:       2;  /* 0 — 1Hz, 1 — 2Hz 2 — 5Hz, 3 — 10Hz */
        uint8_t Format:     2;  /* 0 — DDD.DDDDDD 1 — DDMM.MMM 2 — DDMMSS.SS */
        uint8_t reserve:    4;
    };
    uint8_t value;
} nii_gps_conf_t;

typedef union {
    struct {
       uint8_t Freq:      2;    /* 0 — 1000Hz, 1 — 2000Hz 2 — 4000Hz */
       uint8_t Range:     2;    /* 0 — 75º/sec 1 — 150º/sec 2 — 300º/sec 3 — 900º/sec */
       uint8_t MovAver:   4;    /* Moving average value is (MovAver + 1) * 0,1 */
    };
    uint8_t value;
} nii_gyro_conf_t;

typedef union {
    struct {
       uint8_t Freq:      2;    /* 0 — 1000Hz, 1 — 2000Hz 2 — 4000Hz */
       uint8_t Range:     2;    /* 0 — 2.048g 1 — 4.096g 2 — 8.192g */
       uint8_t MovAver:   4;    /* Moving average value is (MovAver + 1) * 0,1 */
    };
    uint8_t value;
} nii_accel_conf_t;

typedef union
{
    struct {
        uint8_t Freq:       2;  /* 0 — 100Hz, 1 — 200Hz 2 — 400Hz, 3 — 600Hz */
        uint8_t reserve:    2;
        uint8_t MovAver:    4;  /* Moving average value is (MovAver + 1) * 0,1 */
    };
    uint8_t value;
} nii_mag_conf_t;

typedef union {
    struct {
        uint8_t Freq:       2;  /* 0 — 2Hz, 1 — 4Hz 2 — 8Hz, 3 — 16Hz */
        uint8_t reserve:    2;
        uint8_t MovAver:    4;  /* Moving average value is (MovAver + 1) * 0,1 */
    };
    uint8_t value;
} nii_press_conf_t;

typedef struct {
    uint16_t    pattern;
    uint8_t     size;
    uint8_t     count;
} nii_packet_header_t;

#define NII_PACKET_HEADER_MARKER    0xaa55

#define NII_PACKET_TYPE_VER    0
#define NII_PACKET_TYPE_CONFIG 1
#define NII_PACKET_TYPE_STATUS 2
#define NII_PACKET_TYPE_GPS    3
#define NII_PACKET_TYPE_GYRO   4
#define NII_PACKET_TYPE_ACCEL  5
#define NII_PACKET_TYPE_MAGNET 6
#define NII_PACKET_TYPE_PRESS  7
// EEPROM:
#define NII_PACKET_TYPE_ERASE  8
#define NII_PACKET_TYPE_WRITE  9
#define NII_PACKET_TYPE_READ   10

#define NII_PACKET_TYPE_PPS    11

#endif // _NII_INTERNAL_H_
