#ifndef PMTL_H
#define PMTL_H

#include <QMainWindow>
#include <QtSql>
#include <QtNetwork>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();
    
private:
    Ui::MainWindow* ui;
    QSqlDatabase db;
    QSqlTableModel* countryModel;
    QSqlTableModel* playerModel;
    QNetworkAccessManager *manager;
    bool loginOk;
    bool loginExpired;
    QString userName;
    QString passWord;
    QList<long> checkList;
    QList<long> ageList;
    QTimer *checkTimer;
    QTimer *ageTimer;
    int searchType;

    void doSearch(int);
    void processPlayer(QByteArray& );
    void displayLoginDialog();
    void doGetDetails(long&);
    void processPlayerDetails(QByteArray& , QString&);

private slots:
    void loadCountry(QModelIndex );
    void doLogin();
    void addPlayer();
    void deletePlayer();
    void ageUpdate();
    void loginFinished(QNetworkReply* );
    void searchFinished(QNetworkReply* );
    void updateFinished(QNetworkReply *);
    void acceptLogin(QString&,QString&,bool&);
    void loginExpire();
    void acceptPlayer(QString&);
    void playerTimeout();
    void ageTimeout();
};

#define PMTL_AGE_CHECK_INTERVAL_MSEC 1000
#define PMTL_PLAYER_CHECK_INTERVAL_MSEC 1000

#define PM_NO_COUNTRIES 66
#define PM_COUNTRY_NAME_LEN 40
class PmTlCountry
{
public:
    int PmId;
    char countryName[PM_COUNTRY_NAME_LEN];
};

#endif // PMTL_H
