#include "sonar.h"
#include <QPainter>
Sonar::Sonar(QWidget *parent) :
    QWidget(parent)
{
    Angle = 0;
    FaceColor = Qt::blue;
}

void Sonar::paintEvent(QPaintEvent *)
{
    static const QPoint dial[3] = { QPoint(7,8),QPoint(-7,8),QPoint(0,-100)};
    QColor dialColor(Qt::black);
    int side = qMin(width(), height());
    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);
    painter.translate(width() / 2, height() / 2);
    painter.scale(side / 200.0, side / 200.0);

    //tło
    painter.setPen(Qt::NoPen);
    painter.setBrush(FaceColor);
    painter.drawEllipse(QPoint(0,0),100,100);

    //wskazówka
    painter.setPen(Qt::NoPen);
    painter.setBrush(dialColor);
    painter.save();
    painter.rotate(Angle);
    painter.drawConvexPolygon(dial, 3);
    painter.restore();
}
QColor Sonar::getFaceColor() const
{
    return FaceColor;
}

void Sonar::setFaceColor(const QColor &value)
{
    FaceColor = value;
}



int Sonar::getAngle() const
{
    return Angle;
}

void Sonar::setAngle(int value)
{
    Angle = value;
    update();
}

void Sonar::setAngle(float value)
{
    Angle = value;
    update();
}


