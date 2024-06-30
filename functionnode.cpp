#include "functionnode.h"
#include "mainscene.h"
#include "node.h"
#include <QDebug>
#include <fstream>
#include <vector>
#include <opencv2/opencv.hpp>
#include "ui_functionnodewidget.h"
#include "ui_filternodewidget.h"

FunctionNodeWidget::FunctionNodeWidget(QWidget *parent)
    : QWidget(parent),
    ui(new Ui::FunctionNodeWidget)
{
    ui->setupUi(this);
}

FunctionNodeWidget::~FunctionNodeWidget()
{
    qDebug() << "~FunctionNodeWidget()";
    delete ui;
}

FilterNodeWidget::FilterNodeWidget(QWidget *parent)
    : QWidget(parent),
    ui(new Ui::FilterNodeWidget)
{
    ui->setupUi(this);
}

FilterNodeWidget::~FilterNodeWidget()
{
    delete ui;
}

FunctionNode::FunctionNode(MainScene *scene, QGraphicsItem *parent)
    : Node(scene,parent)
{
    _basicNodeType = BasicNodeType::functionNode;
}

FunctionNode::~FunctionNode()
{
    qDebug() << "~FunctionNode()";
}

ContrastNode::ContrastNode(MainScene *scene, QGraphicsItem *parent)
    : FunctionNode(scene,parent)
{
    _functionNodeType = FunctionNodeType::basicFunctionNode;
    _nodeType = NodeType::fun_contrastNode;
    _name = "Contrast";
    qDebug() << "ContrastNode";
    _inputTerminals.append(new Terminal(_scene,TerminalType::inputTerminal,this));
    _outputTerminals.append(new Terminal(_scene,TerminalType::outputTerminal,this));
    _canvasHeight = _scene->_canvasHeight;
    _canvasWidth = _scene->_canvasWidth;
    initializeBoundingRect();
    initializeTerminal();
    initProxyWidget();
}

ContrastNode::~ContrastNode()
{
    qDebug() << "~ContrastNode()";
}

void ContrastNode::initProxyWidget()
{
    _widget = new FunctionNodeWidget;
    _widget->ui->nameLabel->setText("对比度");
    connect(_widget->ui->closeButton, &QPushButton::clicked, [this]() {
        this->_scene->_deleteItem((QGraphicsItem*)this);
    });
    // 创建一个slider
    _slider = _widget->ui->funcSlider;
    _slider->setRange(0, 100);
    _slider->setValue(50);
    _slider->setTickInterval(10);
    _slider->setTickPosition(QSlider::TicksBelow);
    // 创建一个checkbox 是否显示preview
    _checkBox = _widget->ui->checkBox;
    // 默认勾选
    _checkBox->setCheckState(Qt::Checked);
    // 显示outputImage
    _label = _widget->ui->showLabel;
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
    // 创建一个proxywidget
    _nodeSettings = _scene->addWidget(_widget);
    _nodeSettings->setParentItem(this);
    _nodeSettings->setPos(0, 0);
    connect(_slider, &QSlider::valueChanged, this, &ContrastNode::execute);
}

void ContrastNode::execute()
{
    if (_inputImages.size() == 0)
    {

        QImage img(":/window/null.png");
        img = img.width() > img.height() ? img.scaledToWidth(200) : img.scaledToHeight(200);
        nodeSetPixmap(QPixmap::fromImage(img),_label); //_label->setPixmap(QPixmap::fromImage(img));
        return;
    }
    else
    {
        cv::Mat img = _inputImages[0].clone();
        cv::Mat dst;
        img = channelUpgrade(img);
        cv::Mat channels[4];
        cv::split(img, channels);

        int contrast = _slider->value();
        double alpha = contrast / 50.0;

        for(int i = 0; i < 3; i++)
        {
            channels[i].convertTo(channels[i], -1, alpha, 0);
        }

        cv::merge(channels, 4, dst);
        _outputImage = dst.clone();
        if (_checkBox->isChecked())
        {
            QImage img = cvMat2QImage(_outputImage);
            img = img.width() > img.height() ? img.scaledToWidth(200) : img.scaledToHeight(200);
            nodeSetPixmap(QPixmap::fromImage(img),_label); //_label->setPixmap(QPixmap::fromImage(img));
        }
    }
}

void ContrastNode::resizeWidget()
{
    if (_widget != nullptr) {
        _widget->setGeometry(0,0,300,_boundingRect.height());
        _widget->ui->frame->setGeometry(0, 0, 300, _boundingRect.height());
        _r = 50.0 / (double)_boundingRect.height();
    }
}

void ContrastNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    qDebug()<<"contrast paint";
    QString sty = "border-radius: 15px;"
                  "border: 3px solid %1;"
                  "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
                  "stop:0 rgb(243,218,108), "
                  "stop:%2 rgb(243,218,108), "
                  "stop:%3 rgb(255,253,220), "
                  "stop:1 rgb(255, 253, 220));";
    if (isSelected()) _widget->ui->frame->setStyleSheet(sty.arg("blue").arg(QString::number(_r)).arg(QString::number(_r+0.0001)));
    else _widget->ui->frame->setStyleSheet(sty.arg("black").arg(QString::number(_r)).arg(QString::number(_r+0.0001)));
    return;
}

BrightnessNode::BrightnessNode(MainScene *scene, QGraphicsItem *parent)
    : FunctionNode(scene, parent)
{
    _functionNodeType = FunctionNodeType::basicFunctionNode;
    _nodeType = NodeType::fun_brightnessNode;
    _name = "Brightness";
    qDebug() << "BrightnessNode";
    _inputTerminals.append(new Terminal(_scene, TerminalType::inputTerminal, this));
    _outputTerminals.append(new Terminal(_scene, TerminalType::outputTerminal, this));
    _canvasHeight = _scene->_canvasHeight;
    _canvasWidth = _scene->_canvasWidth;
    initializeBoundingRect();
    initializeTerminal();
    initProxyWidget();
}

BrightnessNode::~BrightnessNode()
{
    qDebug() << "~BrightnessNode()";
}

void BrightnessNode::initProxyWidget()
{
    _widget = new FunctionNodeWidget();
    _widget->ui->nameLabel->setText("亮度");
    _widget->ui->iconLabel->setStyleSheet("border: none;"
                                          "border-radius: 10px;"
                                          "image: url(:/nodewidget/BRIGHTNESS.png);"
                                          "background-color: rgb(255, 255, 255);");
    connect(_widget->ui->closeButton, &QPushButton::clicked, [this]() {
        this->_scene->_deleteItem((QGraphicsItem*)this);
    });
    // 创建一个slider
    _slider = _widget->ui->funcSlider;
    _slider->setRange(0, 100);
    _slider->setValue(50);
    _slider->setTickInterval(10);
    _slider->setTickPosition(QSlider::TicksBelow);
    // 创建一个checkbox 是否显示preview
    _checkBox = _widget->ui->checkBox;
    // 默认勾选
    _checkBox->setCheckState(Qt::Checked);
    // 显示outputImage
    _label = _widget->ui->showLabel;
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
    // 用checkbox控制是否显示
    // 创建一个proxywidget
    _nodeSettings = _scene->addWidget(_widget);
    _nodeSettings->setParentItem(this);
    _nodeSettings->setPos(0, 0);

    // Connect the slider value change signal to the execute function
    connect(_slider, &QSlider::valueChanged, this, &BrightnessNode::execute);
}

void BrightnessNode::execute()
{
    if (_inputImages.size() == 0)
    {

        QImage img(":/window/null.png");
        img = img.width() > img.height() ? img.scaledToWidth(200) : img.scaledToHeight(200);
        nodeSetPixmap(QPixmap::fromImage(img),_label);
        return;
    }
    else
    {
        cv::Mat img = _inputImages[0].clone();
        img = channelUpgrade(img);
        cv::Mat dst;
        cv::Mat channels[4];
        cv::split(img, channels);
        int brightness = _slider->value();
        double beta = brightness - 50; // Adjust the range from 0-100 to -50 to 50

        for(int i = 0; i < 3; i++)
        {
            channels[i].convertTo(channels[i], -1, 1, beta);
        }

        cv::merge(channels, 4, dst);
        _outputImage = dst.clone();
        if (_checkBox->isChecked())
        {
            QImage img = cvMat2QImage(_outputImage);
            img = img.width() > img.height() ? img.scaledToWidth(200) : img.scaledToHeight(200);
            nodeSetPixmap(QPixmap::fromImage(img),_label);
        }
    }
}

void BrightnessNode::resizeWidget()
{
    if (_widget != nullptr) {
        _widget->setGeometry(0,0,300,_boundingRect.height());
        _widget->ui->frame->setGeometry(0, 0, 300, _boundingRect.height());
        _r = 50.0 / (double)_boundingRect.height();
    }
}

void BrightnessNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QString sty = "border-radius: 15px;"
                  "border: 3px solid %1;"
                  "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
                  "stop:0 rgb(243,218,108), "
                  "stop:%2 rgb(243,218,108), "
                  "stop:%3 rgb(255,253,220), "
                  "stop:1 rgb(255, 253, 220));";
    if (isSelected()) _widget->ui->frame->setStyleSheet(sty.arg("blue").arg(QString::number(_r)).arg(QString::number(_r+0.0001)));
    else _widget->ui->frame->setStyleSheet(sty.arg("black").arg(QString::number(_r)).arg(QString::number(_r+0.0001)));
    return;
}

SaturationNode::SaturationNode(MainScene *scene, QGraphicsItem *parent)
    : FunctionNode(scene, parent)
{
    _functionNodeType = FunctionNodeType::basicFunctionNode;
    _nodeType = NodeType::fun_saturationNode;
    _name = "Saturation";
    qDebug() << "SaturationNode";
    _inputTerminals.append(new Terminal(_scene, TerminalType::inputTerminal, this));
    _outputTerminals.append(new Terminal(_scene, TerminalType::outputTerminal, this));
    _canvasHeight = _scene->_canvasHeight;
    _canvasWidth = _scene->_canvasWidth;
    initializeBoundingRect();
    initializeTerminal();
    initProxyWidget();
}

SaturationNode::~SaturationNode()
{
    qDebug() << "~SaturationNode()";
}

void SaturationNode::initProxyWidget()
{
    _widget = new FunctionNodeWidget();
    _widget->ui->nameLabel->setText("饱和度");
    _widget->ui->iconLabel->setStyleSheet("border: none;"
                                          "border-radius: 10px;"
                                          "image: url(:/nodewidget/SATURATION.png);"
                                          "background-color: rgb(255, 255, 255);");
    connect(_widget->ui->closeButton, &QPushButton::clicked, [this]() {
        this->_scene->_deleteItem((QGraphicsItem*)this);
    });
    // 创建一个slider
    _slider = _widget->ui->funcSlider;
    _slider->setRange(0, 100);
    _slider->setValue(50);
    _slider->setTickInterval(10);
    _slider->setTickPosition(QSlider::TicksBelow);

    // 创建一个checkbox 是否显示preview
    _checkBox = _widget->ui->checkBox;
    // 默认勾选
    _checkBox->setCheckState(Qt::Checked);
    // 显示outputImage
    _label = _widget->ui->showLabel;
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
    // 用checkbox控制是否显示
    // 创建一个proxywidget
    _nodeSettings = _scene->addWidget(_widget);
    _nodeSettings->setParentItem(this);
    _nodeSettings->setPos(0, 0);

    // Connect the slider value change signal to the execute function
    connect(_slider, &QSlider::valueChanged, this, &SaturationNode::execute);
}

void SaturationNode::execute()
{
    if (_inputImages.size() == 0)
    {

        QImage img(":/window/null.png");
        img = img.width() > img.height() ? img.scaledToWidth(200) : img.scaledToHeight(200);
        nodeSetPixmap(QPixmap::fromImage(img),_label);
        return;
    }
    else
    {
        cv::Mat img = _inputImages[0].clone();
        cv::Mat channels[4];
        cv::Mat alpha_channel;
        cv::split(img, channels);
        alpha_channel = channels[3].clone();
        cv::Mat cpy;
        cv::merge(channels, 3, cpy);
        cv::Mat hsv, dst;
        cv::cvtColor(cpy, hsv, cv::COLOR_BGR2HSV);

        int saturation = _slider->value();
        double alpha = saturation / 50.0;

        std::vector<cv::Mat> hsvChannels;
        cv::split(hsv, hsvChannels);

        hsvChannels[1].convertTo(hsvChannels[1], -1, alpha, 0);
        cv::merge(hsvChannels, hsv);

        cv::cvtColor(hsv, dst, cv::COLOR_HSV2BGR);
        cv::split(dst,channels);
        channels[3] = alpha_channel;
        cv::merge(channels, 4, dst);

        _outputImage = dst.clone();
        if (_checkBox->isChecked())
        {
            QImage img = cvMat2QImage(_outputImage);
            img = img.width() > img.height() ? img.scaledToWidth(200) : img.scaledToHeight(200);
            nodeSetPixmap(QPixmap::fromImage(img),_label);
        }
    }
}

void SaturationNode::resizeWidget()
{
    if (_widget != nullptr) {
        _widget->setGeometry(0,0,300,_boundingRect.height());
        _widget->ui->frame->setGeometry(0, 0, 300, _boundingRect.height());
        _r = 50.0 / (double)_boundingRect.height();
    }
}

void SaturationNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QString sty = "border-radius: 15px;"
                  "border: 3px solid %1;"
                  "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
                  "stop:0 rgb(243,218,108), "
                  "stop:%2 rgb(243,218,108), "
                  "stop:%3 rgb(255,253,220), "
                  "stop:1 rgb(255, 253, 220));";
    if (isSelected()) _widget->ui->frame->setStyleSheet(sty.arg("blue").arg(QString::number(_r)).arg(QString::number(_r+0.0001)));
    else _widget->ui->frame->setStyleSheet(sty.arg("black").arg(QString::number(_r)).arg(QString::number(_r+0.0001)));
    return;
}

ColorTemperatureNode::ColorTemperatureNode(MainScene *scene, QGraphicsItem *parent)
    : FunctionNode(scene, parent)
{
    _functionNodeType = FunctionNodeType::basicFunctionNode;
    _nodeType = NodeType::fun_colorTemperatureNode;
    _name = "Color Temperature";
    qDebug() << "ColorTemperatureNode";
    _inputTerminals.append(new Terminal(_scene, TerminalType::inputTerminal, this));
    _outputTerminals.append(new Terminal(_scene, TerminalType::outputTerminal, this));
    _canvasHeight = _scene->_canvasHeight;
    _canvasWidth = _scene->_canvasWidth;
    initializeBoundingRect();
    initializeTerminal();
    initProxyWidget();
}

ColorTemperatureNode::~ColorTemperatureNode()
{
    qDebug() << "~ColorTemperatureNode";
}

void ColorTemperatureNode::initProxyWidget()
{
    _widget = new FunctionNodeWidget();
    _widget->ui->nameLabel->setText("色温");
    _widget->ui->iconLabel->setStyleSheet("border: none;"
                                          "border-radius: 10px;"
                                          "image: url(:/nodewidget/TEMPERATURE.png);"
                                          "background-color: rgb(255, 255, 255);");
    connect(_widget->ui->closeButton, &QPushButton::clicked, [this]() {
        this->_scene->_deleteItem((QGraphicsItem*)this);
    });
    _slider = _widget->ui->funcSlider;
    _slider->setRange(0, 100);
    _slider->setValue(50);
    _slider->setTickInterval(10);
    _slider->setTickPosition(QSlider::TicksBelow);


    QString sliderStyle = R"(
                        QSlider::groove:horizontal {
                        border: 1px solid #999999;
                        height: 8px;
                        background: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0,
                        stop: 0 #ffff00, stop: 1 #0000ff);
                        margin: 2px 0;
                        }
                        QSlider::handle:horizontal {
                        background: #ffffff;
                        border: 1px solid #5c5c5c;
                        width: 18px;
                        margin: -2px 0;
                        border-radius: 3px;
                        }
                        )";
    _slider->setStyleSheet(sliderStyle);

    _checkBox = _widget->ui->checkBox;
    _checkBox->setCheckState(Qt::Checked);

    _label = _widget->ui->showLabel;
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

    connect(_slider, &QSlider::valueChanged, this, &ColorTemperatureNode::execute);
}

void ColorTemperatureNode::execute()
{
    if (_inputImages.size() == 0)
    {
        QImage img(":/window/null.png");
        img = img.width() > img.height() ? img.scaledToWidth(200) : img.scaledToHeight(200);
        nodeSetPixmap(QPixmap::fromImage(img),_label);
        return;
    }
    else
    {
        cv::Mat img = _inputImages[0].clone();
        cv::Mat channels[4];
        cv::split(img, channels);
        cv::Mat cpy;
        cv::merge(channels, 3, cpy);
        cv::Mat alpha_channel = channels[3].clone();

        int sliderValue = _slider->value();
        double kelvin = 2000 + (sliderValue / 100.0) * 8000;
        cv::Mat tempImg = adjustColorTemperature(cpy, kelvin);

        cv::split(tempImg, channels);
        channels[3] = alpha_channel;
        cv::merge(channels, 4, _outputImage);

        if (_checkBox->isChecked()) {
            QImage img = cvMat2QImage(_outputImage);
            img = img.width() > img.height() ? img.scaledToWidth(200) : img.scaledToHeight(200);
            nodeSetPixmap(QPixmap::fromImage(img),_label);
        }
    }
}

cv::Mat ColorTemperatureNode::adjustColorTemperature(const cv::Mat& img, double kelvin)
{
    cv::Mat result = img.clone();
    cv::Mat channels[3];
    cv::split(result, channels);

    double temperature = kelvin / 100;
    double red, green, blue;

    if (temperature <= 66) {
        red = 255;
        green = temperature;
        green = 99.4708025861 * log(green) - 161.1195681661;

        if (temperature <= 19) {
            blue = 0;
        } else {
            blue = temperature - 10;
            blue = 138.5177312231 * log(blue) - 305.0447927307;
        }
    } else {
        red = temperature - 60;
        red = 329.698727446 * pow(red, -0.1332047592);

        green = temperature - 60;
        green = 288.1221695283 * pow(green, -0.0755148492);

        blue = 255;
    }

    red = std::clamp(red, 0.0, 255.0);
    green = std::clamp(green, 0.0, 255.0);
    blue = std::clamp(blue, 0.0, 255.0);

    double r_gain = red / 255.0;
    double g_gain = green / 255.0;
    double b_gain = blue / 255.0;

    channels[2] *= r_gain; // Red channel
    channels[1] *= g_gain; // Green channel
    channels[0] *= b_gain; // Blue channel

    cv::merge(channels, 3, result);

    return result;
}

void ColorTemperatureNode::resizeWidget()
{
    if (_widget != nullptr) {
        _widget->setGeometry(0,0,300,_boundingRect.height());
        _widget->ui->frame->setGeometry(0, 0, 300, _boundingRect.height());
        _r = 50.0 / (double)_boundingRect.height();
    }
}

void ColorTemperatureNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QString sty = "border-radius: 15px;"
                  "border: 3px solid %1;"
                  "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
                  "stop:0 rgb(243,218,108), "
                  "stop:%2 rgb(243,218,108), "
                  "stop:%3 rgb(255,253,220), "
                  "stop:1 rgb(255, 253, 220));";
    if (isSelected()) _widget->ui->frame->setStyleSheet(sty.arg("blue").arg(QString::number(_r)).arg(QString::number(_r+0.0001)));
    else _widget->ui->frame->setStyleSheet(sty.arg("black").arg(QString::number(_r)).arg(QString::number(_r+0.0001)));
    return;
}

HueNode::HueNode(MainScene *scene, QGraphicsItem *parent)
    : FunctionNode(scene, parent)
{
    _functionNodeType = FunctionNodeType::basicFunctionNode;
    _nodeType = NodeType::fun_hueNode;
    _name = "Hue";
    qDebug() << "HueNode";
    _inputTerminals.append(new Terminal(_scene, TerminalType::inputTerminal, this));
    _outputTerminals.append(new Terminal(_scene, TerminalType::outputTerminal, this));
    _canvasHeight = _scene->_canvasHeight;
    _canvasWidth = _scene->_canvasWidth;
    initializeBoundingRect();
    initializeTerminal();
    initProxyWidget();
}

HueNode::~HueNode()
{
    qDebug() << "~HueNode";
}

void HueNode::initProxyWidget()
{
    _widget = new FunctionNodeWidget();
    _widget->ui->nameLabel->setText("色调");
    _widget->ui->iconLabel->setStyleSheet("border-radius: 10px;"
                                          "border: none;"
                                          "background-color:white;"
                                          "image: url(:/nodewidget/HUE.png);");
    connect(_widget->ui->closeButton, &QPushButton::clicked, [this]() {
        this->_scene->_deleteItem((QGraphicsItem*)this);
    });
    _slider = _widget->ui->funcSlider;
    _slider->setRange(-49, 49);
    _slider->setValue(0);
    _slider->setTickInterval(10);
    _slider->setTickPosition(QSlider::TicksBelow);

    QString sliderStyle = R"(
        QSlider::groove:horizontal {
        border: 1px solid #999999;
        height: 8px;
        background: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0,
        stop: 0 #00ff00, stop: 1 #ff0000);
        margin: 2px 0;
        }
        QSlider::handle:horizontal {
        background: #ffffff;
        border: 1px solid #5c5c5c;
        width: 18px;
        margin: -2px 0;
        border-radius: 3px;
        }
        )";
    _slider->setStyleSheet(sliderStyle);

    _checkBox = _widget->ui->checkBox;
    _checkBox->setCheckState(Qt::Checked);

    _label = _widget->ui->showLabel;
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

    connect(_slider, &QSlider::valueChanged, this, &HueNode::execute);
}

void HueNode::execute()
{
    if (_inputImages.size() == 0)
    {
        QImage img(":/window/null.png");
        img = img.width() > img.height() ? img.scaledToWidth(200) : img.scaledToHeight(200);
        nodeSetPixmap(QPixmap::fromImage(img),_label);
        return;
    }
    else
    {
        cv::Mat img = _inputImages[0].clone();
        double hue = _slider->value();
        _outputImage = adjustHue(img, hue).clone();
        if (_checkBox->isChecked())
        {
            QImage img = cvMat2QImage(_outputImage);
            img = img.width() > img.height() ? img.scaledToWidth(200) : img.scaledToHeight(200);
            nodeSetPixmap(QPixmap::fromImage(img),_label);
        }
    }
}

void HueNode::resizeWidget()
{
    if (_widget != nullptr) {
        _widget->setGeometry(0,0,300,_boundingRect.height());
        _widget->ui->frame->setGeometry(0, 0, 300, _boundingRect.height());
        _r = 50.0 / (double)_boundingRect.height();
    }
}

void HueNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QString sty = "border-radius: 15px;"
                  "border: 3px solid %1;"
                  "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
                  "stop:0 rgb(243,218,108), "
                  "stop:%2 rgb(243,218,108), "
                  "stop:%3 rgb(255,253,220), "
                  "stop:1 rgb(255, 253, 220));";
    if (isSelected()) _widget->ui->frame->setStyleSheet(sty.arg("blue").arg(QString::number(_r)).arg(QString::number(_r+0.0001)));
    else _widget->ui->frame->setStyleSheet(sty.arg("black").arg(QString::number(_r)).arg(QString::number(_r+0.0001)));
    return;
}

cv::Mat HueNode::adjustHue(const cv::Mat &img, double hue)
{
    cv::Mat hsv, result;
    cv::cvtColor(img, hsv, cv::COLOR_BGR2HSV);

    for (int y = 0; y < hsv.rows; y++)
    {
        for (int x = 0; x < hsv.cols; x++)
        {
            int h = hsv.at<cv::Vec3b>(y, x)[0];
            h = (h + static_cast<int>(hue)) % 180;
            hsv.at<cv::Vec3b>(y, x)[0] = h;
        }
    }

    cv::cvtColor(hsv, result, cv::COLOR_HSV2BGR);
    return result;
}


FilterNode::FilterNode(MainScene *scene, QGraphicsItem *parent)
    : FunctionNode(scene, parent)
{
    _functionNodeType = FunctionNodeType::basicFunctionNode;
    _nodeType = NodeType::fun_filterNode;
    _name = "Filter";
    qDebug() << "FilterNode";
    _inputTerminals.append(new Terminal(_scene, TerminalType::inputTerminal, this));
    _outputTerminals.append(new Terminal(_scene, TerminalType::outputTerminal, this));
    _canvasHeight = _scene->_canvasHeight;
    _canvasWidth = _scene->_canvasWidth;
    initializeBoundingRect();
    initializeTerminal();
    initProxyWidget();
}

FilterNode::~FilterNode()
{
    qDebug() << "~FilterNode";
}

void FilterNode::initProxyWidget()
{
    _widget = new FilterNodeWidget();
    QPushButton *button = _widget->ui->fileButton;
    connect(button, &QPushButton::clicked, this, &FilterNode::loadCubeFile);

    connect(_widget->ui->closeButton, &QPushButton::clicked, [this]() {
        this->_scene->_deleteItem((QGraphicsItem*)this);
    });
    _slider = _widget->ui->funcSlider;
    _slider->setRange(0, 100);
    _slider->setValue(50);
    _slider->setTickInterval(10);
    _slider->setTickPosition(QSlider::TicksBelow);

    _checkBox = _widget->ui->checkBox;
    _checkBox->setCheckState(Qt::Checked);

    _label = _widget->ui->showLabel;
    QImage img(":/window/null.png");
    img = img.width() > img.height() ? img.scaledToWidth(200) : img.scaledToHeight(200);
    nodeSetPixmap(QPixmap::fromImage(img),_label);

    _nodeSettings = _scene->addWidget(_widget);
    _nodeSettings->setParentItem(this);
    _nodeSettings->setPos(0, 0);

    connect(_checkBox, &QCheckBox::stateChanged, [this](int state) {
        this->_label->setVisible(state == Qt::Checked);
    });

    connect(_slider, &QSlider::valueChanged, this, &FilterNode::execute);
}

void FilterNode::loadCubeFile()
{
    QString fileName = QFileDialog::getOpenFileName(nullptr, "Open LUT File", "", "LUT Files (*.cube)");
    if (!fileName.isEmpty())
    {
        loadCube(fileName.toStdString());
        //execute();
    }
}

void FilterNode::loadCube(const std::string &fileName)
{
    std::ifstream file(fileName);
    if (!file.is_open()) {
        qDebug() << "Failed to open file: ";
        // 弹出警告框
        QMessageBox::warning(nullptr, "警告", "无法打开文件: " + QString::fromStdString(fileName), QMessageBox::Yes);
        return;
    }

    std::string line;
    int lutSize = 0;
    std::vector<cv::Vec3f> lutData;

    while (std::getline(file, line))
    {
        if (line.empty() || line[0] == '#') {
            continue;
        }
        std::istringstream iss(line);
        std::string token;
        iss >> token;
        if (token == "LUT_3D_SIZE")
        {
            iss >> lutSize;
            lutData.reserve(lutSize * lutSize * lutSize);
        }
        else if (token == "DOMAIN_MIN" || token == "DOMAIN_MAX")
        {
            // Skip domain min/max as we're assuming normalized LUT for simplicity
        }
        else
        {
            float r, g, b;
            iss.clear();
            iss.str(line);
            iss >> r >> g >> b;
            lutData.emplace_back(r, g, b);
        }
    }

    if (int(lutData.size()) != lutSize * lutSize * lutSize)
    {
        qDebug() << "Incorrect LUT data size: " << lutData.size() << " expected: " << (lutSize * lutSize * lutSize) ;
        // 弹出警告框
        QMessageBox::warning(nullptr, "警告", "不受支持的LUT数据格式", QMessageBox::Yes);
        return;
    }

    _lut.size = lutSize;
    _lut.data = lutData;
}

void FilterNode::applyLUT(const LUT3D& lut, cv::Mat& image,float intensity)
{
    /*
    // 检查输入图像的通道数和深度是否满足要求

    if (img.channels() != 1 || img.depth() != CV_8U)
    {

        // 如果不满足要求，将输入图像转换为单通道、8位无符号深度
        cv::Mat grayImg;
        cv::cvtColor(img, grayImg, cv::COLOR_BGR2GRAY);

        // 应用查找表到转换后的图像
        cv::Mat result;
        cv::LUT(grayImg, lut, result);

        return result;
    }
    else
    {
        // 输入图像已经是单通道、8位无符号深度，直接应用查找表
        cv::Mat result;
        cv::LUT(img, lut, result);
        return result;
    }*/
    CV_Assert(image.type() == CV_8UC3 || image.type() == CV_32FC3);
    cv::Mat lutmask = image.clone();
    lutmask.forEach<cv::Vec3b>([&](cv::Vec3b &pixel, const int * position) -> void {
        cv::Vec3f color(pixel[0] / 255.0f, pixel[1] / 255.0f, pixel[2] / 255.0f);
        cv::Vec3f newColor = applyLUTColor(_lut, color);
        // 根据强度混合原始颜色和新的颜色
        //cv::Vec3f blendedColor = color * (1.0f - intensity) + newColor * intensity;
        pixel[0] = cv::saturate_cast<uchar>(newColor[0] * 255.0f);
        pixel[1] = cv::saturate_cast<uchar>(newColor[1] * 255.0f);
        pixel[2] = cv::saturate_cast<uchar>(newColor[2] * 255.0f);
    });
    cv::addWeighted(image,(1.0f-intensity),lutmask,intensity,0,image);
}

cv::Vec3f FilterNode::applyLUTColor(const LUT3D &lut, const cv::Vec3f &color)
{
    float scale = static_cast<float>(lut.size - 1);
    int r = static_cast<int>(color[0] * scale);
    int g = static_cast<int>(color[1] * scale);
    int b = static_cast<int>(color[2] * scale);

    r = std::min(r, lut.size - 1);
    g = std::min(g, lut.size - 1);
    b = std::min(b, lut.size - 1);

    int index = r + g * lut.size + b * lut.size * lut.size;
    return lut.data[index];
}

void FilterNode::execute()
{
    if (_inputImages.size() == 0)
    {
        QImage img(":/window/null.png");
        img = img.width() > img.height() ? img.scaledToWidth(200) : img.scaledToHeight(200);
        nodeSetPixmap(QPixmap::fromImage(img),_label);
        return;
    }
    else
    {
        _outputImage = _inputImages[0].clone();
        cv::Mat channels[4];
        cv::Mat alpha_channel;
        cv::split(_outputImage, channels);
        alpha_channel = channels[3].clone();
        cv::merge(channels, 3, _outputImage);
        if(_lut.size)
        {

            float intensity = _slider->value() / 100.0f;
            // 使用给定强度应用LUT
            applyLUT(_lut, _outputImage, intensity);
            cv::split(_outputImage, channels);
            channels[3] = alpha_channel;
            cv::merge(channels, 4, _outputImage);
        }
        else
        {
            cv::split(_outputImage, channels);
            channels[3] = alpha_channel;
            cv::merge(channels, 4, _outputImage);
        }
        if (_checkBox->isChecked())
        {
            updateLabelWithImage(_outputImage);
        }
    }
}

void FilterNode::updateLabelWithImage(const cv::Mat &image)
{
    QImage img = cvMat2QImage(image);
    img = img.width() > img.height() ? img.scaledToWidth(200) : img.scaledToHeight(200);
    nodeSetPixmap(QPixmap::fromImage(img),_label);
}

void FilterNode::resizeWidget()
{
    if (_widget != nullptr) {
        _widget->setGeometry(0,0,300,_boundingRect.height());
        _widget->ui->frame->setGeometry(0, 0, 300, _boundingRect.height());
        _r = 50.0 / (double)_boundingRect.height();
    }
}

void FilterNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QString sty = "border-radius: 15px;"
                  "border: 3px solid %1;"
                  "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
                  "stop:0 rgb(243,218,108), "
                  "stop:%2 rgb(243,218,108), "
                  "stop:%3 rgb(255,253,220), "
                  "stop:1 rgb(255, 253, 220));";
    if (isSelected()) _widget->ui->frame->setStyleSheet(sty.arg("blue").arg(QString::number(_r)).arg(QString::number(_r+0.0001)));
    else _widget->ui->frame->setStyleSheet(sty.arg("black").arg(QString::number(_r)).arg(QString::number(_r+0.0001)));
    return;
}
