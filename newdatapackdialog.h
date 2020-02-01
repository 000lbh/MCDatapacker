#ifndef NEWDATAPACKDIALOG_H
#define NEWDATAPACKDIALOG_H

#include <QDialog>

namespace Ui {
class NewDatapackDialog;
}

class NewDatapackDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewDatapackDialog(QWidget *parent = nullptr);
    ~NewDatapackDialog();

    void browse();
    QString getName();
    QString getDesc();
    int getFormat();
    QString getDirPath();

public slots:
    void checkOK();

private:
    Ui::NewDatapackDialog *ui;
};

#endif // NEWDATAPACKDIALOG_H
