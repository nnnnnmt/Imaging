#include "addnode.h"
#include <QDebug>
#include "mainscene.h"
#include "node.h"
#include "ui_addnodewidget.h"

AddNodeWidget::AddNodeWidget(QWidget *parent)
    : QWidget(parent),
    ui(new Ui::AddNodeWidget)
{
    ui->setupUi(this);
}

AddNodeWidget::~AddNodeWidget()
{
    delete ui;
}

AddNode::AddNode(MainScene *scene, QGraphicsItem *parent)
    : Node(scene,parent)
{
    _functionNodeType = FunctionNodeType::NotFunctionNode;
    _nodeType = NodeType::add_addNode;
    _name = "Add";
    qDebug() << "AddNode";
    _canvasHeight = _scene->_canvasHeight;
    _canvasWidth = _scene->_canvasWidth;
    _inputTerminals.append(new Terminal(_scene,TerminalType::inputTerminal,this));
    _inputTerminals.append(new Terminal(_scene,TerminalType::inputTerminal,this));
    _outputTerminals.append(new Terminal(_scene,TerminalType::outputTerminal,this));
    initializeBoundingRect();
    initializeTerminal();
    initProxyWidget();
}

AddNode::~AddNode()
{
    qDebug() << "~AddNode";
}

void AddNode::initProxyWidget()
{
    _widget = new AddNodeWidget();
    connect(_widget->ui->closeButton, &QPushButton::clicked, [this]() {
        this->_scene->_deleteItem((QGraphicsItem*)this);
    });

    _addInputButton = _widget->ui->addButton;

    connect(_addInputButton, &QPushButton::clicked, [this]() {
        _inputTerminals.append(new Terminal(_scene, TerminalType::inputTerminal, this));
        //_inputOrder.append(_inputTerminals.size());
        initializeTerminal();
    });
    _removeInputButton = _widget->ui->delButton;
    connect(_removeInputButton, &QPushButton::clicked, [this]() {
        if (_inputTerminals.size() > 2)
        {
            // 把连在这个terminal上的线删掉
            Terminal* terminaltoremove = _inputTerminals.last();
            ConnectLine *line = terminaltoremove->_connectLines;
            if(line!=nullptr)
            {
                if(line->_inTerminal)
                {
                    line->_inTerminal->_connectLines = nullptr;
                }
                if(line->_outTerminal)
                {
                    // 把line连接的outTerminal的图片清空
                    if (line->_outTerminal->_node->type() != 4201)
                    {
                        line->_outTerminal->_node->_inputImages.clear();
                        line->_outTerminal->_node->_outputImage = cv::Mat();
                        line->_outTerminal->_connectLines = nullptr;
                    }
                    else
                    {
                        static_cast<AddNode *>(line->_outTerminal->_node)->_inputMapImages.remove(line->_outTerminal);

                        line->_outTerminal->_node->_outputImage = cv::Mat();
                        line->_outTerminal->_connectLines = nullptr;
                    }
                }
            }
            _scene->removeItem(line);
            delete line;
            _scene->removeItem(_inputTerminals.last());
            _inputTerminals.removeLast();
            //_inputOrder.removeLast();
            initializeTerminal();
        }
    });

    // 创建一个checkbox 是否显示preview
    _checkBox = _widget->ui->checkBox;
    // 默认勾选
    _checkBox->setCheckState(Qt::Checked);
    // 显示outputImage
    _label = _widget->ui->label;

    if (_outputImage.empty())
    {
        QImage img(":/window/null.png");
        img = img.width() > img.height() ? img.scaledToWidth(200) : img.scaledToHeight(200);
        nodeSetPixmap(QPixmap::fromImage(img),_label);
    }
    else
    {
        QImage img = cvMat2QImage(_outputImage);
        img = img.width() > img.height() ? img.scaledToWidth(200) : img.scaledToHeight(200);
        nodeSetPixmap(QPixmap::fromImage(img),_label);
    }

    connect(_checkBox, &QCheckBox::stateChanged, [this](int state) {
        this->_label->setVisible(state == Qt::Checked);
    });

    _nodeSettings = _scene->addWidget(_widget);
    _nodeSettings->setParentItem(this);
    _nodeSettings->setPos(0, 0);
}

void AddNode::execute()
{
    // 把Input的image按顺序相加
    qDebug() << "AddNode::execute()";
    if (_inputMapImages.size() == 0)
    {
        QImage img(":/window/null.png");
        img = img.width() > img.height() ? img.scaledToWidth(200) : img.scaledToHeight(200);
        nodeSetPixmap(QPixmap::fromImage(img),_label);
        return;
    }
    else
    {
        _outputImage = cv::Mat::zeros(_canvasHeight, _canvasWidth, CV_8UC4);
        for (int i = _inputTerminals.size()-1; i >=0; i--)
        {
            //判断key中是否有_inputTerminals[i]
            if (_inputMapImages.find(_inputTerminals[i]) == _inputMapImages.end())
                continue;
            if (_inputMapImages[_inputTerminals[i]].rows>0 && _inputMapImages[_inputTerminals[i]].cols>0)
            {
                cv::Mat overlay = _inputMapImages[_inputTerminals[i]].clone();
                cv::Mat background = _outputImage.clone();
                overlay = channelUpgrade(overlay);

                if (overlay.channels() == 4)
                {
                    // 将四通道分离
                    std::vector<cv::Mat> overlayChannels(4);
                    overlay.convertTo(overlay, CV_32FC4);
                    cv::split(overlay, overlayChannels);

                    // 将alpha通道归一化到0-1之间
                    cv::Mat alphaFloat;
                    cv::Mat overlayAlpha = overlayChannels[3];
                    overlayAlpha.convertTo(alphaFloat, CV_32FC1, 1.0 / 255.0);

                    // 上层加权透明度
                    for (int i = 0; i < 3; ++i) {
                        cv::multiply(alphaFloat, overlayChannels[i], overlayChannels[i]);
                    }

                    // 将背景四通道分离
                    std::vector<cv::Mat> backgroundChannels(4);
                    background.convertTo(background, CV_32FC4);
                    cv::split(background, backgroundChannels);

                    // 背景加权透明度
                    for (int i = 0; i < 3; ++i) {
                        cv::multiply(cv::Scalar::all(1.0) - alphaFloat, backgroundChannels[i], backgroundChannels[i]);
                    }

                    // 还原四通道
                    cv::merge(overlayChannels, overlay);
                    cv::merge(backgroundChannels, background);

                    // 叠加并还原格式
                    cv::add(overlay, background, background);
                    background.convertTo(background, CV_8UC4);
                    _outputImage = background.clone();
                }
                else
                {
                    // 如果没有alpha通道，直接覆盖
                    overlay.copyTo(background);
                    _outputImage = background.clone();
                }
            }
        }
        if (_checkBox->isChecked())
        {
            QImage img = cvMat2QImage(_outputImage);
            img = img.width() > img.height() ? img.scaledToWidth(200) : img.scaledToHeight(200);
            nodeSetPixmap(QPixmap::fromImage(img),_label);
        }
    }
}

void AddNode::resizeWidget()
{
    if (_widget != nullptr) {
        _widget->setGeometry(0,0,300,_boundingRect.height());
        _widget->ui->frame->setGeometry(0, 0, 300, _boundingRect.height());
        _r = 50.0 / (double)_boundingRect.height();
    }
}

void AddNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{

    QString sty = "border-radius: 15px;"
                  "border: 3px solid %1;"
                  "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
                  "stop:0 rgb(255,204,150), "
                  "stop:%2 rgb(255,204,150), "
                  "stop:%3 rgb(255,250,240), "
                  "stop:1 rgb(255, 250, 240));";
    if (isSelected()) _widget->ui->frame->setStyleSheet(sty.arg("blue").arg(QString::number(_r)).arg(QString::number(_r+0.0001)));
    else _widget->ui->frame->setStyleSheet(sty.arg("black").arg(QString::number(_r)).arg(QString::number(_r+0.0001)));
    return;
}
