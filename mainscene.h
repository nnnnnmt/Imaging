#ifndef MAINSCENE_H
#define MAINSCENE_H

#include "utils.h"
#include "node.h"
#include "connectline.h"

#define QChinese(str) QString(str)

class MainScene : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit MainScene(QWidget *parent = nullptr);
    /**
     * @brief 如果鼠标单击，选中Terminal 设置_lineToConnect 准备开始绘制
     * @param event
     */
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    /**
     * @brief 如果鼠标移动，不断更新线段的终点
     * @param event
     */
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    /**
     * @brief 按下delete键删除选中的连线或者节点
     * @param event
     */
    void keyPressEvent(QKeyEvent *event);
    /**
     * @brief 如果鼠标释放，结束绘制线段，创建新的连线
     * @param event
     */
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    /**
     * @brief 绘制背景
     * @param painter
     * @param rect
     */
    void drawBackground(QPainter *painter, const QRectF &rect);
    /**
     * @brief 从MainView同步节点
     * @param nodes
     */
    void syncNodes(QList<Node*> *nodes);
    /**
     * @brief 如果右键点击，显示菜单 如果选中对象为连线则可删除连线
     * @param event
     */
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
    /**
     * @brief 由MainWindows调用，改变画布大小，同步改变所有节点画布大小
     * @param width
     * @param height
     */
    void changeCanvasSize(int width,int height);
    /**
     * @brief 由MainWindows调用，查找所有连线，同步输入输出 并执行每个节点的计算
     * 注：连线不是顺序执行，而是根据添加顺序全部执行，不保证一次执行后就能刷新全图
     * 但是MainWindows中有Timer定时刷新 所以总可以
     */
    void refresh_NodeOutputs();
    /**
     * @brief 删除当前选中的连线
     */
    void deleteConnectLine();
    /**
     * @brief 初始化连线，准备绘制
     * @param line
     */
    void initConnectLine(ConnectLine *line);
    /**
     * @brief 撤销上一步操作
     */
    void undo();

public:
    QList<Node*>* _nodes = nullptr; // 所有节点
    ConnectLine *_lineToConnect = nullptr; // 当前正在绘制的连线
    QList<QGraphicsItem*> _addedItems; // 维护一个队列，代表所有被添加的节点和连接线，用于undo
    int _canvasWidth; // 画布宽度
    int _canvasHeight; // 画布高度

    /**
     * @brief 删除一个节点
     * @param node
     */
    void _deleteItem(QGraphicsItem* node);
public slots:
    /**
     * @brief 清除所有选中的连线和节点
     */
    void _clearselected();
};

#endif // MAINSCENE_H
