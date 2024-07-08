#ifndef CIRCUITCONFIGURATION_H
#define CIRCUITCONFIGURATION_H

#include <QtCore>

struct cConfig {
    QString type;
    int freq;
    int avg;
    int range;

public:
    QString toString()
    {
        return QString(type + " %1 %2 %3").arg(freq).arg(avg).arg(range);
    }
};

static cConfig parseStructFromString(QString line)
{
    QList<QString> tokens = line.simplified().split(' ');
    cConfig config;
    config.type = tokens[0];
    config.freq = tokens[1].toInt();
    config.range = tokens[2].toInt();
    config.avg = tokens[3].toInt();

    return config;
}

#endif // CIRCUITCONFIGURATION_H
