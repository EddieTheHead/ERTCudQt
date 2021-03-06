#ifndef DIALOGLOG_H
#define DIALOGLOG_H

#include <QDialog>

namespace Ui {
class DialogLog;
}

class DialogLog : public QDialog
{
    Q_OBJECT

public:
    explicit DialogLog(QWidget *parent = 0);
    ~DialogLog();

public slots:
    void appendFrame(QByteArray frame);

private:
    Ui::DialogLog *ui;
};

#endif // DIALOGLOG_H
