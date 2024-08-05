#include "circuitdatareceiver.h"
#include "circuitmanager.h"
#include "dataprocessor.h"

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

CircuitDataReceiver::CircuitDataReceiver()
{
}

void CircuitDataReceiver::connectCircuit()
{
    int err;
    err = libnii_open(&handle, VENDOR_ID, PRODUCT_ID, LIBNII_MATCH_ANY, LIBNII_MATCH_ANY,
                receiveData, handleError, handle);
    if ( LIBNII_SUCCESS != err )
    {
        printError("failed to open: %1", libnii_strerror(err));
    };
}

void CircuitDataReceiver::disconnectCircuit()
{
    libnii_close(handle);
}

int CircuitDataReceiver::setConfigParams()
{
    int r = -1, attempts = 100;

    while ( ! libnii_is_connected(handle) && (attempts-- > 0)) {
        QThread::sleep(1);
    }

    r = libnii_set_params(handle, &params);
    if ( LIBNII_SUCCESS != r )
        printError("failed to set parameters: %1", libnii_strerror(r));
    return r;
}

void CircuitDataReceiver::printError(QString format, uint64_t diff)
{
    QString err = QString(format).arg(diff);
    qDebug().noquote() << err;
    sendError(err);
}

void CircuitDataReceiver::printError(QString format, const char *str_error)
{
    QString err = QString(format).arg(str_error);
    qDebug().noquote() << err;
    sendError(err);
}

void CircuitDataReceiver::printError(QString format, int error_code, const char *str_error)
{
    QString err = QString(format).arg(QString::number(error_code), str_error);
    qDebug().noquote() << err;
    sendError(err);
}

void CircuitDataReceiver::sendError(QString error)
{
    CircuitManager::receiveErrorFromDataReceiver(error);
}

void CircuitDataReceiver::receiveData(void *user_ptr, enum libnii_data_type type, int packet_number, void *rawData)
{
    (void) user_ptr;

    switch ( type ) {
    case LIBNII_ACCEL_DATA:
    {
        xyzCircuitData data = convertToXyzData('A', packet_number, rawData);
        DataProcessor::receiveDataFromDataReceiver(data);
    }
    return;
    case LIBNII_GYRO_DATA:
    {
        xyzCircuitData data = convertToXyzData('G', packet_number, rawData);
        DataProcessor::receiveDataFromDataReceiver(data);
    }
    return;
    case LIBNII_MAGNET_DATA:
    {
        xyzCircuitData data = convertToXyzData('M', packet_number, rawData);
        DataProcessor::receiveDataFromDataReceiver(data);
    }
    return;
    default:
        // Other type data were received
    return;
    }
}

xyzCircuitData CircuitDataReceiver::convertToXyzData(char type, int packet_number, void *rawData)
{
    libnii_xyz_data_t *xyz = (libnii_xyz_data_t *) rawData;

    xyzCircuitData data;
    data.group = type;
    data.id = packet_number;
    data.x = (float)(xyz->x);
    data.y = (float)(xyz->y);
    data.z = (float)(xyz->z);
    data.timestamp = (unsigned long)(xyz->ts);

    return data;
}

void CircuitDataReceiver::handleError (void *user_ptr, int error_code)
{
    static int prev = LIBNII_SUCCESS;
    (void) user_ptr;

    if ( LIBNII_SUCCESS != error_code && error_code != prev )
        printError("Error: code %1 %2", error_code, libnii_strerror(error_code));

    prev = error_code;
}

int CircuitDataReceiver::checkForValidConfigParams(fullConfig config)
{
    QString err = "";
    int r = -1;

    // accelerometer configuration
    if ((config.aFreq < 0) || (config.aFreq > 2))
    {
        err += "Accelerator frequency option must be in 0-2 range\n";
    }
    else params.accel_freq = config.aFreq;

    if ((config.aAvg < 0) || (config.aAvg > 8))
    {
        err += "Accelerator average option must be in 0-8 range\n";
    }
    else params.accel_avr = config.aAvg;

    if ((config.aRange < 0) || (config.aRange > 2))
    {
        err += "Accelerator range option must be in 0-2 range\n";
    }
    else params.accel_range = config.aRange;

    // gyroscope configuration
    if ((config.gFreq < 0) || (config.gFreq > 2))
    {
        err += "Gyroscope frequency option must be in 0-2 range\n";
    }
    else params.gyro_freq = config.gFreq;

    if ((config.gAvg < 0) || (config.gAvg > 8))
    {
        err += "Gyroscope average option must be in 0-8 range\n";
    }
    else params.gyro_avr = config.gAvg;

    if ((config.gRange < 0) || (config.gRange > 3))
    {
        err += "Gyroscope range option must be in 0-3 range\n";
    }
    else params.gyro_range = config.gRange;

    // magnetometer configuration
    if ((config.mFreq < 0) || (config.mFreq > 3))
    {
        err += "Magnet frequency option must be in 0-3 range\n";
    }
    else params.magnet_duty = config.mFreq;

    if ((config.mAvg < 0) || (config.mAvg > 8))
    {
        err += "Magnet average option must be in 0-8 range\n";
    }
    else params.magnet_avr = config.mAvg;

    if(err != "")
    {
        qDebug().noquote() << err;
    }
    else
    {
        err = "Received configuration is valid";
        r = 0;
    }
    sendError(err);

    return r;
}
