#ifndef NODE_H
#define NODE_H

#include "utils.h"
#include "terminal.h"

#define QChinese(str) QString(str)

enum class BasicNodeType {
    inputNode,
    functionNode,
    addNode,
    outputNode
};
enum class FunctionNodeType{
    NotFunctionNode,
    basicFunctionNode,
    AIFunctionNode
};
enum class NodeType{
    input_inputNode,
    fun_editNode,
    fun_contrastNode,
    fun_brightnessNode,
    fun_saturationNode,
    fun_colourtuneNode,
    fun_lutNode,
    ai_word2imgNode,
    ai_img2imgNode,
    add_addNode,
    out_outputNode,
    fun_colorTemperatureNode,
    fun_filterNode,
    fun_hueNode
};

class MainScene;
class Node : public QObject ,public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:
    Node(MainScene *scene,QGraphicsItem *parent = nullptr);
    virtual ~Node();
    QRectF boundingRect() const override;
    /**
     * @brief 绘制Node 暂时由基类实现 子类如需要可以重写
     * @param painter
     * @param option
     * @param widget
     */
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    /**
     * @brief 如果鼠标点击，选中Node
     * @param event
     */
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    /**
     * @brief 改变画布大小
     * @param width
     * @param height
     */
    virtual void changeCanvasSize(int width,int height);
    /**
     * @brief 添加输出端口，可复制当前节点的输出
     */
    void addOutputTerminal();
    /**
     * @brief 当前节点计算函数 需要由子类重写
     */
    virtual void execute() {;};
    /**
     * @brief 初始化端口位置
     */
    virtual void initializeTerminal();
    /**
     * @brief 初始化Node的边界大小
     */
    virtual void initializeBoundingRect();
    /**
     * @brief 初始化Node的设置界面 需要由子类重写
     */
    virtual void initProxyWidget() {;};
    virtual void resizeWidget() {;}
    /**
     * @brief 设置Node的Terminal数量，打开项目时使用
     * @param terminalType
     * @param num
     */
    void setTerminalNum(TerminalType terminalType, int num);
    void nodeSetPixmap(QPixmap pm, QLabel *_label);

public:
    BasicNodeType _basicNodeType;
    FunctionNodeType _functionNodeType;
    NodeType _nodeType;
    int type() const override {return 4000;};

public:
    MainScene *_scene; // 节点所在的场景
    QRectF _boundingRect;
    QString _name; // Node名称 需要显示
    int _canvasWidth;
    int _canvasHeight;

    cv::Mat _outputImage; // 输出图片 有且只有一个
    QList<cv::Mat> _inputImages; // 输入的图片，除AddNode外只有一个
    QList<Terminal*> _inputTerminals; // 输入端口 除AddNode外只有一个
    QList<Terminal*> _outputTerminals; // 输出端口 可有多个，将输出复制多份
    double _r = 0.167; // Node颜色分界线的比例
    QGraphicsProxyWidget *_nodeSettings; // 节点设置的proxyWidget
    bool _show_preview_or_not = false; // 是否显示预览
signals:
    void clearSelected();

};

cv::Mat channelUpgrade(cv::Mat image);
QPixmap cvMat2QPixmap(cv::Mat image);
QImage cvMat2QImage(cv::Mat image);

#endif // NODE_H
