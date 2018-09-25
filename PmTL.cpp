#include "PmTL.h"
#include "pmtllogindialog.h"
#include "addplayerdialog.h"
#include "ui_mainwindow.h"
#include <QTimer>
#include <QDesktopWidget>
#include <QResizeEvent>

PmTlCountry PmCountries[PM_NO_COUNTRIES]=
{
    {1,"Portugal"},
    {2,"Spain"},
    {3,"France"},
    {4,"England"},
    {5,"Netherlands"},
    {6,"Italy"},
    {7,"Belgium"},
    {8,"Russia"},//8
    {10,"Germany"},//10
    {11,"Brazil"},//11
    {12,"Argentina"},//12
    {13,"South Africa"},//13
    {14,"Australia"},//14
    {15,"USA"},//15
    {16,"Hungary"},//16
    {17,"Canada"},//17
    {18,"China"},//18
    {19,"Croatia"},//19
    {20,"Egypt"},//20
    {21,"Finland"},//21
    {22,"Greece"},//22
    {23,"Ireland"},//23
    {24,"Israel"},//24
    {25,"Japan"},//25
    {26,"Serbia"},//26
    {27,"Switzerland"},//27
    {28,"Mexico"},//28
    {30,"Scotland"},//30
    {31,"Peru"},//31
    {32,"Sweden"},//32
    {33,"Romania"},//33
    {66,"Slovenia"},//66
    {67,"Bolivia"},//67
    {68,"Poland"},//68
    {69,"Uruguay"},//69
    {75,"Chile"},//75
    {77,"Colombia"},//77
    {78,"Denmark"},//78
    {79,"Bosnia and Herzegovina"},//79
    {83,"Austria"},//83
    {85,"Norway"},//85
    {87,"Estonia"},//87
    {89,"Turkey"},//89
    {91,"Thailand"},//91
    {93,"Venezuela"},//93
    {95,"Indonesia"},//95
    {97,"Lithuania"},//97
    {99,"Bulgaria"},//99
    {101,"Latvia"},//101
    {103,"Malaysia"},//103
    {105,"Singapore"},//105
    {107,"Czech Republic"},//107
    {109,"Slovakia"},//109
    {111,"New Zealand"},//111
    {113,"Malta"},//113
    {115,"Faroe Islands"},//115
    {117,"Iceland"},//117
    {119,"Wales"},//119
    {121,"Ukraine"},//121
    {123,"Paraguay"},//123
    {125,"Panama"},//125
    {127,"Saudi Arabia"},//127
    {129,"Kuwait"},//129
    {131,"Algeria"},//131
    {133,"United Arab Emirates"},//133
    {135,"Cyprus"}//135
};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    loginOk = false;
    loginExpired = false;
    searchType = 0;

    db = QSqlDatabase::addDatabase("QSQLITE","tldb");
    db.setDatabaseName("pmtl.db");
    db.open();
    if ( db.isOpen() )
    {
        QStringList tables = db.tables(QSql::Tables);
        int i = 0;
        if (0 == tables.count())
        {
            QSqlQuery query(db);

            query.exec("create table countries (id integer primary key, name varchar(40))");
            query.exec("create table players (pm_id integer primary key, country varchar(40), name varchar(50), role varchar(6), "
                             "age integer, han varchar(6), out varchar(6), ref varchar(6), agi varchar(6), "
                             "tac varchar(6), hea varchar(6), pas varchar(6), pos varchar(6), fin varchar(6), "
                       "tec varchar(6), spe varchar(6), str varchar(6), talent varchar(20) default 'None', "
                       "market_date date default CURRENT_DATE, no_sel integer default 0, last_select date, obs varchar(20))");
            for ( i=0; i<PM_NO_COUNTRIES; i++)
            {
                query.exec(QString("insert into countries values('%1','%2')").arg(PmCountries[i].PmId).arg(PmCountries[i].countryName));
            }
        }
        countryModel = new QSqlTableModel(this,db);
        countryModel->setTable("countries");
        countryModel->select();
        ui->countriesList->setModel(countryModel);
        ui->countriesList->hideColumn(0);
        ui->countriesList->setSortingEnabled(true);
        ui->countriesList->sortByColumn(1,Qt::AscendingOrder);
        connect(ui->countriesList,SIGNAL(clicked(QModelIndex)),
                this, SLOT(loadCountry(QModelIndex)));

        playerModel = new QSqlTableModel(this,db);
        ui->playerList->setSortingEnabled(true);
        manager = new QNetworkAccessManager(this);

        connect(ui->updateButton,SIGNAL(clicked()),this,SLOT(doLogin()));
        connect(ui->addButton,SIGNAL(clicked()),this,SLOT(addPlayer()));
        connect(ui->deleteButton,SIGNAL(clicked()),this,SLOT(deletePlayer()));
        connect(ui->ageButton,SIGNAL(clicked()),this,SLOT(ageUpdate()));

        checkTimer = new QTimer(this);
        ageTimer = new QTimer(this);
        connect(checkTimer, SIGNAL(timeout()), this, SLOT(playerTimeout()));
        connect(ageTimer, SIGNAL(timeout()), this, SLOT(ageTimeout()));
    }
}

MainWindow::~MainWindow()
{
    delete ui;

    if ( db.isOpen() )
    {
        db.close();
    }
    QSqlDatabase::removeDatabase("tldb");
}

void MainWindow::resizeEvent(QResizeEvent* /* event */)
{
//    QRect screenGeometry = QApplication::desktop()->screenGeometry();
//    screenGeometry.setHeight(16777215);
//    ui->countriesList->setGeometry(screenGeometry);
//    ui->playerList->setGeometry(screenGeometry);
/*
    QSize size = event->size();
    QSize current_size = ui->countriesList->sizeHint();
    current_size.setHeight(size.height()-50);
    ui->countriesList->resize(current_size);
    current_size = ui->playerList->sizeHint();
    current_size.setHeight(size.height()-20);
    current_size.setWidth(size.width()-300);
    ui->playerList->resize(current_size);
*/
    QSize size = ui->scrollAreaWidgetContents->size();
    ui->countriesList->resize(size);
    size = ui->scrollAreaWidgetContents_2->size();
    ui->playerList->resize(size);
/*
     ui->countriesScrollArea->takeWidget();
    ui->countriesScrollArea->setWidget(ui->countriesList);
    ui->playersScrollArea->takeWidget();
    ui->playersScrollArea->setWidget(ui->playerList);
*/
}

void MainWindow::loadCountry(QModelIndex countryIndex)
{
    playerModel->setTable("players");
    playerModel->setFilter(QString("country = '%1'").arg(countryIndex.data().toString()));
    playerModel->select();
    ui->playerList->setModel(playerModel);
    ui->playerList->hideColumn(0);
    ui->playerList->hideColumn(1);
    ui->playerList->resizeColumnsToContents();
    ui->playerList->sortByColumn(18,Qt::AscendingOrder);
}

void MainWindow::displayLoginDialog()
{
//    pmTlLoginDialog* loginDialog = new pmTlLoginDialog(this);
    pmTlLoginDialog* loginDialog = new pmTlLoginDialog(this,MainWindow::userName,MainWindow::passWord);
    connect( loginDialog, SIGNAL(acceptLogin(QString&,QString&,bool&)),
             this, SLOT(acceptLogin(QString&,QString&,bool&)));
    loginDialog->exec();
}

void MainWindow::doLogin()
{
    if ( true == loginOk )
    {
        searchType = 0;
        doSearch(searchType);
        return;
    }
    else if ( true == loginExpired )
    {
        bool checked = false;
        acceptLogin(userName, passWord, checked);
        return;
    }
    displayLoginDialog();
}

void MainWindow::loginFinished(QNetworkReply *searchReply)
{
    QByteArray replyPage = searchReply->readAll();

    if ( ( QNetworkReply::NoError == searchReply->error() ) &&
         ( -1 == replyPage.indexOf("logout") ) )
    {
        QObject::disconnect(manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(loginFinished(QNetworkReply *)));
        loginOk = true;
        searchType = 0;
        doSearch(searchType);
        QTimer::singleShot(10*60*1000, this, SLOT(loginExpire()));  // expire after 10 min
    }
    else
    {
        displayLoginDialog();
    }
}

void MainWindow::doSearch(int type)
{
    QNetworkRequest request(QUrl("https://www.pmanager.org/procurar.asp?action=proc_jog"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    request.setRawHeader("User-Agent","Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/46.0.2490.80 Safari/537.36 OPR/33.0.1990.58");
    request.setRawHeader("Referer","https://www.pmanager.org/procurar.asp?action=jogador");
    request.setRawHeader("Host","www.pmanager.org");
    request.setRawHeader("Accept","text/html, application/xml;q=0.9, application/xhtml+xml, image/png, image/webp, image/jpeg, image/gif, image/x-xbitmap, */*;q=0.1");
    request.setRawHeader("Accept-Language","en-US,en;q=0.9");
    request.setRawHeader("Connection","Keep-Alive");

    QObject::disconnect(manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(updateFinished(QNetworkReply *)));
    QObject::connect(manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(searchFinished(QNetworkReply *)));

    QUrl params;
    params.addQueryItem("pos", "0");
    params.addQueryItem("lado", "-1");
    params.addQueryItem("idd_op", "<");
    if ( 0 == type || 1 == type )
    {
        params.addQueryItem("idd", "33");
    }
    else if ( 2 == type )
    {
        params.addQueryItem("idd", "25");
    }
    params.addQueryItem("nacional", "-1");
    if ( 0 == type )
    {
        params.addQueryItem("qual_op", ">");
        params.addQueryItem("qual", "7");// very good = 7, excellent = 8
    }
    else if ( 1 == type )
    {
        params.addQueryItem("qual_op", "=");
        params.addQueryItem("qual", "7");// very good = 7, excellent = 8
    }
    params.addQueryItem("prog_op", ">");
    if ( 2 == type )
    {
        params.addQueryItem("prog", "3");
    }
    else
    {
        params.addQueryItem("prog", "Any");
    }
    params.addQueryItem("pre_op", "<=");
    params.addQueryItem("pre", "Any");
    params.addQueryItem("ord_op", "<=");
    params.addQueryItem("ord", "Any");
    params.addQueryItem("con_op", "<");
    params.addQueryItem("con", "Any");
    params.addQueryItem("internacional", "Yes");
    params.addQueryItem("pais", "-1");
    if ( 0 == type )
    {
        params.addQueryItem("expe_op", ">=");
        params.addQueryItem("expe", "Any");
    }
    else if ( 1 == type )
    {
        params.addQueryItem("expe_op", ">=");
        params.addQueryItem("expe", "4");
    }
    else if ( 2 == type )
    {
        params.addQueryItem("expe_op", ">=");
        params.addQueryItem("expe", "3");
    }
    params.addQueryItem("temp_op", "<");
    params.addQueryItem("temp", "Any");
    params.addQueryItem("pro_op", ">");
    params.addQueryItem("pro", "Any");
    params.addQueryItem("forma_op", ">");
    params.addQueryItem("forma", "Any");
    params.addQueryItem("lesionado", "Any");
    params.addQueryItem("talento", "Any");
    params.addQueryItem("vel_op", ">");
    params.addQueryItem("vel", "Any");
    params.addQueryItem("cab_op", ">");
    params.addQueryItem("cab", "Any");
    params.addQueryItem("cul_op", ">");
    params.addQueryItem("cul", "Any");
    params.addQueryItem("forca_op", ">");
    params.addQueryItem("forca", "Any");
    params.addQueryItem("tack_op", ">");
    params.addQueryItem("tack", "Any");
    params.addQueryItem("passe_op", ">");
    params.addQueryItem("passe", "Any");
    params.addQueryItem("rem_op", ">");
    params.addQueryItem("rem", "Any");
    params.addQueryItem("tec_op", ">");
    params.addQueryItem("tec", "Any");
    params.addQueryItem("jmaos_op", ">");
    params.addQueryItem("jmaos", "Any");
    params.addQueryItem("saidas_op", ">");
    params.addQueryItem("saidas", "Any");
    params.addQueryItem("reflexos_op", ">");
    params.addQueryItem("reflexos", "Any");
    params.addQueryItem("agilidade_op", ">");
    params.addQueryItem("agilidade", "Any");
    params.addQueryItem("B1", "Search");
    params.addQueryItem("nome_filtro","");

    manager->post(request, params.encodedQuery());

}

void MainWindow::searchFinished(QNetworkReply *searchReply)
{
    QByteArray replyPage = searchReply->readAll();
    QString error = searchReply->errorString();

    QString url = searchReply->url().toString();
    int pos = replyPage.indexOf("<tr class=list1 onmouseover");
    if ( -1 != pos )
    {
        replyPage.remove(0,pos);
        pos = replyPage.indexOf("</table>");
        pos = replyPage.indexOf("</table>",pos+1);
        replyPage = replyPage.left(pos);    // cut it down to the tables we need;

        pos = replyPage.indexOf("<tr class=list");
        while ( -1 != pos )
        {
            replyPage.remove(0,pos);
            processPlayer(replyPage);
            pos = replyPage.indexOf("<tr class=list");
        }

        pos = replyPage.indexOf("Go to Page");
        pos = replyPage.indexOf("</b>",pos);
        pos = replyPage.indexOf("procurar.asp",pos);
        if ( pos > 0 )
        {
            replyPage.remove(0,pos);

            pos = replyPage.indexOf("[");
            pos-=2;
            if ( pos > 0 )
            {
                QNetworkRequest request(QUrl(QString("https://www.pmanager.org/%1").arg(replyPage.left(pos).data())));
                request.setRawHeader("User-Agent","Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/46.0.2490.80 Safari/537.36 OPR/33.0.1990.58");
                request.setRawHeader("Host","www.pmanager.org");
                request.setRawHeader("Accept","text/html, application/xml;q=0.9, application/xhtml+xml, image/png, image/webp, image/jpeg, image/gif, image/x-xbitmap, */*;q=0.1");
                request.setRawHeader("Accept-Language","en-US,en;q=0.9");
                request.setRawHeader("Connection","Keep-Alive");
                request.setRawHeader("Referer","https://www.pmanager.org/procurar.asp?action=proc_jog");

                manager->get(request);
            }
        }
        else
        {
            searchType++;
            if ( searchType <= 2 )
            {
                doSearch(searchType);
            }
            else
            {
                searchType = 0;
            }
        }
    }
    else if ( !replyPage.isEmpty() )
    {
        pos = url.indexOf("jog_id=");
        if ( -1 != pos )
        {
            url.remove(0,pos+7);
            processPlayerDetails(replyPage,url);
        }
    }
}

void MainWindow::updateFinished(QNetworkReply *searchReply)
{
    QByteArray replyPage = searchReply->readAll();

    int pos = replyPage.indexOf("&nbsp;Years");
    replyPage.remove(0,pos-2);
    QByteArray playerAge = replyPage.left(2);
    replyPage.remove(0,replyPage.indexOf("jog_id")+7);
    int end = replyPage.indexOf(">");
    QByteArray playerId = replyPage.left(end);

    QByteArray db_line("update players set ");
    db_line += "age='" + playerAge + "'";
    db_line += " where pm_id='"+playerId+";";

//    qDebug() << db_line << endl;
    QSqlQuery query(db);
    query.exec(QString::fromUtf8(db_line));
}

void MainWindow::acceptLogin(QString& userName, QString& passWord, bool& checkEnabled)
{
    QNetworkRequest request(QUrl("https://www.pmanager.org/default.asp?action=login"));
    request.setRawHeader("User-Agent","Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/46.0.2490.80 Safari/537.36 OPR/33.0.1990.58");
    request.setRawHeader("Host","www.pmanager.org");
    request.setRawHeader("Accept","text/html, application/xml;q=0.9, application/xhtml+xml, image/png, image/webp, image/jpeg, image/gif, image/x-xbitmap, */*;q=0.1");
    request.setRawHeader("Accept-Language","en-US,en;q=0.9");
//    request.setRawHeader("Accept-Encoding","gzip, deflate");
    request.setRawHeader("Connection","Keep-Alive");
    request.setRawHeader("Referer","https://www.pmanager.org/default.asp");
#if 0
    Connection: Keep-Alive
    Content-Length: 31
#endif

    QObject::connect(manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(loginFinished(QNetworkReply *)));

    manager->setCookieJar(new QNetworkCookieJar(this));

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QUrl params;
    params.addQueryItem("utilizador", userName);
    params.addQueryItem("password", passWord);
    if ( true == checkEnabled )
    {
        MainWindow::userName = userName;
        MainWindow::passWord = passWord;
    }

    manager->post(request, params.encodedQuery());
}

static char* findCountry(int id)
{
    for ( int count = 0; count<PM_NO_COUNTRIES; count++)
    {
        if ( id == PmCountries[count].PmId)
        {
            return PmCountries[count].countryName;
        }
    }

    return NULL;
}

void MainWindow::processPlayer(QByteArray &replyPage)
{
    replyPage.remove(0,replyPage.indexOf("<b>") + 3);   // remove all including <b>
    int end = replyPage.indexOf("</b>");    // get end of <b> tag
    QByteArray  playerPosition = replyPage.left(end);
    replyPage.remove(0,end+4);  // remove </b>

    end = replyPage.indexOf("</font>");
    playerPosition += replyPage.left(end);

    replyPage.remove(0,replyPage.indexOf("jog_id")+7);
    end = replyPage.indexOf(">");
    QByteArray playerId = replyPage.left(end);
    replyPage.remove(0,end+1);
    end = replyPage.indexOf("</a>");
    QByteArray playerName = replyPage.left(end);

    replyPage.remove(0,replyPage.indexOf("</font>")-2);
    QByteArray playerAge = replyPage.left(2);

    QByteArray db_line("replace into players (last_select, obs, pm_id, name, role, age, "
                    "country, han, out, ref, agi, tac, hea, pas, pos, fin, "
                    "tec, spe, str) values ((select last_select from players where pm_id='");
    db_line += playerId + "'), ";
    db_line += "(select obs from players where pm_id='";
    db_line += playerId + "'), '";
    db_line += playerId + "', '";
    db_line += playerName + "', '";
    db_line += playerPosition + "', '";
    db_line += playerAge;

    int largeSkills = 0;

    for ( int count = 0; count<13; count++ )
    {
        replyPage.remove(0,replyPage.indexOf("</font>")+7);
        replyPage.remove(0,replyPage.indexOf("<font class=team_players>")+25);
        end = replyPage.indexOf("</font>");
        QByteArray skill = replyPage.left(end);
        end = skill.indexOf("small/");
        if ( -1 != end )
        {
            skill.remove(0,end+6);
            end = skill.indexOf(".");
            skill = skill.left(end);
            skill = findCountry(skill.toInt());
        }
        end = skill.indexOf("<b>");
        if ( -1 != end )
        {
            skill.remove(0,end+3);
            end = skill.indexOf("</b>");
            skill = skill.left(end);
        }
        end = skill.indexOf("<u>");
        if ( -1 != end )
        {
            skill.remove(0,end+3);
            end = skill.indexOf("</u>");
            skill = skill.left(end);
        }
        db_line += "', '" + skill;
        if ( skill.toInt() > 15 )
        {
            largeSkills++;
        }
    }
    db_line += "');";

//    qDebug() << db_line << endl;
    QSqlQuery query(db);
    query.exec(QString::fromUtf8(db_line));
//    doGetDetails(playerId.toLong());
    checkList.append(playerId.toLong());
    if ( !checkTimer->isActive() )
    {
        checkTimer->start(PMTL_PLAYER_CHECK_INTERVAL_MSEC);
    }
}

void MainWindow::doGetDetails(long& playerId)
{
    QNetworkRequest request(QUrl(QString("https://www.pmanager.org/ver_jogador.asp?jog_id=%1").arg(playerId)));
    request.setRawHeader("User-Agent","Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/46.0.2490.80 Safari/537.36 OPR/33.0.1990.58");
    request.setRawHeader("Host","www.pmanager.org");
    request.setRawHeader("Accept","text/html, application/xml;q=0.9, application/xhtml+xml, image/png, image/webp, image/jpeg, image/gif, image/x-xbitmap, */*;q=0.1");
    request.setRawHeader("Accept-Language","en-US,en;q=0.9");
    request.setRawHeader("Connection","Keep-Alive");
    request.setRawHeader("Referer","https://www.pmanager.org/procurar.asp?action=proc_jog");

    manager->get(request);
}

void MainWindow::processPlayerDetails(QByteArray& replyPage,QString& playerId)
{
    QByteArray caps("0");
    int start=0;
    int pos = replyPage.indexOf("Caps /");
    if ( -1 != pos )
    {
        start = replyPage.indexOf("ers>",pos-10);
        caps = replyPage.mid(start+4,pos-1-start-4).simplified();
    }
    QByteArray talent("None");

    pos = replyPage.indexOf("<font class=comentarios>");
    if ( -1 != pos )
    {
        start = replyPage.indexOf("<",pos+1);
        talent = replyPage.mid(pos+24,start-pos-24).simplified();
    }

    QByteArray db_line("update players set ");
    db_line += "talent='" + talent + "', no_sel='" + caps + "'";

    pos = replyPage.indexOf("<font size=+1>");
    if ( -1 != pos )
    {
        start = replyPage.indexOf("<",pos+1);
        talent = replyPage.mid(pos+14,start-pos-14).simplified();
        db_line += ", name='" + talent + "'";
    }

    pos = replyPage.indexOf("<font color=blue>");
    while ( -1 != pos )
    {
        start = replyPage.lastIndexOf("align='left' width='35%'><b>",pos);
        replyPage.remove(0,start+28);

        db_line += "," + replyPage.left(3) + "='20m'";

        pos = replyPage.indexOf("<font color=blue>");
        pos = replyPage.indexOf("<font color=blue>",pos+1);
    }
    db_line += " where pm_id='"+playerId+"';";

//    qDebug() << db_line << endl;
    QSqlQuery query(db);
    query.exec(QString::fromUtf8(db_line));
}

void MainWindow::loginExpire()
{
    loginOk = false;
    loginExpired = true;
}

void MainWindow::addPlayer()
{
    addPlayerDialog* addDialog = new addPlayerDialog(this);

    connect( addDialog, SIGNAL(acceptPlayer(QString&)),
             this, SLOT(acceptPlayer(QString&)));
    addDialog->exec();
}

void MainWindow::acceptPlayer(QString &newPlayer)
{
    QSqlQuery query(db);
//    qDebug() << newPlayer << endl;
    query.exec(newPlayer);
}

void MainWindow::deletePlayer()
{
    QModelIndexList selectedIdx = ui->playerList->selectionModel()->selectedIndexes();
    QModelIndex playerIdx;
    QSqlQuery query(db);

    for ( int cnt=0; cnt<selectedIdx.size(); cnt++)
    {
        playerIdx = playerModel->index(selectedIdx.at(cnt).row(),0);
        query.exec(QString("delete from players where pm_id='%1'").arg(playerIdx.data().toString()));
    }
}

void MainWindow::ageUpdate()
{
    QSqlQuery query(db);

    query.exec("select pm_id from players");
    QObject::disconnect(manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(searchFinished(QNetworkReply *)));
    QObject::connect(manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(updateFinished(QNetworkReply *)));
    while ( true == query.next() )
    {
        long playerId = query.record().value(0).toByteArray().toLong();
//        doGetDetails(playerId);
        ageList.append(playerId);
        if ( !ageTimer->isActive() )
        {
            ageTimer->start(PMTL_AGE_CHECK_INTERVAL_MSEC);
        }
    }
}

void MainWindow::playerTimeout()
{
    if ( checkList.isEmpty() )
    {
        checkTimer->stop();
        return;
    }

    long playerId = checkList.first();
    doGetDetails(playerId);
    checkList.removeFirst();
}

void MainWindow::ageTimeout()
{
    if ( ageList.isEmpty() )
    {
        ageTimer->stop();
        return;
    }

    long playerId = ageList.first();
    doGetDetails(playerId);
    ageList.removeFirst();
}
