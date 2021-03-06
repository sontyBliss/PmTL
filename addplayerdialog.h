#ifndef ADDPLAYERDIALOG_H
#define ADDPLAYERDIALOG_H

#include <QDialog>

namespace Ui {
class addPlayerDialog;
}

class addPlayerDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit addPlayerDialog(QWidget *parent = 0);
    ~addPlayerDialog();

signals:
    void acceptPlayer( QString& db_line );

private:
    Ui::addPlayerDialog *ui;

private slots:
    void slotAcceptPlayer();
    void slotParseExcel();
};

#endif // ADDPLAYERDIALOG_H
