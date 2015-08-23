#include "workindicator.h"
#include <QPainter>

WorkIndicator::WorkIndicator(QWidget *parent) :
    QLabel(parent)
{
    setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    resize(100,100);

    redCircle = std::make_shared<QPixmap>(width(),height());
    std::unique_ptr<QPainter> paintRed(new QPainter(redCircle.get()));
    QPen redPen(Qt::red);
    QBrush redBrush(Qt::red);
    paintRed->setPen(redPen);
    paintRed->setBrush(redBrush);
    paintRed->fillRect(0,0,width(),height(),parent->palette().color(QPalette::Background));
    paintRed->drawEllipse(0,height()/4.0,width()/2.2,height()/2.2);

    greenCircle = std::make_shared<QPixmap>(width(),height());
    std::unique_ptr<QPainter> paintGreen(new QPainter(greenCircle.get()));
    QPen greenPen(Qt::green);
    QBrush greenBrush(Qt::green);
    paintGreen->setPen(greenPen);
    paintGreen->setBrush(greenBrush);
    paintGreen->fillRect(0,0,width(),height(),parent->palette().color(QPalette::Background));
    paintGreen->drawEllipse(0,height()/4.0,width()/2.2,height()/2.2);

    setGreen();
}

void WorkIndicator::setRed()
{
    if(CurrentState != Red)
    {
        setPixmap(*redCircle);
        CurrentState = Red;
    }
}

void WorkIndicator::setGreen()
{
    if(CurrentState != Green)
    {
        setPixmap(*greenCircle);
        CurrentState = Green;
    }
}

