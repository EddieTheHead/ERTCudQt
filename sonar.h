#ifndef SONAR_H
#define SONAR_H

#include <QLabel>

class Sonar : public QWidget
{
    Q_OBJECT
public:
    explicit Sonar(QWidget *parent = 0);

    int getAngle() const;

    QColor getFaceColor() const;
    void setFaceColor(const QColor &value);

signals:

public slots:
    void setAngle(int value);
    void setAngle(float value);
protected:
    void paintEvent(QPaintEvent *);


private:
    int Angle;
    QColor FaceColor;

};

#endif // SONAR_H
