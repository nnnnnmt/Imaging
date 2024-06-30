#ifndef MAINVIEW_H
#define MAINVIEW_H

#include "utils.h"
#include "mainscene.h"

#define QChinese(str) QString(str)
class MainView : public QGraphicsView
{
public:
    explicit MainView(MainScene *scene,QWidget *parent = nullptr);
    ~MainView();
    /**
     * @brief 检测鼠标中键是否按下，如果按下则记录鼠标位置
     * @param event
     */
    void mousePressEvent(QMouseEvent *event);
    /**
     * @brief 检测鼠标中键是否释放，如果释放则记录鼠标位置
     * @param event
     */
    void mouseReleaseEvent(QMouseEvent *event);
    /**
     * @brief 检测鼠标是否移动，如果移动则移动画布
     * @param event
     */
    void mouseMoveEvent(QMouseEvent *event);
    /**
     * @brief 检测鼠标滚轮事件，放大缩小画布
     * @param event
     */
    void wheelEvent(QWheelEvent *event);
    /**
     * @brief 检测拖拽事件，如果拖拽的是Node则添加Node
     * @param event
     */
    void dropEvent(QDropEvent *event);
    /**
     * @brief 负责添加Node到MainScene
     * @param nodename
     * @param position
     */
    Node* addNode(NodeType nodename,QPointF position);
    /**
     * @brief 检测拖拽事件，如果拖拽的是Node则添加Node
     * @param event
     */
    void dragEnterEvent(QDragEnterEvent *event);
    /**
     * @brief 检测拖拽事件，如果拖拽的是Node则添加Node
     * @param event
     */
    void dragMoveEvent(QDragMoveEvent *event);

    //QList<ConnectLine*> _lines;
public:
    MainScene *_scene; // 主场景
    bool _isMousePressed = false; // 鼠标中键是否按下 是否要准备移动画布
    QList<Node*> _nodes; // 存储所有节点
    QPointF _centerPos; // 画布中心位置
    QPointF _eventPos; // 鼠标位置
};

#endif // MAINVIEW_H
