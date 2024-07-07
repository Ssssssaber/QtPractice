#include "circuitdatareceiver.h"

CircuitDataReceiver::CircuitDataReceiver()
{
    libnii_open(&handle, VENDOR_ID, PRODUCT_ID, LIBNII_MATCH_ANY, LIBNII_MATCH_ANY,
                receiveData, handleError, handle);
}

void CircuitDataReceiver::receiveData (void *user_ptr, enum libnii_data_type type, int packet_number, void *data)
{
    (void) user_ptr;

    //if ( __atomic_load_n(&silent, __ATOMIC_SEQ_CST)) return;

    switch ( type ) {
    case LIBNII_ACCEL_DATA:
    {
        libnii_xyz_data_t *xyz = (libnii_xyz_data_t *) data;
        //uint64_t diff, ts = xyz->ts;
        QString data = QString("%1 %2 %3 %4 %5 %6").arg("A", QString::number(packet_number),
                                                        QString::number((int)(xyz->x)), QString::number((int)(xyz->y)), QString::number((int)(xyz->z)),
                                                        QString::number((unsigned long)(xyz->ts)));
        qDebug() << data;
        //print_xyz('A', xyz, packet_number);
        /*
        if ( accel_ts && accel_duration ) {
            if ( ts > accel_ts ) diff = ts - accel_ts;
            else                 diff = accel_ts - ts;
            if ( diff > accel_duration )
                nii_print_error("accel timeout: %lu\n", diff);
        }
        accel_ts = ts;
        */
        DataProcessor::receiveDataFromDataReceiver(data);
    }
    break;
    case LIBNII_GYRO_DATA:
    {
        libnii_xyz_data_t *xyz = (libnii_xyz_data_t *) data;
        //uint64_t diff, ts = xyz->ts;
        //print_xyz('G', xyz, packet_number);
        QString data = QString("%1 %2 %3 %4 %5 %6").arg("G", QString::number(packet_number),
                QString::number((int)(xyz->x)), QString::number((int)(xyz->y)), QString::number((int)(xyz->z)),
                QString::number((unsigned long)(xyz->ts)));
        qDebug() << data;

        /*
        if ( gyro_ts && gyro_duration ) {
            if ( ts > gyro_ts ) diff = ts - gyro_ts;
            else                diff = gyro_ts - ts;
            if ( diff > gyro_duration )
                nii_print_error("gyro timeout:  %lu\n", diff);
        }
        gyro_ts = ts;
        */
        DataProcessor::receiveDataFromDataReceiver(data);
    }
    break;
    case LIBNII_MAGNET_DATA:
        libnii_xyz_data_t *xyz = (libnii_xyz_data_t *) data;
        //print_xyz('M', (libnii_xyz_data_t *) data, packet_number);
        QString data = QString("%1 %2 %3 %4 %5 %6").arg("M", QString::number(packet_number),
                                                        QString::number((int)(xyz->x)), QString::number((int)(xyz->y)), QString::number((int)(xyz->z)),
                                                        QString::number((unsigned long)(xyz->ts)));
        qDebug() << data;
        DataProcessor::receiveDataFromDataReceiver(data);
        break;
    //default:
        //qDebug() << "Other type data were received.";
    }
}

void CircuitDataReceiver::handleError (void *user_ptr, int error_code)
{
    static int prev = LIBNII_SUCCESS;
    (void) user_ptr;

    if ( LIBNII_SUCCESS != error_code && error_code != prev )
        //nii_print_error("code %d: %s\n", error_code, libnii_strerror(error_code));
        qDebug() << "Error: code" << error_code << libnii_strerror(error_code);

    prev = error_code;
}

CircuitDataReceiver::~CircuitDataReceiver()
{
    libnii_close(handle);
}
