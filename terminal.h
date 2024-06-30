#ifndef TERMINAL_H
#define TERMINAL_H

#include "utils.h"
#include "connectline.h"

#define QChinese(str) QString(str)

enum TerminalType{
    inputTerminal,
    outputTerminal
};
class MainScene;
class Node;
class Terminal : public QGraphicsItem// 记录连接节点的端点
{
public:
    Terminal(MainScene *scene,TerminalType terminaltype,Node *parent = nullptr);
    ~Terminal();
    int type() const override {if (_terminalType == inputTerminal) return 3001; else return 3002;}
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
public:
    ConnectLine* _connectLines = nullptr;
    QRectF boundingRect() const override;
    QRectF _boundingRect;
    MainScene *_scene;
    Node *_node;
    TerminalType _terminalType;

};

#endif // TERMINAL_H
