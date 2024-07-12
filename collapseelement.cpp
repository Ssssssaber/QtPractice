#include "collapseelement.h"

CollapseElement::CollapseElement(const QString & name, const int animationDuration, QWidget *parent) {
    // button
    clButton.setText(name);
    clButton.setStyleSheet("QToolButton { border: none; }");
    clButton.setToolButtonStyle(Qt::ToolButtonTextBesideIcon); // adding icon
    clButton.setArrowType(Qt::ArrowType::RightArrow); // setting arrow
    clButton.setCheckable(true);
    clButton.setChecked(false); // colapsed at the begining

    // scroll
    clScrolAr.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); // hor/ver stat
    clScrolAr.hide();
    clLayout.setSizeConstraint(QLayout::SetMaximumSize);

    // layout
    clLayout.setContentsMargins(0,0,0,0);

    // adding widgets
    clLayout.addWidget(&clButton, Qt::AlignLeft);
    clLayout.addWidget(&clScrolAr, Qt::AlignBottom);

    setLayout(&clLayout);

    QObject::connect(&clButton, &QToolButton::toggled, [this](const bool checked) {
        clButton.setArrowType(checked ? Qt::ArrowType::DownArrow : Qt::ArrowType::RightArrow); // using teranry for changing arrow
        if (checked){
            clScrolAr.show();
            clLayout.setSizeConstraint(QLayout::SetMaximumSize);
            this->adjustSize();
        }
        else{
            clScrolAr.hide();
            clLayout.setSizeConstraint(QLayout::SetFixedSize);
            this->adjustSize();
        }
    });
}
void CollapseElement::addNewElement(QLayout & content) {
    delete clScrolAr.layout(); // deleting old layout
    clScrolAr.setLayout(&content);// setting content as part of a lay out
}
