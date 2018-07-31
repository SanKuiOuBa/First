#include <QTime>
#include <QBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QApplication>
#include <QStyleFactory>
#include <QGraphicsWidget>
#include <QGraphicsLinearLayout>
#include <QGraphicsProxyWidget>
#include <QLineEdit>
#include <QPalette>
#include "centersqlmodel.h"
#include <QSqlTableModel>
#include <QSqlRelationalTableModel>
#include <QTableView>
#include <QMessageBox>
#include <QHeaderView>
#include <QTimer>
#include <QtMultimedia/QMediaPlayer>
#include <QMediaPlaylist>
#include <QMessageBox>
#include <QBitmap>
#include <QMovie>
#include <QFont>
#include <QSlider>
#include <QString>
#include "mywidget.h"
#include "myview.h"
#include "mylabel.h"
#include "tipwidget.h"
#include "mypushbutton.h"
#include <QDebug>
#include "global.h"
myWidget::myWidget(QWidget *parent) :
    QWidget(parent)
{
    cursor=new QCursor(QPixmap(":/new/prefix1/test.png"));
    setCursor(*cursor);
    player=new QMediaPlayer(this);
    player2=new QMediaPlayer(this);
    mousePress=false;
    isActive=true;
    isGame=true;
    isLogin=true;
    isRestartGame=false;

    qApp->setStyle(QStyleFactory::create("fusion"));
    this->setStyleSheet("background-color:rgb(50,50,50);color:white;");
    setWindowFlags(Qt::FramelessWindowHint);
    installEventFilter(this);
    playGame();
    setLook();
    view->setFocus();                                                                 //让view获得焦点、不使用或许会出BUG
}

void myWidget::playGame()
{
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    view=new myView;
    view->setMaximumSize(811,519);
    view->setMinimumSize(811,519);
    view->scene()->setBackgroundBrush(QPixmap(":/new/prefix1/teris.png"));
    view->setParentWidget(this);

    connect(this,SIGNAL(pause()),view,SLOT(pauseGame()));
    connect(this,SIGNAL(repause()),view,SLOT(rePauseGame()));
    connect(view,SIGNAL(resendPressZ()),this,SLOT(receiveResendPressZ()));
    connect(view,SIGNAL(gameIsOver()),this,SLOT(setIsGame()));
    connect(view,SIGNAL(closeAliWidget()),this,SLOT(close()));                        //响应view的messBox传来的关闭窗口信息
    connect(view,SIGNAL(gameIsRestart()),this,SLOT(setGameMusic()));
}

void myWidget::setLook()
{
    QHBoxLayout *hlayout=new QHBoxLayout;
    smallBtn=new myPushButton(tr("_"));
    closeBtn=new myPushButton(tr("X"));
    connect(smallBtn,SIGNAL(clicked()),this,SLOT(showMinimized()));
    connect(closeBtn,SIGNAL(clicked()),this,SLOT(close()));

    smallBtn->setMaximumSize(30,30);
    smallBtn->setMinimumSize(30,30);
    closeBtn->setMaximumSize(30,30);
    closeBtn->setMinimumSize(30,30);


    pixmapLabel=new QLabel("*");
    titleLabel=new QLabel(tr("俄罗斯方块"));
    scoreShow=new QLabel(tr(""));
    scoreShow->setMaximumWidth(600);
    scoreShow->setMaximumHeight(38);

    pixmapLabel->setPixmap(QPixmap(":/new/prefix1/myico.ico"));
    pixmapLabel->setMaximumSize(30,30);
    pixmapLabel->setMinimumSize(30,30);


    hlayout->setSpacing(2);
    hlayout->addWidget(pixmapLabel);
    hlayout->addWidget(titleLabel);
    hlayout->addWidget(scoreShow,Qt::AlignLeft);
    hlayout->addSpacing(20);
    hlayout->addWidget(smallBtn);
    hlayout->addWidget(closeBtn);
    hlayout->setContentsMargins(1,4,4,4);

    QVBoxLayout *layout=new QVBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);
    layout->addLayout(hlayout);
    layout->addWidget(view);
    layout->setSpacing(0);
    setLayout(layout);

    w=new tipWidget(this,view);
    connect(w,SIGNAL(jPressEvent()),this,SLOT(receiveJEvent()));

    test=new QLabel(this);
    //test->resize(view->size());
    test->setGeometry(view->x(),view->y()+37,830,519);
    test->setPixmap(QPixmap(":/new/prefix1/images/pause.png"));
    test->setStyleSheet("background-color:rgba(255,255,255,200)");
    test->hide();
    //test->setWindowFlags(Qt::FramelessWindowHint);
    //test->setAttribute(Qt::WA_TranslucentBackground);
    w->hide();
    setLoginWidget();
}

void myWidget::setLoginWidget()
{
    beginBtn=new myPushButton(tr("开始游戏"));
    setBtn=new myPushButton(tr("设置"));
    beginBtn->setStyleSheet("font-size:18px;color:white;background-color:rgba(137,157,192,100);font-weight:bold");
    beginBtn->setCursor(*cursor);
    setBtn->setStyleSheet("font-size:18px;color:white;background-color:rgba(137,157,192,100);font-weight:bold");
    setBtn->setCursor(*cursor);
    connect(beginBtn,SIGNAL(clicked()),this,SLOT(loginGame()));
    connect(setBtn,SIGNAL(clicked()),this,SLOT(settings()));
    button=view->scene()->addWidget(beginBtn);
    button_1=view->scene()->addWidget(setBtn);
    QGraphicsLinearLayout *loginSubLayout = new QGraphicsLinearLayout(Qt::Horizontal);
    loginSubLayout->addItem(button);
    loginSubLayout->addItem(button_1);
    loginSubLayout->setContentsMargins(0,0,0,0);
    QGraphicsLinearLayout *loginLayout = new QGraphicsLinearLayout(Qt::Vertical);
    loginLayout->addItem(loginSubLayout);
    loginLayout->setContentsMargins(10,10,10,10);
    grapLoginWidget=new QGraphicsWidget;
    grapLoginWidget->setCursor(*cursor);
    QPalette palette=grapLoginWidget->palette();
    palette.setBrush(QPalette::Background,QBrush(QColor(255,255,255,0)));
    grapLoginWidget->setPalette(palette);
    grapLoginWidget->setAutoFillBackground(true);
    view->scene()->addItem(grapLoginWidget);
    grapLoginWidget->setGeometry(260,200,300,200);
    grapLoginWidget->setLayout(loginLayout);

    //播放开头音乐->（开始）
    playList=new QMediaPlaylist(this);
    player->setPlaylist(playList);
    playList->addMedia(QUrl("qrc:/new/prefix1/open1.wav"));
    playList->setCurrentIndex(0);
    playList->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
    player->setVolume(global::back_volume);
    player->play();
    //播放开头音乐<-（结束）

}

void myWidget::sleep(unsigned int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

void myWidget::helloWidget()
{


    widget=new QWidget(view);
    widget->setObjectName("SCOREWIDGET");
    widget->setStyleSheet("QWidget#SCOREWIDGET{background-color:rgba(255,255,255,0)}");
    widget->resize(100,50);
    widget->move(350,200);
    start=new myPushButton(tr("开始游戏"));
    start->setMinimumHeight(30);
    start->setObjectName("STARTBUTTON");
    start->setStyleSheet("myPushButton#STARTBUTTON{background-color:rgba(138,43,226,50)}");
    connect(start,SIGNAL(clicked()),this,SLOT(readyText()));                  //响应开始按键、调用readyText
    QVBoxLayout *scoreLayout=new QVBoxLayout;
    scoreLayout->addWidget(start);
    widget->setLayout(scoreLayout);
    widget->show();
}

void myWidget::gifWidget()
{
    gifLabel=new myLabel(this);
    QMovie *gif=new QMovie(":/new/prefix1/gif.gif");
    gifLabel->setMovie(gif);
    gif->start();
    gifLabel->setStyleSheet("background-color:rgba(255,255,255,0);");
    gifLabel->setGeometry(-320,70,800,400);
    gifLabel->show();
    if(isGame==false)gifLabel->hide();
}

void myWidget::receiveResendPressZ()
{
    //暂停Z键的实现、如果是活动的、则暂停、否则重新开始游戏
    if(isActive)
    {

        test->show();
        isActive=false;
        w->isGame=false;
       // QApplication::sendEvent(view,keyEvent);
        player->pause();
        emit pause();
        return;
    }
    else
    {

        test->hide();
        isActive=true;
        view->setFocus();
        w->isGame=true;
        player->play();
        //QApplication::sendEvent(view,keyEvent);
        emit repause();
        return;
    }
}



void myWidget::setIsGame()
{
    isGame=false;                                                      //如果游戏结束、不再响应任何键盘事件

    //播放结束音乐->（开始）
    player->setPlaylist(playList);
    playList->clear();
    playList->addMedia(QUrl("qrc:/new/prefix1/end.mp3"));
    playList->setCurrentIndex(0);
    playList->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
    player->setVolume(global::back_volume);
    player->play();
    //播放结束音乐<-（结束）
    view->endWidget();
}

void myWidget::receiveJEvent()
{
    if(view->isAllowJ())
    {
        view->clearAll();
    }
}

void myWidget::loginGame()
{
    grapLoginWidget->hide();                                                                         //登陆窗口隐藏
    view->scene()->setBackgroundBrush(QPixmap(":/new/prefix1/playing.jpg"));             //背景图片更换
    w->show();                                                                                       //按键提示窗口显示
    helloWidget();                                                                                   //欢迎窗口显示
}

void myWidget::settings()
{
    SET=new QWidget;
    slide1=new QSlider(Qt::Horizontal,SET);
    slide2=new QSlider(Qt::Horizontal,SET);
    slide1->setMinimum(0);
    slide1->setMaximum(100);
    slide1->setValue(global::back_volume);
    slide2->setMinimum(0);
    slide2->setMaximum(100);
    slide2->setValue(global::effect_volume);
    myPushButton *enter=new myPushButton(tr("确定"));
    connect(slide1,SIGNAL(valueChanged(int)),this,SLOT(setvolume1()));
    connect(slide2,SIGNAL(valueChanged(int)),this,SLOT(setvolume2()));
    QHBoxLayout *slidebox1=new QHBoxLayout;
    QHBoxLayout *slidebox2=new QHBoxLayout;
    QLabel *volume1=new QLabel(tr("音乐"));
    QLabel *volume2=new QLabel(tr("音效"));
    slidebox1->addWidget(volume1);
    slidebox1->addWidget(slide1);
    slidebox2->addWidget(volume2);
    slidebox2->addWidget(slide2);
    QVBoxLayout *slidebox=new QVBoxLayout;
    slidebox->addLayout(slidebox1);
    slidebox->addLayout(slidebox2);
    slidebox->addWidget(enter);
    connect(enter,SIGNAL(clicked()),this,SLOT(change()));
    SET->setLayout(slidebox);
    SET->move(700,350);
    SET->show();
}
void myWidget::setvolume1()
{
    global::back_volume=slide1->value();
}

void myWidget ::setvolume2()
{
    global::effect_volume=slide2->value();
}
void myWidget::change()
{
    SET->close();
    qDebug()<<"back_volume="<<global::back_volume<<endl
           <<"effect_volume="<<global::effect_volume;
}
void myWidget::readyText()
{
    //在欢迎界面点击开始游戏后调用setGameMusic、setGameMusic调用这个函数、显示ready并播放音乐、1秒后调用goText函数
    view->setFocus();
    player->setMedia(QUrl("qrc:/new/prefix1/readygo.mp3"));
    player->setVolume(global::effect_volume);
    player->play();
    widget->hide();
    readyLabel=new QLabel(view);
    readyLabel->setStyleSheet("background-color:rgba(255,255,255,0);");
    readyLabel->move(343,90);
    readyLabel->show();
    readyLabel->resize(200,300);
    readyLabel->setPixmap(QPixmap(":/new/prefix1/images/ready.png"));
    QTimer::singleShot(1000,this,SLOT(goText()));
}

void myWidget::goText()
{
    //显示go、0.45秒后调用goGame开始游戏
    readyLabel->setPixmap(QPixmap(":/new/prefix1/images/go.png"));
    QTimer::singleShot(450,this,SLOT(goGame()));
}

void myWidget::goGame()
{
    readyLabel->hide();
    w->isGame=true;
    isLogin=false;
    gifWidget();

    //播放游戏中音乐并且开始游戏
    player->stop();
    player->setPlaylist(playList);
    playList->clear();
    playList->addMedia(QUrl("qrc:/new/prefix1/ing.wav"));
    playList->setCurrentIndex(0);
    playList->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
    player->setVolume(global::back_volume);
    player->play();


    view->clearNextBox();
    view->setFocus();
    view->startGame();
}

void myWidget::setGameMusic()
{
    view->setFocus();
    isRestartGame=true;
    player->setMedia(QUrl("qrc:/new/prefix1/readygo.mp3"));
    player->setVolume(global::effect_volume);
    player->play();
    readyLabel->setPixmap(QPixmap(":/new/prefix1/images/ready.png"));
    readyLabel->show();
    view->newScore();
    w->resetQss();
    QTimer::singleShot(1000,this,SLOT(goText()));

}



void myWidget::mousePressEvent(QMouseEvent *myEvent)
{
    if(myEvent->button()==Qt::LeftButton)
    {
        mousePress=true;
        offset=myEvent->globalPos()-pos();
    }
}

void myWidget::mouseMoveEvent(QMouseEvent *myEvent)
{
    if((myEvent->buttons()&Qt::LeftButton)&&mousePress)
    {
        QPoint point=myEvent->globalPos()-offset;
        move(point);
    }
}

void myWidget::mouseReleaseEvent(QMouseEvent *myEvent)
{
    mousePress=false;
}


bool myWidget::eventFilter(QObject *object, QEvent *event)
{

    //判断窗口是否正在游戏、如果窗口失去焦点变成非活动、自动暂停游戏
    if(object==this&&isGame&&!isLogin)
    {

        if(event->type()==QEvent::WindowDeactivate)
        {

            isActive=false;
            test->show();
            w->isGame=false;
            player->pause();
            emit pause();
            return true;
        }

    }
    return false;
}
