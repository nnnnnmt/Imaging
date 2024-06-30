#ifndef CONNECTLINE_H
#define CONNECTLINE_H

#define QChinese(str) QString(str)
#include "utils.h"

class MainScene;
class Terminal;
class ConnectLine: public QGraphicsPathItem
{
public:
    ConnectLine(Terminal* inTerminal,Terminal* outTerminal,QGraphicsItem* parent = nullptr);
    /**
     * @brief 为了打开老项目，另一种构造方式，通过输入输出端点的位置来构造ConnectLine
     */
    ConnectLine(QPointF inTerminalPos, QPointF outTerminal,MainScene* parentScene, QGraphicsItem* parent = nullptr);
    ~ConnectLine();

    /**
     * @brief 把当前线段的终点更新为鼠标位置
     * @param mousepos
     */
    void updateLine(QPointF mousepos);
    /**
     * @brief 根据Line连接的终末Terminal位置重新绘制线段
     */
    void redrawline();

    int type() const override {return 3000;}


public:
    Terminal* _inTerminal = nullptr; // 连接的输入端点（对应Node的输出端口）
    Terminal* _outTerminal = nullptr; // 连接的输出端点（对应Node的输入端口）
    QRectF _boundingRect; // 线段的边界
};

#endif // CONNECTLINE_H
