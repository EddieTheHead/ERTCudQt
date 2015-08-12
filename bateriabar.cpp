//Sławek S.
#include "bateriabar.h"

BateriaBar::BateriaBar(QWidget *parent) :
    QGLWidget(parent)
{
    max=100;
    w=1;
    l=100;
    size=1;
    //test
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

    glBegin(GL_POLYGON);
    if(w>=0.5)
    {
    glColor3f(0, 1, 0);
    }
    else if(w<0.5&&w>=0.2)
    {
        glColor3f(1, 1, 0);
    }
    else if(w<0.2)
    {
        glColor3f(1, 0, 0);
    }
    glVertex2f(-size/8,w*size);
    glVertex2f(size/8,w*size);
    glVertex2f(size/8,0);
    glVertex2f(-size/8,0);
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

float BateriaBar::getSize() const
{
    return size;
}

void BateriaBar::setSize(float value)
{
    size = value;
}

void BateriaBar::setValue(int value)
{
    l= (float) value;
    w=l/max;
    updateGL();
}

void BateriaBar::setValue(float value)
{
    l= value;
    w=l/max;
    updateGL();
}
