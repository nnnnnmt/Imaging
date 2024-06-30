#include "mainview.h"
#include "inputnode.h"
#include "editnode.h"
#include "outputnode.h"
#include "functionnode.h"
#include "addnode.h"
#include "ainode.h"
#include <QDebug>

MainView::MainView(MainScene *scene, QWidget *parent)
    : QGraphicsView(scene,parent)
{
    setAcceptDrops(true);
    setDragMode(NoDrag);
    setRenderHint(QPainter::Antialiasing);
    _scene = scene;
    addNode(NodeType::out_outputNode,QPointF(1500,1500));

    setStyleSheet(
        "QGraphicsView { border: none; }"
        "QScrollBar:vertical {"
            "border: none;"
            "background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
                "stop:0 rgb(230,250,255),"
                "stop:0.5 rgb(228,245,235),"
                "stop:1.0 rgb(255,250,220));"
            "width: 10px;"
        "}"
        "QScrollBar::handle:vertical {"
        "    background-color: rgba(0, 0, 0, 20);"
        "    border-radius: 5px;"
        "}"
        "QScrollBar::add-line:vertical {"
        "    height: 0px;"
        "}"
        "QScrollBar::sub-line:vertical {"
        "    height: 0px;"
        "}"

        "QScrollBar:horizontal {"
            "border: none;"
            "background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
                "stop:0 rgb(230,250,255),"
                "stop:0.5 rgb(228,245,235),"
                "stop:1.0 rgb(253,250,221));"
            "height: 10px;"
        "}"
        "QScrollBar::handle:horizontal {"
            "background-color: rgba(0, 0, 0, 20);"
            "border-radius: 5px;"
        "}"
        "QScrollBar::add-line:horizontal {"
            "width: 0px;"
        "}"
        "QScrollBar::sub-line:horizontal {"
            "width: 0px;"
        "}");
}

MainView::~MainView()
{
    qDebug() << "~MainView";
}

void MainView::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::MiddleButton)
    {
        _centerPos = mapToScene(QPoint(viewport()->width() / 2.0, viewport()->height() / 2.0));
        _eventPos = event->pos();
        _isMousePressed = true;
    }
    else
    {
        QGraphicsView::mousePressEvent(event);
    }
}

void MainView::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::MiddleButton)
    {
        _isMousePressed = false;
    }
    else
    {
        QGraphicsView::mouseReleaseEvent(event);
    }
}

void MainView::mouseMoveEvent(QMouseEvent *event)
{
    if(_isMousePressed)
    {
        QPointF offsetPos = QPointF(event->pos() - _eventPos);
        offsetPos /= transform().m11();
        setTransformationAnchor(QGraphicsView::AnchorViewCenter);
        centerOn(_centerPos - offsetPos);
    }
    else
    {
        QGraphicsView::mouseMoveEvent(event);
    }
}

void MainView::wheelEvent(QWheelEvent *event)
{
    QPointF cursorPoint = event->position();
    QPointF scenePos = this->mapToScene(QPoint(cursorPoint.x(), cursorPoint.y()));
    qreal viewWidth = this->viewport()->width();
    qreal viewHeight = this->viewport()->height();
    qreal hScale = cursorPoint.x() / viewWidth;
    qreal vScale = cursorPoint.y() / viewHeight;
    int wheelDeltaValue = event->angleDelta().y();
    if (wheelDeltaValue > 0){
        this->scale(1.2, 1.2);
    }else{
        this->scale(1.0 / 1.2, 1.0 / 1.2);
    }
    QPointF viewPoint = this->transform().map(scenePos);
    horizontalScrollBar()->setValue(int(viewPoint.x() - viewWidth * hScale));
    verticalScrollBar()->setValue(int(viewPoint.y() - viewHeight * vScale));
}

void MainView::dropEvent(QDropEvent *event)
{
    //qDebug() << "dropEvent";

    if (event->mimeData()->hasText()) {
        QString nodeName = event->mimeData()->text();
        QPointF scenePos = mapToScene(event->pos());
        if (nodeName == "inputOption") {
            addNode(NodeType::input_inputNode,scenePos);
        }
        else if (nodeName == "addOption") {
            addNode(NodeType::add_addNode,scenePos);
        }
        else if (nodeName == "briOption") {
            addNode(NodeType::fun_brightnessNode,scenePos);
        }
        else if (nodeName == "ctrOption") {
            addNode(NodeType::fun_contrastNode,scenePos);
        }
        else if (nodeName == "editOption") {
            addNode(NodeType::fun_editNode,scenePos);
        }
        else if (nodeName == "hueOption") {
            addNode(NodeType::fun_hueNode,scenePos);
        }
        else if (nodeName == "lutOption") {
            addNode(NodeType::fun_filterNode,scenePos);
        }
        else if (nodeName == "outOption") {
            addNode(NodeType::out_outputNode,scenePos);
        }
        else if (nodeName == "pic2picOption") {
            addNode(NodeType::ai_img2imgNode,scenePos);
        }
        else if (nodeName == "satuOption") {
            addNode(NodeType::fun_saturationNode,scenePos);
        }
        else if (nodeName == "tempOption") {
            addNode(NodeType::fun_colourtuneNode,scenePos);
        }
        else if (nodeName == "txt2picOption") {
            addNode(NodeType::ai_word2imgNode,scenePos);
        }

        event->acceptProposedAction();
    }
}

Node* MainView::addNode(NodeType nodetype, QPointF position)
{
    if (nodetype==NodeType::fun_contrastNode)
        _nodes.append(new ContrastNode(_scene));
    else if (nodetype==NodeType::add_addNode)
        _nodes.append(new AddNode(_scene));
    else if (nodetype==NodeType::input_inputNode)
        _nodes.append(new InputNode(_scene));
    else if (nodetype==NodeType::out_outputNode)
        _nodes.append(new OutputNode(_scene));
    else if (nodetype==NodeType::fun_brightnessNode)
        _nodes.append(new BrightnessNode(_scene));
    else if (nodetype==NodeType::fun_saturationNode)
        _nodes.append(new SaturationNode(_scene));
    else if (nodetype==NodeType::ai_word2imgNode)
        _nodes.append(new AIword2imgNode(_scene));
    else if (nodetype==NodeType::ai_img2imgNode)
        _nodes.append(new AIimg2imgNode(_scene));
    else if (nodetype==NodeType::fun_editNode)
        _nodes.append(new EditNode(_scene));
    else if (nodetype==NodeType::fun_filterNode)
        _nodes.append(new FilterNode(_scene));
    else if (nodetype==NodeType::fun_colourtuneNode)
        _nodes.append(new ColorTemperatureNode(_scene));
    else if (nodetype==NodeType::fun_hueNode)
        _nodes.append(new HueNode(_scene));

    _scene->addItem(_nodes.last());
    _scene->_addedItems.append(_nodes.last());
    _scene->syncNodes(&_nodes);
    _nodes.last()->setPos(position);
    // 同步画布大小
    _scene->changeCanvasSize(_scene->_canvasWidth,_scene->_canvasHeight);
    return _nodes.last();
}

void MainView::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasText()) {
        event->acceptProposedAction();
    }
    event->accept();
}


void MainView::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasText()) {
        event->acceptProposedAction();
    }
    event->accept();
}
