//Sławek S.
#include "bateriabar.h"

BateriaBar::BateriaBar(QWidget *parent) :
    QGLWidget(parent)
{
    max = 30;
    min = 18;
    w=1;
    l=100;
    size=1;
    lowerGreenLimit = 21.6;
    upperGreenLimit = 28.2;
    yeallowLimit = 23;
}

void BateriaBar::setMaximum(int value)
{
    max= (float) value;
    w=l/max;
    updateGL();
}

void BateriaBar::setMaximum(float value)
{
    max= value;
    w=l/max;
    updateGL();
}

void BateriaBar::initializeGL()
{
    glShadeModel(GL_SMOOTH);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glOrtho(-size/8,size/8,0,size,-1,1);
}

void BateriaBar::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glPushMatrix();
    glLoadIdentity();

    if(l < lowerGreenLimit || l>upperGreenLimit) glColor3f(1,0,0);
    else glColor3f(0,1,0);
    if(l> lowerGreenLimit && l< yeallowLimit) glColor3f(1,1,0);

    glBegin(GL_POLYGON);
     glVertex2f(-size/8,w*size);
     glVertex2f(size/8,w*size);
     glVertex2f(size/8,0);
     glVertex2f(-size/8,0);
    glEnd();
    //linie
    glColor3f(1,0,0);
    glBegin(GL_POLYGON);
     glVertex2f(-size/8,(upperGreenLimit-min)/(max-min)*size);
     glVertex2f(size/8,(upperGreenLimit-min)/(max-min)*size);
     glVertex2f(size/8,(upperGreenLimit-min)/(max-min)*size+0.007*size);
     glVertex2f(-size/8,(upperGreenLimit-min)/(max-min)*size+0.007*size);
    glEnd();

    glColor3f(1,0,0);
    glBegin(GL_POLYGON);
     glVertex2f(-size/8,(lowerGreenLimit-min)/(max-min)*size);
     glVertex2f(size/8,(lowerGreenLimit-min)/(max-min)*size);
     glVertex2f(size/8,(lowerGreenLimit-min)/(max-min)*size+0.007*size);
     glVertex2f(-size/8,(lowerGreenLimit-min)/(max-min)*size+0.007*size);
    glEnd();

    glPopMatrix();
}

void BateriaBar::resizeGL(int width, int height)
{
    glViewport(0,0,width,height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-size/8,size/8,0,size,-1,1);
    glMatrixMode(GL_MODELVIEW);
}
float BateriaBar::getYeallowLimit() const
{
    return yeallowLimit;
}

void BateriaBar::setYeallowLimit(float value)
{
    yeallowLimit = value;
}

float BateriaBar::getUpperGreenLimit() const
{
    return upperGreenLimit;
}

void BateriaBar::setUpperGreenLimit(float value)
{
    upperGreenLimit = value;
}

float BateriaBar::getLowerGreenLimit() const
{
    return lowerGreenLimit;
}

void BateriaBar::setLowerGreenLimit(float value)
{
    lowerGreenLimit = value;
}

float BateriaBar::getMin() const
{
    return min;
}

void BateriaBar::setMin(float value)
{
    min = value;
}


float BateriaBar::getSize() const
{
    return size;
}

void BateriaBar::setSize(float value)
{
    size = value;
}

float BateriaBar::getMax() const
{
    return max;
}

void BateriaBar::setValue(int value)
{
    l= (float) value;
    w=(l-min)/(max-min);
    updateGL();
}

void BateriaBar::setValue(float value)
{
    l= value;
    w=(l-min)/(max-min);
    updateGL();
}
