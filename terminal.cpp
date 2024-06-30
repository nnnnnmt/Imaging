#include "terminal.h"
#include "node.h"
#include <QDebug>

Terminal::Terminal(MainScene *scene,TerminalType terminaltype, Node *parent)
    : QGraphicsItem(parent)
{
    _scene = scene;
    _node = parent;
    _boundingRect = QRectF(0,0,30,30);
    _terminalType = terminaltype;
    setZValue(50);
    setFlag(ItemSendsScenePositionChanges);
}

Terminal::~Terminal()
{
    qDebug() << "~Terminal";
}

void Terminal::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QBrush fillBlack;
    QPen border;
    border.setColor(QColor(150,150,150));
    border.setWidth(2);
    if (_terminalType == TerminalType::inputTerminal)
    {
        fillBlack.setColor(QColor(228,206,244));
        fillBlack.setStyle(Qt::SolidPattern);
        painter->setBrush(fillBlack);
        //painter->drawEllipse(QPoint(8, 8), 6, 6);
        // 画正方形
        QPolygonF polygon;
        polygon << QPointF(-8+8, -8+8) << QPointF(8+8, -8+8) << QPointF(8+8, 8+8) << QPointF(-8+8, 8+8);
        painter->drawPolygon(polygon);
        painter->setPen(border);
        painter->drawPolygon(polygon);
    }
    else
    {
        fillBlack.setColor(QColor(249,218,218));
        fillBlack.setStyle(Qt::SolidPattern);
        painter->setBrush(fillBlack);
        //painter->drawEllipse(QPoint(8, 8), 6, 6);
        // 画三角形
        QPolygonF polygon;
        polygon << QPointF(-20.0/3+8, 10+8) << QPointF(30.0/3+8,0+8) << QPointF(-20.0/3+8, -10+8);
        painter->drawPolygon(polygon);
        painter->setPen(border);
        painter->drawPolygon(polygon);
    }


}

QVariant Terminal::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if(change == QGraphicsItem::ItemScenePositionHasChanged)
    {
        //qDebug() << "Terminal::itemChange()";
        if (_connectLines)
        {
            _connectLines->redrawline();
        }
    }
    return QGraphicsItem::itemChange(change, value);
}

QRectF Terminal::boundingRect() const
{
    return _boundingRect;
}
