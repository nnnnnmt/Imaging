#ifndef OUTPUTNODE_H
#define OUTPUTNODE_H

#include "utils.h"
#include "node.h"
#define QChinese(str) QString(str)

QT_BEGIN_NAMESPACE
namespace Ui {
class OutputNodeWidget;
}
QT_END_NAMESPACE

class OutputNodeWidget : public QWidget
{
    Q_OBJECT
public:
    OutputNodeWidget(QWidget *parent = nullptr);
    ~OutputNodeWidget();

public:
    Ui::OutputNodeWidget *ui;
};

class OutputNode : public Node
{
public:
    OutputNode(MainScene *scene,QGraphicsItem *parent = nullptr);
    ~OutputNode();
public:
    int type() const override {return 4301;}
    virtual void execute() override;
    void initializeBoundingRect() override;
    OutputNodeWidget *_widget;
};

#endif // OUTPUTNODE_H
