#ifndef ADDNODE_H
#define ADDNODE_H
#include "node.h"
#include "mainscene.h"
#include "utils.h"

#define QChinese(str) QString(str)

QT_BEGIN_NAMESPACE
namespace Ui {
class AddNodeWidget;
}
QT_END_NAMESPACE

class AddNodeWidget : public QWidget
{
    Q_OBJECT
public:
    AddNodeWidget(QWidget *parent = nullptr);
    ~AddNodeWidget();

public:
    Ui::AddNodeWidget *ui;
};

class AddNode : public Node
{
public:
    AddNode(MainScene *scene,QGraphicsItem *parent = nullptr);
    ~AddNode();

    int type() const override {return 4201;}
    virtual void initProxyWidget() override;
    void execute() override;

    QPushButton *_addInputButton;
    QPushButton *_removeInputButton;
    QLabel *_label;
    QCheckBox *_checkBox;
    QMap<Terminal*,cv::Mat> _inputMapImages;
    AddNodeWidget *_widget = nullptr;
    void resizeWidget() override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};


#endif // ADDNODE_H
