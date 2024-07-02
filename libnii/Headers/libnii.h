#ifndef _LIBNII_H_
#define _LIBNII_H_

#include <stdint.h>
#include <stdbool.h>
#include <libusb.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LIBNII_MATCH_ANY  (-1)

struct libnii_params {
    int accel_freq;
    int accel_range;
    int accel_avr;
    int gyro_freq;
    int gyro_range;
    int gyro_avr;
    int magnet_duty;
    int magnet_avr;
    int press_freq;
    int press_filter;
    int nv08c_freq;
    int display_refresh;
};

typedef struct __attribute__((__packed__)) {
    int32_t x;
    int32_t y;
    int32_t z;
    uint64_t ts;
} libnii_xyz_data_t;

typedef struct __attribute__((__packed__)) {
    uint32_t pressure;
    int32_t  temperature;
    uint64_t ts;
} libnii_press_data_t;

typedef struct {
    uint64_t  ts;
    uint16_t  lat;        // Lattitude degrees
    uint16_t  lon;        // Longitude degrees
    uint32_t  latm_;      // mantissa of Lattitude minutes
    uint32_t  lonm_;      // mantissa of Longitude minutes
    uint32_t  ms;         // MS
    uint16_t  alt;        // Altitude
    uint16_t  alt_;       // mantissa of Altitude
    uint16_t  speed;      // Speed
    uint16_t  speed_;     // mantissa of Speed
    uint16_t  course;     // Course
    uint16_t  course_;    // mantisse of Course
    uint8_t   latm;       // Lattitude minutes
    uint8_t   lonm;       // Longitude minutes
    uint8_t   NS;         // North/South
    uint8_t   WE;         // West/East
    uint8_t   year;       // YY
    uint8_t   month;      // MM
    uint8_t   day;        // DD
    uint8_t   hours;      // HH
    uint8_t   min;        // MM
    uint8_t   sec;        // SS
    uint8_t   satelites;  // number of satelites
    uint8_t   valid;      // flags
} libnii_gps_data_t;

typedef uint64_t libnii_pps_data_t;

enum libnii_data_type {
    LIBNII_STATUS_DATA,
    LIBNII_GPS_DATA,
    LIBNII_GYRO_DATA,
    LIBNII_ACCEL_DATA,
    LIBNII_MAGNET_DATA,
    LIBNII_PRESSURE_DATA,
    LIBNII_PPS_DATA
};

enum libnii_error {
	/** Success (no error) */
	LIBNII_SUCCESS = 0,
	/** Input/output error */
	LIBNII_ERROR_IO,
	/** Invalid parameter */
	LIBNII_ERROR_INVALID_PARAM,
	/** No such device */
	LIBNII_ERROR_NO_DEVICE,
    /** more than one device with same vendor & product ids */
	LIBNII_ERROR_TOO_MANY_DEVICES,
    /** Entity not found */
	LIBNII_ERROR_NOT_FOUND,
	/** Operation timed out */
	LIBNII_ERROR_TIMEOUT,
	/** System call interrupted (perhaps due to signal) */
	LIBNII_ERROR_INTERRUPTED,
	/** Insufficient memory */
	LIBNII_ERROR_NO_MEM,
    /** Requested to close */
    LIBNII_ERROR_ABORT,
	/** Operation not supported or unimplemented on this platform */
	LIBNII_ERROR_NOT_SUPPORTED,
    /** Packet parsing error */
    LIBNII_ERROR_PARSE,
    /** Other error */
	LIBNII_ERROR_OTHER,
    LIBNII_ERROR_COUNT
    /* NB: Remember to update LIBUSB_ERROR_COUNT below as well as the
	   message strings in strerror.c when adding new error codes here. */
};

typedef struct libnii_device libnii_device;

int libnii_open (libnii_device **handle,
                uint16_t vendor, uint16_t product, int bus, int device,
                void (*data_cb)(void *user_ptr, enum libnii_data_type type, int packet_number, void *data),
                void (*error_cb)(void *user_ptr, int error_code),
                void *user_ptr);

void libnii_close (libnii_device *dev);

bool libnii_is_connected (struct libnii_device *dev);

int libnii_get_version_0 (struct libnii_device *dev, int *major, int *minor, int *patch);
int libnii_get_version (struct libnii_device *dev, int *major, int *minor, int *patch);

int libnii_set_params (struct libnii_device *dev, struct libnii_params *params);

int libnii_read_eeprom (struct libnii_device *dev, uint16_t address, uint64_t *value);
int libnii_write_eeprom (struct libnii_device *dev, uint16_t address, uint64_t value);
int libnii_erase_eeprom (struct libnii_device *dev);

int libnii_setlocale (const char *locale);
const char * libnii_strerror (int errcode);

#ifdef __cplusplus
}
#endif

#endif /* _LIBNII_H_ */
