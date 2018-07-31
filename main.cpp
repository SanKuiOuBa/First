#include <QApplication>
#include <QSplashScreen>
#include <QPixmap>
#include <QThread>
#include "mywidget.h"

int main(int argc,char* argv[])
{

    QApplication a(argc,argv);
    //QApplication::addLibraryPath("./plugins");
    QSplashScreen *splash=new QSplashScreen;
    splash->setPixmap(QPixmap(":/new/prefix1/loading.png"));
    splash->setStyleSheet("font-size:30px;font-weight:600");
    splash->showMessage(QObject::tr(
                                    "   程序正在启动中，请耐心等待..........\n"
                                    " <小贴士：能量超过30才可以使用技能哦^_^>"),Qt::AlignHCenter|Qt::AlignVCenter,QColor(84,132,242,255));
    splash->show();
    QThread::sleep(5);
    myWidget w;
    w.show();
    splash->finish(&w);
    delete splash;
    return a.exec();
}
