#include "dialoglog.h"
#include "ui_dialoglog.h"

DialogLog::DialogLog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogLog)
{
    ui->setupUi(this);
    setWindowTitle("Log");

    ui->plainTextEdit->setReadOnly(true);
}

DialogLog::~DialogLog()
{
    delete ui;
}

void DialogLog::appendFrame(QByteArray frame)
{
    QString FrameStr(frame.toHex());
    for(int i = 1; i< FrameStr.size(); ++i)
    {
        if(i % 3)
        {
            i++;
            FrameStr.insert(i," ");
        }
    }
    ui->plainTextEdit->insertPlainText(FrameStr + QString("\n"));
}

