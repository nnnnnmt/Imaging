#include "connectline.h"
#include "terminal.h"
#include <QDebug>
#include "mainscene.h"

ConnectLine::ConnectLine(Terminal *inTerminal, Terminal *outTerminal, QGraphicsItem *parent)
    : QGraphicsPathItem(parent)
{
    qDebug() << "ConnectLine Constructed";
    _inTerminal = inTerminal;
    _outTerminal = outTerminal;
    setPen(QPen(Qt::black, 4, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    setZValue(60);
    setFlag(QGraphicsItem::ItemIsSelectable);
}

ConnectLine::ConnectLine(QPointF inTerminalPos, QPointF outTerminalPos, MainScene *parentScene, QGraphicsItem *parent)
    : QGraphicsPathItem(parent)
{
    qDebug() << "ConnectLine Constructed";
    // 查找parentScene->nodes()中的Terminal,如果有Terminal的boundingRect包含inTerminalPos,则将其赋值给_inTerminal
    // 查找parentScene->nodes()中的Terminal,如果有Terminal的boundingRect包含outTerminalPos,则将其赋值给_outTerminal
    for (int i = 0; i < parentScene->_nodes->size(); i++)
    {
        Node *node = parentScene->_nodes->at(i);
        for (int j = 0; j < node->_outputTerminals.size(); j++)
        {
            Terminal *terminal = node->_outputTerminals.at(j);
            if (inTerminalPos==terminal->scenePos())
            {
                _inTerminal = terminal;
                _inTerminal->_connectLines = this;
            }
        }
        for (int j = 0; j < node->_inputTerminals.size(); j++)
        {
            Terminal *terminal = node->_inputTerminals.at(j);
            if (outTerminalPos==terminal->scenePos())
            {
                _outTerminal = terminal;
                _outTerminal->_connectLines = this;
            }
        }
    }
    setPen(QPen(Qt::black, 4, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    setZValue(60);
    setFlag(QGraphicsItem::ItemIsSelectable);
}

ConnectLine::~ConnectLine()
{
    qDebug() << "~ConnectLine";
}

void ConnectLine::updateLine(QPointF mousepos)
{
    prepareGeometryChange();
    QPainterPath path;
    QPointF startPos;
    if (_inTerminal)
    {
        startPos = QPointF(_inTerminal->scenePos().x() + 8, _inTerminal->scenePos().y() + 8);
        path.moveTo(startPos);
    }
    else
    {
        startPos = QPointF(_outTerminal->scenePos().x() + 8, _outTerminal->scenePos().y() + 8);
        path.moveTo(startPos);
    }

    qreal scale = 1.5;
    QPointF controlPoint1, controlPoint2;
    if (_inTerminal)
    {
        controlPoint1 = QPointF((_inTerminal->scenePos().x() * scale + mousepos.x()) / (1 + scale), _inTerminal->scenePos().y());
        controlPoint2 = QPointF((_inTerminal->scenePos().x() + mousepos.x() * scale) / (1 + scale), mousepos.y());
        path.cubicTo(controlPoint1, controlPoint2, mousepos);
    }
    else
    {
        controlPoint1 = QPointF((mousepos.x() * scale + _outTerminal->scenePos().x()) / (1 + scale), mousepos.y());
        controlPoint2 = QPointF((mousepos.x() + _outTerminal->scenePos().x() * scale) / (1 + scale), _outTerminal->scenePos().y());
        path.cubicTo(controlPoint2, controlPoint1, mousepos);
    }
    setPath(path);
}

void ConnectLine::redrawline()
{
    prepareGeometryChange();
    QPainterPath path;
    qreal scale = 1.5;
    path.moveTo(QPointF(_inTerminal->scenePos().x() + 8, _inTerminal->scenePos().y() + 8));
    QPointF controlPoint1 = QPointF((_inTerminal->scenePos().x()*scale + _outTerminal->scenePos().x()) / (1+scale), _inTerminal->scenePos().y());
    QPointF controlPoint2 = QPointF((_inTerminal->scenePos().x() + _outTerminal->scenePos().x()*scale) / (1+scale), _outTerminal->scenePos().y());
    path.cubicTo(controlPoint1, controlPoint2,QPointF(_outTerminal->scenePos().x() + 8, _outTerminal->scenePos().y() + 8));
    setPath(path);
}
