#include <QApplication>
#include <QDesktopWidget>
#include <QtCore/QCoreApplication>
#include "PmTL.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

//    w.setGeometry(QApplication::desktop()->screenGeometry());
    w.show();
    
    return a.exec();
}
