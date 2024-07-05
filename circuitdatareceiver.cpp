#include "circuitdatareceiver.h"

libnii_device* CircuitDataReceiver::handle = NULL;
uint64_t CircuitDataReceiver::accel_duration = 0,
    CircuitDataReceiver::gyro_duration = 0,
    CircuitDataReceiver::accel_ts = 0,
    CircuitDataReceiver::gyro_ts = 0;

struct libnii_params CircuitDataReceiver::params = {
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

void CircuitDataReceiver::connectCircuit()
{
    libnii_open(&handle, VENDOR_ID, PRODUCT_ID, LIBNII_MATCH_ANY, LIBNII_MATCH_ANY,
                receiveData, handleError, handle);
}

void CircuitDataReceiver::disconnectCircuit()
{
    libnii_close(handle);
}

QString CircuitDataReceiver::handleUserMagnetParams(int freq, int avr)
{
    QString err = "";
    if ((freq < 0) || (freq > 3))
    {
        err += "Magnet frequency option must be in 0-3 range\n";
    }
    else params.magnet_duty = freq;

    if ((avr < 0) || (avr > 8))
    {
        err += "Magnet average option must be in 0-8 range";
    }
    else params.magnet_avr = avr;

    if(err != "")
    {
        qDebug().noquote() << err;
    }

    return err;
}

QString CircuitDataReceiver::setCircuitParams()
{
    int r;
    QString err = "";
    //struct libnii_device *dev = (struct libnii_device *) handle;

    while ( ! libnii_is_connected(handle) ) {
        usleep(10000);
    }

    r = libnii_set_params(handle, &params);
    if ( LIBNII_SUCCESS != r )
    {
        err = QString("failed to set parameters: %1").arg(libnii_strerror(r));
    }

    return err;
}

int CircuitDataReceiver::calcDuration(int freq, int avr, int d)
{
    uint64_t duration = 0;

    switch ( freq ) {
    case 0: duration = 1000000 * d; break;
    case 1: duration = 500000 * d; break;
    case 2: duration = 250000 * d; break;
    }

    switch ( avr ) {
    case 8: duration <<= 8; break;
    case 7: duration <<= 7; break;
    case 6: duration <<= 6; break;
    case 5: duration <<= 5; break;
    case 4: duration <<= 4; break;
    case 3: duration <<= 3; break;
    case 2: duration <<= 2; break;
    case 1: duration <<= 1; break;
    }

    return duration;
}

void CircuitDataReceiver::printError(QString format, uint64_t diff)
{
    qDebug().noquote() << QString(format).arg(diff);
}

void CircuitDataReceiver::printError(QString format, const char *str_error)
{
    qDebug().noquote() << QString(format).arg(str_error);
}

void CircuitDataReceiver::printError(QString format, int error_code, const char *str_error)
{
    qDebug().noquote() << QString(format).arg(QString::number(error_code), str_error);
}

void CircuitDataReceiver::receiveData(void *user_ptr, enum libnii_data_type type, int packet_number, void *data)
{
    (void) user_ptr;

    switch ( type ) {
    case LIBNII_ACCEL_DATA:
    {
        libnii_xyz_data_t *xyz = (libnii_xyz_data_t *) data;
        uint64_t diff, ts = xyz->ts;
        QString data = QString("%1 %2 %3 %4 %5 %6").arg("A", QString::number(packet_number),
                                                        QString::number((int)(xyz->x)),
                                                        QString::number((int)(xyz->y)),
                                                        QString::number((int)(xyz->z)),
                                                        QString::number((unsigned long)(xyz->ts)));
        qDebug().noquote() << data;
        DataProcessor::receiveDataFromDataReceiver(data);

        if ( accel_ts && accel_duration ) {
            if ( ts > accel_ts ) diff = ts - accel_ts;
            else                 diff = accel_ts - ts;
            if ( diff > accel_duration )
                printError("accel timeout: %1", diff);
        }
        accel_ts = ts;
    }
    break;
    case LIBNII_GYRO_DATA:
    {
        libnii_xyz_data_t *xyz = (libnii_xyz_data_t *) data;
        uint64_t diff, ts = xyz->ts;
        QString data = QString("%1 %2 %3 %4 %5 %6").arg("G", QString::number(packet_number),
                                                        QString::number((int)(xyz->x)),
                                                        QString::number((int)(xyz->y)),
                                                        QString::number((int)(xyz->z)),
                                                        QString::number((unsigned long)(xyz->ts)));
        qDebug().noquote() << data;
        DataProcessor::receiveDataFromDataReceiver(data);

        if ( gyro_ts && gyro_duration ) {
            if ( ts > gyro_ts ) diff = ts - gyro_ts;
            else                diff = gyro_ts - ts;
            if ( diff > gyro_duration )
                printError("gyro timeout:  %lu\n", diff);
        }
        gyro_ts = ts;
    }
    break;
    case LIBNII_MAGNET_DATA:
    {
        libnii_xyz_data_t *xyz = (libnii_xyz_data_t *) data;
        QString data = QString("%1 %2 %3 %4 %5 %6").arg("M", QString::number(packet_number),
                                                        QString::number((int)(xyz->x)),
                                                        QString::number((int)(xyz->y)),
                                                        QString::number((int)(xyz->z)),
                                                        QString::number((unsigned long)(xyz->ts)));
        qDebug().noquote() << data;
        DataProcessor::receiveDataFromDataReceiver(data);
    }
    break;
    default:
        qDebug() << "Other type data were received.";
    }
}

void CircuitDataReceiver::handleError(void *user_ptr, int error_code)
{
    static int prev = LIBNII_SUCCESS;
    (void) user_ptr;

    if ( LIBNII_SUCCESS != error_code && error_code != prev )
        printError("Error: code %1: %2", error_code, libnii_strerror(error_code));

    prev = error_code;
}

