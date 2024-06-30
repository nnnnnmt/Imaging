#include "inputnode.h"
#include "editnode.h"
#include "mainScene.h"
#include "node.h"
#include "ui_editnodewidget.h"
#include "ui_editwindow.h"
#include "ui_editcropwindow.h"
#include "ui_editcanvaswindow.h"

EditNodeWidget::EditNodeWidget(QWidget *parent)
    : QWidget(parent),
    ui(new Ui::EditNodeWidget)
{
    ui->setupUi(this);
}

EditNodeWidget::~EditNodeWidget()
{
    qDebug() << "~EditNodeWidget";
    delete ui;
}

EditNode::EditNode(MainScene *scene, QGraphicsItem *parent)
    : Node(scene,parent)
{
    _functionNodeType = FunctionNodeType::NotFunctionNode;
    _nodeType = NodeType::fun_editNode;
    _name = "Edit";
    qDebug() << "EditNode";
    //_inputTerminals.append(new Terminal(_scene,TerminalType::inputTerminal,this));
    _inputTerminals.append(new Terminal(_scene,TerminalType::inputTerminal,this));
    _outputTerminals.append(new Terminal(_scene,TerminalType::outputTerminal,this));
    _canvasHeight = _scene->_canvasHeight;
    _canvasWidth = _scene->_canvasWidth;
    initializeBoundingRect();
    initializeTerminal();
    initProxyWidget();
}

void EditNode::initProxyWidget()
{
    _widget = new EditNodeWidget();
    connect(_widget->ui->closeButton, &QPushButton::clicked, [this]() {
        this->_scene->_deleteItem((QGraphicsItem*)this);
    });
    // 创建一个文件选择按钮
    _xSlider = _widget->ui->xSlider;
    _xSlider->setRange(5 - _canvasWidth, _canvasWidth - 5);
    _xSlider->setValue(0);
    _ySlider = _widget->ui->ySlider;
    _ySlider->setRange(5 - _canvasHeight, _canvasHeight - 5);
    _ySlider->setValue(0);

    // 预处理按钮
    _editButton = _widget->ui->editButton;
    _editWinBuilt = false;
    _editWinOpen = false;
    connect(_editButton, &QPushButton::clicked, [this]() {
        // 创建和显示编辑窗口
        showEditWindow();
    });

    _xSlider->setEnabled(false);
    connect(_xSlider, &QSlider::valueChanged, [this](int value) {
        setImageProperties(value,-1,-1);
        updateImage();
    });

    _ySlider->setEnabled(false);
    connect(_ySlider, &QSlider::valueChanged, [this](int value) {
        setImageProperties(-1, value, -1);
        updateImage();
    });

    // 创建一个checkbox 是否显示preview
    _checkBox = _widget->ui->checkBox;
    _checkBox->setCheckState(Qt::Checked);
    // 显示outputImage
    _label = _widget->ui->label;
    if (_inputImages.size() == 0)
    {
        QImage img(":/window/null.png");
        img = img.width() > img.height() ? img.scaledToWidth(200) : img.scaledToHeight(200);
        nodeSetPixmap(QPixmap::fromImage(img),_label);
    }
    else
    {
        QImage img = cvMat2QImage(_inputImages[0]);
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
}

void EditNode::showEditWindow()
{
    if (_inputImages.size() == 0) return;

    _xSlider->setEnabled(false);
    _ySlider->setEnabled(false);
    _editButton->setEnabled(false);

    if (!_editWinBuilt) {
        _editWindow = new EditWindow(this, _inputImages[0]);
        _editWinBuilt = true;
    }
    _editWindow->_canvasWindow->_canvasRegion->_imageRegion->inputSetX(_x);
    _editWindow->_canvasWindow->_canvasRegion->_imageRegion->inputSetY(_y);
    _editWindow->show();
    _editWinOpen = true;
}

// 上游传递的image变化时调用：当大小或形状变化时重置参数，否则不改变参数
void EditNode::inputImageUpdate()
{
    //qDebug()<<"EditNode::inputImageChanged";
    if (_inputImages[0].cols != _lastInputImage.cols || _inputImages[0].rows != _lastInputImage.rows) {
        _cropRect = cv::Rect(0, 0, _inputImages[0].cols, _inputImages[0].rows);
        _x = 0;
        _y = 0;
        _scale = 1.00;
        _xSlider->setValue(0);
        _ySlider->setValue(0);
        _xSlider->setRange(5 - _inputImages[0].cols, _inputImages[0].cols - 5);
        _ySlider->setRange(5 - _inputImages[0].rows, _inputImages[0].rows - 5);
        _angle = 0;
        if (_editWinBuilt) {
            _editWindow->_originalImage = _inputImages[0];

            _editWindow->_canvasWindow->_xFillin->setValue(0);
            _editWindow->_canvasWindow->_yFillin->setValue(0);
            _editWindow->_canvasWindow->_scaleSlider->setValue(100);
            _editWindow->_canvasWindow->_angleSlider->setValue(0);

            double cw = _editWindow->_canvasWindow->_canvasRegion->_cW;
            double ch = _editWindow->_canvasWindow->_canvasRegion->_cH;
            double ratioW = 750.0 / cw;
            double ratioH = 400.0 / ch;
            if (ratioW > ratioH) {
                _editWindow->_canvasWindow->_canvasRegion->_showRatio = ratioH;
                _editWindow->_canvasWindow->_canvasRegion->setGeometry(0, 0, 750.0 * ratioH, 400);
                _editWindow->_canvasWindow->_canvasRegion->_imageRegion->loadImage(_inputImages[0], ratioH);
            }
            else {
                _editWindow->_canvasWindow->_canvasRegion->_showRatio = ratioW;
                _editWindow->_canvasWindow->_canvasRegion->setGeometry(0, 0, 750, 400.0 * ratioW);
                _editWindow->_canvasWindow->_canvasRegion->_imageRegion->loadImage(_inputImages[0], ratioW);
            }
            _editWindow->_canvasWindow->_canvasRegion->_imageRegion->_originalPosition = QPoint(0, 0);
            _editWindow->_canvasWindow->_canvasRegion->_imageRegion->_shownPosition = QPoint(0, 0);
            _editWindow->_canvasWindow->_canvasRegion->_imageRegion->_angle = 0;
            _editWindow->_canvasWindow->_canvasRegion->_imageRegion->_scale = 1.00;

            _editWindow->_cropWindow->_cropRegion->loadMat(_inputImages[0]);
        }
    }
    else {
        if (_editWinBuilt) {
            _editWindow->_originalImage = _inputImages[0];
            _editWindow->_canvasWindow->_canvasRegion->_imageRegion->loadImage(_inputImages[0](_cropRect), -1);
            _editWindow->_cropWindow->_cropRegion->loadMat(_inputImages[0]);
        }
    }
}

void EditNode::setImageProperties(int x, int y, double s, int a)
{
    if (x != -1) _x = x;
    if (y != -1) _y = y;
    if (s != -1) _scale = s;
    if (a != -1) _angle = a;
}

void EditNode::updateImage()
{
    //qDebug()<<"update called";
    if (_inputImages[0].channels()==3) {
        _outputImage = cv::Mat::zeros(_canvasHeight, _canvasWidth, CV_8UC3);
    }
    else {
        _outputImage = cv::Mat::zeros(_canvasHeight, _canvasWidth, CV_8UC4);
    }

    // 裁剪
    cv::Mat croppedImage = _inputImages[0](_cropRect).clone();

    // 缩放
    // qDebug()<<"scale "<<_scale;
    // qDebug()<<"_x "<<_x<<" _y "<<_y;
    cv::Size scaledSize((double)croppedImage.cols * _scale, (double)croppedImage.rows * _scale);
    cv::Mat scaledImage;
    cv::resize(croppedImage, scaledImage, scaledSize);

    // 旋转
    cv::Mat shownImage = rotateImage(scaledImage, _angle);
    //qDebug()<<"shownImage: "<<shownImage.cols<<" "<<shownImage.rows;

    cv::Rect dstROI(_x, _y, shownImage.cols, shownImage.rows);
    cv::Rect srcROI(0, 0, dstROI.width, dstROI.height);

    if (dstROI.width + dstROI.x > _canvasWidth) {
        dstROI.width = _canvasWidth - dstROI.x;
    }
    if (dstROI.height + dstROI.y > _canvasHeight) {
        dstROI.height = _canvasHeight - dstROI.y;
    }
    if (_x < 0) {
        dstROI.x = 0;
        srcROI.x = -_x;
        dstROI.width += _x;
    }
    if (_y < 0) {
        dstROI.y = 0;
        srcROI.y = -_y;
        dstROI.height += _y;
    }

    srcROI.width = dstROI.width;
    srcROI.height = dstROI.height;

    // qDebug()<<"srcROI: "<<srcROI.x<<" "<<srcROI.y<<" "<<srcROI.width<<" "<<srcROI.height;
    // qDebug()<<"dstROI: "<<dstROI.x<<" "<<dstROI.y<<" "<<dstROI.width<<" "<<dstROI.height;

    shownImage(srcROI).copyTo(_outputImage(dstROI));

    QImage img = cvMat2QImage(_outputImage);
    img = img.width() > img.height() ? img.scaledToWidth(200) : img.scaledToHeight(200);

    nodeSetPixmap(QPixmap::fromImage(img),_label);
}

void EditNode::execute()
{
    if (_editWinOpen) {
        return;
    }
    if (_inputImages.size() == 0)
    {
        QImage img(":/window/null.png");
        img = img.width() > img.height() ? img.scaledToWidth(200) : img.scaledToHeight(200);
        nodeSetPixmap(QPixmap::fromImage(img),_label);
        _xSlider->setEnabled(false);
        _ySlider->setEnabled(false);
        _editButton->setEnabled(false);
        return;
    }
    _xSlider->setEnabled(true);
    _ySlider->setEnabled(true);
    _editButton->setEnabled(true);

    inputImageUpdate();
    _lastInputImage = _inputImages[0];
    //qDebug() << "EditNode::execute";
    updateImage();
}

void EditNode::resizeWidget()
{
    if (_widget != nullptr) {
        _widget->setGeometry(0,0,300,_boundingRect.height());
        _widget->ui->frame->setGeometry(0, 0, 300, _boundingRect.height());
        _r = 50.0 / (double)_boundingRect.height();
    }
}

void EditNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
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

// EditWindow
EditWindow::EditWindow(Node *superior, cv::Mat image)
    : QWidget(nullptr), ui(new Ui::EditWindow)
{
    ui->setupUi(this);
    setWindowTitle(QChinese("基础调整"));
    setFixedSize(800, 600);
    _superior = (EditNode*)superior;

    QPushButton *cropButton = ui->cropButton;
    QPushButton *canvasButton = ui->canvasButton;
    QPushButton *acceptButton = ui->acceptButton;

    _originalImage = image;
    _cropWindow = new EditCropWindow(superior, _originalImage);
    _cropWindow->_editWin = this;
    _canvasWindow = new EditCanvasWindow(superior, _originalImage);
    _canvasWindow->_editWin = this;

    QStackedWidget* editStackWindow = ui->stackedWidget;
    editStackWindow->addWidget(_cropWindow);
    editStackWindow->addWidget(_canvasWindow);
    editStackWindow->setCurrentIndex(2);

    connect(cropButton, &QPushButton::clicked, [editStackWindow]() {
        editStackWindow->setCurrentIndex(2);
    });
    connect(canvasButton, &QPushButton::clicked, [editStackWindow]() {
        editStackWindow->setCurrentIndex(3);
    });
    connect(acceptButton, &QPushButton::clicked, [this]() {
        // 传递 _croppedRect
        if (this->_cropWindow->_cropRegion->_croppedRect.width < 5 || this->_cropWindow->_cropRegion->_croppedRect.height < 5) {
            this->_superior->_cropRect = cv::Rect(0, 0, this->_originalImage.cols, this->_originalImage.rows);
        }
        else {
        this->_superior->_cropRect = this->_cropWindow->_cropRegion->_croppedRect;
        }

        // 传递 _imageGeometry
        int x = this->_canvasWindow->_canvasRegion->_imageRegion->_shownPosition.x();
        //qDebug()<<"_shownPositionX: "<<x;
        int y = this->_canvasWindow->_canvasRegion->_imageRegion->_shownPosition.y();
        //qDebug()<<"_shownPositionY: "<<y;
        double ratio = this->_canvasWindow->_canvasRegion->_showRatio;
        double scale = this->_canvasWindow->_canvasRegion->_imageRegion->_scale;
        int angle = this->_canvasWindow->_canvasRegion->_imageRegion->_angle;
        //qDebug()<<"inf"<<x<<" "<<y<<" "<<ratio;
        this->_superior->setImageProperties(
            qCeil((double)x / ratio), qCeil((double)y / ratio), scale, angle
            );
        this->_superior->updateImage();

        this->_superior->_xSlider->setValue((double)x/ratio);
        this->_superior->_ySlider->setValue((double)y/ratio);

        double sn = qSin((double)angle * M_PI / 180.0);
        double cs = qCos((double)angle * M_PI / 180.0);
        if (sn < 0) {
            sn = -sn;
        }
        if (cs < 0) {
            cs = -cs;
        }

        int coreWidth = this->_superior->_cropRect.width;
        int coreHeight = this->_superior->_cropRect.height;
        int shownWidth = qCeil(qCeil(((double)coreWidth * cs + (double)coreHeight * sn)) * scale);
        int shownHeight = qCeil(qCeil(((double)coreWidth * sn + (double)coreHeight * cs)) * scale);
        //qDebug()<<"range x: "<<5 - shownWidth<<" "<<this->_superior->_canvasWidth - 5;
        this->_superior->_xSlider->setRange(
            5 - shownWidth,
            this->_superior->_canvasWidth - 5
            );

        //qDebug()<<"range y: "<<5 - shownHeight<<" "<<this->_superior->_canvasHeight - 5;
        this->_superior->_ySlider->setRange(
            5 - shownHeight,
            this->_superior->_canvasHeight - 5
            );
        this->close();
    });

    show();
}

EditWindow::~EditWindow()
{
    qDebug()<<"EditWindow::~EditWindow";
}

void EditWindow::closeEvent(QCloseEvent *e)
{
    QWidget::closeEvent(e);
    _superior->_xSlider->setEnabled(true);
    _superior->_ySlider->setEnabled(true);
    _superior->_editButton->setEnabled(true);
    _superior->_editWinOpen = false;
}

// EditCanvasWindow
EditCanvasWindow::EditCanvasWindow(Node *superior, cv::Mat image)
    : QWidget(nullptr), ui(new Ui::EditCanvasWindow)
{
    ui->setupUi(this);
    _superior = (EditNode*)superior;

    setWindowTitle(QChinese("画布"));
    setFixedSize(800, 550);

    QVBoxLayout *canvasRegionLayout = ui->canvasRegionLayout;
    canvasRegionLayout->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

    _canvasRegion = new EditCanvasRegion(this);
    _canvasRegion->loadImage(image);
    canvasRegionLayout->addWidget(_canvasRegion);

    _xFillin = ui->xSpinBox;
    _xFillin->setRange(-image.cols * _canvasRegion->_showRatio + 5, _W - 5);
    _xFillin->setSingleStep(1);
    _xFillin->setValue(0);
    connect(_xFillin, QOverload<int>::of(&QSpinBox::valueChanged), [this](int newValue) {
        _canvasRegion->_imageRegion->resetPos(newValue, -1);
    });

    _yFillin = ui->ySpinBox;
    _yFillin->setRange(-image.rows * _canvasRegion->_showRatio + 5, _H - 5);
    _yFillin->setSingleStep(1);
    _yFillin->setValue(0);
    connect(_yFillin, QOverload<int>::of(&QSpinBox::valueChanged), [this](int newValue) {
        _canvasRegion->_imageRegion->resetPos(-1, newValue);
    });

    _scaleSlider = ui->scaleSlider;
    _scaleSlider->setRange(1, 200);
    _scaleSlider->setSingleStep(1);
    _scaleSlider->setValue(100);
    connect(_scaleSlider, &QSlider::valueChanged, [this](int newValue) {
        _canvasRegion->_imageRegion->resetSize((double)newValue / (double)100, -1);
    });

    _angleSlider = ui->rotateSlider;
    _angleSlider->setRange(0, 360);
    _angleSlider->setValue(0);
    connect(_angleSlider, &QSlider::valueChanged, [this](int newValue) {
        _canvasRegion->_imageRegion->resetSize(-1, newValue);
    });

    show();
}

EditCanvasWindow::~EditCanvasWindow()
{
    qDebug()<<"EditCanvasWindow::~EditCanvasWindow";
    delete ui;
}

QSize EditCanvasWindow::sizeHint() const
{
    return QSize(1000, 750);
}

// EditCanvasRegion & EditImageRegion
EditCanvasRegion::EditCanvasRegion(EditCanvasWindow *parent) : QWidget((QWidget*)parent)
{
    _canvasWindow = parent;

    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    setStyleSheet("background-color: white;");
    _cW = parent->_superior->_canvasWidth;
    _cH = parent->_superior->_canvasHeight;

    double ratioW = 750.0 / (double)_cW;
    double ratioH = 400.0 / (double)_cH;

    // qDebug()<<"H "<<_H<<" W "<<_W<<" cH "<<_cH<<" cW "<<_cW;
    // qDebug()<<"ratioW "<<ratioW<<" ratioH "<<ratioH;

    if (ratioW > ratioH) {
        setGeometry(0, 0, (double)_cW * ratioH, 400);
        _showRatio = ratioH;
        _W = (double)_cW * ratioH;
        _H = 400;
        parent->_W = (double)_cW * ratioH;
        parent->_H = 400;
    }
    else {
        setGeometry(0, 0, 750, (double)_cH * ratioW);
        _showRatio = ratioW;
        _W = 750;
        _H = (double)_cH * ratioW;
        parent->_W = 750;
        parent->_H = (double)_cH * ratioW;
    }
    updateGeometry();
    // qDebug()<<"EditNode Geometry: "<<_W<<_H;
    _imageRegion = new EditImageRegion(this);
}

EditCanvasRegion::~EditCanvasRegion()
{
    qDebug()<<"EditCanvasRegion::~EditCanvasRegion";
}

QSize EditCanvasRegion::sizeHint() const
{
    return QSize(_W, _H);
}

void EditCanvasRegion::paintEvent(QPaintEvent *e)
{
    QWidget::paintEvent(e);
    qDebug()<<"canvaspaint";
}

void EditCanvasRegion::loadImage(cv::Mat img)
{
    _imageRegion->loadImage(img, _showRatio);
}

EditImageRegion::EditImageRegion(EditCanvasRegion *parent) : QLabel((QWidget*)parent)
{
    setMouseTracking(true);
    _imageMoving = false;
    _canvasRegion = parent;
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
}

EditImageRegion::~EditImageRegion()
{
    qDebug()<<"EditImageRegion::~EditImageRegion";
}

void EditImageRegion::resetPos(int x, int y)
{
    if (x != -1) {
        _shownPosition.setX(x);
    }
    if (y != -1) {
        _shownPosition.setY(y);
    }
    setGeometry(_shownPosition.x(), _shownPosition.y(), _shownWidth, _shownHeight);
}

void EditImageRegion::resetSize(double scale, int angle)
{
    if (scale != -1) {
        _scale = scale;
    }
    if (angle != -1) {
        _angle = angle;
    }

    double sn = qSin((double)_angle * M_PI / 180.0);
    double cs = qCos((double)_angle * M_PI / 180.0);
    if (sn < 0) {
        sn = -sn;
    }
    if (cs < 0) {
        cs = -cs;
    }

    _shownWidth = qCeil(qCeil(((double)_originalWidth * cs + (double)_originalHeight * sn)) * _scale);
    _shownHeight = qCeil(qCeil(((double)_originalWidth * sn + (double)_originalHeight * cs)) * _scale);

    // qDebug()<<"original: "<<_originalWidth<<" "<<_originalHeight;
    // qDebug()<<"shown: "<<_shownWidth<<" "<<_shownHeight;
    if (_shownPosition.x() + _shownWidth <= 4) {
        _shownPosition.setX(5 - _shownWidth);
    }
    if (_shownPosition.y() + _shownHeight <= 4) {
        _shownPosition.setY(5 - _shownHeight);
    }

    _canvasRegion->_canvasWindow->_xFillin->setRange(5 - _shownWidth, _canvasRegion->_canvasWindow->_W - 5);
    _canvasRegion->_canvasWindow->_yFillin->setRange(5 - _shownHeight, _canvasRegion->_canvasWindow->_H - 5);

    setGeometry(_shownPosition.x(), _shownPosition.y(), _shownWidth, _shownHeight);

    cv::Mat displayImg;
    cv::resize(_originalImage, displayImg, cv::Size(_originalImage.cols * _scale, _originalImage.rows * _scale));

    QImage displayQImg = cvMat2QImage(displayImg);
    QPixmap shownPixmap = QPixmap::fromImage(displayQImg);

    QTransform transform;
    transform.rotate(_angle);
    shownPixmap = shownPixmap.transformed(transform);
    setPixmap(shownPixmap);
}

// 加载图像：只有在创建时发起
void EditImageRegion::loadImage(cv::Mat img, double ratioShow)
{
    // 将img按比例缩放，作为_originalImage（_coreImage : _originalImage = _canvasSize : _canvasRegionSize）
    if (ratioShow != -1) {
        _ratioShow = ratioShow;
    }
    cv::resize(img, img, cv::Size(img.cols * _ratioShow, img.rows * _ratioShow));
    _originalImage = img;
    _originalWidth = img.cols;
    _originalHeight = img.rows;
    _shownWidth = img.cols;
    _shownHeight = img.rows;

    setGeometry(_shownPosition.x(), _shownPosition.y(), _shownWidth, _shownHeight);

    // 显示
    QImage displayQImg = cvMat2QImage(img);
    setPixmap(QPixmap::fromImage(displayQImg));
}

void EditImageRegion::reloadImage(cv::Mat img)
{
    cv::resize(img, img, cv::Size(img.cols * _ratioShow, img.rows * _ratioShow));
    _originalImage = img;
    _originalWidth = img.cols;
    _originalHeight = img.rows;

    double sn = qSin((double)_angle * M_PI / 180.0);
    double cs = qCos((double)_angle * M_PI / 180.0);
    if (sn < 0) {
        sn = -sn;
    }
    if (cs < 0) {
        cs = -cs;
    }

    _shownWidth = qCeil(qCeil(((double)_originalWidth * cs + (double)_originalHeight * sn)) * _scale);
    _shownHeight = qCeil(qCeil(((double)_originalWidth * sn + (double)_originalHeight * cs)) * _scale);

    if (_shownPosition.x() + _shownWidth <= 4) {
        //qDebug()<<"adjusted";
        _shownPosition.setX(5 - _shownWidth);
        //qDebug()<<_shownPosition.x();
    }
    if (_shownPosition.y() + _shownHeight <= 4) {
        //qDebug()<<"adjusted";
        _shownPosition.setY(5 - _shownHeight);
        //qDebug()<<_shownPosition.y();
    }

    _canvasRegion->_canvasWindow->_xFillin->setRange(5 - _shownWidth, _canvasRegion->_canvasWindow->_W - 5);
    _canvasRegion->_canvasWindow->_yFillin->setRange(5 - _shownHeight, _canvasRegion->_canvasWindow->_H - 5);

    setGeometry(_shownPosition.x(), _shownPosition.y(), _shownWidth, _shownHeight);

    cv::Mat displayImg;
    cv::resize(_originalImage, displayImg, cv::Size(_originalImage.cols * _scale, _originalImage.rows * _scale));

    //QImage displayQImg = QImage((const unsigned char*)(displayImg.data), displayImg.cols, displayImg.rows, displayImg.step, QImage::Format_RGB888).rgbSwapped();
    QImage displayQImg = cvMat2QImage(displayImg);
    QPixmap shownPixmap = QPixmap::fromImage(displayQImg);

    QTransform transform;
    transform.rotate(_angle);
    shownPixmap = shownPixmap.transformed(transform);
    setPixmap(shownPixmap);
    //qDebug()<<"ok";
}

// 按下鼠标：开始拖动
void EditImageRegion::mousePressEvent(QMouseEvent *e)
{
    QLabel::mousePressEvent(e);
    if (e->button() == Qt::LeftButton)
    {
        // 设置“拖动中”状态，记录鼠标落点以供后续计算
        _imageMoving = true;
        _startPoint = e->pos();
        _endPoint = e->pos();
    }
}

// 释放鼠标：结束拖动
void EditImageRegion::mouseReleaseEvent(QMouseEvent *e)
{
    //qDebug()<<"mouserelease";
    QLabel::mouseReleaseEvent(e);
    if (e->button() == Qt::LeftButton && _imageMoving)
    {
        // 结束“拖动中”状态，更新_originalPosition供下一次拖动时计算
        _imageMoving = false;
        _originalPosition = _shownPosition;
    }
}

// 鼠标移动：（按下时）拖动图片
void EditImageRegion::mouseMoveEvent(QMouseEvent *e)
{
    if (_imageMoving)
    {
        // 不断更新最终落点，供绘图
        //qDebug()<<"mouseMove";
        _endPoint = e->pos();

        //qDebug()<<"startPoint: "<<_startPoint<<" endPoint: "<<_endPoint;

        // 计算图像显示位置并实时显示
        QPoint dPos = _endPoint - _startPoint;
        _shownPosition = mapToParent(dPos);
        //qDebug()<<"shownPosition: "<<_shownPosition;

        if (_shownPosition.x() <= -_shownWidth) {
            _shownPosition.setX(5 - _shownWidth);
        }
        if (_shownPosition.y() <= -_shownHeight) {
            _shownPosition.setY(5 - _shownHeight);
        }
        if (_shownPosition.x() >= _canvasRegion->_W) {
            _shownPosition.setX(_canvasRegion->_W - 5);
        }
        if (_shownPosition.y() >= _canvasRegion->_H) {
            _shownPosition.setY(_canvasRegion->_H - 5);
        }

        setGeometry(_shownPosition.x(), _shownPosition.y(), _shownWidth, _shownHeight);

        // 更新位置输入框数值
        _canvasRegion->_canvasWindow->_xFillin->setValue(_shownPosition.x());
        _canvasRegion->_canvasWindow->_yFillin->setValue(_shownPosition.y());
        //update();
    }
}

// 使用EditNode的滑块修改坐标
void EditImageRegion::inputSetX(int val)
{
    //qDebug()<<"inputSetX Called";
    _shownPosition.setX(val * _ratioShow);  // mark 这里的计算也许有问题？
    setGeometry(_shownPosition.x(), _shownPosition.y(), _shownWidth, _shownHeight);
    // 更新输入框的数值
    _canvasRegion->_canvasWindow->_xFillin->setValue(val * _ratioShow);
}

void EditImageRegion::inputSetY(int val)
{
    //qDebug()<<"inputSetY Called";
    _shownPosition.setY(val * _ratioShow);
    setGeometry(_shownPosition.x(), _shownPosition.y(), _shownWidth, _shownHeight);
    _canvasRegion->_canvasWindow->_yFillin->setValue(val * _ratioShow);
}

// EditCropWindow
EditCropWindow::EditCropWindow(Node *superior, cv::Mat image)
    : QWidget(nullptr), ui(new Ui::EditCropWindow)
{
    ui->setupUi(this);
    setWindowTitle(QChinese("裁剪"));
    setFixedSize(800, 550);
    _superior = (EditNode*)superior;

    _cropRegion = new EditCropRegion(this);
    _cropRegion->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    //QImage qImage(image.data, image.cols, image.rows, image.step, QImage::Format_RGB888);
    _cropRegion->loadMat(image);
    ui->cropRegionLayout->addWidget(_cropRegion);
    ui->cropRegionLayout->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

    QPushButton *aiCropButton = ui->aiCropButton;
/*
    connect(aiCropButton, &QPushButton::clicked, [this]() {
        // 勤俭持家crz
        // 其他功能调试完成之后，删除下面的部分，开放最后一行代码
        // edit的这里还是得再思考一下
        QLabel* lb = new QLabel(QChinese("Edit Node的AI抠图功能正在开发中，敬请期待！"));
        lb->show();
        // 其他功能调试完成之后，删除上面的部分，开放下面这一行代码
        //_cropRegion->aiCrop();
    });
*/
    QPushButton *acceptButton = ui->acceptButton;
    connect(acceptButton, &QPushButton::clicked, [this]() {
        this->_cropRegion->apply();
    });

    QPushButton *cancelButton = ui->cancelButton;
    connect(cancelButton, &QPushButton::clicked, [this]() {
        this->_cropRegion->cancel();
    });

    show();
}

EditCropWindow::~EditCropWindow()
{
    qDebug()<<"EditCropWindow::~EditCropWindow()";
}

QSize EditCropWindow::sizeHint() const
{
    return QSize(1000,750);
}

// EditCropRegion
EditCropRegion::EditCropRegion(EditCropWindow *parent) : QLabel((QWidget*)parent)
{
    _cropWindow = parent;
    W = 580;
    H = 410;
    setMouseTracking(true);
    setGeometry(0, 0, W, H);
    //qDebug() << "Geometry: " << geometry();
    setStyleSheet("background: white;");
    //setScaledContents(true);
    _mousePressed = false;
    //show();
}

EditCropRegion::~EditCropRegion()
{
    qDebug()<<"EditCropRegion::~EditCropRegion()";
}

void EditCropRegion::loadMat(cv::Mat src)
{
    //QImage qt_img = QImage(src.data, src.cols, src.rows, src.step, QImage::Format_RGB888).rgbSwapped();
    QImage qt_img = cvMat2QImage(src);
    _originalImage = src;
    _currentCoreImage = src;
    _shownImage = QPixmap::fromImage(qt_img);
    _shownImage = _shownImage.scaled(geometry().width(), geometry().height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    int w = _shownImage.width(), h = _shownImage.height();
    setPixmap(_shownImage);
    setAlignment(Qt::AlignCenter);
    _upBorder = (geometry().height() - h) / 2;
    _downBorder = (geometry().height() + h) / 2;
    _leftBorder = (geometry().width() - w) / 2;
    _rightBorder = (geometry().width() + w) / 2;
    //qDebug() << "pixmap " << _upBorder << ' ' << _downBorder << ' ' << _leftBorder << ' ' << _rightBorder;
}

void EditCropRegion::paintEvent(QPaintEvent *e)
{
    QLabel::paintEvent(e);
    QPainter painter(this);
    //设置画笔，颜色、宽度
    if (_mousePressed || _mouseCropChecking)
    {
        QPen pen;
        pen.setColor(Qt::red);
        pen.setWidth(2);
        painter.setPen(pen);
        _paintedRect = QRect(_paintedRectStart, _paintedRectEnd);
        painter.drawRect(_paintedRect);
    }
    else if (_cropShow)
    {
        QPen pen;
        pen.setColor(Qt::black);
        pen.setWidth(2);
        painter.setPen(pen);
        _paintedRect = QRect(_paintedRectStart, _paintedRectEnd);
        painter.drawRect(_paintedRect);
    }

}

void EditCropRegion::mousePressEvent(QMouseEvent *e)
{
    if (e->buttons()&Qt::LeftButton)
    {
        if (_cropMode == 2) {
            QLabel* lb = new QLabel("正在AI抠图中，请在完成或取消后再手动截取图像。");
            lb->show();
            return;
        }
        _mouseCropChecking = false;
        _cropMode = 1;
        QPoint p = e->pos();
        if (p.y() < _upBorder) {
            p.setY(_upBorder);
        }
        if (p.y() > _downBorder) {
            p.setY(_downBorder);
        }
        if (p.x() < _leftBorder) {
            p.setX(_leftBorder);
        }
        if (p.x() > _rightBorder) {
            p.setX(_rightBorder);
        }
        _paintedRectStart = p;
        _paintedRectEnd = p;
        //qDebug() << "start_point: " << _paintedRectStart;
        _mousePressed = true;
    }

    update();
    QLabel::mousePressEvent(e);
}

void EditCropRegion::mouseReleaseEvent(QMouseEvent *e)
{
    if (_mousePressed) {
        _mousePressed = false;
        if (e->button() == Qt::LeftButton)
        {
            //qDebug() << "end_point: " << _paintedRectEnd;
            _mouseCropChecking = true;
        }
    }
}

void EditCropRegion::mouseMoveEvent(QMouseEvent *e)
{
    if (e->buttons()&Qt::LeftButton)
    {
        if (_mousePressed)
        {
            QPoint p = e->pos();
            if (p.y() < _upBorder) {
                p.setY(_upBorder);
            }
            if (p.y() > _downBorder) {
                p.setY(_downBorder);
            }
            if (p.x() < _leftBorder) {
                p.setX(_leftBorder);
            }
            if (p.x() > _rightBorder) {
                p.setX(_rightBorder);
            }
            _paintedRectEnd = p;
            update();
        }
    }
}

void EditCropRegion::apply()    // 更新裁剪框
{
    if (_cropMode == 1) {
        _mouseCropChecking = false;     // 结束确认阶段
        _cropShow = true;

        // 计算裁剪区域
        int minX, minY;
        if (_paintedRectStart.x() > _paintedRectEnd.x()) {
            minX = _paintedRectEnd.x();
        }
        else {
            minX = _paintedRectStart.x();
        }
        if (_paintedRectStart.y() > _paintedRectEnd.y()) {
            minY = _paintedRectEnd.y();
        }
        else {
            minY = _paintedRectStart.y();
        }
        int dX = abs(_paintedRectEnd.x() - _paintedRectStart.x());
        int dY = abs(_paintedRectEnd.y() - _paintedRectStart.y());
        if (dX < 5 || dY < 5) {
            update();
            return;
        }

        _croppedRect.x = (int)(((double)minX - (double)_leftBorder) * (double)_currentCoreImage.cols / (double)_shownImage.width());
        _croppedRect.y = (int)(((double)minY - (double)_upBorder) * (double)_currentCoreImage.rows / (double)_shownImage.height());
        _croppedRect.width = (int)((double)dX * (double)_currentCoreImage.cols / (double)_shownImage.width());
        _croppedRect.height = (int)((double)dY * (double)_currentCoreImage.rows / (double)_shownImage.height());
        if (_croppedRect.x + _croppedRect.width > _currentCoreImage.cols) {
            _croppedRect.width = _currentCoreImage.cols - _croppedRect.x;
        }
        if (_croppedRect.y + _croppedRect.height > _currentCoreImage.rows) {
            _croppedRect.height = _currentCoreImage.rows - _croppedRect.y;
        }

        _cropWindow->_editWin->_superior->_cropRect = _croppedRect;

        // 裁剪
        cv::Mat croppedImage = _currentCoreImage(_croppedRect).clone();

        // 在 QLabel 中显示图像
        _cropWindow->_editWin->_canvasWindow->_canvasRegion->_imageRegion->reloadImage(croppedImage);

        update();
    }
    else if (_cropMode == 2) {
        _currentCoreImage = _tempAiCroppedImage.clone();
        _cropWindow->_editWin->_canvasWindow->_canvasRegion->_imageRegion->reloadImage(_currentCoreImage);
        _tempAiCroppedImage = cv::Mat().clone();
    }
    _cropMode = 0;
}

void EditCropRegion::cancel()
{
    if (_cropMode == 1) {
        _mouseCropChecking = false;
        _cropShow = false;
        _cropMode = 0;
        update();
    }
    else if (_cropMode == 2) {
        QImage qt_img = cvMat2QImage(_currentCoreImage);
        _shownImage = QPixmap::fromImage(qt_img);
        _shownImage = _shownImage.scaled(geometry().width(), geometry().height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        setPixmap(_shownImage);
        _cropMode = 0;
    }
}

QSize EditCropRegion::sizeHint() const
{
    QSize s;
    s.setWidth(W);
    s.setHeight(H);
    return s;
}
