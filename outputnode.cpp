#include "outputnode.h"
#include <QDebug>
#include "mainscene.h"
#include "node.h"
#include "ui_outputnodewidget.h"

OutputNodeWidget::OutputNodeWidget(QWidget *parent)
    : QWidget(parent),
    ui(new Ui::OutputNodeWidget)
{
    ui->setupUi(this);
}

OutputNodeWidget::~OutputNodeWidget()
{
    qDebug() << "~OutputNodeWidget";
    delete ui;
}

OutputNode::OutputNode(MainScene *scene, QGraphicsItem *parent)
    : Node(scene,parent)
{
    _functionNodeType = FunctionNodeType::NotFunctionNode;
    _nodeType = NodeType::out_outputNode;
    _name = "Output";
    qDebug() << "OutputNode";
    _inputTerminals.append(new Terminal(_scene,TerminalType::inputTerminal,this));
    //_inputTerminals.append(new Terminal(_scene,TerminalType::inputTerminal,this));
    //_outputTerminal = new Terminal(_scene,TerminalType::outputTerminal,this);
    _widget = new OutputNodeWidget();
    _nodeSettings = _scene->addWidget((QWidget*)_widget);
    _nodeSettings->setParentItem(this);
    _nodeSettings->setPos(0, 0);
    initializeBoundingRect();
    initializeTerminal();
}

OutputNode::~OutputNode()
{
    qDebug() << "~OutputNode";
}

void OutputNode::initializeBoundingRect()
{
    _boundingRect = QRectF(0,0,300,100);
}

void OutputNode::execute()
{
    //qDebug() << "OutputNode::execute()";
    if (_inputImages.size() == 0)
    {
        //_outputImage = cv::imread("./pics/null.png", -1); // 替换成你的图片路径
        QFile file(":/window/null.png");
        cv::Mat m;
        if(file.open(QIODevice::ReadOnly))
        {
            qint64 sz = file.size();
            std::vector<uchar> buf(sz);
            file.read((char*)buf.data(), sz);
            m = cv::imdecode(buf, -1);
        }
        _outputImage = m.clone();
        //_outputImage = nullimg.clone();
        return;
    }
    else
    {
        _outputImage = _inputImages[0].clone();
    }
}
