#include "mainscene.h"
#include "terminal.h"
#include "connectline.h"
#include "addnode.h"
#include <QDebug>

QGraphicsItem* getTerminal(QList<QGraphicsItem *> &mouseItems)
{
    // qDebug() << "getTerminal";
    // qDebug() << mouseItems.size();
    for(int i = 0;i < mouseItems.size(); i++)
    {
        if(mouseItems[i]->type() == 3001 || mouseItems[i]->type() == 3002)
        {
            //qDebug() << "getrealTerminal";
            return mouseItems[i];
        }
    }
    return nullptr;
}

MainScene::MainScene(QWidget *parent)
    : QGraphicsScene(parent)
{
    qDebug() << "MainScene";
    setItemIndexMethod(QGraphicsScene::NoIndex);
    setSceneRect(QRectF(0, 0, 3000, 3000));
}

void MainScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QList<QGraphicsItem *>mouseItems = items(event->scenePos());
    QGraphicsItem *topItem = getTerminal(mouseItems);
    if(topItem)
    {
        if(topItem->type() == 3002)
        {
            //new connection
            Terminal *outPortItem = static_cast<Terminal *>(topItem);
            if (!outPortItem->_connectLines)
            {
                _lineToConnect = new ConnectLine(outPortItem, nullptr);
                addItem(_lineToConnect);
                _addedItems.append(_lineToConnect);
            }
        }
        else if(topItem->type() == 3001)
        {
            Terminal *inPortItem = static_cast<Terminal *>(topItem);
            if(!inPortItem->_connectLines)
            {
                _lineToConnect = new ConnectLine(nullptr, inPortItem);
                addItem(_lineToConnect);
                _addedItems.append(_lineToConnect);
            }
        }
        else
            QGraphicsScene::mousePressEvent(event);
    }
    else
        QGraphicsScene::mousePressEvent(event);
}

void MainScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(_lineToConnect)
    {
        _lineToConnect->updateLine(event->scenePos());
    }
    QGraphicsScene::mouseMoveEvent(event);
}

void MainScene::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Delete)
    {
        //delete selected item(s)
        //qDebug() << "delete";
        QList<QGraphicsItem *>selectedItems = this->selectedItems();
        for(QGraphicsItem *item : selectedItems)
        {
            _addedItems.removeOne(item);
            _deleteItem(item);
        }
    }
    // ctrl + z撤销
    if(event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_Z)
    {
        undo();
    }
}

void MainScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if(_lineToConnect)
    {
        QList<QGraphicsItem *>mouseItems = items(event->scenePos());
        QGraphicsItem *topItem = getTerminal(mouseItems);
        if(topItem)
        {
            if(_lineToConnect->_inTerminal != nullptr && topItem->type() == 3001)
            {
                Terminal *inPortItem = static_cast<Terminal *>(topItem);
                if(inPortItem->_connectLines==nullptr && inPortItem->_node != _lineToConnect->_inTerminal->_node)
                {
                    //not the same node and inport has no connection
                    //connect to port
                    _lineToConnect->_inTerminal->_connectLines = _lineToConnect;
                    inPortItem->_connectLines = _lineToConnect;
                    _lineToConnect->_outTerminal = inPortItem;
                    _lineToConnect->redrawline();
                    _lineToConnect = nullptr;
                }
                else
                {
                    deleteConnectLine();
                }
            }
            else if(_lineToConnect->_outTerminal != nullptr && topItem->type() == 3002)
            {
                Terminal *outPortItem = static_cast<Terminal *>(topItem);
                    //not the same node
                    //connect to port
                if(outPortItem->_connectLines==nullptr && outPortItem->_node != _lineToConnect->_outTerminal->_node)
                {
                    _lineToConnect->_outTerminal->_connectLines = _lineToConnect;
                    outPortItem->_connectLines = _lineToConnect;
                    _lineToConnect->_inTerminal = outPortItem;
                    _lineToConnect->redrawline();
                    _lineToConnect = nullptr;
                }
                else
                {
                    deleteConnectLine();
                }
            }
            else
            {
                deleteConnectLine();
            }
        }
        else
        {
            deleteConnectLine();
        }
    }
    QGraphicsScene::mouseReleaseEvent(event);
}

void MainScene::drawBackground(QPainter *painter, const QRectF &rect)
{
    QPen pen(QColor(200,200,200));
    pen.setWidth(0);
    pen.setStyle(Qt::SolidLine);
    painter->setPen(pen);

    qreal left = rect.left();
    qreal right = rect.right();
    qreal top = rect.top();
    qreal bottom = rect.bottom();

    // 网格化坐标
    left = (left / 128) * 128;
    right = (right / 128) * 128;
    top = (top / 128) * 128;
    bottom = (bottom / 128) * 128;
    for(int i = 0; i >= top; i -= 64){
        painter->drawLine(left, i, right, i);
    }
    for(int i = 0; i <= bottom; i += 64){
        painter->drawLine(left, i, right, i);
    }
    for(int i = 0; i <= right; i += 64){
        painter->drawLine(i, top, i, bottom);
    }
    for(int i = 0; i >= left; i -= 64){
        painter->drawLine(i, top, i, bottom);
    }
}

void MainScene::syncNodes(QList<Node *>* nodes)
{
    _nodes = nodes;
}

void MainScene::changeCanvasSize(int width, int height)
{
    _canvasWidth = width;
    _canvasHeight = height;
    // 改变每个节点的画布大小
    if (_nodes == nullptr)
        return;
    //qDebug() << "_nodes->size()=" << _nodes->size();
    for(int i = 0; i < _nodes->size(); i++)
    {
        //qDebug() << "changeCanvasSize";
        //qDebug() << width << height;
        if (_nodes->at(i))
        {
            //qDebug() << "changeCanvasSize123789172381283";
            //qDebug() << _nodes->at(i)->type();
            _nodes->at(i)->changeCanvasSize(width, height);
        }
        qDebug() << width << height;
    }
}

void MainScene::refresh_NodeOutputs()
{
    // 查找场景中所有ConnectLine
    QList<QGraphicsItem *> items = this->items();
    // 新建一个新list 提取所有connectline
    QList<ConnectLine *> lines;
    for(int i = 0; i < items.size(); i++)
        if(items[i]->type() == 3000)
            lines.append(static_cast<ConnectLine *>(items[i]));

    // qDebug() << "refresh_NodeOutputs";
    // qDebug() << "lines.size()=" << lines.size();
    for(ConnectLine* line : lines)
    {
        if (line->_inTerminal && line->_outTerminal)
        {
            // qDebug() <<"in"<< line->_inTerminal->_node->type();
            // qDebug() <<"out"<< line->_outTerminal->_node->type();
            if (!line->_inTerminal->_node->_outputImage.empty())
            {
                //qDebug() << line->_outTerminal->_node->_inputImages.size();
                if (line->_outTerminal->_node->type() != 4201)
                {
                    if (line->_outTerminal->_node->_inputImages.size() != 0)
                    {
                         line->_outTerminal->_node->_inputImages[0] = line->_inTerminal->_node->_outputImage.clone();
                    }
                    else
                        line->_outTerminal->_node->_inputImages.append(line->_inTerminal->_node->_outputImage.clone());
                }
                else
                {
                    // qDebug() << "AddNode**************";
                    static_cast<AddNode *>(line->_outTerminal->_node)->_inputMapImages[line->_outTerminal] = line->_inTerminal->_node->_outputImage.clone();
                    // qDebug() << static_cast<AddNode *>(line->_outTerminal->_node)->_inputMapImages[line->_outTerminal].rows;
                }
            }
        }
    }
    // 遍历每个节点 执行execute函数
    for(int i = 0; i < _nodes->size(); i++)
    {
        _nodes->at(i)->execute();
    }
}

void MainScene::deleteConnectLine()
{
    removeItem(_lineToConnect);
    // 从addedItems中删除
    _addedItems.removeOne(_lineToConnect);
    delete _lineToConnect;
    _lineToConnect = nullptr;
}

void MainScene::initConnectLine(ConnectLine *line)
{
    line->_outTerminal = nullptr;
    line->_inTerminal = nullptr;
    _lineToConnect = line;
}

void MainScene::undo()
{
    if(_addedItems.size() == 0)
        return;
    QGraphicsItem *lastItem = _addedItems.takeLast();
    // 不允许撤销输出节点
    if (lastItem->type() == 4301)
        return;
    _deleteItem(lastItem);
}


void MainScene::_clearselected()
{
    //qDebug() << "_clearselected";
    for(int i = 0; i < this->selectedItems().length(); i++)
    {
        this->selectedItems()[i]->setSelected(false);
    }
}


void MainScene::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    qDebug() << "contextMenuEvent";
    QGraphicsItem *item = itemAt(event->scenePos(), QTransform());
    _clearselected();
    if(item == nullptr)
        return;

    if(item->type() == 3000)
    {
        item->setSelected(true);
        QMenu menu;
        QAction *deleteAction = menu.addAction(QChinese("删除连线"));
        QAction *action = menu.exec(event->screenPos());
        if(action == deleteAction)
            _deleteItem(item);
    }
    else if (item->type()>=4000 && item->type()<5000)
    {
        item->setSelected(true);
        QMenu menu;
        QAction *addTerminal = menu.addAction(QChinese("复制输出"));
        QAction *action = menu.exec(event->screenPos());
        if(action == addTerminal)
        {
            //add output terminal
            Node *node = static_cast<Node *>(item);
            node->addOutputTerminal();
        }
    }
}

void MainScene::_deleteItem(QGraphicsItem *item)
{
    if (item->type() ==3000 )
    {
        //delete connection
        ConnectLine *line = static_cast<ConnectLine *>(item);
        if(line->_inTerminal)
        {
            line->_inTerminal->_connectLines = nullptr;
        }
        if(line->_outTerminal)
        {
            // 把line连接的outTerminal的图片清空
            //qDebug() << "line->_outTerminal->_node->type()=" << line->_outTerminal->_node->type();
            if (line->_outTerminal->_node->type() != 4201)
            {
                line->_outTerminal->_node->_inputImages.clear();
                line->_outTerminal->_node->_outputImage = cv::Mat();
                line->_outTerminal->_connectLines = nullptr;
            }
            else
            {
                //qDebug()<<"**********";
                //qDebug()<<static_cast<AddNode *>(line->_outTerminal->_node)->_inputMapImages.size();
                static_cast<AddNode *>(line->_outTerminal->_node)->_inputMapImages.remove(line->_outTerminal);
                // qDebug()<<static_cast<AddNode *>(line->_outTerminal->_node)->_inputMapImages.size();
                // qDebug()<<"**********";
                line->_outTerminal->_node->_outputImage = cv::Mat();
                line->_outTerminal->_connectLines = nullptr;
            }
        }
        removeItem(item);
        delete item;
    }
    else if (item->type()==4301) return;
    else if (item->type()>=4000&& item->type()<5000)
    {
        // 删除节点
        Node *node = static_cast<Node *>(item);
        for(Terminal *terminal : qAsConst(node->_inputTerminals))
        {
            ConnectLine* line = terminal->_connectLines;
            if(line)
            {
                if (line->_inTerminal)
                {
                    line->_inTerminal->_connectLines = nullptr;
                }
                if (line->_outTerminal)
                {
                    // 把line连接的outTerminal的图片清空
                    // qDebug() << "line->_outTerminal->_node->type()=" << line->_outTerminal->_node->type();
                    line->_outTerminal->_node->_inputImages.clear();
                    line->_outTerminal->_node->_outputImage = cv::Mat();
                    line->_outTerminal->_connectLines = nullptr;
                }
                _addedItems.removeOne(line);
                removeItem(line);
                delete line;
            }
        }
        for(Terminal *terminal : node->_outputTerminals)
        {
            ConnectLine* line = terminal->_connectLines;
            if(line)
            {
                if (line->_inTerminal)
                {
                    line->_inTerminal->_connectLines = nullptr;
                }
                if (line->_outTerminal)
                {
                    // 把line连接的outTerminal的图片清空
                    // qDebug() << "line->_outTerminal->_node->type()=" << line->_outTerminal->_node->type();
                    line->_outTerminal->_node->_inputImages.clear();
                    line->_outTerminal->_node->_outputImage = cv::Mat();
                    line->_outTerminal->_connectLines = nullptr;
                }
                _addedItems.removeOne(line);
                removeItem(line);
                delete line;
            }
        }
        removeItem(item);
        delete item;
        _nodes->removeOne(node);
    }
}
