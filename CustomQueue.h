#ifndef CUSTOMQUEUE_H
#define CUSTOMQUEUE_H

#include "xyzcircuitdata.h"
#include <array>

struct customQueue
{
    int currentIndex = -1;

    std::array<xyzCircuitData, 100> dataArray;

    void enqueue(xyzCircuitData data)
    {
        if (currentIndex == 99)
        {
            return;
        }
        dataArray[currentIndex + 1] = data;
        currentIndex += 1;
        if (currentIndex > 99)
        {
            currentIndex = 99;
        }
    }

    xyzCircuitData dequeue()
    {
        xyzCircuitData data = dataArray[currentIndex];
        dataArray[currentIndex] = {};
        currentIndex --;
        if (currentIndex < 0)
        {
            currentIndex = -1;
        }
        return data;
    }

    int size()
    {
        return currentIndex;
    }

    bool isEmpty()
    {
        return currentIndex < 0;
    }

    QString toString()
    {
        QString keke = QString::number(size()) + ": ";
        for (int i = 0; i < size(); i++)
        {
            keke += QString::number(dataArray[i].id) + "; ";
        }
        return keke;
    }

};


#endif // CUSTOMQUEUE_H
