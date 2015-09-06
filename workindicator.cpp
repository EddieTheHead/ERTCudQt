#include "workindicator.h"
#include <QPainter>

WorkIndicator::WorkIndicator(QWidget *parent) :
    QLabel(parent)
{
    setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    resize(100,100);

    redCircle = std::make_shared<QPixmap>(width(),height());
    std::unique_ptr<QPainter> paintRed(new QPainter(redCircle.get()));
    paintRed->setRenderHint(QPainter::Antialiasing);
    QBrush redBrush(Qt::red);
    paintRed->setPen(Qt::NoPen);
    paintRed->setBrush(redBrush);
    paintRed->fillRect(0,0,width(),height(),parent->palette().color(QPalette::Background));
    paintRed->drawEllipse(0,height()/4,width()/2,height()/2);

    greenCircle = std::make_shared<QPixmap>(width(),height());
    std::unique_ptr<QPainter> paintGreen(new QPainter(greenCircle.get()));
    paintGreen->setRenderHint(QPainter::Antialiasing);
    QBrush greenBrush(Qt::green);
    paintGreen->setPen(Qt::NoPen);
    paintGreen->setBrush(greenBrush);
    paintGreen->fillRect(0,0,width(),height(),parent->palette().color(QPalette::Background));
    paintGreen->drawEllipse(0,height()/4,width()/2 ,height()/2);
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

void WorkIndicator::setState(bool value)
{
    if(value) setGreen();
    else setRed();
}

