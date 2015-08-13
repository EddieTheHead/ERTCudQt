//Sławek Smyczyński
#ifndef BATERIABAR_H
#define BATERIABAR_H

#include <QGLWidget>

class BateriaBar : public QGLWidget
{
    Q_OBJECT
public:
    explicit BateriaBar(QWidget *parent = 0);

    float getSize() const;
    void setSize(float value);
    float getMax() const;

    float getMin() const;
    void setMin(float value);

    float getLowerGreenLimit() const;
    void setLowerGreenLimit(float value);

    float getUpperGreenLimit() const;
    void setUpperGreenLimit(float value);

    float getYeallowLimit() const;
    void setYeallowLimit(float value);

signals:

public slots:
    void setMaximum(int value);
    void setMaximum(float value);
    void setValue(int value);
    void setValue(float value);
protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
private:
    float size;
    float w;
    float l;
    float max;
    float min;
    float lowerGreenLimit;
    float upperGreenLimit;
    float yeallowLimit;
};

#endif // BATERIABAR_H
