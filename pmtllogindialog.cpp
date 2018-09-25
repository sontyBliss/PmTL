#include "pmtllogindialog.h"
#include "ui_pmtllogindialog.h"

pmTlLoginDialog::pmTlLoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::pmTlLoginDialog)
{
    ui->setupUi(this);

    connect( ui->buttonBox, SIGNAL(rejected()),
             this, SLOT(close()));
    connect( ui->buttonBox, SIGNAL(accepted()),
             this, SLOT(slotAcceptLogin()) );
}

pmTlLoginDialog::pmTlLoginDialog(QWidget *parent,QString& user, QString& pass) :
    QDialog(parent),
    ui(new Ui::pmTlLoginDialog)
{
    ui->setupUi(this);

    ui->userLineEdit->setText(user);
    ui->passLineEdit->setText(pass);
    ui->saveCheckBox->setCheckState(Qt::Checked);
    connect( ui->buttonBox, SIGNAL(rejected()),
             this, SLOT(close()));
    connect( ui->buttonBox, SIGNAL(accepted()),
             this, SLOT(slotAcceptLogin()) );
}

pmTlLoginDialog::~pmTlLoginDialog()
{
    delete ui;
}

void pmTlLoginDialog::slotAcceptLogin()
{
    QString username = ui->userLineEdit->text();
    QString password = ui->passLineEdit->text();
    bool    checkEnabled = (ui->saveCheckBox->checkState()==Qt::Checked)?true:false;

    emit acceptLogin( username, password, checkEnabled );
    close();
}
