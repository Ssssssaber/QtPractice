#ifndef COLLAPSEELEMENT_H
#define COLLAPSEELEMENT_H

#include <QWidget>
#include <QToolButton>
#include <QSpacerItem>
#include <QVBoxLayout>
#include <QScrollArea>

class CollapseElement: public QWidget
{
    Q_OBJECT
private:
    QToolButton clButton;
    QScrollArea clScrolAr;
    QVBoxLayout clLayout;
public:
    CollapseElement(const QString & name = "", const int animationDuration = 100, QWidget *parent = 0);
    void addNewElement(QLayout & content);
};

#endif // COLLAPSEELEMENT_H
