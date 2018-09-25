#ifndef PMTLLOGINDIALOG_H
#define PMTLLOGINDIALOG_H

#include <QDialog>

namespace Ui {
class pmTlLoginDialog;
}

class pmTlLoginDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit pmTlLoginDialog(QWidget *parent = 0);
    pmTlLoginDialog(QWidget *parent,QString& user, QString& pass);
    ~pmTlLoginDialog();

signals:
    void acceptLogin( QString& username, QString& password, bool& checkEnabled );
    
private:
    Ui::pmTlLoginDialog *ui;

private slots:
    void slotAcceptLogin();
};

#endif // PMTLLOGINDIALOG_H
