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

signals:

public slots:
    void setMaximum(int value);
    void setValue(int value);
    void setMaximum(float value);
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

};

#endif // BATERIABAR_H
