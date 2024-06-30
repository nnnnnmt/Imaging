#include "node.h"
#include "mainscene.h"
#include <QDebug>


Node::Node(MainScene *scene, QGraphicsItem *parent)
    : QGraphicsItem(parent)
{
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemAcceptsInputMethod);
    setCacheMode(DeviceCoordinateCache);
    _scene = scene;
    setZValue(1);
    setAcceptHoverEvents(true);
    //_boundingRect = QRectF(0,0,100,100);
    connect(this, &Node::clearSelected, _scene, &MainScene::_clearselected);

}

Node::~Node()
{
    qDebug() << "~Node";
}

QRectF Node::boundingRect() const
{
    return _boundingRect;
}

void Node::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    ;
}

void Node::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        //qDebug() << "Node::mousePressEvent";
        setSelected(true);
        if(event->modifiers() != Qt::ShiftModifier)
        {
            //qDebug() << "clearSelected";
            emit clearSelected();
        }
    }
    else if (event->button() == Qt::RightButton)
    {
        if (type() >= 4000 && type() < 5000)
        {
            _scene->_clearselected();
            setSelected(true);
            QMenu menu;
            QAction *addTerminal = menu.addAction(QChinese("复制输出"));
            QAction *deleteNode = menu.addAction(QChinese("删除节点"));
            QAction *action = menu.exec(event->screenPos());
            if(action == addTerminal)
                addOutputTerminal();
            else if(action == deleteNode && this->type() != 4301)
                _scene->_deleteItem(this);
        }
    }
    QGraphicsItem::mousePressEvent(event);
}

void Node::changeCanvasSize(int width, int height)
{
    //qDebug() << "Node::changeCanvasSize";
    _canvasWidth = width;
    _canvasHeight = height;
    if (!_outputImage.empty())
        cv::resize(_outputImage,_outputImage,cv::Size(_canvasWidth,_canvasHeight));
}

void Node::addOutputTerminal()
{
    _outputTerminals.append(new Terminal(_scene,TerminalType::outputTerminal,this));
    initializeTerminal();
}

void Node::initializeTerminal()
{
    int n = _inputTerminals.size();
    if (n < _outputTerminals.size()) {
        n = _outputTerminals.size();
    }
    int height = 50 * n + 17;
    if (height + 67 > 300) {
        _boundingRect = QRectF(0, 0, 300, height + 67);
    }
    else {
        _boundingRect = QRectF(0, 0, 300, 300);
    }
    resizeWidget();
    for(int i = 0; i < _inputTerminals.size(); i++){
        _inputTerminals[i]->setPos(-8, (i + 1.5) * 50 - 8);
    }
    for (int i = 0; i < _outputTerminals.size(); i++)
    {
        _outputTerminals[i]->setPos(_boundingRect.width() - 8, (i + 1.5) * 50 - 8);
    }
}

void Node::initializeBoundingRect()
{
    _boundingRect = QRectF(0,0,300,300);
}

void Node::setTerminalNum(TerminalType terminalType, int num)
{
    // 判断是否是叠加节点，只有叠加节点输入端口数量才可能变化
    if (type() != 4201 && num >1 && terminalType == TerminalType::inputTerminal)
        return;
    else if (type() == 4201 && num > 1 && terminalType == TerminalType::inputTerminal)
    {
        // 添加输入端口
        for (int i = 2; i < num; i++)
        {
            _inputTerminals.append(new Terminal(_scene, terminalType, this));
        }
    }
    if(terminalType == TerminalType::outputTerminal)
    {
        // 添加输出端口
        for (int i = 1; i < num; i++)
        {
            _outputTerminals.append(new Terminal(_scene, terminalType, this));
        }
    }
    initializeTerminal();
}

void Node::nodeSetPixmap(QPixmap pm, QLabel *_label)
{
    QPixmap bg(":/window/transparent.png");
    QPixmap shown(pm.size());
    QPainter painter(&shown);
    painter.drawPixmap(0, 0, bg);
    painter.drawPixmap(0, 0, pm);
    _label->setPixmap(shown);
}



