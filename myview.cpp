#include <QTimer>
#include <QMessageBox>
#include <QLayout>
#include <QLabel>
#include <QString>
#include <QPropertyAnimation>
#include <QGraphicsBlurEffect>
#include <QtMultimedia/QMediaPlayer>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QSqlRelationalTableModel>
#include <QTableView>
#include <QMessageBox>
#include <QHeaderView>
#include <QFont>
#include <QTime>
#include <QDebug>
#include "centersqlmodel.h"
#include "mypushbutton.h"
#include "myview.h"
#include "onebox.h"
#include "boxgroup.h"
#include "mywidget.h"
#include "global.h"
static const qreal INITSPEED=800;

myView::myView(QWidget *parent) :
    QGraphicsView(parent)
{
    cursor=new QCursor(QPixmap(":/new/prefix1/test.png"));
    setCursor(*cursor);
    JPress=false;
    initView();
    gameScore=0;
    gameEnergy=0;
    clearedNumber=0;
    allowJ=false;
    installDialog();
    player=new QMediaPlayer(this);
    QTimer::singleShot(2000,this,SLOT(abc()));
    connect(group,SIGNAL(pressZ()),this,SLOT(recvievePressZ()));
}

void myView::JScore()
{
    if(gameEnergy>=50)
    {
        gameEnergy-=50;
    }
    gameScore+=100;
    i=30;k=31;
    gameScoreText->setHtml(tr("<font color=\"#082567\">%1</font>").arg(gameScore));
    gameEnergyText->setHtml(tr("<font color=\"#082567\">%1</font>").arg(gameEnergy));

    //使用技能后进入冷却时间
    cd=new QGraphicsTextItem;
    cd->setFont(QFont("Times",20,QFont::Bold));
    cd->setPos(570,450);   
    scene()->addItem(cd);
    cd->setHtml(tr("<font color=\"#5e0091\">冷却:</font>"));
    Timer=new QTimer(this);
    connect(Timer,SIGNAL(timeout()),this,SLOT(cdchange()));
    Timer->start(1000);

    //播放分数改变动画->(开始)
    QPropertyAnimation *scoreAnimation=new QPropertyAnimation(gameScoreText,"scale");
    scoreAnimation->setEasingCurve(QEasingCurve::OutBounce);
    scoreAnimation->setDuration(250);
    scoreAnimation->setStartValue(4);
    scoreAnimation->setEndValue(1);
    scoreAnimation->start();
    //播放分数改变动画<-(结束)
    QTimer::singleShot(10,this,SLOT(newSlots()));
}

void myView::cdchange()
{
    allowJ=false;
    if(k-i==2)
    {
        CDtime->hide();
        k--;
    }
    CDtime = new QGraphicsTextItem;
    CDtime->setPos(650,450);
    CDtime->setFont(QFont("Times",20,QFont::Bold));
    CDtime->setDefaultTextColor(QColor(8,37,103));
    CDtime->setPlainText(QString::number(i));
    scene()->addItem(CDtime);
    i--;
    qDebug()<<"CDtime:"<<i<<" "<<k;
    if(i<0)
    {
        Timer->stop();
        CDtime->hide();
        cd->hide();
        allowJ=true;
    }
}
void myView::clearAll()
{
    //播放方块消除音效->(开始)
    player->setMedia(QUrl("qrc:/new/prefix1/flystar.wav"));
    player->setVolume(global::effect_volume);
    player->play(); 
    //播放方块消除音效<-(结束)

    JPress=true;

    //一行行地消除方块并播放动画->(开始)
    for(int y=429;y>30;y=y-20)
    {
        QList<QGraphicsItem*> list=scene()->items(QRectF(299,y,202,22),Qt::ContainsItemShape,Qt::DescendingOrder);
        if(list.count()>=0)
        {
            foreach (QGraphicsItem* item,list)
            {
                oneBox *box=(oneBox*)item;
                QGraphicsBlurEffect *effect=new QGraphicsBlurEffect;
                box->setGraphicsEffect(effect);
                QPropertyAnimation *animation=new QPropertyAnimation(box,"scale");
                animation->setEasingCurve(QEasingCurve::OutBounce);
                animation->setDuration(250);
                animation->setStartValue(0.25);
                animation->setEndValue(1);
                animation->start(QAbstractAnimation::DeleteWhenStopped);
                connect(animation,SIGNAL(finished()),box,SLOT(deleteLater()));
            }
            rows<<y;
        }

    }
    //一行行地消除方块并播放动画->(结束)


    //下移并申请新的方块
    if(rows.count()>0)
    {
        QTimer::singleShot(400,this,SLOT(moveLines()));
    }
    else
    {
        group->createBoxGroup(QPointF(400,70),nextGroup->getCurrentBox());
        nextGroup->clearBoxGroup(true);
        nextGroup->createBoxGroup(QPointF(600,110));
    }
    JScore();
    allowJ=false;
    QTimer::singleShot(2000,this,SLOT(abc()));
}

bool myView::isAllowJ()
{
    if(gameEnergy>=30&&allowJ)
        return true;
    else
        return false;
}

long myView::getGameScore()
{
    return gameScore;
}

void myView::newScore()
{
    gameScore=0;
    gameEnergy=0;
    gameScoreText->setHtml(tr("<font color=\"#082567\">%1</font>").arg(gameScore));
    gameEnergyText->setHtml(tr("<font color=\"#082567\">%1</font>").arg(gameEnergy));
}

void myView::clearNextBox()
{
    nextGroup->clearBoxGroup(true);
}

void myView::installDialog()
{
    box=new QDialog(this);
    box->setWindowOpacity(0.90);
    box->setStyleSheet("background-color:rgba(80,84,176,255);border-color:pink;");
    label=new QLabel;
    btn1=new myPushButton(tr("重新开始"));
    btn2=new myPushButton(tr("关闭游戏"));
    connect(btn1,SIGNAL(clicked()),this,SLOT(reStartGame()));
    connect(btn2,SIGNAL(clicked()),this,SLOT(closeWidget()));
    messageTitle=new QLabel(tr("            游戏结束"));
    messageTitle->setMinimumWidth(275);
    QFont font_0;
    font_0.setPointSize(20);
    font_0.setBold(true);

    messageTitle->setFont(font_0);
    btn1->setMaximumWidth(140);
    btn1->setMinimumWidth(140);

    btn2->setMaximumWidth(140);
    btn2->setMinimumWidth(140);

    btn1->setMaximumHeight(30);
    btn1->setMinimumHeight(30);

    btn2->setMaximumHeight(30);
    btn2->setMinimumHeight(30);

    btn1->setAutoDefault(false);
    btn2->setAutoDefault(false);


}

void myView::setParentWidget(QWidget *widget)
{
    parentWidget=widget;
}

void myView::endWidget()
{

    QHBoxLayout *hlayout=new QHBoxLayout;
    hlayout->addWidget(btn1);
    hlayout->addWidget(btn2);
    hlayout->setSpacing(3);
    hlayout->setContentsMargins(5,0,5,0);

    messageTitle->setStyleSheet("background-color:rgba(184,7,235,106)");
    hlayout->setAlignment(btn1,Qt::AlignCenter);
    hlayout->setAlignment(btn2,Qt::AlignCenter);

    QVBoxLayout *layout=new QVBoxLayout;
    layout->addWidget(messageTitle);
    layout->addWidget(label);
    layout->addLayout(hlayout);
    layout->setAlignment(messageTitle,Qt::AlignTop);
    layout->setAlignment(label,Qt::AlignCenter);

    layout->setContentsMargins(0,0,0,0);
    layout->setStretch(0,0);
    box->setLayout(layout);
    box->setWindowFlags(Qt::Window|Qt::FramelessWindowHint);
    label->setText(tr(" 很抱歉你已经输了！是否重来一局？"));
    QFont font_1;
    font_1.setPointSize(20);
    font_1.setBold(true);
    label->setFont(font_1);
    box->setGeometry(parentWidget->pos().x()+210,parentWidget->pos().y()+250,307,200);
    box->exec();

}

void myView::recvievePressZ()
{
    emit resendPressZ();
}

void myView::closeWidget()
{
    emit closeAliWidget();
}

void myView::startGame()
{
    initGame();
}

void myView::clearFullLines()
{
    //以下和clearAll函数相同、所不同的是进行了满行判断、只消除满行的格子、而clearAll消除全部
    player->setMedia(QUrl("qrc:/new/prefix1/useitem.wav"));
    player->setVolume(global::effect_volume);
    player->play();
    for(int y=429;y>50;y=y-20)
    {
        QList<QGraphicsItem*> list=scene()->items(QRectF(299,y,202,22),Qt::ContainsItemShape,Qt::DescendingOrder);
        if(list.count()==10)
        {
            foreach (QGraphicsItem* item,list)
            {
                group->waring=true;
                oneBox *box=(oneBox*)item;
                QGraphicsBlurEffect *effect=new QGraphicsBlurEffect;
                box->setGraphicsEffect(effect);
                QPropertyAnimation *animation=new QPropertyAnimation(box,"scale");
                animation->setEasingCurve(QEasingCurve::OutBounce);
                animation->setDuration(250);
                animation->setStartValue(4);
                animation->setEndValue(0.25);
                animation->start(QAbstractAnimation::DeleteWhenStopped);
                connect(animation,SIGNAL(finished()),box,SLOT(deleteLater()));
            }
            rows<<y;
        }

    }

    if(rows.count()>0)
    {
        player->setMedia(QUrl("qrc:/new/prefix1/flystar.wav"));
        player->setVolume(global::effect_volume);
        player->play();
        QTimer::singleShot(400,this,SLOT(moveLines()));
    }
    else
    {
        group->createBoxGroup(QPointF(400,70),nextGroup->getCurrentBox());
        nextGroup->clearBoxGroup(true);
        nextGroup->createBoxGroup(QPointF(600,110));
    }
}

void myView::moveLines()
{
    //下移操作并申请新的方块
    for(int i=rows.count();i>0;i--)
    {
        int row=rows.at(i-1);
        QGraphicsItem* item;
        QList<QGraphicsItem*> list=scene()->items(QRectF(299,49,202,row-47),Qt::ContainsItemShape);
        setFocus();
        foreach (item,list)
        {
            item->moveBy(0,20);
        }
    }


    if(JPress)
    {
        JPress=false;
    }
    else
    {
        updateScore(rows.count());
    }

    rows.clear();
    group->createBoxGroup(QPointF(400,70),nextGroup->getCurrentBox());
    nextGroup->clearBoxGroup(true);
    nextGroup->createBoxGroup(QPointF(600,110));
    group->waring=false;                                             //view的操作完成、设置waring为假、group可以影响键盘事件
}

void myView::gameOver()
{
    Timer->stop();
    cd->hide();
    CDtime->hide();
    allowJ=true;
    emit gameIsOver();
}

void myView::pauseGame()
{
    group->stopTimer();
}

void myView::rePauseGame()
{
    group->startTimer(gameSpeed);
}

void myView::reStartGame()
{
    box->close();
    setFocus();
    nextGroup->clearBoxGroup(true);
    group->clearBoxGroup();
    QList<QGraphicsItem*> list=scene()->items(QRectF(299,49,202,402),Qt::ContainsItemShape);
    foreach (QGraphicsItem* item,list)
    {
        scene()->removeItem(item);
        oneBox *box=(oneBox*)item;
        box->deleteLater();
    }
    emit gameIsRestart();
}

void myView::newSlots()
{
    //播放分数改变动画->(开始)
    QPropertyAnimation *energyAnimation=new QPropertyAnimation(gameEnergyText,"scale");
    energyAnimation->setEasingCurve(QEasingCurve::OutBounce);
    energyAnimation->setDuration(250);
    energyAnimation->setStartValue(4);
    energyAnimation->setEndValue(1);
    energyAnimation->start();
    //播放分数改变动画<-(结束)
}

void myView::abc()
{
    allowJ=true;
}

void myView::initGame()
{
    group->createBoxGroup(QPointF(400,70));
    group->setFocus();
    group->startTimer(INITSPEED);
    gameSpeed=INITSPEED;
    nextGroup->createBoxGroup(QPointF(600,110));

    scene()->addItem(label1Text);
    scene()->addItem(gameScoreText);
    scene()->addItem(label2Text);
    scene()->addItem(gameEnergyText);


    gameScoreText->setHtml(tr("<font color=\"#082567\">0</font>"));
    gameEnergyText->setHtml(tr("<font color=\"#082567\">0</font>"));
    label1Text->setHtml(tr("<font color=\"#5e0091\">分数:</font>"));
    label2Text->setHtml(tr("<font color=\"#5e0091\">能量:</font>"));
    lineItem1->show();
    lineItem2->show();
    lineItem3->show();
    lineItem4->show();

}

void myView::initView()
{
    setRenderHint(QPainter::Antialiasing);
    setCacheMode(CacheBackground);
    setMaximumSize(800,500);
    setMinimumSize(800,500);
    QPixmap map(":/new/prefix1/images/03.png");
        map = map.scaled(800, 500);
        map.save("D:/k.jpg");
    QGraphicsScene *scene=new QGraphicsScene;
    scene->setSceneRect(5,5,800,500);
    scene->setBackgroundBrush(QPixmap(":/new/prefix1/playing.jpg"));
    setScene(scene);
    lineItem1=scene->addLine(297,47,503,47);
    lineItem2=scene->addLine(297,453,503,453);
    lineItem3=scene->addLine(297,47,297,453);
    lineItem4=scene->addLine(503,47,503,453);

    lineItem1->setPen(QColor(0,0,0,255));
    lineItem2->setPen(QColor(0,0,0,255));
    lineItem3->setPen(QColor(0,0,0,255));
    lineItem4->setPen(QColor(0,0,0,255));

    group=new boxGroup;
    connect(group,SIGNAL(needNextGroup()),this,SLOT(clearFullLines()));
    connect(group,SIGNAL(gameFinish()),this,SLOT(gameOver()));
    scene->addItem(group);

    nextGroup=new boxGroup;
    scene->addItem(nextGroup);

    gameScoreText=new QGraphicsTextItem;
    gameScoreText->setFont(QFont("Times",20,QFont::Bold));
    gameScoreText->setPos(650,350);

    gameEnergyText=new QGraphicsTextItem;
    gameEnergyText->setFont(QFont("Times",20,QFont::Bold));
    gameEnergyText->setPos(650,400);

    label1Text=new QGraphicsTextItem;
    label1Text->setFont(QFont("Times",20,QFont::Bold));
    label1Text->setPos(570,350);

    label2Text=new QGraphicsTextItem;
    label2Text->setFont(QFont("Times",20,QFont::Bold));
    label2Text->setPos(570,400);


    lineItem1->hide();
    lineItem2->hide();
    lineItem3->hide();
    lineItem4->hide();
    //startGame();
}

void myView::updateScore(const int fullRowNum)
{
    clearedNumber+=fullRowNum;
    gameScore+=fullRowNum*10;

    switch(fullRowNum)
    {
    case 0:gameEnergy=0;break;
    case 1:gameEnergy+=2;break;
    case 2:gameEnergy+=6;break;
    case 3:gameEnergy+=15;break;
    case 4:gameEnergy+=35;break;
    }


    gameScoreText->setHtml(tr("<font color=\"#082567\">%1</font>").arg(gameScore));
    gameEnergyText->setHtml(tr("<font color=\"#082567\">%1</font>").arg(gameEnergy));

    QPropertyAnimation *scoreAnimation=new QPropertyAnimation(gameScoreText,"scale");
    scoreAnimation->setEasingCurve(QEasingCurve::OutBounce);
    scoreAnimation->setDuration(250);
    scoreAnimation->setStartValue(4);
    scoreAnimation->setEndValue(1);
    scoreAnimation->start();

    QTimer::singleShot(10,this,SLOT(newSlots()));


    //以消除的总行数提升游戏速度、最小值为1
    if(clearedNumber>=3)
    {
        gameSpeed-=50;
        if(gameSpeed<1)
        {
            gameSpeed=1;
        }
        group->stopTimer();
        group->startTimer(gameSpeed);
        clearedNumber=0;
    }
}
