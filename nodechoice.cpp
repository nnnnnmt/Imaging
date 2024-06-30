#include "nodechoice.h"
#include <QDebug>
#include <QWidget>
#include <QMouseEvent>
#include <QDrag>
#include <QMimeData>
#include "ui_nodechoice.h"

#define QChinese(str) QString(str)

NodeChoice::NodeChoice(QWidget *parent)
    : QWidget(parent),
    ui(new Ui::NodeChoice)
{
    ui->setupUi(this);
}

NodeChoice::~NodeChoice()
{
    delete ui;
}

void NodeChoice::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);
    QWidget* childClicked = childAt(event->pos());
    if(childClicked){
        QString name = childClicked->objectName();
        QString sub = "Option";
        if (name.contains(sub)) {
            if (name == "outOption" /*|| name == "pic2picOption" || name == "txt2picOption"*/)
                return;
            if (name == "pic2picOption" || name == "txt2picOption"){
                // 弹出信息框
                QMessageBox::information(this, QChinese("提示"), QChinese("该功能在 release 版本中暂未开放，如需使用请在 ainode.h 中替换自己的 API-KEY 后注释掉 nodechoice.cpp 中的这段代码"),QMessageBox::Ok);
                return;
            }
            QDrag* drag = new QDrag(childClicked);
            QMimeData* mimeData = new QMimeData;
            mimeData->setText(name);
            drag->setMimeData(mimeData);
/**/
            if (name == "ctrOption")
                drag->setPixmap(QPixmap(":/nodechoice/contrastPx.png").scaled(QSize(120,90)));
            else if (name == "editOption")
                drag->setPixmap(QPixmap(":/nodechoice/basicPx.png").scaled(QSize(120,90)));
            else if (name == "hueOption")
                drag->setPixmap(QPixmap(":/nodechoice/huePx.png").scaled(QSize(120,90)));
            else if (name == "tempOption")
                drag->setPixmap(QPixmap(":/nodechoice/temperaturePx.png").scaled(QSize(120,90)));
            else if (name == "briOption")
                drag->setPixmap(QPixmap(":/nodechoice/brightPx.png").scaled(QSize(120,90)));
            else if (name == "satuOption")
                drag->setPixmap(QPixmap(":/nodechoice/saturatPx.png").scaled(QSize(120,90)));
            else if (name == "lutOption")
                drag->setPixmap(QPixmap(":/nodechoice/filterPx.png").scaled(QSize(120,90)));
            else if (name == "addOption")
                drag->setPixmap(QPixmap(":/nodechoice/addPx.png").scaled(QSize(120,90)));
            else if (name == "inputOption")
                drag->setPixmap(QPixmap(":/nodechoice/inputPx.png").scaled(QSize(120,90)));
            else if (name == "pic2picOption")
                drag->setPixmap(QPixmap(":/nodechoice/aipic2picPx.png").scaled(QSize(120,90)));
            else if (name == "txt2picOption")
                drag->setPixmap(QPixmap(":/nodechoice/aiword2picPx.png").scaled(QSize(120,90)));
            else if (name == "outOption")
                drag->setPixmap(QPixmap(":/nodechoice/outPx.png").scaled(QSize(120,90)));

            drag->exec(Qt::CopyAction);
        }
    }
    else{
        qDebug()<<"None";
    }
}
