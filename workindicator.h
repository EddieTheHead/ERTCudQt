#ifndef WORKINDICATOR_H
#define WORKINDICATOR_H

#include <QLabel>
#include <memory>

class WorkIndicator : public QLabel
{
    Q_OBJECT
    enum state {Red,Green};
public:
    explicit WorkIndicator(QWidget *parent = 0);
public slots:
    void setRed();
    void setGreen();
    void setState(bool value);
private:
    std::shared_ptr<QPixmap> redCircle;
    std::shared_ptr<QPixmap> greenCircle;
    state CurrentState;


};

#endif // WORKINDICATOR_H
