#include "addplayerdialog.h"
#include "ui_addplayerdialog.h"
#include <QDebug>
#include <QMessageBox>

addPlayerDialog::addPlayerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::addPlayerDialog)
{
    ui->setupUi(this);

    connect( ui->buttonBox, SIGNAL(accepted()),
             this, SLOT(slotAcceptPlayer()) );
    connect( ui->parseExcel, SIGNAL(clicked()),
             this, SLOT(slotParseExcel()) );
}

addPlayerDialog::~addPlayerDialog()
{
    delete ui;
}

void addPlayerDialog::slotParseExcel()
{
    QString excel_line = ui->excelLE->displayText();
    QStringList parList = excel_line.split("\t");

    int i=0;
    ui->nameLE->setText(parList.at(i));i++;
    ui->roleLE->setText(parList.at(i));i++;
    ui->ageLE->setText(parList.at(i));i++;
    ui->hanLE->setText(parList.at(i));i++;
    ui->outLE->setText(parList.at(i));i++;
    ui->refLE->setText(parList.at(i));i++;
    ui->agiLE->setText(parList.at(i));i++;
    ui->tacLE->setText(parList.at(i));i++;
    ui->heaLE->setText(parList.at(i));i++;
    ui->pasLE->setText(parList.at(i));i++;
    ui->posLE->setText(parList.at(i));i++;
    ui->finLE->setText(parList.at(i));i++;
    ui->tecLE->setText(parList.at(i));i++;
    ui->speLE->setText(parList.at(i));i++;
    ui->strLE->setText(parList.at(i));i++;
    if ( !parList.at(i).isEmpty() )
    {
        QStringList dateList = parList.at(i).split("/");
        if ( 3 == dateList.size() )
        {
            ui->tlDate->setDate(QDate(dateList.at(2).toInt(),dateList.at(1).toInt(),dateList.at(0).toInt()));
        }
    }
    i++;
    ui->capLE->setText(parList.at(i));i++;
    ui->talentLE->setText(parList.at(i));i++;
    if ( !parList.at(i).isEmpty() )
    {
        QStringList dateList = parList.at(i).split("/");
        if ( 3 == dateList.size() )
        {
            ui->capDate->setDate(QDate(dateList.at(2).toInt(),dateList.at(1).toInt(),dateList.at(0).toInt()));
        }
    }
    i++;
}

void addPlayerDialog::slotAcceptPlayer()
{
    if ( ui->idLE->text().isEmpty() )
    {
        QMessageBox::critical(this,"Missing parameters","Player ID must be specified");
        return;
    }
    if ( ui->nameLE->text().isEmpty() )
    {
        QMessageBox::critical(this,"Missing parameters","Player name must be specified");
        return;
    }
    if ( ui->countryLE->text().isEmpty() )
    {
        QMessageBox::critical(this,"Missing parameters","Player country must be specified");
        return;
    }
    QString db_line("insert into players (pm_id, name, role, age, "
                    "country, han, out, ref, agi, tac, hea, pas, pos, fin, "
                    "tec, spe, str, talent, no_sel, market_date, last_select) values ('");
    db_line += ui->idLE->text() + "', '";
    db_line += ui->nameLE->text() + "', '";
    db_line += ui->roleLE->text() + "', '";
    db_line += ui->ageLE->text() + "', '";
    db_line += ui->countryLE->text() + "', '";
    db_line += ui->hanLE->text() + "', '";
    db_line += ui->outLE->text() + "', '";
    db_line += ui->refLE->text() + "', '";
    db_line += ui->agiLE->text() + "', '";
    db_line += ui->tacLE->text() + "', '";
    db_line += ui->heaLE->text() + "', '";
    db_line += ui->pasLE->text() + "', '";
    db_line += ui->posLE->text() + "', '";
    db_line += ui->finLE->text() + "', '";
    db_line += ui->tecLE->text() + "', '";
    db_line += ui->speLE->text() + "', '";
    db_line += ui->strLE->text() + "', '";
    db_line += ui->talentLE->text() + "', '";
    db_line += ui->capLE->text() + "', '";
    db_line += ui->tlDate->text() + "', '";
    db_line += ui->capDate->text();
    db_line += "');";

    emit acceptPlayer( db_line );
    close();
}
